#include "chatroom_form.h"
#include "chatroom_frontpage.h"
#include "nim_chatroom_helper.h"
#include "module/session/session_util.h"

#define ROOMMSG_R_N _T("\r\n")
namespace nim_chatroom
{

using namespace ui;


void ChatroomForm::RequestEnter(const __int64 room_id)
{
	if (room_id == 0)
	{
		RequestRoomError();
		return;
	}

	room_id_ = room_id;
	//获取聊天室登录信息
	nim::PluginIn::ChatRoomRequestEnterAsync(room_id_, nbase::Bind(&ChatroomForm::OnChatRoomRequestEnterCallback, this, std::placeholders::_1, std::placeholders::_2));
}

__int64 ChatroomForm::GetRoomId()
{
	return room_id_;
}

void ChatroomForm::OnReceiveMsgCallback(const ChatRoomMessage& result)
{
	if (result.msg_type_ == kNIMChatRoomMsgTypeNotification)
	{
		// 登录成功后，会收到一个“欢迎进入直播间”的通知消息，这个消息的时间戳作为获取历史消息的标准
		// 通知消息由OnNotificationCallback回调处理，这里不做处理
		if (time_start_history == 0)
			time_start_history = result.timetag_ - 1;
		if (time_start_history < 0)
			time_start_history = 0;
		return;
	}
		
	AddMsgItem(result, false);
}

void ChatroomForm::OnEnterCallback(int error_code, const ChatRoomInfo& info, const ChatRoomMemberInfo& my_info)
{
	if (error_code == nim::kNIMResTimeoutError)
	{
		ui::Box* kicked_tip_box = (ui::Box*)this->FindControl(L"kicked_tip_box");
		if (kicked_tip_box->IsVisible())
			return;
		kicked_tip_box->SetVisible(true);
		std::wstring kick_tip_str;
		ui::MutiLanSupport *multilan = ui::MutiLanSupport::GetInstance();
		kick_tip_str = L"登录中";
		ui::Label* kick_tip_label = (ui::Label*)kicked_tip_box->FindSubControl(L"kick_tip");
		kick_tip_label->SetText(kick_tip_str);

// 		ui::Label* room_name_label = (ui::Label*)kicked_tip_box->FindSubControl(L"room_name");
// 		room_name_label->SetDataID(nbase::Int64ToString16(info.id_));
// 		if (!info.name_.empty())
// 			room_name_label->SetUTF8Text(info.name_);
// 		else
// 			room_name_label->SetText(nbase::StringPrintf(L"直播间(id %lld)", info.id_));

		//Close();
		return;
	}
	else if (error_code == nim::kNIMResSuccess)
	{
		ui::Box* kicked_tip_box = (ui::Box*)this->FindControl(L"kicked_tip_box");
		kicked_tip_box->SetVisible(false);
	}

	if (info.id_ == 0)
	{
		return;
	}

	room_id_ = info.id_;
	has_enter_ = true;

	StdClosure task = [=](){
		if (!my_info.avatar_.empty() && my_info.avatar_.find_first_of("http") == 0)
			header_icon_->SetBkImage(nim_ui::HttpManager::GetInstance()->GetCustomImage(kChatroomMemberIcon, my_info.account_id_, my_info.avatar_));
		else
			header_icon_->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(my_info.account_id_));

		std::wstring name = my_info.nick_.empty() ? nim_ui::UserManager::GetInstance()->GetUserName(my_info.account_id_, false) : nbase::UTF8ToUTF16(my_info.nick_);
		name_->SetText(name);

	};
	Post2UI(task);

	OnGetChatRoomInfoCallback(room_id_, error_code, info);
	GetMembers();
}

