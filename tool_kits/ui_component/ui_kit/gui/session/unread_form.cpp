#include "resource.h"
#include "unread_form.h"
#include "export/nim_ui_all.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR UnreadForm::kClassName	= L"UnreadForm";

const int kSysmsgCount = 20;

UnreadForm::UnreadForm()
{

}

UnreadForm::~UnreadForm()
{
	
}

std::wstring UnreadForm::GetSkinFolder()
{
	return L"unread_list";
}

std::wstring UnreadForm::GetSkinFile()
{
	return L"unread_form.xml";
}

std::wstring UnreadForm::GetWindowClassName() const 
{
	return kClassName;
}

std::wstring UnreadForm::GetWindowId() const 
{
	return kClassName;
}

UINT UnreadForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void UnreadForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetDefaultIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetDefaultIcon());
	}

	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&UnreadForm::Notify, this, std::placeholders::_1));

	((OptionBox*)FindControl(L"btn_unread_list"))->Selected(true, true);

	unread_list_ = (ListBox*)FindControl(L"unread_list");
	read_list_ = (ListBox*)FindControl(L"read_list");
	read_title_ = (Label *)FindControl(L"label_read");
	unread_title_ = (Label *)FindControl(L"label_unread");
	form_title_ = (Label *)FindControl(L"title");
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&UnreadForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
}

bool UnreadForm::Notify( ui::EventArgs* msg )
{
	if (msg->Type == kEventSelect)
	{

	}
	return true;
}

void GetMessageContent(const nim::IMMessage &msg, std::wstring &show_text)
{
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (msg.type_ == nim::kNIMMessageTypeText)
	{
		show_text = nbase::UTF8ToUTF16(msg.content_);
	}
	else if (msg.type_ == nim::kNIMMessageTypeImage)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_IMAGE");
	}
	else if (msg.type_ == nim::kNIMMessageTypeAudio)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_AUDIO");
	}
	else if (msg.type_ == nim::kNIMMessageTypeVideo)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_VIDEO");
	}
	else if (msg.type_ == nim::kNIMMessageTypeFile)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_FILE");
	}
	else if (msg.type_ == nim::kNIMMessageTypeLocation)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_LOCATION");
	}
// 	else if (msg.type_ == nim::kNIMMessageTypeCustom)
// 	{
// 		show_text = GetCustomMsg(msg.msg_sender_accid_, msg.msg_attach_);
// 	}
	else if (msg.type_ == nim::kNIMMessageTypeUnknown)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_UNDEFINED");
	}
	else if (msg.type_ == nim::kNIMMessageTypeRobot)
	{
		bool out_msg = false;
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(msg.attach_, values) && values.isObject())
			out_msg = values[nim::kNIMBotRobotReceivedMsgKeyMsgOut].asBool();
		if (!out_msg)
			show_text = nbase::UTF8ToUTF16(msg.content_);
		else
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_ROBOT");
	}
	else
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_UNKNOWN");
		std::string id = msg.client_msg_id_;
		QLOG_WAR(L"unknown msg: id_type={0}_{1} msg_type={2}") << id << msg.type_ << msg.type_;
	}
}


void UnreadForm::LoadList(const nim::IMMessage &msg, const std::map<std::string, nim::TeamMemberProperty> &team_members)
{
	//QLOG_PRO(L"query end events: count={0}") <<count;
	members_ = team_members;
	msg_ = msg;
	std::wstring content;
	GetMessageContent(msg, content);
	if (content.length() > 10)
	{
		content = content.substr(0, 10);
		content += L"...";
	}
	content = L"消息:" + content;
	form_title_->SetText(content);

	DoLoadList();
}


void UnreadForm::OnTeamEventCallback(const nim::TeamEvent& result)
{
	UnreadForm * win = (UnreadForm *)nim_comp::WindowsManager::GetInstance()->GetWindow(UnreadForm::kClassName, UnreadForm::kClassName);
	if (win)
		win->OnLoadListCallback(result);
}

