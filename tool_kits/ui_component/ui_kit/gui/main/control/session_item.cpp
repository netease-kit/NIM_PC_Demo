#include "session_item.h"

#include "module/session/session_manager.h"
#include "callback/session/session_callback.h"

#include "shared/ui/ui_menu.h"
#include "shared/pin_yin_helper.h"
#include "gui/profile_form/profile_form.h"

using namespace ui;

namespace nim_comp
{
SessionItem::SessionItem()
{

}

SessionItem::~SessionItem()
{

}

void SessionItem::InitCtrl()
{
	this->AttachDoubleClick(nbase::Bind(&SessionItem::OnDbClicked, this, std::placeholders::_1));
	this->AttachMenu(nbase::Bind(&SessionItem::OnSessionItemMenu, this, std::placeholders::_1));

	label_name_ = (Label*) this->FindSubControl(L"label_name");
	label_msg_ = (Label*) this->FindSubControl(L"label_msg");
	label_time_ = (Label*) this->FindSubControl(L"label_time");
	box_unread_ = (Box*) this->FindSubControl(L"box_unread");
	label_unread_ = (Label*) this->FindSubControl(L"label_unread");
	head_image_ = (ButtonBox*)this->FindSubControl(L"head_image");
	head_image_->AttachClick(nbase::Bind(&SessionItem::OnHeadImageClicked, this, std::placeholders::_1));
}

long long SessionItem::GetMsgTime()
{
	return msg_.msg_timetag_;
}

void SessionItem::InitUserProfile()
{
	if (msg_.type_ == nim::kNIMSessionTypeP2P)
	{
		UserService *user_service = UserService::GetInstance();
		if (LoginManager::GetInstance()->IsEqual(msg_.id_))
		{
			label_name_->SetText(L"我的手机");
		} 
		else
		{
			label_name_->SetText(user_service->GetUserName(msg_.id_));
		}
		head_image_->SetBkImage(user_service->GetUserPhoto(msg_.id_));
	}
	else
	{
		std::wstring tname = TeamService::GetInstance()->GetTeamName(msg_.id_);
		label_name_->SetText(tname);
		head_image_->SetBkImage(TeamService::GetInstance()->GetTeamPhoto(false));
	}
}

static void GetMsgContent(const nim::SessionData &msg, std::wstring &show_text)
{
	if (msg.msg_type_ == nim::kNIMMessageTypeText)
	{
		show_text = nbase::UTF8ToUTF16(msg.msg_content_);
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeImage)
	{
		show_text = L"[图片]";
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeAudio)
	{
		show_text = L"[语音]";
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeVideo)
	{
		show_text = L"[视频]";
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeFile)
	{
		show_text = L"[文件]";
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeLocation)
	{
		show_text = L"[位置]";
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeNotification)
	{
		GetNotifyMsg(msg.msg_attach_, msg.msg_sender_accid_, "", show_text, msg.id_);
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeCustom)
	{
		show_text = GetCustomMsg(msg.msg_attach_);
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeUnknown)
	{
		show_text = L"[未定义消息]";
	}
	else
	{
		show_text = L"[未知消息]";
		std::string id = msg.id_;
		QLOG_WAR(L"unknown msg: id_type={0}_{1} msg_type={2}") << id << msg.type_ << msg.msg_type_;
	}
}

void SessionItem::UpdateMsgContent(const std::string& id /*= ""*/)
{
	if (!id.empty() && relate_ids.find(id) == relate_ids.cend())
		return;

	std::wstring show_text;
	if (msg_.msg_status_ != nim::kNIMMsgLogStatusDeleted)
	{
		GetMsgContent(msg_, show_text);

		if (msg_.type_ == nim::kNIMSessionTypeTeam) 
		{
			if (msg_.msg_type_ == nim::kNIMMessageTypeNotification && !IsNetCallMsg((nim::NIMMessageType)msg_.msg_type_, msg_.msg_attach_))
				; // do nothing
			else
			{
				std::wstring nick_name = UserService::GetInstance()->GetUserName(msg_.msg_sender_accid_);
				if (!nick_name.empty())
				{
					show_text = nick_name + L": " + show_text;
				}
			}
		}

		if (msg_.msg_status_ == nim::kNIMMsgLogStatusSendFailed)
		{
			show_text = L"[失败]" + show_text;
		}
	}
	label_msg_->SetText(show_text);
}

void SessionItem::InitMsg(const nim::SessionData &msg)
{
	msg_ = msg;
	SetUTF8Name(msg_.id_);
	SetUTF8DataID(msg_.id_);

	InitUserProfile(); //设置用户名和头像
	UpdateMsgContent(); //更新消息内容
	UpdateUnread(); //刷新未读条数	

	//更新时间
	if (msg_.msg_timetag_ > 0 && msg_.msg_status_ != nim::kNIMMsgLogStatusDeleted)
	{
		std::wstring str = GetMessageTime(msg_.msg_timetag_, true);
		label_time_->SetText(str);
	}
	else
		label_time_->SetVisible(false);

	if (msg_.type_ == nim::kNIMSessionTypeTeam) // 需要先获得群里最近一条消息中所有人的昵称，再UpdateMsg
	{
		head_image_->SetMouseEnabled(false); //群头像不响应点击

		relate_ids.clear();
		relate_ids.insert(msg_.msg_sender_accid_);
		Json::Reader reader;
		Json::Value attach;
		if (reader.parse(msg_.msg_attach_, attach))
		{
			if (attach.isObject() && attach.isMember(nim::kNIMNotificationKeyData))
			{
				Json::Value data = attach[nim::kNIMNotificationKeyData];
				if (data.isObject() && data.isMember(nim::kNIMNotificationKeyDataId))
					relate_ids.insert(data[nim::kNIMNotificationKeyDataId].asString());
				if (data.isObject() && data.isMember(nim::kNIMNotificationKeyDataIds) && data[nim::kNIMNotificationKeyDataIds].isArray())
				{
					Json::Value ids_json = data[nim::kNIMNotificationKeyDataIds];
					if (ids_json.isArray())
					{
						for (uint32_t i = 0; i < ids_json.size(); i++)
							relate_ids.insert(ids_json[i].asString());
					}
				}
			}
		}

		if (!relate_ids.empty())
		{
			std::list<std::string> uids(relate_ids.cbegin(), relate_ids.cend());
			std::list<nim::UserNameCard> uinfos;
			UserService::GetInstance()->GetUserInfos(uids, uinfos);
		}
	}
}

void SessionItem::ClearMsg()
{
	label_msg_->SetText(L"");
	label_time_->SetVisible(false);
	ResetUnread();
}

bool SessionItem::OnDbClicked(ui::EventArgs* arg)
{
	if(msg_.unread_count_ > 0)
	{
		ResetUnread();
	}
	SessionManager::GetInstance()->OpenSessionForm(msg_.id_, msg_.type_);
	return true;
}

bool SessionItem::OnSessionItemMenu( ui::EventArgs* arg )
{
	POINT point;
	::GetCursorPos(&point);
	PopupSessionItemMenu(point);
	return true;
}

bool SessionItem::OnHeadImageClicked(ui::EventArgs * arg)
{
	ProfileForm::ShowProfileForm(msg_.id_);
	return true;
}

void SessionItem::PopupSessionItemMenu( POINT point )
{
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"session_item_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	CMenuElementUI* del_session_item = (CMenuElementUI*) pMenu->FindControl(L"del_session_item");
	del_session_item->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* del_session_msg = (CMenuElementUI*) pMenu->FindControl(L"del_session_msg");
	del_session_msg->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

	pMenu->Show();
}

bool SessionItem::DelSessionItemMenuItemClick( ui::EventArgs* param )
{
	std::wstring name = param->pSender->GetName();
	if(name == L"del_session_item")
	{
		nim::Session::DeleteRecentSession(msg_.type_, msg_.id_, nbase::Bind(&SessionItem::DeleteRecentSessionCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		m_pWindow->SendNotify(this, ui::kEventNotify, SET_DELETE, 0);
	}
	else if(name == L"del_session_msg")
	{
		nim::MsgLog::BatchStatusDeleteAsync(msg_.id_, msg_.type_, nbase::Bind(&SessionItem::BatchStatusDeleteCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	return true;
}

int SessionItem::GetUnread()
{
	return msg_.unread_count_;
}

void SessionItem::SetUnread(int unread)
{
	msg_.unread_count_ = unread;
	UpdateUnread();
}

void SessionItem::AddUnread()
{
	msg_.unread_count_++;
	UpdateUnread();
}

void SessionItem::ResetUnread()
{
	if(msg_.unread_count_ > 0)
	{
		msg_.unread_count_ = 0;
		UpdateUnread();

		InvokeResetUnread(msg_.id_, msg_.type_);
	}
}

void SessionItem::UpdateUnread()
{
	if (msg_.unread_count_ > 0)
	{
		if (msg_.unread_count_ < 100) {
			label_unread_->SetText(nbase::StringPrintf(L"%d", msg_.unread_count_));
		}
		else {
			label_unread_->SetText(L"99+");
		}
		box_unread_->SetVisible(true);
	}
	else
	{
		box_unread_->SetVisible(false);
	}
}

void SessionItem::DeleteRecentSessionCb(nim::NIMResCode code, const nim::SessionData &result, int total_unread_counts)
{
	QLOG_APP(L"delete recent session, code={0} command={1} total_un_cn={2}") <<code <<result.command_ <<total_unread_counts;
}

void SessionItem::BatchStatusDeleteCb(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)
{
	QLOG_APP(L"batch delete msg, id={0} type={1} code={2}") << uid << to_type << res_code;
}

void InvokeResetUnread(const std::string &id, nim::NIMSessionType type)
{
	nim::Session::SetUnreadCountZeroAsync(type, id, nim::Session::SetUnreadCountZeroCallback());
	nim::MsgLog::BatchStatusReadAsync(id, type, nim::MsgLog::BatchStatusReadCallback());
}
}