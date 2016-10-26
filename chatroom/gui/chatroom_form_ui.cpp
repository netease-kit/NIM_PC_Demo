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

	time_start_history = 0;

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

ui::UILIB_RESOURCETYPE ChatroomForm::GetResourceType() const
{
	return ui::UILIB_RESOURCETYPE::UILIB_FILE;
}

std::wstring ChatroomForm::GetZIPFileName() const
{
	return L"chatroom.zip";
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
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&ChatroomForm::OnSelChanged, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&ChatroomForm::OnSelChanged, this, std::placeholders::_1));

	input_edit_ = (RichEdit*)FindControl(L"input_edit");
	input_edit_->SetSelAllOnFocus(false);
	//input_edit_->SetAutoURLDetect(true);
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

	msg_list_ = (RichEdit*)FindControl(L"intercommunicate_list");
	msg_list_->SetSelAllOnFocus(false);
	msg_list_->SetNoSelOnKillFocus(false);
//	msg_list_->SetAutoURLDetect(true); 开启自动检测URL功能后，对于自定义超链接的判断会产生影响
	msg_list_->SetNoCaretReadonly();
	msg_list_->SetEventMask(msg_list_->GetEventMask() | ENM_LINK | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);
	msg_list_->SetLimitText(INT_MAX);

	text_services = msg_list_->GetTextServices();
	richeditolecallback_ = new nim_comp::IRichEditOleCallbackEx(text_services, std::function<void()>());
	text_services->Release();
	msg_list_->SetOleCallback(richeditolecallback_);
	richeditolecallback_->SetCustomMode(true);

	// 当需要支持gif时，再启用此功能
	//auto callback = nbase::Bind(&ChatroomForm::TimeHandle, this);
	//nbase::ThreadManager::PostRepeatedTask(callback, nbase::TimeDelta::FromMilliseconds(50));

	header_icon_ = FindControl(L"header_icon");
	name_ = ((ui::Label*)FindControl(L"name"));
	btn_face_ = (ui::CheckBox*)FindControl(L"btn_face");
	online_members_list_ = (ui::ListBox*)FindControl(L"online_members_list");
	empty_members_list_ = FindControl(L"empty_members_list");
	InitHeader();

	list_tab_ = (ui::TabBox*)FindControl(L"list_tab");
	host_icon_ = FindControl(L"host_image");
	room_name_ = (ui::Label*)FindControl(L"room_name");
	host_name_ = (ui::Label*)FindControl(L"host_name");
	online_num_ = (ui::Label*)FindControl(L"online_num");
	bulletin_ = ((ui::RichEdit*)FindControl(L"bulletin"));
	bulletin_->SetLimitText(300);

	ui::Option *option_online_members = (ui::Option*)FindControl(L"option_online_members");
	option_online_members->AttachSelect(nbase::Bind(&ChatroomForm::OnSelOnlineMembers, this, std::placeholders::_1));

	//unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserInfoChange(nbase::Bind(&ChatroomForm::OnUserInfoChange, this, std::placeholders::_1)));
	//unregister_cb.Add(nim_ui::PhotoManager::GetInstance()->RegPhotoReady(nbase::Bind(&ChatroomForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
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
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);	
		ScreenToClient(GetHWND(), &pt);
		if (msg_list_->GetPos().IsPointIn(pt) && list_tab_->GetCurSel() == 0 && !is_loading_history_)
		{
			int zDelta = (int)(short)HIWORD(wParam);
			if (zDelta > 0)
			{
				if (msg_list_->GetScrollPos().cy == 0)
				{
					is_loading_history_ = true;
					GetHistorys();
					return 0;
				}
			}
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool ChatroomForm::Notify(ui::EventArgs * msg)
{
	return true;
}

bool ChatroomForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"send")
	{
		OnBtnSend();
	}
	else if (name == L"btn_jsb")
	{
		OnBtnJsb();
	}

	return true;
}

bool ChatroomForm::OnEditEnter(ui::EventArgs* param)
{
	OnBtnSend();
	return true;
}

