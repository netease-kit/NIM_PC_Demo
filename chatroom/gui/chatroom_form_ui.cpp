#include "stdafx.h"
#include "chatroom_form.h"
#include "gui/emoji/emoji_form.h"
#include "module/emoji/richedit_util.h"
#include "module/session/session_util.h"
#include "shared/ui/ui_menu.h"
#include "richedit.h"

namespace nim_chatroom
{
const LPTSTR ChatroomForm::kClassName = L"ChatroomForm";
using namespace ui;
ChatroomForm::ChatroomForm(__int64 room_id)
{
	room_id_ = room_id;
	has_enter_ = false;
	droptarget_ = NULL;

	input_edit_ = NULL;
	btn_face_ = NULL;
	msg_list_ = NULL;

	time_start_history_ = 0;

	room_mute_ = false;

}

ChatroomForm::~ChatroomForm()
{
	for each (auto var in temp_unmute_id_task_map_)
	{
		var.second.Cancel();
	}
	temp_unmute_id_task_map_.clear();
}

std::wstring ChatroomForm::GetSkinFolder()
{
	return L"chatroom";
}

std::wstring ChatroomForm::GetSkinFile()
{
	return L"chatroom.xml";
}

std::wstring ChatroomForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ChatroomForm::GetWindowId() const
{
	return nbase::Int64ToString16(room_id_);
}

UINT ChatroomForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}


ui::Control* ChatroomForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefControl")
	{
		return new ui::CefNativeControl;
	}

	return NULL;
}

LRESULT ChatroomForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (kAllowClose == wParam)
	{
		bHandled = FALSE;
		return 0;
	}
	else
	{
		if (room_id_ != 0)
			ChatRoom::Exit(room_id_);
	}

	bHandled = TRUE;
	return 0;
}

void ChatroomForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&ChatroomForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&ChatroomForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&ChatroomForm::OnSelectChanged, this, std::placeholders::_1));

	input_edit_ = (RichEdit*)FindControl(L"input_edit");
	input_edit_->SetSelAllOnFocus(false);
//	input_edit_->SetAutoURLDetect(true);
	input_edit_->SetEventMask(input_edit_->GetEventMask() | ENM_LINK | ENM_CHANGE);
	input_edit_->AttachReturn(nbase::Bind(&ChatroomForm::OnEditEnter, this, std::placeholders::_1));
	IDropTarget *pdt = input_edit_->GetTxDropTarget();
// 	if (pdt)
// 	{
// 		droptarget_ = pdt;
// 		::RegisterDragDrop(m_hWnd, this);
// 	}

	ITextServices * text_services = input_edit_->GetTextServices();
	richeditolecallback_ = new nim_comp::IRichEditOleCallbackEx(text_services, std::function<void()>());
	text_services->Release();
	input_edit_->SetOleCallback(richeditolecallback_);
	richeditolecallback_->SetCustomMode(true);

	msg_list_ = (ui::CefNativeControl*)FindControl(L"intercommunicate_list");
	msg_list_->AttachLoadEnd(nbase::Bind(&ChatroomForm::OnLoadEnd, this, std::placeholders::_1));
	msg_list_->AttachAfterCreated(nbase::Bind(&ChatroomForm::OnAfterCreated, this, std::placeholders::_1));
	msg_list_->AttachBeforeCLose(nbase::Bind(&ChatroomForm::OnBeforeClose, this, std::placeholders::_1));
	msg_list_->AttachBeforeContextMenu(nbase::Bind(&ChatroomForm::OnBeforeContextMenu, this, std::placeholders::_1, std::placeholders::_2));
	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef_themes/chatroom_list/chat_list.html";
	msg_list_->LoadURL(html_path);

