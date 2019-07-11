#include "stdafx.h"
#include "session_plugin_page.h"
#include "module\session\session_manager.h"
#include "gui\session\session_box.h"
#include "export\nim_ui_session_list_manager.h"
using namespace nim_comp;
SessionPluginPage::SessionPluginPage() :
active_session_box_(nullptr)
{
	
}
SessionPluginPage::~SessionPluginPage()
{
	if (session_box_tab_ != nullptr)
		session_box_tab_->GetWindow()->RemoveMessageFilter(this);
}
void SessionPluginPage::DoInit()
{
	session_box_tab_ = dynamic_cast<ui::TabBox*>(FindSubControl(L"session_container"));
	if (session_box_tab_ != nullptr)
		session_box_tab_->GetWindow()->AddMessageFilter(this);
	GetPlugin()->AttachSelect([this](ui::EventArgs* param) {
		if (active_session_box_ != nullptr)
		{
			BOOL handle = false;
			active_session_box_->HandleMessage(WM_ACTIVATE, WA_ACTIVE, 0, handle);
			//::SetActiveWindow(NULL);
			auto session_id = active_session_box_->GetSessionId();
			nbase::ThreadManager::PostTask(active_session_box_->ToWeakCallback([this, session_id]() {
				//::SetActiveWindow(GetHWND());
				nim_ui::SessionListManager::GetInstance()->InvokeSelectSessionItem(session_id, true, false);
			}));			
		}
		return true;
	});
}
void SessionPluginPage::OnSessionListAttached()
{
	unregister_cb.Add(nim_ui::SessionListManager::GetInstance()->RegAddItem(ToWeakCallback([this](const std::string& id){
		if (IsActiveSessionBox(nbase::UTF8ToUTF16(id)))
			nim_ui::SessionListManager::GetInstance()->InvokeSelectSessionItem(id);
	})));
	unregister_cb.Add(nim_ui::SessionListManager::GetInstance()->RegRemoveItem(ToWeakCallback([this](const std::string& id){
		CloseSessionBox(id);
	})));
}
LRESULT SessionPluginPage::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT ret = S_FALSE;
	auto active_session_box = GetSelectedSessionBox();
	if (active_session_box == nullptr)
		return S_FALSE;
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == 'V')
		{
			if (::GetKeyState(VK_CONTROL) < 0)
			{
				active_session_box->HandleMessage(uMsg, wParam, lParam, bHandled);
				if(bHandled)
					return S_OK;
			}
		}
	}
	if (uMsg == WM_NOTIFY)
	{
		if (wParam == EN_LINK)
		{
			std::wstring url = *(std::wstring*)lParam;
			if (!url.empty())
			{
				std::wstring ws = url;
				nbase::LowerString(ws);
				// 以"file:"开头 或者 包含".." 的超链接不允许打开
				if (!(ws.find(L"file:", 0, 5) == 0 || ws.find(L"..") != std::wstring::npos))
				{
					Post2GlobalMisc(nbase::Bind(&shared::tools::SafeOpenUrl, url, SW_SHOW));
				}			
			}
			bHandled = true;
			ret = S_OK;
		}
	}
	else if (uMsg == WM_DROPFILES)
	{
		//Run desktop helper from 360 or tencent
		QLOG_APP(L"##Receive dropfiles msg.");
		POINT pt;
		GetCursorPos(&pt);
		POINTL ppt;
		ppt.x = pt.x;
		ppt.y = pt.y;
		
		if (NULL != active_session_box && active_session_box->CheckDropEnable(ppt))
			active_session_box->OnDropFile((HDROP)wParam);
		return 0;
	}
	else if (uMsg == WM_KEYDOWN && wParam == VK_RETURN)
	{
		if (::GetKeyState(VK_CONTROL) >= 0)
		{
			if (RunTimeDataManager::GetInstance()->IsAttingSomeOne())
			{
				bHandled = true;
				return S_OK;
			}
		}
	}
	if (NULL != active_session_box)
	{	
		if (uMsg == WM_CHAR)
		{
			if (wParam != VK_BACK)
			{
				if (lParam == 0xFFFF)
				{
					bHandled = true;
					return S_OK;
				}					
				else
				{
					lParam = 0xFFFF;
				}					
			}
		}
		BOOL handle = false;
		ret = active_session_box->HandleMessage(uMsg, wParam, lParam, handle);
		if (handle)
			return ret;
	}
	return ret;
}