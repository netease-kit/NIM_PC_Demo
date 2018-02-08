#include "session_item.h"

#include "module/session/session_manager.h"
#include "module/session/force_push_manager.h"
#include "callback/session/session_callback.h"
#include "gui/profile_form/profile_form.h"
#include "gui/session/session_box.h"
#include "gui/session/session_form.h"

#include "shared/ui/ui_menu.h"
#include "shared/pin_yin_helper.h"

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
	label_online_state_ = (Label*) this->FindSubControl(L"online_state");
	label_atme_ = (Label*) this->FindSubControl(L"at_me_label");
	label_time_ = (Label*) this->FindSubControl(L"label_time");
	box_unread_ = (Box*) this->FindSubControl(L"box_unread");
	label_unread_ = (Label*) this->FindSubControl(L"label_unread");
	head_image_ = (ButtonBox*)this->FindSubControl(L"head_image");
}

void SessionItem::InitMsg(const nim::SessionData &msg)
{
	msg_ = msg;
	head_image_->AttachClick(nbase::Bind(&SessionItem::OnHeadImageClicked, this, msg_.is_robot_session_, std::placeholders::_1));

	SetUTF8Name(msg_.id_);
	SetUTF8DataID(msg_.id_);

	if (msg.is_robot_session_)
		InitRobotProfile();
	else
		InitUserProfile(); //设置用户名和头像
	UpdateMsgContent(); //更新消息内容
	UpdateUnread(); //刷新未读条数	
	ShowAtmeTip(true);

	//更新时间
	if (msg_.msg_timetag_ > 0 && msg_.msg_status_ != nim::kNIMMsgLogStatusDeleted)
	{
		std::wstring str = GetMessageTime(msg_.msg_timetag_, true);
		label_time_->SetText(str);
		label_time_->SetVisible(true);
	}
	else
		label_time_->SetVisible(false);

	if (msg_.type_ == nim::kNIMSessionTypeTeam) // 需要先获得群里最近一条消息中所有人的昵称，再UpdateMsg
	{
		//head_image_->SetMouseEnabled(false); //群头像不响应点击

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
				if (data.isObject() && data.isMember(nim::kNIMNotificationKeyUserNameCards) && data[nim::kNIMNotificationKeyUserNameCards].isArray())
				{
					Json::Value name_cards_json = data[nim::kNIMNotificationKeyUserNameCards];
					for (uint32_t i = 0; i < name_cards_json.size(); i++)
						relate_ids.insert(name_cards_json[i][nim::kNIMNameCardKeyAccid].asString());
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
	else if (SubscribeEventManager::GetInstance()->IsEnabled())
	{
		EventDataEx data;
		if (!msg_.is_robot_session_)
			SubscribeEventManager::GetInstance()->GetEventData(nim::kNIMEventTypeOnlineState, msg.id_, data);
		else
			data.online_client_.online_client_type_.insert(nim::kNIMClientTypeDefault);
		SetOnlineState(data);
	}

	if (msg_.type_ == nim::kNIMSessionTypeP2P)
	{
		SetMute(nim_comp::MuteBlackService::GetInstance()->IsInMuteList(msg_.id_));
	}
	else
	{
		SetMute(nim_comp::SessionManager::GetInstance()->IsTeamMsgMuteShown(msg_.id_, -1));
	}
}

void SessionItem::InitRobotProfile()
{
	nim::RobotInfo info;
	UserService::GetInstance()->GetRobotInfo(msg_.id_, info);
	label_name_->SetText(nbase::UTF8ToUTF16(info.GetName()));
	head_image_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(msg_.id_, true));
}

void SessionItem::SetMute(bool mute)
{
	FindSubControl(L"not_disturb")->SetVisible(mute);
}

void SessionItem::InitUserProfile()
{
	if (msg_.type_ == nim::kNIMSessionTypeP2P)
	{
		if (LoginManager::GetInstance()->IsEqual(msg_.id_))
		{
			label_name_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MY_MOBILEPHONE"));
		}
		else
		{
			label_name_->SetText(UserService::GetInstance()->GetUserName(msg_.id_));
		}
		head_image_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(msg_.id_));
	}
	else
	{
		std::wstring tname = TeamService::GetInstance()->GetTeamName(msg_.id_);
		label_name_->SetText(tname);
		head_image_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(msg_.id_, false));
	}
}