void ChatroomForm::OnGetChatRoomInfoCallback(__int64 room_id, int error_code, const ChatRoomInfo& info)
{
	if (error_code != nim::kNIMResSuccess || room_id != room_id_)
	{
		return;
	}

	StdClosure cb = [=](){
		time_refresh_ = time(NULL);

		ASSERT(!info.creator_id_.empty());
		creater_id_ = info.creator_id_;

		host_icon_->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(info.creator_id_));
		host_name_->SetText(std::wstring(L"主播：") + nim_ui::UserManager::GetInstance()->GetUserName(info.creator_id_, false));

		std::wstring room_name = nbase::UTF8ToUTF16(info.name_);
		if (room_name.length() > 15)
			room_name = room_name.substr(0, 15) + L"...";
		room_name_->SetText(room_name);

		if (info.online_count_ >= 10000)
			online_num_->SetText(nbase::StringPrintf(L"在线人数：%.1f万人", (float)info.online_count_ / (float)10000));
		else
			online_num_->SetText(nbase::StringPrintf(L"在线人数：%d人", info.online_count_));

		if (!info.announcement_.empty())
		{
			bulletin_ = ((ui::RichEdit*)FindControl(L"bulletin"));
			bulletin_->SetUTF8Text(info.announcement_);
			bulletin_->SetBkImage(L"");
			if (nim_ui::LoginManager::GetInstance()->GetAccount() == info.creator_id_)
			{
				bulletin_->SetReadOnly(false);
			}
		}

		room_mute_ = info.mute_all_ == 1;
	};
	Post2UI(cb);
}

void ChatroomForm::OnNotificationCallback(const ChatRoomNotification& notification)
{	
	AddNotifyItem(notification, false);
}

