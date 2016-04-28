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
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserPhotoReady(nbase::Bind(&ChatroomFrontpage::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2)));

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

void ChatroomFrontpage::InvokeGetRoomList()
{
	FindControl(L"loading_tip")->SetVisible(true);

	auto http_cb = ToWeakCallback([this](bool ret, int response_code, const std::string& reply)
	{
		StdClosure error_cb = ToWeakCallback([this]()
		{
			FindControl(L"loading_tip")->SetVisible(false);
			FindControl(L"load_error_tip")->SetVisible(true);
		});

		if (!ret || response_code != nim::kNIMResSuccess)
		{
			QLOG_ERR(L"Invoke get room list error. Error code: {0}.") << response_code;
			Post2UI(error_cb);

			return;
		}

		Json::Value json_reply;
		Json::Reader reader;
		if (reader.parse(reply, json_reply) && json_reply.isObject())
		{
			int res = json_reply["res"].asInt();
			if (res != 200)
			{
				QLOG_ERR(L"Invoke get room list error. Json rescode: {0}.") << res;
				Post2UI(error_cb);

				return;
			}

			int count = json_reply["msg"]["total"].asInt();
			Json::Value json_list = json_reply["msg"]["list"];
			if (!json_list.isArray())
			{
				QLOG_ERR(L"Invoke get room list error. Reason: Not an array.");
				Post2UI(error_cb);

				return;
			}

			StdClosure closure = ToWeakCallback([this, json_list, count]() 
			{
				FindControl(L"loading_tip")->SetVisible(false);

				for (int i = 0; i < count; i++)
				{
					ChatRoomInfo info;
					info.id_ = json_list[i]["roomid"].asInt64();
					info.name_ = json_list[i]["name"].asString();
					info.creator_id_ = json_list[i]["creator"].asString();
					nbase::StringToInt(json_list[i]["onlineusercount"].asString(), &info.online_count_);
					info.valid_flag_ = json_list[i]["status"].asInt();
					info.ext_ = json_list[i]["ext"].asString();
					info.announcement_ = json_list[i]["announcement"].asString();
					info.broadcast_url_ = json_list[i]["broadcasturl"].asString();
					id_info_map_[info.id_] = info;

					CreateRoomItem(info);
				}
			});
			Post2UI(closure);
		}
	});

	std::string api_addr = "https://app.netease.im/api/chatroom/homeList";
	std::wstring server_conf_path = QPath::GetAppPath();
	server_conf_path.append(L"server_conf.txt");
	nim::SDKConfig config;
	TiXmlDocument document;
	if (shared::LoadXmlFromFile(document, server_conf_path))
	{
		TiXmlElement* root = document.RootElement();
		if (root)
		{
			if (auto pchar = root->Attribute("kNIMChatRoomAddress")) {
				api_addr = pchar;
			}
		}
	}
	std::string app_key = "45c6af3c98409b18a84451215d0bdd6e";
	std::string new_app_key = GetConfigValue(g_AppKey);
	if (!new_app_key.empty())
		app_key = new_app_key;
	nim_http::HttpRequest request(api_addr, "", 0, http_cb);
	request.AddHeader("Content-Type", "application/json; charset=utf-8");
	request.AddHeader("appKey", app_key);
	nim_http::PostRequest(request);
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
			online_count_label->SetText(nbase::StringPrintf(L"在线人数：%.1f万人", (float)room_info.online_count_ / (float)10000));
		else
			online_count_label->SetText(nbase::StringPrintf(L"在线人数：%d人", room_info.online_count_));

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
					ChatroomForm* form = static_cast<ChatroomForm*>(it);
					form->Close();
				}
			}
			__int64 id;
			nbase::StringToInt64(room_id, &id);
			if (0 == id)
				return true;
			
			chat_form = new ChatroomForm(id);
			if (chat_form != NULL)
			{
				chat_form->RequestEnter(id);
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

void ChatroomFrontpage::OnUserPhotoReady(const std::string& account, const std::wstring& photo_path)
{
	if (nim_ui::LoginManager::GetInstance()->GetAccount() == account)
		FindControl(L"header_image")->SetBkImage(photo_path);
}

void ChatroomFrontpage::InitHeader()
{
	std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
	nim_ui::UserManager* user_service = nim_ui::UserManager::GetInstance();
	FindControl(L"header_image")->SetBkImage(user_service->GetUserPhoto(my_id));
	((ui::Label*)FindControl(L"name"))->SetText(user_service->GetUserName(my_id, false));
}

}