//	msg_list_ = (RichEdit*)FindControl(L"intercommunicate_list");
//	msg_list_->SetSelAllOnFocus(false);
//	msg_list_->SetNoSelOnKillFocus(false);
//	msg_list_->SetAutoURLDetect(true); 开启自动检测URL功能后，对于自定义超链接的判断会产生影响
//	msg_list_->SetNoCaretReadonly();
//	msg_list_->SetEventMask(msg_list_->GetEventMask() | ENM_LINK | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);
//	msg_list_->SetLimitText(INT_MAX);

//	text_services = msg_list_->GetTextServices();
//	richeditolecallback_ = new nim_comp::IRichEditOleCallbackEx(text_services, std::function<void()>());
//	text_services->Release();
//	msg_list_->SetOleCallback(richeditolecallback_);
//	richeditolecallback_->SetCustomMode(true);

	// 当需要支持gif时，再启用此功能
	//auto callback = nbase::Bind(&ChatroomForm::TimeHandle, this);
	//nbase::ThreadManager::PostRepeatedTask(callback, nbase::TimeDelta::FromMilliseconds(50));

	my_icon_ = FindControl(L"header_icon");
	my_name_ = ((ui::Label*)FindControl(L"name"));
	host_icon_ = FindControl(L"host_image");
	host_name_ = (ui::Label*)FindControl(L"host_name");
	room_name_ = (ui::Label*)FindControl(L"room_name");
	online_num_ = (ui::Label*)FindControl(L"online_num");
	bulletin_ = ((ui::RichEdit*)FindControl(L"bulletin"));
	bulletin_->SetLimitText(300);
	btn_face_ = (ui::CheckBox*)FindControl(L"btn_face");

	list_tab_ = (ui::TabBox*)FindControl(L"list_tab");
	option_online_members_ = (ui::Option*)FindControl(L"option_online_members");
	empty_members_list_ = FindControl(L"empty_members_list");

//	online_members_list_ = (ui::ListBox*)FindControl(L"online_members_list");
	online_members_virtual_list_ = (ui::VirtualListBox*)FindControl(L"online_members_virtual_list");
	int kRoomMemberItemHeight = 35;
	online_members_virtual_list_->SetElementHeight(ui::DpiManager::GetInstance()->ScaleInt(kRoomMemberItemHeight));
	online_members_virtual_list_->SetDataProvider(this);
	online_members_virtual_list_->InitElement(50);
	online_members_virtual_list_->AttachBubbledEvent(kEventMouseMenu, nbase::Bind(&ChatroomForm::OnMemberMenu, this, std::placeholders::_1));

	InitHeader();
	unregister_cb.Add(nim_ui::HttpManager::GetInstance()->RegDownloadComplete(nbase::Bind(&ChatroomForm::OnHttoDownloadReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
}

LRESULT ChatroomForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
			OnWndSizeMax(false);
		else if (wParam == SIZE_MAXIMIZED)
			OnWndSizeMax(true);
	}
	else if (uMsg == WM_NOTIFY)  // 超链接消息
	{
 		if (OnLinkClick(wParam, lParam))
 			return 0;
	}
	else if (uMsg == WM_MOUSEWHEEL)
	{
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (wParam == VK_DOWN || wParam == VK_UP || wParam == VK_RETURN || wParam == VK_BACK)
		{
			if (HandleAtMsg(wParam, lParam))
			{
				return 0;
			}
		}
	}
	else if (uMsg == WM_CHAR)
	{
		if (wParam != VK_BACK)
		{
			//先让RichEdit处理完，然后再处理@消息
			LRESULT res = __super::HandleMessage(uMsg, wParam, lParam);
			HandleAtMsg(wParam, lParam);
			return res;
		}
	}
	else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_KILLFOCUS)
	{
		HideAtListForm();
	}
	else if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONUP)
	{
		auto res = HandleDiscuzMessage(uMsg, wParam, lParam);
		if (res > 0)
			return res;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void ChatroomForm::OnWndSizeMax(bool max)
{
	if (!m_pRoot)
		return;

	FindControl(L"maxbtn")->SetVisible(!max);
	FindControl(L"restorebtn")->SetVisible(max);
}

bool ChatroomForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"send")
	{
		if (is_anonymity_)
			OnBtnLogin();
		else
			OnBtnSend();
	}
	else if (name == L"btn_jsb")
	{
		OnBtnJsb();
	}
	else if (name == L"logout")
	{
		param->pSender->SetEnabled(false);
		Logout();
	}

	return true;
}

