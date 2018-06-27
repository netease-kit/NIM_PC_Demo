#include "stdafx.h"
#include "session_plugin_page.h"
#include "gui/session/session_box.h"
#include "export\nim_ui_session_list_manager.h"
#include "api\nim_cpp_session.h"
using namespace nim_comp;
HWND SessionPluginPage::Create()
{
	return ui::Box::GetWindow()->GetHWND();
}
void SessionPluginPage::CenterWindow()
{
	//ui::Box::GetWindow()->CenterWindow();
}
HWND SessionPluginPage::GetHWND()
{
	return ui::Box::GetWindow()->GetHWND();
}
ui::UiRect SessionPluginPage::GetPos(bool bContainShadow) const
{
	return ui::Box::GetPos(bContainShadow);
}
void SessionPluginPage::SetPos(const ui::UiRect& rc, bool bNeedDpiScale, UINT uFlags, HWND hWndInsertAfter, bool bContainShadow)
{
	ui::Box::GetWindow()->SetPos(rc, bNeedDpiScale, uFlags, hWndInsertAfter, bContainShadow);
}
void SessionPluginPage::ActiveWindow()
{
	dynamic_cast<WindowEx*>(ui::Box::GetWindow())->ActiveWindow();
}
LRESULT SessionPluginPage::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ui::Box::GetWindow()->PostMessage(uMsg, wParam, lParam);
}
void SessionPluginPage::SetTaskbarTitle(const std::wstring &title)
{
}
void SessionPluginPage::SetTaskbarIcon(const std::wstring &icon)
{
}
SessionBox* SessionPluginPage::CreateSessionBox(const std::string &session_id, nim::NIMSessionType session_type)
{
	SessionBox* session_box = new SessionBox(session_id, session_type);
	ui::GlobalManager::FillBoxWithCache(session_box, L"session/session_box.xml");
	std::wstring id = nbase::UTF8ToUTF16(session_id);
	session_box->SetName(id);
	session_box_tab_->Add(session_box);
	session_box->InitSessionBox();
	session_box_tab_->SelectItem(session_box);
	SetActiveSessionBox(session_id);
	return session_box;
}
void SessionPluginPage::CloseSessionBox(const std::string &session_id)
{
	auto task = [this,session_id](){
		auto box = GetSessionBoxByID(session_id);
		if (box != nullptr)
		{
			if (IsActiveSessionBox(box))
				session_box_tab_->SelectItem(0);
			box->UninitSessionBox();			
			session_box_tab_->Remove(box);
		}
	};
	Post2UI(ToWeakCallback(task));	
}
bool SessionPluginPage::AttachSessionBox(SessionBox *session_box)
{
	return false;
}
bool SessionPluginPage::DetachSessionBox(SessionBox *session_box)
{
	return false;
}
SessionBox* SessionPluginPage::GetSelectedSessionBox()
{
	return dynamic_cast<SessionBox*>(session_box_tab_->GetItemAt(session_box_tab_->GetCurSel()));;
}
SessionBox* SessionPluginPage::GetSessionBoxByID(const std::string& session_id) const
{
	SessionBox* ret = nullptr;
	std::wstring id = nbase::UTF8ToUTF16(session_id);
	for (int i = 0; i < session_box_tab_->GetCount(); i++)
	{
		Control *box_item = session_box_tab_->GetItemAt(i);
		if (box_item->GetName() == id)
		{
			ret = dynamic_cast<SessionBox*>(box_item);
			break;
		}
	}
	return ret;
}
void SessionPluginPage::SetActiveSessionBox(const std::string &session_id)
{
	auto box = dynamic_cast<SessionBox*>(GetSessionBoxByID(session_id));
	if (box != nullptr)
	{
		GetPlugin()->Selected(true, true);
		session_box_tab_->SelectItem(box); bool handle = false;
		box->HandleMessage(WM_ACTIVATE, WA_ACTIVE, 0, handle);
		Post2UI(box->ToWeakCallback([session_id, box](){
			if (nim_ui::SessionListManager::GetInstance()->CheckSessionItem(session_id))
				nim_ui::SessionListManager::GetInstance()->InvokeSelectSessionItem(session_id);
			else
				nim_ui::SessionListManager::GetInstance()->InvokeSelectSessionItem(session_id, box->GetSessionType());
		}));		
	}
}
bool SessionPluginPage::IsActiveSessionBox(const SessionBox *session_box)
{
	if (session_box == nullptr)
		return false;
	return session_box_tab_->GetCurSel() == session_box_tab_->GetItemIndex((ui::Control*)session_box);
}
bool SessionPluginPage::IsActiveSessionBox(const std::wstring &session_id)
{
	return IsActiveSessionBox(GetSessionBoxByID(nbase::UTF16ToUTF8(session_id)));
}
int SessionPluginPage::GetSessionBoxCount() const
{
	return 0;
}
void SessionPluginPage::OnBeforeDragSessionBoxCallback(const std::wstring &session_id)
{
}
void SessionPluginPage::OnAfterDragSessionBoxCallback(bool drop_succeed)
{
}
void SessionPluginPage::InvokeSetSessionUnread(const std::string &id, int unread)
{
}
void SessionPluginPage::SetMergeItemName(const std::wstring &session_id, const std::wstring &name)
{
}
void SessionPluginPage::SetMergeItemHeaderPhoto(const std::wstring &session_id, const std::wstring &photo)
{
}
void SessionPluginPage::OnNewMsg(SessionBox &session_box, bool create, bool flash)
{
}
void SessionPluginPage::AdjustFormSize()
{
}
LRESULT SessionPluginPage::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto selected_session = dynamic_cast<SessionBox*>(session_box_tab_->GetItemAt(session_box_tab_->GetCurSel()));
	if (selected_session != nullptr)
	{
		bool bHandle = false;
		LRESULT ret = selected_session->HandleMessage(uMsg, wParam, lParam, bHandle);
		if (bHandle)
			return ret;
	}
	return ui::Box::GetWindow()->HandleMessage(uMsg, wParam, lParam);
}
LRESULT SessionPluginPage::HostWindowHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ui::Box::GetWindow()->HandleMessage(uMsg, wParam, lParam);
}