void ChatroomForm::GetMembers()
{
	ChatRoomGetMembersParameters member_param;
	member_param.type_ = kNIMChatRoomGetMemberTypeSolid;
	ChatRoom::GetMembersOnlineAsync(room_id_, member_param, nbase::Bind(&ChatroomForm::OnGetMembersCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	member_param.type_ = kNIMChatRoomGetMemberTypeTemp;
	ChatRoom::GetMembersOnlineAsync(room_id_, member_param, nbase::Bind(&ChatroomForm::OnGetMembersCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void ChatroomForm::OnGetMembersCallback(__int64 room_id, int error_code, const std::list<ChatRoomMemberInfo>& infos)
{
	if (error_code != nim::kNIMResSuccess || room_id_ != room_id)
		return;

	StdClosure cb = [=](){
		empty_members_list_->SetVisible(false);

		for (auto& it : infos)
		{
			// 不加入离线的游客
			if (it.guest_flag_ == kNIMChatRoomGuestFlagGuest && it.state_ == kNIMChatRoomOnlineStateOffline)
				continue;

			if (NULL != online_members_list_->FindSubControl(nbase::UTF8ToUTF16(it.account_id_)))
				continue;

			members_list_[it.account_id_] = it;
			nick_account_map_[it.nick_] = it.account_id_;

			ui::ButtonBox* room_member_item = (ui::ButtonBox*)ui::GlobalManager::CreateBoxWithCache(L"chatroom/room_member_item.xml");
			if (room_member_item)
			{
// #ifdef _DEBUG
				room_member_item->AttachMenu(nbase::Bind(&ChatroomForm::OnMemberMenu, this, std::placeholders::_1));
// #endif
				ui::Control* member_type = (ui::Control*)room_member_item->FindSubControl(L"member_type");
				ui::Control* header_image = (ui::Control*)room_member_item->FindSubControl(L"header_image");
				ui::Label* name = (ui::Label*)room_member_item->FindSubControl(L"name");

				if (it.type_ == 1)//创建者
				{
					if (!it.avatar_.empty() && it.avatar_.find_first_of("http") == 0)
						host_icon_->SetBkImage(nim_ui::HttpManager::GetInstance()->GetCustomImage(kChatroomMemberIcon, it.account_id_, it.avatar_));
					else
						host_icon_->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(it.account_id_));
					host_name_->SetText(std::wstring(L"主播：") + nbase::UTF8ToUTF16(it.nick_));
					member_type->SetBkImage(L"icon_anchor.png");
					online_members_list_->AddAt(room_member_item, 0);
					has_add_creater_ = true;
				}
				else if (it.type_ == 2)//管理员
				{
					member_type->SetBkImage(L"icon_manager.png");
					managers_list_[it.account_id_] = it;
					if (online_members_list_->GetCount() == 0)
					{
						online_members_list_->Add(room_member_item);
					}
					else
					{
						if (has_add_creater_)
							online_members_list_->AddAt(room_member_item, 1);
						else
							online_members_list_->AddAt(room_member_item, 0);
					}
				}
				else
				{
					online_members_list_->Add(room_member_item);
				}

				name->SetUTF8Text(it.nick_);
				if (!it.avatar_.empty() && it.avatar_.find_first_of("http") == 0)
					header_image->SetBkImage(nim_ui::HttpManager::GetInstance()->GetCustomImage(kChatroomMemberIcon, it.account_id_, it.avatar_));
				else
					header_image->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(it.account_id_));

				room_member_item->SetUTF8Name(it.account_id_);
			}
		}
	};
	Post2UI(cb);
}

void ChatroomForm::GetHistorys()
{
	ChatRoomGetMsgHistoryParameters history_param;
	history_param.limit_ = 10;
	history_param.start_timetag_ = time_start_history;
	ChatRoom::GetMessageHistoryOnlineAsync(room_id_, history_param, nbase::Bind(&ChatroomForm::GetMsgHistoryCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void ChatroomForm::GetMsgHistoryCallback(__int64 room_id, int error_code, const std::list<ChatRoomMessage>& msgs)
{
	if (error_code != nim::kNIMResSuccess || room_id_ != room_id)
		return;

	StdClosure cb = [=](){
		is_loading_history_ = false;
		for (auto it = msgs.begin(); it != msgs.end(); it++)
		{
			AddMsgItem(*it, true);
			time_start_history = it->timetag_ - 1;
			if (time_start_history < 0)
				time_start_history = 0;
		}
	};
	Post2UI(cb);

}

void ChatroomForm::SetMemberAttributeCallback(__int64 room_id, int error_code, const ChatRoomMemberInfo& info)
{
	if (error_code != nim::kNIMResSuccess || room_id_ != room_id)
		return;

	StdClosure cb = [=](){
		auto it = members_list_.find(info.account_id_);
		if (it != members_list_.end())
		{
			if (it->second.type_ == 2 && info.type_ != 2)
			{
				SetMemberAdmin(info.account_id_, false);
			}
			else if (it->second.type_ != 2 && info.type_ == 2)
			{
				SetMemberAdmin(info.account_id_, true);
			}
		}
		else
		{
			if (info.type_ != 2)
			{
				SetMemberAdmin(info.account_id_, false);
			}
			else if (info.type_ == 2)
			{
				SetMemberAdmin(info.account_id_, true);
			}
		}
		members_list_[info.account_id_] = info;
	};
	Post2UI(cb);

}

void ChatroomForm::TempMuteCallback(__int64 room_id, int error_code, const ChatRoomMemberInfo& info)
{
	if (error_code != nim::kNIMResSuccess || room_id_ != room_id)
		return;

	StdClosure cb = [=](){
		SetMemberTempMute(info.account_id_, info.temp_muted_, info.temp_muted_ ? info.temp_muted_duration_ : 0);
	};
	Post2UI(cb);
}

void ChatroomForm::KickMemberCallback(__int64 room_id, int error_code)
{
	if (error_code != nim::kNIMResSuccess || room_id_ != room_id)
		return;

	StdClosure cb = [=](){
		RemoveMember(kicked_user_account_);
		kicked_user_account_.clear();
	};
	Post2UI(cb);

}

void ChatroomForm::OnChatRoomRequestEnterCallback(int error_code, const std::string& result)
{
	StdClosure closure_err = ToWeakCallback([this, error_code]()
	{
		RequestRoomError();
	});
	if (error_code != nim::kNIMResSuccess)
	{
		if (error_code == nim::kNIMResForbidden 
			|| error_code == nim::kNIMResNotExist
			|| error_code == nim::kNIMLocalResAPIErrorInitUndone
			|| error_code == nim::kNIMLocalResAPIErrorLoginUndone
			|| error_code == nim::kNIMResTimeoutError)
		{
			StdClosure closure = ToWeakCallback([this, error_code]()
			{
				ChatroomFrontpage* front_page = nim_ui::WindowsManager::GetInstance()->SingletonShow<ChatroomFrontpage>(ChatroomFrontpage::kClassName);
				if (!front_page) return;
				ui::Box* kicked_tip_box = (ui::Box*)front_page->FindControl(L"kicked_tip_box");
				kicked_tip_box->SetVisible(true);
				nbase::ThreadManager::PostDelayedTask(front_page->ToWeakCallback([kicked_tip_box]() {
					kicked_tip_box->SetVisible(false);
				}), nbase::TimeDelta::FromSeconds(2));

				std::wstring kick_tip_str = L"进入直播间失败";
				if (error_code == nim::kNIMResForbidden)
					kick_tip_str = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_CHATROOM_TIP_BLACKLISTED");
				else if (error_code == nim::kNIMResNotExist)
					kick_tip_str = L"直播间不存在";
				else if (error_code == nim::kNIMLocalResAPIErrorInitUndone 
					|| error_code == nim::kNIMLocalResAPIErrorLoginUndone
					|| error_code == nim::kNIMResTimeoutError)
					kick_tip_str = L"网络异常，请稍后尝试";
				ui::Label* kick_tip_label = (ui::Label*)kicked_tip_box->FindSubControl(L"kick_tip");
				kick_tip_label->SetText(kick_tip_str);

				ui::Label* room_name_label = (ui::Label*)kicked_tip_box->FindSubControl(L"room_name");
				if (error_code == nim::kNIMResForbidden)
				{
					room_name_label->SetDataID(nbase::Int64ToString16(room_id_));
					ChatRoomInfo info = front_page->GetRoomInfo(room_id_);
					if (!info.name_.empty())
						room_name_label->SetUTF8Text(info.name_);
					else
						room_name_label->SetText(nbase::StringPrintf(L"直播间(id %lld)", room_id_));
				}
				else if (error_code == nim::kNIMResNotExist)
					room_name_label->SetText(L"");
				RequestRoomError();
			});
			Post2UI(closure);
		}
		else
		{
			Post2UI(closure_err);
			QLOG_APP(L"OnChatRoomRequestEnterCallback error: error id={0}") << error_code;
		}
		return;
	}
	
	if (!result.empty())
	{
		StdClosure cb = [result, this](){
			room_enter_token_ = result;
			ChatRoomEnterInfo info;
			//Json::Value values;
			//Json::Reader reader;
			//std::string test_string = "{\"remote\":{\"mapmap\":{\"int\":1,\"boolean\":false,\"list\":[1,2,3],\"string\":\"string, lalala\"}}}";
			//if (reader.parse(test_string, values))
			//	info.SetExt(values);
			//info.SetNotifyExt(values);
			bool bRet = ChatRoom::Enter(room_id_, room_enter_token_, info);
			if (bRet)
			{
				this->CenterWindow();
				this->ShowWindow();
			}
			else
			{
				QLOG_APP(L"ChatRoom::Enter error");

				ChatroomFrontpage* front_page = (ChatroomFrontpage*)nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomFrontpage::kClassName, ChatroomFrontpage::kClassName);
				if (!front_page) return;
				ShowMsgBox(front_page->GetHWND(), L"进入直播间失败，请重试", nullptr, L"提示", L"确定", L"");

				RequestRoomError();
			}
		};
		Post2UI(cb);
	}
	else
	{
		Post2UI(closure_err);
	}

}

void ChatroomForm::OnRegLinkConditionCallback(__int64 room_id, const NIMChatRoomLinkCondition condition)
{
	if (room_id_ != room_id)
		return;

	if (condition == kNIMChatRoomLinkConditionAlive && has_enter_)
	{
		GetHistorys();
	}
	else if (condition == kNIMChatRoomLinkConditionDead)
	{
		msg_list_->SetText(L"");
		input_edit_->SetText(L"");
	}
}

void ChatroomForm::AddMsgItem(const ChatRoomMessage& result, bool is_history)
{
	if (result.msg_type_ == kNIMChatRoomMsgTypeText)
	{
		AddText(nbase::UTF8ToUTF16(result.msg_attach_), nbase::UTF8ToUTF16(result.from_nick_), is_history);
	}
	else if (result.msg_type_ == kNIMChatRoomMsgTypeNotification)
	{
		Json::Value json;
		Json::Reader reader;
		if (reader.parse(result.msg_attach_, json))
		{
			ChatRoomNotification notification;
			notification.ParseFromJsonValue(json);
			AddNotifyItem(notification, is_history);
		}
	}
	else if (result.msg_type_ == kNIMChatRoomMsgTypeCustom)
	{
		Json::Value json;
		Json::Reader reader;
		if (reader.parse(result.msg_attach_, json))
		{
			int sub_type = json["type"].asInt();
			if (sub_type == nim_comp::CustomMsgType_Jsb && json["data"].isObject())
			{
				int value = json["data"]["value"].asInt();
				AddJsb(value, nbase::UTF8ToUTF16(result.from_nick_), is_history);
			}
		}
	}

	if (!result.from_nick_.empty())
		nick_account_map_[result.from_nick_] = result.from_id_;
}

void ChatroomForm::AddNotifyItem(const ChatRoomNotification& notification, bool is_history)
{
	std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();

	auto it_nick = notification.target_nick_.cbegin();
	auto it_id = notification.target_ids_.cbegin();
	for (;	it_nick != notification.target_nick_.cend(), it_id != notification.target_ids_.cend();
		++it_id, ++it_nick)
	{
		std::wstring nick = nbase::UTF8ToUTF16(*it_nick);
		if (*it_id == my_id)
			nick = L"你";

		std::wstring str;
		if (notification.id_ == kNIMChatRoomNotificationIdMemberIn)
		{
			str = nbase::StringPrintf(L"欢迎%s进入直播间", nick.c_str());
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdMemberExit)
		{
			str = nbase::StringPrintf(L"%s退出直播间", nick.c_str());
			RemoveMember(*it_id);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdAddBlack)
		{
			str = nbase::StringPrintf(L"%s被管理员拉入黑名单", nick.c_str());
			SetMemberBlacklist(*it_id, true);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdRemoveBlack)
		{
			str = nbase::StringPrintf(L"%s被管理员解除拉黑", nick.c_str());
			SetMemberBlacklist(*it_id, false);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdAddMute)
		{
			str = nbase::StringPrintf(L"%s被管理员禁言", nick.c_str());
			SetMemberMute(*it_id, true);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdRemoveMute)
		{
			str = nbase::StringPrintf(L"%s被管理员解除禁言", nick.c_str());
			SetMemberMute(*it_id, false);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdAddManager)
		{
			str = nbase::StringPrintf(L"%s被任命管理员身份", nick.c_str());
			SetMemberAdmin(*it_id, true);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdRemoveManager)
		{
			str = nbase::StringPrintf(L"%s被解除管理员身份", nick.c_str());
			SetMemberAdmin(*it_id, false);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdAddFixed)
		{
			SetMemberFixed(*it_id, true);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdRemoveFixed)
		{
			SetMemberFixed(*it_id, false);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdMemberKicked)
		{
			str = nbase::StringPrintf(L"%s被管理员移出直播间", nick.c_str());
			RemoveMember(*it_id);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdMemberTempMute)
		{
			str = nbase::StringPrintf(L"%s被管理员临时禁言%lld秒", nick.c_str(), notification.temp_mute_duration_);
			SetMemberTempMute(*it_id, true, notification.temp_mute_duration_);
		}
		else if (notification.id_ == kNIMChatRoomNotificationIdMemberTempUnMute)
		{
			str = nbase::StringPrintf(L"%s被管理员解除临时禁言，此时离自动解除还有%lld秒", nick.c_str(), notification.temp_mute_duration_);
			SetMemberTempMute(*it_id, false, notification.temp_mute_duration_);
		}
		AddNotify(str, is_history);
	}

	if (notification.id_ == kNIMChatRoomNotificationIdRoomMuted || notification.id_ == kNIMChatRoomNotificationIdRoomDeMuted)
	{
		std::wstring str;
		str = nbase::StringPrintf(L"聊天室%s", notification.id_ == kNIMChatRoomNotificationIdRoomMuted ? L"全员禁言" : L"解除全员禁言");
		room_mute_ = notification.id_ == kNIMChatRoomNotificationIdRoomMuted;
		AddNotify(str, is_history);
	}

	if (!notification.ext_.empty())
	{
		std::string toast = nbase::StringPrintf("notification_id:%d, from_nick:%s(%s), notify_ext:%s", notification.id_, notification.operator_nick_.c_str(), notification.operator_id_.c_str(), notification.ext_.c_str());
		nim_ui::ShowToast(nbase::UTF8ToUTF16(toast), 5000, this->GetHWND());
	}
}

void ChatroomForm::OnBtnSend()
{
	if (!nim_ui::LoginManager::GetInstance()->IsLinkActive())
	{
		msg_list_->SetText(L"");
		return;
	}

	if (room_mute_)
	{
		std::wstring toast = nbase::StringPrintf(L"聊天室全员禁言中。");
		nim_ui::ShowToast(toast, 5000, this->GetHWND());
	}
	else
	{
		auto my_info = members_list_.find(nim_ui::LoginManager::GetInstance()->GetAccount());
		if (my_info != members_list_.end())
		{
			if (my_info->second.is_muted_)
			{
				std::wstring toast = nbase::StringPrintf(L"你被禁言了。");
				nim_ui::ShowToast(toast, 5000, this->GetHWND());
			}
			else if (my_info->second.temp_muted_)
			{
				std::wstring toast = nbase::StringPrintf(L"你被临时禁言了。");
				nim_ui::ShowToast(toast, 5000, this->GetHWND());
			}
			else
			{
				wstring sText;
				nim_comp::Re_GetText(input_edit_->GetTextServices(), sText);
				StringHelper::Trim(sText);
				if (sText.empty()) return;
				input_edit_->SetText(_T(""));
				SendText(nbase::UTF16ToUTF8(sText));

				std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
				AddText(sText, my_name, false);
			}
		}
	}
}

void ChatroomForm::AddText(const std::wstring &text, const std::wstring &sender_name, bool is_history)
{
	if (text.empty()) return;
	// 
	if (msg_list_->GetTextLength() != 0)
	{
		if (is_history)
		{
			msg_list_->SetSel(0, 0);
			msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		}
		else
		{
			msg_list_->SetSel(-1, -1);
			msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		}
	}

	//
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	msg_list_->GetDefaultCharFormat(cf); //获取消息字体
	cf.dwMask = CFM_LINK | CFM_FACE | CFM_COLOR;
	cf.dwEffects = CFE_LINK;

	// 添加发言人，并设置他的颜色
	lSelEnd = lSelBegin = is_history ? 0 : msg_list_->GetTextLength();
	msg_list_->SetSel(lSelEnd, lSelEnd);
	msg_list_->ReplaceSel(sender_name, false);

	lSelEnd = is_history ? sender_name.length() : msg_list_->GetTextLength();
	msg_list_->SetSel(lSelBegin, lSelEnd);
	msg_list_->SetSelectionCharFormat(cf);

	// 设置文本的缩进
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT | PFM_LINESPACING;
	LONG lineSpace = (LONG)(1.2 * 20);//设置1.2行间距
	pf.dxStartIndent = 0;
	pf.bLineSpacingRule = 5;
	pf.dyLineSpacing = lineSpace;
	msg_list_->SetParaFormat(pf);
	msg_list_->SetSel(lSelEnd, lSelEnd);
	msg_list_->ReplaceSel(ROOMMSG_R_N, false);
	lSelEnd++;
	// 添加正文，并设置他的颜色	
	msg_list_->SetSel(lSelEnd, lSelEnd);
	nim_comp::InsertTextToEdit(msg_list_, text);
	//设置文本字体
	msg_list_->GetDefaultCharFormat(cf); //获取消息字体
	long lSelBegin2 = 0, lSelEnd2 = 0;
	msg_list_->GetSel(lSelBegin2, lSelEnd2);
	msg_list_->SetSel(lSelEnd, lSelEnd2);
	msg_list_->SetSelectionCharFormat(cf);

	// 设置正文文本的缩进
	msg_list_->SetSel(lSelEnd, lSelEnd);
	pf.dxStartIndent = 150;
	msg_list_->SetParaFormat(pf);

	if (!is_history)
		msg_list_->EndDown();
	input_edit_->SetFocus();
}

void ChatroomForm::AddNotify(const std::wstring &notify, bool is_history)
{
	if (notify.empty()) return;
	// 
	if (msg_list_->GetTextLength() != 0)
	{
		if (is_history)
		{
			msg_list_->SetSel(0, 0);
			msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		}
		else
		{
			msg_list_->SetSel(-1, -1);
			msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		}
	}

	//
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	msg_list_->GetDefaultCharFormat(cf); //获取消息字体
	cf.dwMask |= CFM_COLOR;
	cf.crTextColor = RGB(255, 0, 0);

	// 添加通知消息，并设置他的颜色
	lSelEnd = lSelBegin = is_history ? 0 : msg_list_->GetTextLength();
	msg_list_->SetSel(lSelEnd, lSelEnd);
	msg_list_->ReplaceSel(notify, false);

	lSelEnd = is_history ? notify.length() : msg_list_->GetTextLength();
	msg_list_->SetSel(lSelBegin, lSelEnd);
	msg_list_->SetSelectionCharFormat(cf);

	// 设置正文文本的缩进
	msg_list_->SetSel(lSelEnd, lSelEnd);
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = 150;
	msg_list_->SetParaFormat(pf);

	if (!is_history)
		msg_list_->EndDown();
}

void ChatroomForm::SendImage(const std::wstring &src)
{
	nim::IMImage img;
	std::string utf8 = nbase::UTF16ToUTF8(src);
	img.md5_ = nim::Tool::GetFileMd5(utf8);
	img.size_ = (long)nbase::GetFileSize(src);
	std::wstring file_name, file_ext;
	nbase::FilePathApartFileName(src, file_name);
	nbase::FilePathExtension(src, file_ext);
	img.display_name_ = nbase::UTF16ToUTF8(file_name);
	img.file_extension_ = nbase::UTF16ToUTF8(file_ext);
	Gdiplus::Image image(src.c_str());
	if (image.GetLastStatus() != Gdiplus::Ok)
	{
		assert(0);
	}
	else
	{
		img.width_ = image.GetWidth();
		img.height_ = image.GetHeight();
	}

// 	auto callback = nbase::Bind(&ChatroomForm::OnUploadImageCallback, this, std::placeholders::_1, std::placeholders::_2, img);
// 	nim::NOS::UploadResource(utf8, callback);

	auto weak_flag = this->GetWeakFlag();
	nim::NOS::UploadResource(utf8, [this, img, weak_flag](int res_code, const std::string& url) {
		if (!weak_flag.expired() && res_code == nim::kNIMResSuccess)
		{
			nim::IMImage new_img(img);
			new_img.url_ = url;
			std::string send_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeImage, QString::GetGUID(), new_img.ToJsonString(), ChatRoomMessageSetting());
			ChatRoom::SendMsg(room_id_, send_msg);
		}
	});
}

void ChatroomForm::OnUploadImageCallback(int res_code, const std::string& url, nim::IMImage img)
{
	std::string send_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeImage, QString::GetGUID(), img.ToJsonString(), ChatRoomMessageSetting());
	ChatRoom::SendMsg(room_id_, send_msg);
}

void ChatroomForm::SendText(const std::string &text)
{
	//std::string test_string = "{\"remote\":{\"mapmap\":{\"int\":1,\"boolean\":false,\"list\":[1,2,3],\"string\":\"string, lalala\"}}}";
	std::string send_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeText, QString::GetGUID(), text, ChatRoomMessageSetting());
	ChatRoom::SendMsg(room_id_, send_msg);
}

void ChatroomForm::OnBtnJsb()
{
	if (!nim_ui::LoginManager::GetInstance()->IsLinkActive())
	{
		msg_list_->SetText(L"");
		return;
	}

	auto my_info = members_list_.find(nim_ui::LoginManager::GetInstance()->GetAccount());
	if (my_info != members_list_.end() && !room_mute_ && !my_info->second.is_muted_ && !my_info->second.temp_muted_)
	{
		int jsb = (rand() % 3 + rand() % 4 + rand() % 5) % 3 + 1;

		Json::Value json;
		Json::FastWriter writer;
		json["type"] = nim_comp::CustomMsgType_Jsb;
		json["data"]["value"] = jsb;

		SendJsb(writer.write(json));
		std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
		AddJsb(jsb, my_name, false);
	}
}

void ChatroomForm::AddJsb(const int value, const std::wstring &sender_name, bool is_history)
{
	std::wstring file_name;
	if (value == 1)
		file_name = L"jsb_s.png";
	else if (value == 2)
		file_name = L"jsb_j.png";
	else if (value == 3)
		file_name = L"jsb_b.png";

	if (file_name.empty()) return;
	// 
	if (msg_list_->GetTextLength() != 0)
	{
		if (is_history)
		{
			msg_list_->SetSel(0, 0);
			msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		}
		else
		{
			msg_list_->SetSel(-1, -1);
			msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		}
	}

	//
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_LINK | CFM_FACE | CFM_COLOR;
	cf.dwEffects = CFE_LINK;

	// 添加发言人，并设置他的颜色
	lSelEnd = lSelBegin = is_history ? 0 : msg_list_->GetTextLength();
	msg_list_->SetSel(lSelEnd, lSelEnd);
	msg_list_->ReplaceSel(sender_name, false);

	lSelEnd = is_history ? sender_name.length() : msg_list_->GetTextLength();
	msg_list_->SetSel(lSelBegin, lSelEnd);
	msg_list_->SetSelectionCharFormat(cf);

	// 设置文本的缩进
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT | PFM_LINESPACING;
	LONG lineSpace = (LONG)(1.2 * 20);//设置1.2行间距
	pf.dxStartIndent = 0;
	pf.bLineSpacingRule = 5;
	pf.dyLineSpacing = lineSpace;
	msg_list_->SetParaFormat(pf);
	msg_list_->SetSel(lSelEnd, lSelEnd);
	msg_list_->ReplaceSel(ROOMMSG_R_N, false);
	lSelEnd++;
	// 添加正文，并设置他的颜色	
	msg_list_->SetSel(lSelEnd, lSelEnd);
	std::wstring file = QPath::GetAppPath();
	file.append(L"themes\\default\\session\\" + file_name);
	if (nbase::FilePathIsExist(file, false))
	{
		ITextServices* service = msg_list_->GetTextServices();
		if (!nim_comp::Re_InsertJsb(service, file, file_name))
		{
			QLOG_ERR(L"insert jsb {0} fail") << file;
		}
		service->Release();
	}
	else
	{
		QLOG_ERR(L"jsb {0} miss") << file;
	}

	// 设置正文文本的缩进
	msg_list_->SetSel(lSelEnd, lSelEnd);
	pf.dxStartIndent = 150;
	msg_list_->SetParaFormat(pf);

	if (!is_history)
		msg_list_->EndDown();
	input_edit_->SetFocus();
}

void ChatroomForm::SendJsb(const std::string & attach)
{
	std::string send_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeCustom, QString::GetGUID(), attach, ChatRoomMessageSetting());
	ChatRoom::SendMsg(room_id_, send_msg);

}
}