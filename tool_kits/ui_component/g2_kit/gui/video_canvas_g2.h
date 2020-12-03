#pragma once

#include "tool_kits/duilib/Core/Window.h"


namespace ui
{
	/*
	* G2画布控件,内部维护一个子窗口句柄，供G2渲染视频画面
	*/
	class VideoCanvasG2 : public ui::Box
	{
	public:
		VideoCanvasG2(HWND parent);
		~VideoCanvasG2();

		void ShowCanvas(bool bShow = true, bool bTakeFocus = false);
		void UpdateCanvasPos();
		HWND GetCanvasWndHandle();
	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		bool OnResize(ui::EventArgs* args);
		HWND    canvas_wnd_;
		HWND	parent_wnd_;
	};
}
