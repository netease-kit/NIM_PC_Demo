#pragma once
#include "duilib/UIlib.h"

namespace nim_comp
{
class WindowEx : public ui::WindowImplBase
{
public:
	WindowEx();
	virtual ~WindowEx();

	virtual HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, 
		bool isLayeredWindow = true, const ui::UiRect& rc = ui::UiRect(0, 0, 0, 0)) override;
	
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnEsc(BOOL &bHandled);
		
	virtual std::wstring GetWindowClassName(void) const = 0;
	virtual std::wstring GetWindowId(void) const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ActiveWindow();  //¼¤»î´°¿Ú
	void SetTaskbarTitle(const std::wstring &title);

private:
	bool RegisterWnd();
	void UnRegisterWnd();
};


POINT GetPopupWindowPos(WindowEx* window);

void ToTopMost(HWND hwnd, bool forever);
}