void UnreadForm::OnLoadListCallback(const nim::TeamEvent& result)
{
	if (result.src_data_["client_msg_id"].asString() != msg_.client_msg_id_)
		return;

	Json::Value reads = result.src_data_["read"];
	Json::Value unreads = result.src_data_["unread"];
	unread_list_->RemoveAll();
	read_list_->RemoveAll();
	UpdateReadCount(reads.size());
	UpdateUnreadCount(unreads.size());
	int sz = (int)reads.size();
	for (auto i = 0; i < sz; i++)
	{
		nim::TeamMemberProperty prop;
		std::string id = reads[i].asString();
		if (members_.find(id) == members_.end())
			prop.SetAccountID(id);
		else
			prop = members_[id];
		AddItem(prop, false);
	}
	sz = (int)unreads.size();
	for (auto i = 0; i < sz; i++)
	{
		nim::TeamMemberProperty prop;
		std::string id = unreads[i].asString();
		if (members_.find(id) == members_.end())
			prop.SetAccountID(id);
		else
			prop = members_[id];
		AddItem(prop, true);
	}
}

void UnreadForm::UpdateUnreadCount(const std::string &msg_id, const int unread, const std::string &read_accid)
{
	if (msg_id != msg_.client_msg_id_)
		return;

	int total_count = read_list_->GetCount() + unread_list_->GetCount();
	UpdateReadCount(total_count - unread);
	UpdateUnreadCount(unread);

	auto cont = unread_list_->FindSubControl(nbase::UTF8ToUTF16(read_accid));
	if (cont)
		unread_list_->Remove(cont);

	AddItem(members_[read_accid], false);
}

void UnreadForm::DoLoadList()
{
	nim::Team::TeamMsgQueryUnreadList(msg_.receiver_accid_, msg_, nbase::Bind(&UnreadForm::OnTeamEventCallback, std::placeholders::_1));
}

void UnreadForm::AddItem(const nim::TeamMemberProperty &member, bool unread)
{
	UnreadItem* item = new UnreadItem;
	GlobalManager::FillBoxWithCache(item, L"unread_list/unread_item.xml");
	item->InitCtrl();
	bool ret = item->InitInfo(member);
	if (ret)
	{
		if (unread)
			unread_list_->Add(item);
		else
			read_list_->Add(item);
	}
}

void UnreadForm::OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path)
{
	if (type == kUser)
	{
		for (int i = 0; i < unread_list_->GetCount(); i++)
		{
			UnreadItem* event_item = dynamic_cast<UnreadItem*>(unread_list_->GetItemAt(i));
			if (event_item)
			{
				bool ret = event_item->OnUserPhotoReady(account, photo_path);
				if (ret)
					break;
			}
		}

		for (int i = 0; i < read_list_->GetCount(); i++)
		{
			UnreadItem* custom_item = dynamic_cast<UnreadItem*>(read_list_->GetItemAt(i));
			if (custom_item)
			{
				bool ret = custom_item->OnUserPhotoReady(account, photo_path);
				if (ret)
					break;
			}
		}
	}
}

void UnreadForm::UpdateUnreadCount(int count)
{
	std::wstring txt = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_UNREADMSG_CENTER_UNREAD");
	if (count > 0)
	{
		txt += L"(%d)";
		txt = nbase::StringPrintf(txt.c_str(), count);
	}
	unread_title_->SetText(txt);
}

void UnreadForm::UpdateReadCount(int count)
{
	std::wstring txt = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_UNREADMSG_CENTER_READ");
	if (count > 0)
	{
		txt += L"(%d)";
		txt = nbase::StringPrintf(txt.c_str(), count);
	}
	read_title_->SetText(txt);
}

void UnreadForm::ShowLoadingTip(bool add)
{
	static const std::wstring kLoadingName = L"EventLoading";

	Control* ctrl = unread_list_->FindSubControl(kLoadingName);
	if (ctrl)
		unread_list_->Remove(ctrl);

	if (add)
	{
		Box* box = new Box;
		GlobalManager::FillBoxWithCache(box, L"unread_list/unread_tip.xml");
		unread_list_->Add(box);

		box->SetName(kLoadingName);
	}
}


}