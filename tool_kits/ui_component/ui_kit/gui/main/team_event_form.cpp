#include "resource.h"
#include "team_event_form.h"
#include "control/custom_msg.h"
#include "export/nim_ui_all.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR TeamEventForm::kClassName	= L"TeamEventForm";

const int kSysmsgCount = 20;

TeamEventForm::TeamEventForm()
{
	farst_time_ = 0;
	has_more_ = true;
	is_loading_ = false;

	last_custom_msg_time_ = 0;
	has_more_custom_ = true;
}

TeamEventForm::~TeamEventForm()
{
	
}

std::wstring TeamEventForm::GetSkinFolder()
{
	return L"main";
}

std::wstring TeamEventForm::GetSkinFile()
{
	return L"event.xml";
}

ui::UILIB_RESOURCETYPE TeamEventForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring TeamEventForm::GetZIPFileName() const
{
	return L"event.zip";
}

std::wstring TeamEventForm::GetWindowClassName() const 
{
	return kClassName;
}

std::wstring TeamEventForm::GetWindowId() const 
{
	return kClassName;
}

UINT TeamEventForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void TeamEventForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetIcon());
	}

	SetTaskbarTitle(L"消息中心");
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&TeamEventForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&TeamEventForm::OnClicked, this, std::placeholders::_1));

	((OptionBox*)FindControl(L"btn_sysmsg_list"))->Selected(true, true);

	btn_recycle_ = (Button*) FindControl(L"btn_recycle");
	event_list_ = (ListBox*)FindControl(L"event_list");
	custom_list_ = (ListBox*)FindControl(L"custom_list");

	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&TeamEventForm::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(nbase::Bind(&TeamEventForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&TeamEventForm::OnTeamNameChange, this, std::placeholders::_1)));

	GetMoreCustomMsg();
}

bool TeamEventForm::Notify( ui::EventArgs* msg )
{
	if(msg->Type == kEventNotify)
	{
		TeamEventItem* item = dynamic_cast<TeamEventItem*>(msg->pSender);
		assert(item);
		if(msg->wParam == EOT_DELETE)
		{
			__int64 msg_id = item->GetMsgId();
			id_item_pair_.erase(msg_id);

			event_list_->Remove(item);
		}
	}
	else if(msg->Type == kEventScrollChange)
	{
		if (msg->pSender == event_list_)
		{
			bool list_last = (event_list_->GetScrollPos().cy >= event_list_->GetScrollRange().cy - 5);
			if (list_last && has_more_ && !is_loading_)
			{
				is_loading_ = true;
				btn_recycle_->SetEnabled(false);

				OpEventTip(true);
				event_list_->EndDown(true, false);

				nbase::ThreadManager::PostDelayedTask(kThreadUI,
					nbase::Bind(&TeamEventForm::InvokeLoadEvents, this),
					nbase::TimeDelta::FromMilliseconds(1500));
			}
		} 
		else if (msg->pSender == custom_list_)
		{
			bool list_last = (custom_list_->GetScrollPos().cy >= custom_list_->GetScrollRange().cy - 5);
			if (list_last && has_more_custom_)
			{
				GetMoreCustomMsg();
			}
		}
	}
	else if (msg->Type == kEventSelect)
	{
		if (msg->pSender->GetName() == L"btn_custom_list")
		{
			UpdateCustomSysmsgUnread(false);
		}
	}
	return true;
}

bool TeamEventForm::OnClicked( ui::EventArgs* msg )
{
	std::wstring name = msg->pSender->GetName();
	if(name == L"btn_recycle")
	{
		nim::SystemMsg::DeleteAllAsync(nbase::Bind(&TeamEventForm::DeleteAllCb, std::placeholders::_1, std::placeholders::_2));

		event_list_->RemoveAll();
		id_item_pair_.clear();
	}
	return true;
}

void TeamEventForm::ReloadEvents()
{
	if(is_loading_)
		return;

	event_list_->RemoveAll();
	id_item_pair_.clear();

	nim::SystemMsg::ReadAllAsync(nbase::Bind(&TeamEventForm::SysMsgReadAllCb, std::placeholders::_1, std::placeholders::_2));

	has_more_ = true;
	farst_time_ = 0;
	InvokeLoadEvents();
}

