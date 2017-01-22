#include "taskbar_manager.h"
#include "gui/session/session_form.h"
#include "gui/session/session_box.h"
#include "dwm_util.h"
#include <shobjidl.h>

using namespace ui;
namespace nim_comp
{
TaskbarTabItem::TaskbarTabItem(ui::Control *bind_control)
{
	ASSERT(NULL != bind_control);
	bind_control_ = bind_control;
	is_win7_or_greater_ = IsWindows7OrGreater();
	taskbar_manager_ = NULL;
}

ui::Control* TaskbarTabItem::GetBindControl()
{
	return bind_control_;
}

void TaskbarTabItem::Init(const std::wstring &taskbar_title)
{
	if (!is_win7_or_greater_)
		return;

	Create(NULL, taskbar_title.c_str(), WS_OVERLAPPED, 0, false);

	HRESULT ret = S_OK;
	BOOL truth = TRUE;
	ret |= DwmSetWindowAttribute(m_hWnd, DWMWA_HAS_ICONIC_BITMAP, &truth, sizeof(truth));
	ret |= DwmSetWindowAttribute(m_hWnd, DWMWA_FORCE_ICONIC_REPRESENTATION, &truth, sizeof(truth));
	if (ret != S_OK)
	{
		is_win7_or_greater_ = false;
		QLOG_ERR(L"DwmSetWindowAttribute error: {0}") << ret;
	}
}

void TaskbarTabItem::UnInit()
{
	if (NULL != m_hWnd)
		DestroyWindow(m_hWnd);
}

void TaskbarTabItem::SetTaskbarManager(TaskbarManager *taskbar_manager)
{
	taskbar_manager_ = taskbar_manager;
}

nim_comp::TaskbarManager* TaskbarTabItem::GetTaskbarManager()
{
	return taskbar_manager_;
}

bool TaskbarTabItem::InvalidateTab()
{
	if (!is_win7_or_greater_ || NULL == taskbar_manager_)
		return false;

	return (S_OK == DwmInvalidateIconicBitmaps(this->GetHWND()));
}

void TaskbarTabItem::OnSendThumbnail(int width, int height)
{
	if (!is_win7_or_greater_ || NULL == taskbar_manager_)
		return;

	HBITMAP bitmap = taskbar_manager_->GenerateBindControlBitmap(bind_control_, width, height);
	DwmSetIconicThumbnail(m_hWnd, bitmap, 0);

	DeleteObject(bitmap);
}

void TaskbarTabItem::OnSendPreview()
{
	if (!is_win7_or_greater_ || NULL == taskbar_manager_)
		return;

	HBITMAP bitmap = taskbar_manager_->GenerateBindControlBitmapWithForm(bind_control_);
	DwmSetIconicLivePreviewBitmap(m_hWnd, bitmap, NULL, 0);

	DeleteObject(bitmap);
}

std::wstring TaskbarTabItem::GetWindowClassName() const
{
	return L"Nim.TaskbarItem";
}

LRESULT TaskbarTabItem::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DWMSENDICONICTHUMBNAIL)
	{
		OnSendThumbnail(HIWORD(lParam), LOWORD(lParam));
		return 0;
	}
	else if (uMsg == WM_DWMSENDICONICLIVEPREVIEWBITMAP)
	{
		OnSendPreview();
		return 0;
	}
	else if (uMsg == WM_GETICON)
	{
		InvalidateTab();
	}
	else if (uMsg == WM_CLOSE)
	{
		if (NULL != taskbar_manager_)
			taskbar_manager_->OnTabItemClose(*this);

		return 0;
	}
	else if (uMsg == WM_ACTIVATE)
	{
		if (NULL != taskbar_manager_)
		{
			if (wParam != WA_INACTIVE)
			{
				taskbar_manager_->OnTabItemClicked(*this);
			}
		}
			
		return 0;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void TaskbarTabItem::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TaskbarManager::TaskbarManager()
{
	parent_window_ = NULL;
	taskbar_list_ = NULL;
}

void TaskbarManager::Init(SessionForm *parent_window)
{
	ASSERT(NULL != parent_window);
	parent_window_ = parent_window;

	::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&taskbar_list_));
	if (taskbar_list_)
	{
		taskbar_list_->HrInit();

		BOOL truth = FALSE;
		DwmSetWindowAttribute(parent_window->GetHWND(), DWMWA_HAS_ICONIC_BITMAP, &truth, sizeof(truth));
		DwmSetWindowAttribute(parent_window->GetHWND(), DWMWA_FORCE_ICONIC_REPRESENTATION, &truth, sizeof(truth));
	}
		
}