void ChatroomForm::OnBtnEmoji()
{
	auto my_info = members_list_.find(nim_ui::LoginManager::GetInstance()->GetAccount());
	if (my_info != members_list_.end() && !room_mute_ && !my_info->second.is_muted_ && !my_info->second.temp_muted_)
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

bool ChatroomForm::OnSelChanged(ui::EventArgs* param)
{
	EventType et = param->Type;
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_face")
	{
		if (et == kEventSelect)
			OnBtnEmoji();
	}
	else if (name == L"option_intercommunicate")
		list_tab_->SelectItem(0);
	else if (name == L"option_online_members")
		list_tab_->SelectItem(1);

	return true;
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

bool ChatroomForm::OnSelOnlineMembers(ui::EventArgs* param)
{
	// 单击在线成员tab后重新获取在线成员， 一分钟内不刷新
	time_t	sub_time = time(NULL) - time_refresh_;
	if (sub_time <= 60)
		return true;

	online_members_list_->RemoveAll();
	has_add_creater_ = false;
	managers_list_.clear(); 
	
	UpdateOnlineCount();
	GetMembers();

	return true;
}

bool ChatroomForm::OnLinkClick(WPARAM wParam, LPARAM lParam)
{
	if (wParam == EN_LINK)
	{
		std::wstring name = *(std::wstring*)lParam;
		if (!name.empty())
		{
			ShowMemberMenu(name);
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
	if (my_id == creater_id_)
		show_admin = true;
	else if (managers_list_.find(my_id) != managers_list_.end())
	{
		//do nothing
	}
	else
		return;

	// 找到昵称对应的帐号
	auto it = nick_account_map_.find(nbase::UTF16ToUTF8(user_name));
	if (it == nick_account_map_.end())
		return;

	if (it->second == creater_id_)
		return;

	ChatRoomMemberInfo member_info;
	auto it_member = members_list_.find(it->second);
	if (it_member != members_list_.end())
		member_info = it_member->second;

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

		pMenu->Show();
	};
	Post2UI(cb);
}

bool ChatroomForm::KickMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoom::KickMemberAsync(room_id_, clicked_user_account_, "", nbase::Bind(&ChatroomForm::KickMemberCallback, this, std::placeholders::_1, std::placeholders::_2));
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
	
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::SetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::SetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::SetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::SetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::SetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
	ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::SetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	clicked_user_account_.clear();
	return true;
}

bool ChatroomForm::TempMuteMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoom::TempMuteMemberAsync(room_id_, clicked_user_account_, 60, true, "", nbase::Bind(&ChatroomForm::TempMuteCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	return true;
}

bool ChatroomForm::RemoveTempMuteMenuItemClick(ui::EventArgs* args)
{
	if (clicked_user_account_.empty())
		return true;

	ChatRoom::TempMuteMemberAsync(room_id_, clicked_user_account_, 0, true, "", nbase::Bind(&ChatroomForm::TempMuteCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	return true;
}

void ChatroomForm::SetMemberAdmin(const std::string &id, bool is_admin)
{
	if (id == creater_id_)
		return;
	
	if (is_admin)
	{
		if (id == nim_ui::LoginManager::GetInstance()->GetAccount())
		{
			bulletin_->SetReadOnly(false);
		}

		auto info = members_list_.find(id);
		if (info != members_list_.end())
		{
			managers_list_[id] = info->second;
		}

		ui::ButtonBox* member_item = (ui::ButtonBox*)online_members_list_->FindSubControl(nbase::UTF8ToUTF16(id));
		if (member_item)
		{
			ui::Control* member_type = (ui::Control*)member_item->FindSubControl(L"member_type");
			member_type->SetBkImage(L"icon_manager.png");
			online_members_list_->SetItemIndex(member_item, 1);
		}
	}
	else
	{
		if (id == nim_ui::LoginManager::GetInstance()->GetAccount())
		{
			bulletin_->SetReadOnly(true);
		}
			
		auto manager_info = managers_list_.find(id);
		if (manager_info != managers_list_.end())
		{
			managers_list_.erase(manager_info);
		}

		ui::ButtonBox* member_item = (ui::ButtonBox*)online_members_list_->FindSubControl(nbase::UTF8ToUTF16(id));
		if (member_item)
		{
			ui::Control* member_type = (ui::Control*)member_item->FindSubControl(L"member_type");
			member_type->SetBkImage(L"");
			int manager_cout = managers_list_.size();
			if (has_add_creater_)
				manager_cout++;
			online_members_list_->SetItemIndex(member_item, manager_cout);
		}
	}
}

void ChatroomForm::SetMemberBlacklist(const std::string &id, bool is_black)
{
	if (id == creater_id_)
		return;

	auto info = members_list_.find(id);
	if (info != members_list_.end())
	{
		info->second.is_blacklist_ = is_black;
		members_list_[id] = info->second;
	}
}

void ChatroomForm::SetMemberMute(const std::string &id, bool is_mute)
{
	if (id == creater_id_)
		return;

	auto info = members_list_.find(id);
	if (info != members_list_.end())
	{
		info->second.is_muted_ = is_mute;
		members_list_[id] = info->second;
	}
}

void ChatroomForm::SetMemberFixed(const std::string &id, bool is_fixed)
{
	if (id == creater_id_)
		return;

	auto info = members_list_.find(id);
	if (info != members_list_.end())
	{
		info->second.guest_flag_ = is_fixed ? kNIMChatRoomGuestFlagNoGuest : kNIMChatRoomGuestFlagGuest;
		members_list_[id] = info->second;
	}
}

void ChatroomForm::SetMemberTempMute(const std::string &id, bool temp_mute, __int64 duration)
{
	auto info = members_list_.find(id);
	if (info != members_list_.end())
	{
		info->second.temp_muted_ = temp_mute;
		info->second.temp_muted_duration_ = temp_mute ? duration : 0;
		members_list_[id] = info->second;
		auto iter = temp_unmute_id_task_map_.find(id);
		if (iter != temp_unmute_id_task_map_.end())
		{
			(*iter).second.Cancel();
			temp_unmute_id_task_map_.erase(iter);
		}
		if (temp_mute)
		{
			StdClosure task = nbase::Bind(&ChatroomForm::SetMemberTempMute, this, id, false, 0);
			nbase::WeakCallbackFlag weak_flag;
			task = weak_flag.ToWeakCallback(task);
			temp_unmute_id_task_map_[id] = weak_flag;
			nbase::ThreadManager::PostDelayedTask(task, nbase::TimeDelta::FromSeconds(duration));
		}
	}
}

void ChatroomForm::SetRoomMemberMute(bool mute)
{
	if (room_mute_ == mute)
		return;

	for (auto& it : members_list_)
	{
		if (it.second.type_ < 1)
			SetMemberMute(it.first, mute);
	}
}

void ChatroomForm::RemoveMember(const std::string &uid)
{
// 现在单击了在线成员列表后会重新刷新成员，无须单独维护成员列表
	auto exit_member = members_list_.find(uid);
	if (exit_member != members_list_.end())
	{
		if (!exit_member->second.is_blacklist_ && exit_member->second.type_ == 0)
		{
			Control* member_item = online_members_list_->FindSubControl(nbase::UTF8ToUTF16(uid));
			online_members_list_->Remove(member_item);
			members_list_.erase(exit_member);
		}
	}
}

void ChatroomForm::UpdateOnlineCount()
{
	ChatRoom::GetInfoAsync(room_id_, nbase::Bind(&ChatroomForm::OnGetChatRoomInfoCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void ChatroomForm::OnWndSizeMax(bool max)
{
	if (!m_pRoot)
		return;

	FindControl(L"maxbtn")->SetVisible(!max);
	FindControl(L"restorebtn")->SetVisible(max);
}

void ChatroomForm::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	//for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	//{
		//if (nim_ui::LoginManager::GetInstance()->IsEqual(iter->GetAccId()))
		//	InitHeader();
		//if (creater_id_ == iter->GetAccId())
		//	host_icon_->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(iter->GetAccId()));
		//return;
		//聊天室不建议对成员列表做监听后的实现，会有性能问题
		//ui::ButtonBox* room_member_item = (ui::ButtonBox*)online_members_list_->FindSubControl(nbase::UTF8ToUTF16(iter->GetAccId()));
		//if (room_member_item != NULL)
		//{
		//	ui::Control* header_image = (ui::Control*)room_member_item->FindSubControl(L"header_image");
		//	header_image->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(iter->GetAccId()));
		//}
	//}
}

void ChatroomForm::OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path)
{
	//if (type == kUser)
	//{
	//	if (nim_ui::LoginManager::GetInstance()->GetAccount() == account)
	//		header_icon_->SetBkImage(photo_path);
	//	if (creater_id_ == account)
	//		host_icon_->SetBkImage(photo_path);
	//}
	//return;
	//聊天室不建议对成员列表做监听后的实现，可能会有性能问题
	//ui::ButtonBox* room_member_item = (ui::ButtonBox*)online_members_list_->FindSubControl(nbase::UTF8ToUTF16(account));
	//if (room_member_item != NULL)
	//{
	//	ui::Control* header_image = (ui::Control*)room_member_item->FindSubControl(L"header_image");
	//	header_image->SetBkImage(photo_path);
	//}
}

void ChatroomForm::OnHttoDownloadReady(HttpResourceType type, const std::string& account, const std::wstring& photo_path)
{
	if (type == kChatroomMemberIcon)
	{
		ui::ButtonBox* room_member_item = (ui::ButtonBox*)online_members_list_->FindSubControl(nbase::UTF8ToUTF16(account));
		if (room_member_item != NULL)
		{
			ui::Control* header_image = (ui::Control*)room_member_item->FindSubControl(L"header_image");
			header_image->SetBkImage(photo_path);
		}

		if (nim_ui::LoginManager::GetInstance()->IsEqual(account))
			header_icon_->SetBkImage(photo_path);

		if (account == creater_id_)
			host_icon_->SetBkImage(photo_path);
	}
}

void ChatroomForm::InitHeader()
{
	std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
	header_icon_->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(my_id));
	name_->SetText(nim_ui::UserManager::GetInstance()->GetUserName(my_id, false));
}

void ChatroomForm::RequestRoomError()
{
	this->Close();
}
}
