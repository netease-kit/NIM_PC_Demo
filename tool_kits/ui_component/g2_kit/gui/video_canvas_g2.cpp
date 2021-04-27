#include "pch.h"
#include "video_canvas_g2.h"


namespace ui {
#define CANVAS_CLASS_NAME  L"CanvasG2"
#define CANVAS_WND_NAME  L"CanvasG2Wnd"

	VideoCanvasG2::VideoCanvasG2(HWND parent):canvas_wnd_(NULL)
	{
		parent_wnd_ = parent;
		AttachResize(nbase::Bind(&VideoCanvasG2::OnResize, this, std::placeholders::_1));

		
		WNDCLASS wc = { 0 };
		wc.style = CS_DBLCLKS;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = NULL;
		wc.lpfnWndProc = VideoCanvasG2::__WndProc;
		wc.hInstance = ::GetModuleHandle(NULL);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		std::wstring className = CANVAS_CLASS_NAME;
		wc.lpszClassName = className.c_str();
		ATOM ret = ::RegisterClass(&wc);
		int err = ::GetLastError();
		ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
		
		canvas_wnd_ = ::CreateWindow(
			CANVAS_CLASS_NAME,
			L"",
			WS_CHILD|WS_VISIBLE,
			10, 10, 100, 100,
			parent_wnd_,
			NULL,
			::GetModuleHandle(NULL),
			this
		);
		if (!canvas_wnd_)
		{
			int lastErr = ::GetLastError();
			QLOG_ERR(L"VideoCanvasG2 CreateWindowEx lastErr: {0}") << lastErr;
			return;
		}
		//ShowCanvas(false);
	}
	LRESULT CALLBACK VideoCanvasG2::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_ERASEBKGND:
			return S_OK;
			break;
		default:
			break;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	VideoCanvasG2::~VideoCanvasG2()
	{
		if (canvas_wnd_)
		{
			::DestroyWindow(canvas_wnd_);
			canvas_wnd_ = NULL;
		}
	}
	void VideoCanvasG2::ShowCanvas(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
	{
		::ShowWindow(canvas_wnd_, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
		::UpdateWindow(canvas_wnd_);

	}
	void VideoCanvasG2::UpdateCanvasPos()
	{
		//TODO 优化调整方式
		auto selfRc = GetPos();
		POINT point = { selfRc.left, selfRc.top};
		auto wid = selfRc.GetWidth();
		auto hei = selfRc.GetHeight();
		//ClientToScreen(GetWindow()->GetHWND(), &point);
		::MoveWindow(canvas_wnd_, point.x, point.y, selfRc.GetWidth(), selfRc.GetHeight() - 45,true);
		

	}
	HWND VideoCanvasG2::GetCanvasWndHandle()
	{
		return canvas_wnd_;
	}

	bool VideoCanvasG2::OnResize(ui::EventArgs* args)
	{
		if (canvas_wnd_)
			UpdateCanvasPos();
		return true;
	}
}