bool TaskbarManager::RegisterTab(TaskbarTabItem &tab_item)
{
	if (taskbar_list_ && NULL == tab_item.GetTaskbarManager())
	{
		if (S_OK == taskbar_list_->RegisterTab(tab_item.GetHWND(), parent_window_->GetHWND()))
		{
			if (S_OK == taskbar_list_->SetTabOrder(tab_item.GetHWND(), NULL))
			{
				tab_item.SetTaskbarManager(this);
				return true;
			}			
		}
	}

	return false;
}

bool TaskbarManager::UnregisterTab(TaskbarTabItem &tab_item)
{
	if (taskbar_list_)
	{
		tab_item.SetTaskbarManager(NULL);
		return (S_OK == taskbar_list_->UnregisterTab(tab_item.GetHWND()));
	}
	else
		return false;
}

bool TaskbarManager::SetTabOrder(const TaskbarTabItem &tab_item, const TaskbarTabItem &tab_item_insert_before)
{
	if (taskbar_list_)
	{
		return (S_OK == taskbar_list_->SetTabOrder(tab_item.GetHWND(), tab_item_insert_before.GetHWND()));
	}
	else
		return false;
}

bool TaskbarManager::SetTabActive(const TaskbarTabItem &tab_item)
{
	if (taskbar_list_)
	{
		return (S_OK == taskbar_list_->SetTabActive(tab_item.GetHWND(), parent_window_->GetHWND(), 0));
	}
	else
		return false;
}

HBITMAP TaskbarManager::GenerateBindControlBitmapWithForm(ui::Control *control)
{
	ASSERT( NULL != control);
	if ( NULL == control)
		return NULL;

	WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
	::GetWindowPlacement(parent_window_->GetHWND(), &placement);
	int window_width = placement.rcNormalPosition.right - placement.rcNormalPosition.left;
	int window_height = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top;

	// 1.创建内存dc
	unsigned int *bitmapBits = NULL;
	HDC hPaintDC = ::CreateCompatibleDC(parent_window_->GetPaintDC());
	HBITMAP hPaintBitmap = RenderEngine::CreateDIBBitmap(parent_window_->GetPaintDC(), window_width, window_height, (LPVOID*)&bitmapBits);
	ASSERT(hPaintDC);
	ASSERT(hPaintBitmap);
	HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);

	// 2.把窗口双缓冲的位图画到内存dc
	HDC hParentWindowDC = ::CreateCompatibleDC(parent_window_->GetPaintDC());
	ASSERT(hParentWindowDC);
	HBITMAP hOldParentBitmap = (HBITMAP)::SelectObject(hParentWindowDC, parent_window_->GetBackgroundBitmap());

	::BitBlt(hPaintDC, 0, 0, window_width, window_height, hParentWindowDC, 0, 0, SRCCOPY);
	::SelectObject(hParentWindowDC, hOldParentBitmap);
	::DeleteDC(hParentWindowDC);

	// 3.把某个会话盒子的位图画到内存dc，覆盖原窗口对应位置的位图
	UiRect rcPaint = control->GetPos();
	if (rcPaint.IsRectEmpty())
		return NULL;
	rcPaint.Intersect(UiRect(0, 0, window_width, window_height));

	// 这里不设置剪裁区域，就无法正常绘制
	{
		RenderClip rectClip;
		rectClip.GenerateClip(hPaintDC, rcPaint, true);

		bool visible = control->IsInternVisible();
		control->SetInternVisible(true);
		control->Paint(hPaintDC, rcPaint);
		control->SetInternVisible(visible);
	}

	// 4.修复绘制区域的alpha通道
	for (int i = rcPaint.top; i < rcPaint.bottom; i++) {
		for (int j = rcPaint.left; j < rcPaint.right; j++) {
			BYTE* alpha = (BYTE*)(bitmapBits + i * window_width + j) + 3;
			*alpha = 255;
		}
	}

	// 5.清理工作
	::SelectObject(hPaintDC, hOldPaintBitmap);
	::DeleteDC(hPaintDC);

	return hPaintBitmap;
}