void ChatroomForm::Logout()
{
	nim::Client::Logout(nim::kNIMLogoutChangeAccout, ToWeakCallback([this](nim::NIMResCode code){
		nim_comp::LoginManager::GetInstance()->ReleaseSingletonRunMutex();
		nim_comp::LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);
		nim_comp::LoginManager::GetInstance()->SetAccount("");
		nim_comp::LoginManager::GetInstance()->SetPassword("");
		switch_to_login_status_ = kToAnonymous;
		nim_chatroom::ChatRoom::Exit(room_id_);

	}));
}

bool ChatroomForm::OnSelectChanged(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();

	if (name == L"btn_face")
		OnBtnEmoji();
	else if (name == L"option_intercommunicate")
		list_tab_->SelectItem(0);
	else if (name == L"option_online_members")
	{
		list_tab_->SelectItem(1);

		// 单击在线成员tab后重新获取在线成员， 一分钟内不刷新
		time_t	sub_time = time(NULL) - time_refresh_;
		if (sub_time <= 60)
			return true;
		time_refresh_ = time(NULL);

		if (!managers_list_.empty() && !members_list_.empty() && !members_map_.empty())
			online_members_virtual_list_->Refresh();

//		online_members_list_->RemoveAll();
		has_add_creater_ = false;
		managers_list_.clear();
		members_list_.clear();
		members_map_.clear();

		GetOnlineCount();
		GetMembers();
	}

	return true;
}

bool ChatroomForm::Notify(ui::EventArgs* param)
{
// 	std::wstring name = param->pSender->GetName();
// 	if (param->Type == kEventCustomLinkClick)
// 	{
// 		RichEdit* text_edit = dynamic_cast<RichEdit*>(param->pSender);
// 		std::wstring link_info;
// 		if (text_edit->HittestCustomLink(param->ptMouse, link_info))
// 		{
// 			Json::Value values;
// 			Json::Reader reader;
// 			if (reader.parse(nbase::UTF16ToUTF8((std::wstring)link_info), values) && values.isObject())
// 			{
// 				std::string type = values["type"].asString();
// 				if (type == "url")
// 				{
// 					std::string url = values["target"].asString();
// 					Post2GlobalMisc(nbase::Bind(&shared::tools::SafeOpenUrl, nbase::UTF8ToUTF16(url), SW_SHOW));
// 				}
// 				else if (type == "block")
// 				{
// 					std::string robot_id = values[nim::kNIMBotRobotMsgKeyRobotID].asString();
// 					std::string content = values["link_text"].asString();
// 					nim::IMBotRobot bot;
// 					bot.robot_accid_ = robot_id;
// 					bot.sent_param_["target"] = values["target"].asString();
// 					bot.sent_param_["type"] = "03";
// 					bot.sent_param_["params"] = values["params"].asString();
// 					std::string json_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeRobot, nim::Tool::GetUuid(), bot.ToJsonString(), content, ChatRoomMessageSetting());
// 					ChatRoom::SendMsg(room_id_, json_msg);
// 					std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
// 					std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
// 					AddText(nbase::UTF8ToUTF16(content), my_name, my_id, kMember, false);
// 				}
// 				else
// 				{
// 					assert(0);
// 					QLOG_ERR(L"\r\nError custom click: {0}") << (std::wstring)link_info;
// 				}
// 			}
// 		}
// 	}
	return true;
}

