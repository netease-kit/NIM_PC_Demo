#pragma once
#include "gui/session/session_dock_def.h"
#include "module/plugins/main_plugins_base.h"
namespace nim_comp
{
	class SessionBox;
	class SessionPluginPage : public MainPluginPage, public ISessionDock, public ui::IUIMessageFilter
	{
	public:
		SessionPluginPage();
		virtual ~SessionPluginPage();
	public:
		virtual HWND Create() override;
		virtual void CenterWindow() override;
		virtual HWND GetHWND() override;
		virtual ui::UiRect GetPos(bool bContainShadow = false) const override;
		virtual void SetPos(const ui::UiRect& rc, bool bNeedDpiScale, UINT uFlags, HWND hWndInsertAfter = NULL, bool bContainShadow = false) override;
		virtual void ActiveWindow() override;
		virtual LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L) override;		
		virtual void SetTaskbarTitle(const std::wstring &title) override;	
		virtual void SetTaskbarIcon(const std::wstring &icon) override;		
		virtual SessionBox* CreateSessionBox(const std::string &session_id, nim::NIMSessionType session_type) override;
		virtual void CloseSessionBox(const std::string &session_id) override;
		virtual bool AttachSessionBox(SessionBox *session_box) override;
		virtual bool DetachSessionBox(SessionBox *session_box) override;
		virtual SessionBox* GetSelectedSessionBox() override;
		virtual void SetActiveSessionBox(const std::string &session_id) override;
		virtual bool IsActiveSessionBox(const SessionBox *session_box) override;
		virtual bool IsActiveSessionBox(const std::wstring &session_id) override;
		virtual int GetSessionBoxCount() const override;
		virtual void OnBeforeDragSessionBoxCallback(const std::wstring &session_id) override;
		virtual void OnAfterDragSessionBoxCallback(bool drop_succeed) override;
		virtual void InvokeSetSessionUnread(const std::string &id, int unread) override;
		virtual void SetMergeItemName(const std::wstring &session_id, const std::wstring &name) override;
		virtual void SetMergeItemHeaderPhoto(const std::wstring &session_id, const std::wstring &photo) override;
		virtual void OnNewMsg(SessionBox &session_box, bool create, bool flash) override;
		virtual void AdjustFormSize() override;
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT HostWindowHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	public:
		virtual void DoInit() override;
		void OnSessionListAttached();
	private:		
		SessionBox* GetSessionBoxByID(const std::string& session_id) const;
	private:
		ui::TabBox* session_box_tab_;
		SessionBox* active_session_box_;
		AutoUnregister	unregister_cb;
	};
}