HBITMAP TaskbarManager::GenerateBindControlBitmap(ui::Control *control, const int dest_width, const int dest_height)
{
	ASSERT(dest_width > 0 && dest_height > 0 && NULL != control);
	if (dest_width <= 0 || dest_height <= 0 || NULL == control)
		return NULL;

	WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
	::GetWindowPlacement(parent_window_->GetHWND(), &placement);
	int window_width = placement.rcNormalPosition.right - placement.rcNormalPosition.left;
	int window_height = placement.rcNormalPosition.bottom - placement.rcNormalPosition.top;

	// 1.创建内存dc
	unsigned int *bitmapBits = NULL;
	HDC hPaintDC = ::CreateCompatibleDC(parent_window_->GetPaintDC());
	HBITMAP hPaintBitmap = RenderEngine::CreateDIBBitmap(parent_window_->GetPaintDC(), window_width, window_height, (LPVOID*)&bitmapBits);
	ASSERT(hPaintDC);
	ASSERT(hPaintBitmap);
	HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);

	// 2.把某个会话盒子的位图画到内存dc，覆盖原窗口对应位置的位图
	UiRect rcPaint = control->GetPos();
	if (rcPaint.IsRectEmpty())
		return NULL;
	rcPaint.Intersect(UiRect(0, 0, window_width, window_height));

	// 这里不设置剪裁区域，就无法正常绘制
	{
		RenderClip rectClip;
		rectClip.GenerateClip(hPaintDC, rcPaint, true);

		bool visible = control->IsInternVisible();
		control->SetInternVisible(true);
		control->Paint(hPaintDC, rcPaint);
		control->SetInternVisible(visible);
	}

	// 3.修复绘制区域的alpha通道
	for (int i = rcPaint.top; i < rcPaint.bottom; i++) {
		for (int j = rcPaint.left; j < rcPaint.right; j++) {
			BYTE* alpha = (BYTE*)(bitmapBits + i * window_width + j) + 3;
			*alpha = 255;
		}
	}

	// 4.缩放到目标尺寸
	UiRect rcControl = control->GetPos();
	HBITMAP hBitmap = ResizeBitmap(dest_width, dest_height, hPaintDC, rcControl.left, rcControl.top, rcControl.GetWidth(), rcControl.GetHeight());

	// 5.清理工作
	::SelectObject(hPaintDC, hOldPaintBitmap);
	::DeleteObject(hPaintBitmap);
	::DeleteDC(hPaintDC);

	return hBitmap;
}

HBITMAP TaskbarManager::ResizeBitmap(int dest_width, int dest_height, HDC src_dc, int src_x, int src_y, int src_width, int src_height)
{
	LPDWORD pDest = NULL;
	HDC hCloneDC = ::CreateCompatibleDC(parent_window_->GetPaintDC());
	HBITMAP hBitmap = RenderEngine::CreateDIBBitmap(parent_window_->GetPaintDC(), dest_width, dest_height, (LPVOID*)&pDest);
	ASSERT(hCloneDC);
	ASSERT(hBitmap);

	if (hBitmap != NULL)
	{
		int scale_width = 0;
		int scale_height = 0;

		float src_scale = (float)src_width / (float)src_height;
		float dest_scale = (float)dest_width / (float)dest_height;
		if (src_scale >= dest_scale)
		{
			scale_width = dest_width;
			scale_height = (int)(dest_width * (float)src_height / (float)src_width);
		}
		else
		{
			scale_height = dest_height;
			scale_width = (int)(dest_height * (float)src_width / (float)src_height);
		}

		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
		BLENDFUNCTION ftn = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		::AlphaBlend(hCloneDC, (dest_width - scale_width) / 2, (dest_height - scale_height) / 2, scale_width, scale_height, src_dc, src_x, src_y, src_width, src_height, ftn);
		::SelectObject(hCloneDC, hOldBitmap);
		::DeleteDC(hCloneDC);
	}

	return hBitmap;
}

void TaskbarManager::OnTabItemClose(TaskbarTabItem &tab_item)
{
	ui::Control *control = tab_item.GetBindControl();
	if (control)
	{
		SessionBox *session_box = dynamic_cast<SessionBox*>(control);
		if (session_box)
		{
			parent_window_->CloseSessionBox(session_box->GetSessionId());
		}
	}
}

void TaskbarManager::OnTabItemClicked(TaskbarTabItem &tab_item)
{
	ui::Control *control = tab_item.GetBindControl();
	if (control)
	{
		SessionBox *session_box = dynamic_cast<SessionBox*>(control);
		if (session_box)
		{
			parent_window_->SetActiveSessionBox(session_box->GetSessionId());
		}
	}
}

}