bool ChatroomForm::OnNameMenu(const std::string &id, const std::string &name, ui::EventArgs* arg)
{
	nim_comp::InsertTextToEdit(input_edit_, L"@" + nbase::UTF8ToUTF16(name) + L" ");
	AtSomeone da;
	da.uid_ = id;
	da.is_robot_ = true;
	uid_at_someone_[name] = da;
	return true;
}

bool ChatroomForm::OnEditEnter(ui::EventArgs* param)
{
	OnBtnSend();
	return true;
}

void ChatroomForm::OnBtnEmoji()
{
	auto my_info = members_map_.find(nim_ui::LoginManager::GetInstance()->GetAccount());
	if (my_info != members_map_.end() && !room_mute_ && !my_info->second.is_muted_ && !my_info->second.temp_muted_)
	{
		RECT rc = btn_face_->GetPos(true);
		POINT pt = { rc.left - 150, rc.top - 290 };
		::ClientToScreen(m_hWnd, &pt);

		OnSelectEmotion sel = nbase::Bind(&ChatroomForm::OnEmotionSelected, this, std::placeholders::_1);
		OnSelectSticker sel2 = nbase::Bind(&ChatroomForm::OnEmotionSelectedSticker, this, std::placeholders::_1, std::placeholders::_2);
		OnEmotionClose  cls = nbase::Bind(&ChatroomForm::OnEmotionClosed, this);

		nim_comp::EmojiForm* emoji_form = new nim_comp::EmojiForm;
		emoji_form->ShowEmoj(pt, sel, sel2, cls, true);
	}
}

void ChatroomForm::OnEmotionSelected(std::wstring emo)
{
	std::wstring file;
	if (nim_comp::emoji::GetEmojiFileByTag(emo, file))
	{
		nim_comp::InsertFaceToEdit(input_edit_, file, emo);
	}
}

void ChatroomForm::OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name)
{

}

void ChatroomForm::OnEmotionClosed()
{
	btn_face_->Selected(false, false);
	input_edit_->SetFocus();
}

bool ChatroomForm::OnLinkClick(WPARAM wParam, LPARAM lParam)
{
	if (wParam == EN_LINK)
	{
		if (!is_anonymity_)
		{
			std::wstring name = *(std::wstring*)lParam;
			nbase::StringTrim(name);
			if (!name.empty())
			{
				if (msg_list_sender_name_link_.find(name) != msg_list_sender_name_link_.end())
				{
					Json::Value values = msg_list_sender_name_link_[name];
					SenderType type = (SenderType)values["type"].asInt();
					if (type == kRobot)
					{
						std::string robot_name = values["name"].asString();
						std::string robot_id = values["id"].asString();
						StdClosure cb = [robot_name, robot_id, this](){

							POINT point;
							::GetCursorPos(&point);

							CMenuWnd* pMenu = new CMenuWnd(NULL);
							STRINGorID xml(L"cell_head_menu.xml");
							pMenu->Init(xml, _T("xml"), point);

							CMenuElementUI* at_ta = (CMenuElementUI*)pMenu->FindControl(L"at_ta");
							at_ta->AttachSelect(nbase::Bind(&ChatroomForm::OnNameMenu, this, robot_id, robot_name, std::placeholders::_1));
							at_ta->SetVisible(true);
							pMenu->Show();
						};
						Post2UI(cb);
					}
					else if (type == kMember)
					{

					}
					else
					{
						assert(0);
					}
				}
				else
					ShowMemberMenu(name);
			}
		}
	}
	return false;
}

bool ChatroomForm::OnMemberMenu(ui::EventArgs* args)
{
	Box* member_item = static_cast<Box*>(args->pSender);
	if (member_item != NULL)
	{
		ui::Label* name = (ui::Label*)member_item->FindSubControl(L"name");
		if (name != NULL)
		{
			ShowMemberMenu(name->GetText());
		}
	}

	return true;
}