void SessionItem::SetOnlineState(const EventDataEx& data)
{
	label_online_state_->SetText(OnlineStateEventHelper::GetOnlineState(data.online_client_, data.multi_config_, true));
}

void GetMsgContent(const nim::SessionData &msg, std::wstring &show_text)
{
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (msg.msg_type_ == nim::kNIMMessageTypeText)
	{
		show_text = nbase::UTF8ToUTF16(msg.msg_content_);
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeImage)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_IMAGE");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeAudio)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_AUDIO");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeVideo)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_VIDEO");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeFile)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_FILE");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeLocation)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_LOCATION");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeNotification)
	{
		GetNotifyMsg(msg.msg_attach_, msg.msg_sender_accid_, "", show_text, msg.id_);
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeCustom)
	{
		show_text = GetCustomMsg(msg.msg_sender_accid_, msg.msg_attach_);
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeTips)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_NOTIFY");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeUnknown)
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_UNDEFINED");
	}
	else if (msg.msg_type_ == nim::kNIMMessageTypeRobot)
	{
		bool out_msg = false;
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(msg.msg_attach_, values) && values.isObject())
			out_msg = values[nim::kNIMBotRobotReceivedMsgKeyMsgOut].asBool();
		if (!out_msg)
			show_text = nbase::UTF8ToUTF16(msg.msg_content_);
		else
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_ROBOT");
	}
	else
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_UNKNOWN");
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

		bool need_prefix = true;
		if (msg_.msg_type_ == nim::kNIMMessageTypeText)
		{
			Json::Value values;
			Json::Reader reader;
			if (reader.parse(msg_.msg_attach_, values)
				&& values.isObject()
				&& values.isMember("comment")
				&& values["comment"].asString() == "is_recall_notification")
			{
				if (values.isMember("notify_from"))
				{
					need_prefix = false;
					std::string from_id = values["notify_from"].asString();
					std::string from_nick = values["from_nick"].asString();
					std::string operator_id = values["operator_id"].asString();
					if (operator_id.empty())
						operator_id = from_id;
					show_text = GetRecallNotifyTextEx(msg_.id_, msg_.type_, from_id, operator_id,from_nick);
				}
			}
		}

		if (need_prefix && msg_.type_ == nim::kNIMSessionTypeTeam)
		{
			if (msg_.msg_type_ == nim::kNIMMessageTypeNotification && !IsNetCallMsg((nim::NIMMessageType)msg_.msg_type_, msg_.msg_attach_))
				; // do nothing
			else
			{
				if (msg_.msg_type_ != nim::kNIMMessageTypeRobot)
				{
					std::wstring nick_name = UserService::GetInstance()->GetUserName(msg_.msg_sender_accid_);
					if (!nick_name.empty())
					{
						show_text = nick_name + L": " + show_text;
					}
				}
				else
				{
					bool out_msg = false;
					Json::Value values;
					Json::Reader reader;
					if (reader.parse(msg_.msg_attach_, values) && values.isObject())
						out_msg = values[nim::kNIMBotRobotReceivedMsgKeyMsgOut].asBool();
					if (out_msg)
					{
						std::string robot_id = values[nim::kNIMBotRobotMsgKeyRobotID].asString();
						nim::RobotInfo info;
						UserService::GetInstance()->GetRobotInfo(robot_id, info);
						if (!info.GetName().empty())
						{
							show_text = nbase::UTF8ToUTF16(info.GetName()) + L": " + show_text;
						}
					}
				}
			}
		}

		if (msg_.msg_status_ == nim::kNIMMsgLogStatusSendFailed)
		{
			show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_FAILED") + show_text;
		}
	}
	label_msg_->SetText(show_text);
}

void SessionItem::ClearMsg()
{
	label_msg_->SetText(L"");
	label_time_->SetVisible(false);
	ResetUnread();
}

long long SessionItem::GetMsgTime()
{
	return msg_.msg_timetag_;
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

	ShowAtmeTip(true);
}

