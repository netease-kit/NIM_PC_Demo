#include "stdafx.h"
#include "chatroom_frontpage.h"
#include "chatroom_form.h"
#include "personal_center.h"
#include "callback/chatroom_callback.h"
#include "shared/xml_util.h"

namespace nim_chatroom
{
const LPTSTR ChatroomFrontpage::kClassName = L"ChatroomFrontpage";

ChatroomFrontpage::ChatroomFrontpage()
{
}

ChatroomFrontpage::~ChatroomFrontpage()
{
}

std::wstring ChatroomFrontpage::GetSkinFolder()
{
	return L"chatroom";
}

std::wstring ChatroomFrontpage::GetSkinFile()
{
	return L"chatroom_frontpage.xml";
}

ui::UILIB_RESOURCETYPE ChatroomFrontpage::GetResourceType() const
{
	return ui::UILIB_RESOURCETYPE::UILIB_FILE;
}

std::wstring ChatroomFrontpage::GetZIPFileName() const
{
	return L"chatroom_frontpage.zip";
}

std::wstring ChatroomFrontpage::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ChatroomFrontpage::GetWindowId() const
{
	return kClassName;
}

UINT ChatroomFrontpage::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void ChatroomFrontpage::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&ChatroomFrontpage::OnClicked, this, std::placeholders::_1));

	InitHeader();

	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserInfoChange(nbase::Bind(&ChatroomFrontpage::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(nim_ui::PhotoManager::GetInstance()->RegPhotoReady(nbase::Bind(&ChatroomFrontpage::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

	room_list_ = (ui::TileBox*)FindControl(L"room_list");
	
	InvokeGetRoomList();
}

LRESULT ChatroomFrontpage::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
			OnWndSizeMax(false);
		else if (wParam == SIZE_MAXIMIZED)
			OnWndSizeMax(true);
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool ChatroomFrontpage::Notify(ui::EventArgs * msg)
{
	return false;
}

bool ChatroomFrontpage::OnClicked(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"be_host")
	{
		nim_ui::WindowsManager::GetInstance()->SingletonShow<PersonalCenterForm>(PersonalCenterForm::kClassName);
	}
	else if (name == L"retry")
	{
		FindControl(L"load_error_tip")->SetVisible(false);
		InvokeGetRoomList();
	}
	return false;
}

ChatRoomInfo ChatroomFrontpage::GetRoomInfo(const __int64& room_id)
{
	auto iter = id_info_map_.find(room_id);
	if (iter != id_info_map_.cend())
		return iter->second;
	return ChatRoomInfo();
}

void ChatroomFrontpage::SetAnonymity(bool anonymity)
{
	anonymity_ = anonymity;
	FindControl(L"header_box")->SetVisible(!anonymity);
	FindControl(L"be_host")->SetVisible(!anonymity);
	if (anonymity)
	{
		std::wstring txt = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_LOGIN_FORM_CHATROOMDEMO");
		((ui::Label*)FindControl(L"name"))->SetText(txt);
	}
	else
		InitHeader();

}

void ChatroomFrontpage::InvokeGetRoomList()
{
	FindControl(L"loading_tip")->SetVisible(true);
	app_sdk::AppSDKInterface::GetInstance()->InvokeGetChatroomList(ToWeakCallback([this](int code, const std::vector< nim_chatroom::ChatRoomInfo>& chatroom_list){		
		if (code != nim::kNIMResSuccess)
		{
			QLOG_ERR(L"Invoke get room list error. Error code: {0}.") << code;
			FindControl(L"loading_tip")->SetVisible(false);
			FindControl(L"load_error_tip")->SetVisible(true);
			return;
		}
		else
		{
			FindControl(L"loading_tip")->SetVisible(false);
			for (auto it : chatroom_list)
			{
				CreateRoomItem(it);
			}
		}		
	}));
}

void ChatroomFrontpage::CreateRoomItem(const ChatRoomInfo& room_info)
{
	ui::Box* kicked_tip_box = (ui::Box*)FindControl(L"kicked_tip_box");
	if (kicked_tip_box->IsVisible())
	{
		ui::Label* room_name_label = (ui::Label*)kicked_tip_box->FindSubControl(L"room_name");
		__int64 label_room_id;
		nbase::StringToInt64(room_name_label->GetUTF8DataID(), &label_room_id);
		if (label_room_id == room_info.id_)
			room_name_label->SetUTF8Text(room_info.name_);
	}

	ui::ButtonBox* room_item = (ui::ButtonBox*)ui::GlobalManager::CreateBoxWithCache(L"chatroom/room_item.xml");
	if (room_item)
	{
		room_list_->Add(room_item);

		ui::Label* room_name_label = (ui::Label*)room_item->FindSubControl(L"room_name");
		room_name_label->SetUTF8Text(room_info.name_);
		ui::Label* online_count_label = (ui::Label*)room_item->FindSubControl(L"online_num");
		if(room_info.online_count_ >= 10000)
			online_count_label->SetText(nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_CHATROOM_ONLINE_NUM_EX2").c_str(), (float)room_info.online_count_ / (float)10000));
		else
			online_count_label->SetText(nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_CHATROOM_ONLINE_NUM_EX1").c_str(), room_info.online_count_));

		ui::Button* room_shot = (ui::Button*)room_item->FindSubControl(L"room_shot");
		room_shot->SetDataID(nbase::Int64ToString16(room_info.id_));
		room_shot->AttachClick(ToWeakCallback([this](ui::EventArgs* args)
		{
			std::wstring room_id = args->pSender->GetDataID();
			if (room_id.empty())
				return true;
			
			//这里应该查找当前房间号
			ChatroomForm* chat_form = static_cast<ChatroomForm*>(nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomForm::kClassName, room_id));
			if (chat_form != NULL)
			{
				chat_form->ActiveWindow();
				return true;
			}
			else
			{
				nim_comp::WindowList list = nim_ui::WindowsManager::GetInstance()->GetWindowsByClassName(ChatroomForm::kClassName);
				for (auto &it : list)
				{
					nim_comp::WindowsManager::SafeDoWindowOption<ChatroomForm,void>(it, [](ChatroomForm* form) {
						if (::IsWindow(form->GetHWND()))
							form->Close();
					});
				}
			}
			__int64 id;
			nbase::StringToInt64(room_id, &id);
			if (0 == id)
				return true;
			
			chat_form = new ChatroomForm(id);
			if (chat_form != NULL)
			{
				chat_form->Create(NULL, ChatroomForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
				if (!anonymity_)
					chat_form->RequestEnter(id);
				else
					chat_form->AnonymousLogin(id);
			}

			return true;
		}));
	}
}

void ChatroomFrontpage::OnWndSizeMax(bool max)
{
	if (!m_pRoot)
		return;

	FindControl(L"maxbtn")->SetVisible(!max);
	FindControl(L"restorebtn")->SetVisible(max);
}

void ChatroomFrontpage::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	{
		if (nim_ui::LoginManager::GetInstance()->IsEqual(iter->GetAccId()))
		{
			InitHeader();
			break;
		}
	}
}

void ChatroomFrontpage::OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path)
{
	if (type == kUser && nim_ui::LoginManager::GetInstance()->GetAccount() == account)
		FindControl(L"header_image")->SetBkImage(photo_path);
}

void ChatroomFrontpage::InitHeader()
{
	std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
	FindControl(L"header_image")->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(my_id));
	((ui::Label*)FindControl(L"name"))->SetText(nim_ui::UserManager::GetInstance()->GetUserName(my_id, false));
}

LRESULT ChatroomFrontpage::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (nim_comp::LoginManager::GetInstance()->IsAnonymityDemoMode())
		nim_ui::LoginManager::GetInstance()->DoLogout(false);
	return 0;
}

}