void ChatroomForm::ShowMemberMenu(std::wstring &name)
{
	std::wstring user_name = nbase::StringTrim(name);
	if (user_name.empty())
		return;

	clicked_user_account_.clear();

	// 不能操作自己帐号
	std::string my_id = nim_comp::LoginManager::GetInstance()->GetAccount();
	if (user_name == nim_ui::UserManager::GetInstance()->GetUserName(my_id, false))
		return;

	// 只有创建者和管理员可以操作
	bool show_admin = false;
	if (my_id == creater_id_ || std::find(managers_list_.begin(), managers_list_.end(), my_id) != managers_list_.end())
		show_admin = true;
	else
		return;

	// 找到昵称对应的帐号
	auto it = nick_account_map_.find(nbase::UTF16ToUTF8(user_name));
	if (it == nick_account_map_.end())
		return;

	//如果目标用户是创建者，不能操作，直接返回
	if (it->second == creater_id_)
		return;

	ChatRoomMemberInfo member_info;
	auto it_member = members_map_.find(it->second);
	if (it_member != members_map_.end())
		member_info = it_member->second;

	//如果目标用户和自己都是管理员，不能操作，直接返回
	bool my_room = my_id == creater_id_;
	if (!my_room && member_info.type_ == 2)
		return;

	clicked_user_account_ = it->second;

	StdClosure cb = [member_info, show_admin, this](){
		CMenuWnd* pMenu = new CMenuWnd(NULL);
		STRINGorID xml(L"chatroom_menu.xml");
		POINT point;
		GetCursorPos(&point);
		pMenu->Init(xml, _T("xml"), point, CMenuWnd::RIGHT_TOP);
		//注册回调

		CMenuElementUI* kick = (CMenuElementUI*)pMenu->FindControl(L"kick");
		kick->AttachSelect(nbase::Bind(&ChatroomForm::KickMenuItemClick, this, std::placeholders::_1));

		if (member_info.type_ != 4)
		{
			if (member_info.is_muted_)
			{
				CMenuElementUI* removemute = (CMenuElementUI*)pMenu->FindControl(L"removemute");
				removemute->AttachSelect(nbase::Bind(&ChatroomForm::RemoveMuteMenuItemClick, this, std::placeholders::_1));
				removemute->SetVisible(true);
			}
			else
			{
				CMenuElementUI* addmute = (CMenuElementUI*)pMenu->FindControl(L"addmute");
				addmute->AttachSelect(nbase::Bind(&ChatroomForm::AddMuteMenuItemClick, this, std::placeholders::_1));
				addmute->SetVisible(true);
			}

			if (member_info.is_blacklist_)
			{
				CMenuElementUI* removeblacklist = (CMenuElementUI*)pMenu->FindControl(L"removeblacklist");
				removeblacklist->AttachSelect(nbase::Bind(&ChatroomForm::RemoveBlacklistMenuItemClick, this, std::placeholders::_1));
				removeblacklist->SetVisible(true);
			}
			else
			{
				CMenuElementUI* addblacklist = (CMenuElementUI*)pMenu->FindControl(L"addblacklist");
				addblacklist->AttachSelect(nbase::Bind(&ChatroomForm::AddBlacklistMenuItemClick, this, std::placeholders::_1));
				addblacklist->SetVisible(true);
			}

			if (show_admin)
			{
				if (member_info.type_ == 2)
				{
					CMenuElementUI* removeadmin = (CMenuElementUI*)pMenu->FindControl(L"removeadmin");
					removeadmin->AttachSelect(nbase::Bind(&ChatroomForm::RemoveAdminMenuItemClick, this, std::placeholders::_1));
					removeadmin->SetVisible(true);
				}
				else
				{
					CMenuElementUI* addadmin = (CMenuElementUI*)pMenu->FindControl(L"addadmin");
					addadmin->AttachSelect(nbase::Bind(&ChatroomForm::AddAdminMenuItemClick, this, std::placeholders::_1));
					addadmin->SetVisible(true);
				}

				if (member_info.temp_muted_)
				{
					CMenuElementUI* removetempmute = (CMenuElementUI*)pMenu->FindControl(L"tempunmute");
					removetempmute->AttachSelect(nbase::Bind(&ChatroomForm::RemoveTempMuteMenuItemClick, this, std::placeholders::_1));
					removetempmute->SetVisible(true);
				}
				else
				{
					CMenuElementUI* tempmute = (CMenuElementUI*)pMenu->FindControl(L"tempmute");
					tempmute->AttachSelect(nbase::Bind(&ChatroomForm::TempMuteMenuItemClick, this, std::placeholders::_1));
					tempmute->SetVisible(true);
				}
			}
		}

		pMenu->Show();
	};
	Post2UI(cb);
}