void SessionItem::ResetUnread()
{
	if (msg_.unread_count_ > 0)
	{
		msg_.unread_count_ = 0;
		UpdateUnread();
		ShowAtmeTip(false);

		InvokeResetUnread(msg_.id_, msg_.type_);
	}
}

void SessionItem::DeleteRecentSessionCb(nim::NIMResCode code, const nim::SessionData &result, int total_unread_counts)
{
	QLOG_APP(L"delete recent session, code={0} command={1} total_un_cn={2}") << code << result.command_ << total_unread_counts;
}

void SessionItem::BatchStatusDeleteCb(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)
{
	QLOG_APP(L"batch delete msg, id={0} type={1} code={2}") << uid << to_type << res_code;
	//if (res_code == nim::kNIMResSuccess)
	//	nim::Session::SetUnreadCountZeroAsync(to_type, uid, nim::Session::SetUnreadCountZeroCallback());
}

void SessionItem::ShowAtmeTip(bool show)
{
	if (show)
	{
		label_atme_->SetVisible((ForcePushManager::GetInstance()->IsContainAtMeMsg(msg_.id_)));
	}
	else
	{
		label_atme_->SetVisible(false);
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

	if (msg_.unread_count_ == 0)
		// 重置对应会话中的@me消息为已读
		ForcePushManager::GetInstance()->ResetAtMeMsg(msg_.id_);

	// 通知会话窗口中的会话合并项
	SessionBox *session_box = SessionManager::GetInstance()->FindSessionBox(msg_.id_);
	if (session_box)
		session_box->GetSessionForm()->InvokeSetSessionUnread(msg_.id_, msg_.unread_count_);
}

void SessionItem::PopupSessionItemMenu(POINT point)
{
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"session_item_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	CMenuElementUI* del_session_item = (CMenuElementUI*)pMenu->FindControl(L"del_session_item");
	del_session_item->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* del_session_msg = (CMenuElementUI*)pMenu->FindControl(L"del_session_msg");
	del_session_msg->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

	pMenu->Show();
}

bool SessionItem::DelSessionItemMenuItemClick(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"del_session_item")
	{
		nim::Session::DeleteRecentSession(msg_.type_, msg_.id_, nbase::Bind(&SessionItem::DeleteRecentSessionCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(msg_);

		m_pWindow->SendNotify(this, ui::kEventNotify, SET_DELETE, 0);
	}
	else if (name == L"del_session_msg")
	{
		nim::Session::SetUnreadCountZeroAsync(msg_.type_, msg_.id_, ToWeakCallback([this](nim::NIMResCode res_code, const nim::SessionData&, int){
			if (res_code == nim::kNIMResSuccess)
			nim::MsgLog::BatchStatusDeleteAsync(msg_.id_, msg_.type_, nbase::Bind(&SessionItem::BatchStatusDeleteCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		}));		
	}
	return true;
}

bool SessionItem::OnDbClicked(ui::EventArgs* arg)
{
	SessionManager::GetInstance()->OpenSessionBox(msg_.id_, msg_.type_);
	return true;
}

bool SessionItem::OnSessionItemMenu(ui::EventArgs* arg)
{
	POINT point;
	::GetCursorPos(&point);
	PopupSessionItemMenu(point);
	return true;
}

bool SessionItem::OnHeadImageClicked(bool is_robot, ui::EventArgs * arg)
{
	if (msg_.type_ == nim::kNIMSessionTypeTeam)
	{
		auto team_info = nim::Team::QueryTeamInfoBlock(msg_.id_);
		TeamInfoForm::ShowTeamInfoForm(false, team_info.GetType(), msg_.id_, team_info);
	}
	else if (msg_.type_ == nim::kNIMSessionTypeP2P)
	{
		ProfileForm::ShowProfileForm(msg_.id_, is_robot);
	}
	return true;
}

void InvokeResetUnread(const std::string &id, nim::NIMSessionType type)
{
	nim::Session::SetUnreadCountZeroAsync(type, id, nim::Session::SetUnreadCountZeroCallback());
	nim::MsgLog::BatchStatusReadAsync(id, type, nim::MsgLog::BatchStatusReadCallback());
}
}