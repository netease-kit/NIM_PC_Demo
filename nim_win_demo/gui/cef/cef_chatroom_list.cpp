#include "stdafx.h"
#include "cef_chatroom_list.h"
#include "cef_control/cef_control.h"
#include "gui/chatroom_form.h"

using namespace nim_chatroom;

CefChatroomList::CefChatroomList()
{

}

CefChatroomList::~CefChatroomList()
{

}

void CefChatroomList::DoInit()
{
	// 初始化 Cef Control
	cef_control_ = static_cast<ui::CefControl*>(FindSubControl(L"cef_control"));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefChatroomList::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachAfterCreated(nbase::Bind(&CefChatroomList::OnAfterCreated, this, std::placeholders::_1));
	cef_control_->AttachBeforeContextMenu(nbase::Bind(&CefChatroomList::OnBeforeContextMenu, this, std::placeholders::_1, std::placeholders::_2));
	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef_themes/chatroom_list/chatroom_list.html";
	if(ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
		html_path = L"file://" + QPath::GetAppPath() + L"cef_themes/chatroom_list_en/chatroom_list.html";
	cef_control_->LoadURL(html_path);
}

ui::Control* CefChatroomList::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefControl")
	{
		return new ui::CefControl;
	}

	return NULL;
}

void CefChatroomList::InvokChatroomList()
{
	app_sdk::AppSDKInterface::GetInstance()->InvokeGetChatroomList(ToWeakCallback([this](int code, const std::vector< nim_chatroom::ChatRoomInfo>& chatroom_list){
		if (code != nim::kNIMResSuccess)
		{
			QLOG_ERR(L"Invoke get room list error. Error code: {0}.") << code;
			return;
		}
		else
		{
			Json::Value root;
			for (auto it : chatroom_list)
			{
				Json::Value values;
				values["chatroomId"] = it.id_;
				values["chatroomName"] = it.name_;
				values["backgroundImage"] = it.broadcast_url_;
				values["numberOfOnline"] = it.online_count_;
				root.append(values);
			}

			cef_control_->CallJSFunction(L"onChatroomListChanged", nbase::UTF8ToUTF16(root.toStyledString()), nullptr);
		}
	}));
}

void CefChatroomList::ShowChatroomById(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(params, values) && values.isMember("chatroomId"))
	{
		auto chatroom_id = values["chatroomId"].asInt();
		if (0 == chatroom_id)
		{
			return;
		}

		ChatroomForm* chat_form = static_cast<ChatroomForm*>(nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomForm::kClassName, nbase::IntToString16(chatroom_id)));
		if (chat_form != NULL)
		{
			chat_form->ActiveWindow();
			return;
		}
		else
		{
			nim_comp::WindowList list = nim_ui::WindowsManager::GetInstance()->GetWindowsByClassName(ChatroomForm::kClassName);
			for (auto &it : list)
			{
				nim_comp::WindowsManager::SafeDoWindowOption<ChatroomForm, void>(it, [](ChatroomForm* form) {
					if (::IsWindow(form->GetHWND()))
						form->Close();
				});
			}
		}

		chat_form = new ChatroomForm(chatroom_id);
		if (chat_form != NULL)
		{
			chat_form->Create(NULL, ChatroomForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
			chat_form->RequestEnter(chatroom_id);
		}
	}
}

void CefChatroomList::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	cef_control_->RegisterCppFunc(L"showChatroomById", nbase::Bind(&CefChatroomList::ShowChatroomById, this, std::placeholders::_1, std::placeholders::_2));
}

void CefChatroomList::OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear();
}

void CefChatroomList::OnLoadEnd(int httpStatusCode)
{
	// 获取房间列表
	InvokChatroomList();
}