bool ChatroomForm::KickMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;
	Json::Value notify_ext;
	notify_ext["operator_id"] = my_info_.account_id_;
	notify_ext["operator_nick"] = my_info_.nick_;
	ChatRoom::KickMemberAsync(room_id_, clicked_user_account_, Json::FastWriter().write(notify_ext), nbase::Bind(&ChatroomForm::OnKickMemberCallback, this, std::placeholders::_1, std::placeholders::_2));
	kicked_user_account_ = clicked_user_account_;

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::AddMuteMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoomSetMemberAttributeParameters param;
	param.account_id_ = clicked_user_account_;
	param.attribute_ = kNIMChatRoomMemberAttributeMuteList;
	param.opt_ = true;

	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::RemoveMuteMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoomSetMemberAttributeParameters param;
	param.account_id_ = clicked_user_account_;
	param.opt_ = false;
	param.attribute_ = kNIMChatRoomMemberAttributeMuteList;
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::AddBlacklistMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoomSetMemberAttributeParameters param;
	param.account_id_ = clicked_user_account_;
	param.opt_ = true;
	param.attribute_ = kNIMChatRoomMemberAttributeBlackList;
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::RemoveBlacklistMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoomSetMemberAttributeParameters param;
	param.account_id_ = clicked_user_account_;
	param.opt_ = false;
	param.attribute_ = kNIMChatRoomMemberAttributeBlackList;
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::AddAdminMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoomSetMemberAttributeParameters param;
	param.account_id_ = clicked_user_account_;
	param.opt_ = true;
	param.attribute_ = kNIMChatRoomMemberAttributeAdminister;
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::RemoveAdminMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoomSetMemberAttributeParameters param;
	param.account_id_ = clicked_user_account_;
	param.opt_ = false;
	param.attribute_ = kNIMChatRoomMemberAttributeAdminister;
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::TempMuteMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoom::TempMuteMemberAsync(room_id_, clicked_user_account_, 60, true, "", nbase::Bind(&ChatroomForm::OnTempMuteCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	return true;
}

bool ChatroomForm::RemoveTempMuteMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoom::TempMuteMemberAsync(room_id_, clicked_user_account_, 0, true, "", nbase::Bind(&ChatroomForm::OnTempMuteCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	return true;
}

// ui::Control* ChatroomForm::AddMemberItem(const ChatRoomMemberInfo& info)
// {
// 	ui::ButtonBox* room_member_item = (ui::ButtonBox*)ui::GlobalManager::CreateBoxWithCache(L"chatroom/room_member_item.xml");
// 	if (room_member_item)
// 	{
// 		room_member_item->AttachMenu(nbase::Bind(&ChatroomForm::OnMemberMenu, this, std::placeholders::_1));
// 		ui::Control* member_type = (ui::Control*)room_member_item->FindSubControl(L"member_type");
// 
// 		if (info.type_ == 1)//创建者
// 		{
// 			member_type->SetBkImage(L"icon_anchor.png");
// 			online_members_list_->AddAt(room_member_item, 0);
// 		}
// 		else if (info.type_ == 2)//管理员
// 		{
// 			member_type->SetBkImage(L"icon_manager.png");
// 			if (online_members_list_->GetCount() == 0)
// 			{
// 				online_members_list_->Add(room_member_item);
// 			}
// 			else
// 			{
// 				if (has_add_creater_)
// 					online_members_list_->AddAt(room_member_item, 1);
// 				else
// 					online_members_list_->AddAt(room_member_item, 0);
// 			}
// 		}
// 		else
// 		{
// 			online_members_list_->Add(room_member_item);
// 		}
// 
// 		ui::Label* name = (ui::Label*)room_member_item->FindSubControl(L"name");
// 		name->SetUTF8Text(info.nick_);
// 
// 		ui::Control* header_image = (ui::Control*)room_member_item->FindSubControl(L"header_image");
// 		if (!info.avatar_.empty() && info.avatar_.find_first_of("http") == 0)
// 			header_image->SetBkImage(nim_ui::HttpManager::GetInstance()->GetCustomImage(kChatroomMemberIcon, info.account_id_, info.avatar_));
// 		else
// 			header_image->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(info.account_id_));
// 
// 		room_member_item->SetUTF8Name(info.account_id_);
// 	}
// 
// 	return room_member_item;
// }

ui::Control* ChatroomForm::CreateElement()
{
	std::wstring item_xml = L"chatroom/room_member_item.xml";
	if(ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
		item_xml = L"chatroom/room_member_item_en.xml";
	ui::ButtonBox* room_member_item = (ui::ButtonBox*)ui::GlobalManager::CreateBoxWithCache(item_xml);
	return room_member_item;
}

void ChatroomForm::FillElement(ui::Control *control, int index)
{
	ui::ButtonBox* room_member_item = (ui::ButtonBox*)control;
	if (nullptr == room_member_item || index >= (int)members_map_.size())
		return;

	std::string account;
	if (has_add_creater_)
	{
		if (0 == index)
		{
			account = creater_id_;
		}
		else if (index > 0 && index < (int)managers_list_.size() + 1)
			account = managers_list_.at(index - 1);
		else
			account = members_list_.at(index - 1 - managers_list_.size());
	}
	else
	{
		if (index < (int)managers_list_.size())
			account = managers_list_.at(index);
		else
			account = members_list_.at(index - managers_list_.size());
	}

	auto it = members_map_.find(account);
	if (it == members_map_.end())
		return;

	ChatRoomMemberInfo info = it->second;

	ui::Control* member_type = (ui::Control*)room_member_item->FindSubControl(L"member_type");
	if (info.type_ == 1)//创建者
	{
		std::wstring bk = L"icon_anchor.png";
		if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
			bk = L"icon_anchor_en.png";
		member_type->SetBkImage(bk);
	}
	else if (info.type_ == 2)//管理员
	{
		std::wstring bk = L"icon_manager.png";
		if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
			bk = L"icon_manager_en.png";
		member_type->SetBkImage(bk);
	}
	else
	{
		member_type->SetBkImage(L"");
	}

	ui::Label* name = (ui::Label*)room_member_item->FindSubControl(L"name");
	name->SetUTF8Text(info.nick_);

	ui::Control* header_image = (ui::Control*)room_member_item->FindSubControl(L"header_image");
	if (!info.avatar_.empty() && info.avatar_.find_first_of("http") == 0)
		header_image->SetBkImage(nim_ui::HttpManager::GetInstance()->GetCustomImage(kChatroomMemberIcon, info.account_id_, info.avatar_));
	else
		header_image->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(info.account_id_));

	room_member_item->SetUTF8Name(info.account_id_);
}

int ChatroomForm::GetElementtCount()
{
	return managers_list_.size() + members_list_.size() + (has_add_creater_ ? 1 : 0);
}

}
