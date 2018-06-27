#include "stdafx.h"
#include "session_plugin_page.h"
#include "module\session\session_manager.h"
#include "gui\session\session_box.h"
#include "export\nim_ui_session_list_manager.h"
using namespace nim_comp;
SessionPluginPage::SessionPluginPage()
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
LRESULT SessionPluginPage::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT ret = S_FALSE;
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
	return ret;
}