void TeamEventForm::InvokeLoadEvents()
{
	is_loading_ = true;
	btn_recycle_->SetEnabled(false);

	QLOG_PRO(L"query begin events: time={0}") <<farst_time_;
	nim::SystemMsg::QueryMsgAsync(kSysmsgCount, farst_time_, nbase::Bind(&TeamEventForm::LoadEventsCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TeamEventForm::LoadEventsCb(int count, int unread, const std::list<nim::SysMessage> &result)
{
	QLOG_PRO(L"query end events: count={0}") <<count;

	is_loading_ = false;
	btn_recycle_->SetEnabled(true);
	OpEventTip(false);

	UpdateSysmsgUnread(unread);
	for (auto& content : result)
	{
		AddEvent(content, false);
	}
	UpdateFarstTime();

	if (result.size() == kSysmsgCount)
		has_more_ = true;
	else
		has_more_ = false;
}

void TeamEventForm::UpdateFarstTime()
{
	int n = event_list_->GetCount();
	if(n > 0)
	{
		TeamEventItem* item = dynamic_cast<TeamEventItem*>(event_list_->GetItemAt(n-1));
		farst_time_ = item->GetTime();
	}
}

void TeamEventForm::AddEvent(const nim::SysMessage &sys_msg, bool first)
{
	if(id_item_pair_.find(sys_msg.id_) == id_item_pair_.end())
	{
		TeamEventItem* item = new TeamEventItem;
		GlobalManager::FillBoxWithCache(item, L"main/event_item.xml");
		item->InitCtrl();
		bool ret = item->InitInfo(sys_msg);
		if (ret)
		{
			if (first)
				event_list_->AddAt(item, 0);
			else
				event_list_->Add(item);

			id_item_pair_[sys_msg.id_] = item;
		}
	}
	else
	{
		QLOG_ERR(L"repeat sysmsg, id={0}") << sys_msg.id_;
	}
}

void TeamEventForm::OpEventTip( bool add )
{
	static const std::wstring kLoadingName = L"EventLoading";

	Control* ctrl = event_list_->FindSubControl(kLoadingName);
	if(ctrl)
		event_list_->Remove(ctrl);

	if(add)
	{
		Box* box = new Box;
		GlobalManager::FillBoxWithCache(box, L"main/event_tip.xml");
		event_list_->Add(box);

		box->SetName(kLoadingName);
	}
}

void TeamEventForm::OnTeamEventCb(__int64 msg_id, nim::NIMSysMsgStatus status)
{
	std::map<__int64,TeamEventItem*>::iterator it = id_item_pair_.find(msg_id);
	if(it != id_item_pair_.end())
	{
		it->second->OnTeamEventCb(status);
	}
}
void TeamEventForm::OnOneTeamEvent(const nim::SysMessage &json)
{
	AddEvent(json, true);
}

void TeamEventForm::DeleteAllCb(nim::NIMResCode res_code, int unread)
{
	QLOG_APP(L"DeleteAllCb, code={0} unread={1}") <<res_code <<unread;
	UpdateSysmsgUnread(unread);
}

void TeamEventForm::SysMsgReadAllCb(nim::NIMResCode code, int unread)
{
	QLOG_APP(L"SysMsgReadAllCb, code={0} unread={1}") <<code <<unread;
	UpdateSysmsgUnread(unread);
}

void TeamEventForm::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (const auto& info : uinfos)
	{
		for (int i = 0; i < event_list_->GetCount(); i++)
		{
			TeamEventItem* event_item = dynamic_cast<TeamEventItem*>(event_list_->GetItemAt(i));
			if(event_item)
				event_item->OnUserInfoChange(info);
		}

		for (int i = 0; i < custom_list_->GetCount(); i++)
		{
			CustomMsgBubble* custom_item = dynamic_cast<CustomMsgBubble*>(custom_list_->GetItemAt(i));
			if (custom_item)
				custom_item->OnUserInfoChange(info);
		}
	}
}

void TeamEventForm::OnUserPhotoReady(const std::string& account, const std::wstring& photo_path)
{
	for (int i = 0; i < event_list_->GetCount(); i++)
	{
		TeamEventItem* event_item = dynamic_cast<TeamEventItem*>(event_list_->GetItemAt(i));
		if (event_item)
			event_item->OnUserPhotoReady(account, photo_path);
	}

	for (int i = 0; i < custom_list_->GetCount(); i++)
	{
		CustomMsgBubble* custom_item = dynamic_cast<CustomMsgBubble*>(custom_list_->GetItemAt(i));
		if (custom_item)
			custom_item->OnUserPhotoReady(account, photo_path);
	}
}

void TeamEventForm::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	for (int i = 0; i < event_list_->GetCount(); i++)
	{
		TeamEventItem* event_item = dynamic_cast<TeamEventItem*>(event_list_->GetItemAt(i));
		if (event_item)
			event_item->OnTeamNameChange(team_info);
	}

	for (int i = 0; i < custom_list_->GetCount(); i++)
	{
		CustomMsgBubble* custom_item = dynamic_cast<CustomMsgBubble*>(custom_list_->GetItemAt(i));
		if (custom_item)
			custom_item->OnTeamNameChange(team_info);
	}
}

void UpdateSysmsgUnread( int count )
{
	nim_ui::SessionListManager::GetInstance()->UISysmsgUnread(count);
}

void UpdateCustomSysmsgUnread(bool add)
{
	nim_ui::SessionListManager::GetInstance()->UICustomSysmsgUnread(add);
}

}