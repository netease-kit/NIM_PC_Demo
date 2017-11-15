
#include "screen_capture.h"
#include <shlobj.h> // SHGetFolderPathW
#include <shlwapi.h>
#pragma comment(lib,"Version.lib")
#pragma comment(lib,"Shlwapi.lib")
#include <sys/timeb.h>

namespace nim_comp
{
	
//遍历windows窗口
bool GetFileVersion(const wchar_t *file_path, WORD *major_version, WORD *minor_version, WORD *build_number, WORD *revision_number)
{
	DWORD handle, len;
	UINT buf_len;
	LPTSTR buf_data;
	VS_FIXEDFILEINFO *file_info;
	len = GetFileVersionInfoSize(file_path, &handle);
	if (0 == len)
		return false;

	buf_data = (LPTSTR)malloc(len);
	if (!buf_data)
		return false;

	if (!GetFileVersionInfo(file_path, handle, len, buf_data))
	{
		free(buf_data);
		return false;
	}
	if (VerQueryValue(buf_data, L"\\", (LPVOID *)&file_info, (PUINT)&buf_len))
	{
		*major_version = HIWORD(file_info->dwFileVersionMS);
		*minor_version = LOWORD(file_info->dwFileVersionMS);
		*build_number = HIWORD(file_info->dwFileVersionLS);
		*revision_number = LOWORD(file_info->dwFileVersionLS);
		free(buf_data);
		return true;
	}
	free(buf_data);
	return false;
}
int GetNTDLLVersion()
{
	static int ret = 0;
	if (ret == 0)
	{
		wchar_t buf_dll_name[MAX_PATH] = { 0 };
		HRESULT hr = ::SHGetFolderPathW(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, buf_dll_name);
		if (SUCCEEDED(hr) && ::PathAppendW(buf_dll_name, L"ntdll.dll"))
		{
			WORD major_version, minor_version, build_number, revision_number;
			GetFileVersion(buf_dll_name, &major_version, &minor_version, &build_number, &revision_number);
			ret = major_version * 100 + minor_version;
		}
	}
	return ret;
}
BOOL CALLBACK WindowsEnumerationHandler(HWND hwnd, LPARAM param) {
	CaptureWindowInfoList* list =
		reinterpret_cast<CaptureWindowInfoList*>(param);

	// Skip windows that are invisible, minimized, have no title, or are owned,
	// unless they have the app window style set.
	int len = GetWindowTextLength(hwnd);
	HWND owner = GetWindow(hwnd, GW_OWNER);
	LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (len == 0 || IsIconic(hwnd) || !IsWindowVisible(hwnd) ||
		(owner && !(exstyle & WS_EX_APPWINDOW))
		|| (exstyle & WS_EX_LAYERED)) {
		return TRUE;
	}

	// Skip the Program Manager window and the Start button.
	const size_t kClassLength = 256;
	WCHAR class_name[kClassLength];
	int class_name_length = GetClassName(hwnd, class_name, kClassLength);

	// Skip Program Manager window and the Start button. This is the same logic
	// that's used in Win32WindowPicker in libjingle. Consider filtering other
	// windows as well (e.g. toolbars).
	if (wcscmp(class_name, L"Progman") == 0 || wcscmp(class_name, L"Button") == 0)
		return TRUE;

	if (GetNTDLLVersion() >= 602 &&
		(wcscmp(class_name, L"ApplicationFrameWindow") == 0 ||
		wcscmp(class_name, L"Windows.UI.Core.CoreWindow") == 0)) {
		return TRUE;
	}

	CaptureWindowInfo window;
	window.id = hwnd;

	const size_t kTitleLength = 500;
	WCHAR window_title[kTitleLength];
	// Truncate the title if it's longer than kTitleLength.
	GetWindowText(hwnd, window_title, kTitleLength);
	window.title = window_title;

	// Skip windows when we failed to convert the title or it is empty.
	if (window.title.empty())
		return TRUE;

	list->push_back(window);

	return TRUE;
}
bool ScreenCapture::GetCaptureWindowList(CaptureWindowInfoList* windows)
{
	CaptureWindowInfoList result;
	LPARAM param = reinterpret_cast<LPARAM>(&result);
	if (!EnumWindows(&WindowsEnumerationHandler, param))
		return false;
	windows->swap(result);
	return true;
}

int ScreenCapture::GetDesktopScale()
{
	int desktop_scale = 100;
	HINSTANCE hUser32 = LoadLibrary(L"user32.dll");
	if (hUser32)
	{
		typedef BOOL(WINAPI* LPSetProcessDPIAware)(void);
		LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (pSetProcessDPIAware)
		{
			//去掉系统的放大，不应该调用
			//pSetProcessDPIAware();
			std::wstring value;
			HKEY hKEY;
			long ret = 0;
			ret = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"CONTROL PANEL\\DESKTOP\\WINDOWMETRICS", 0, KEY_ALL_ACCESS, &hKEY);
			if (ret != ERROR_SUCCESS)  //该键值不存在
			{
				::RegCloseKey(hKEY);
			}
			else
			{
				DWORD datasize = 4;
				int data;
				DWORD dType = REG_DWORD;
				ret = ::RegQueryValueEx(hKEY, L"AppliedDPI", NULL, &dType, (LPBYTE)&data, &datasize);
				if (ret != ERROR_SUCCESS)
				{
					::RegCloseKey(hKEY);
				}
				else
				{
					if (data > 0)
					{
						desktop_scale = data * 100 / 96;
					}
					::RegCloseKey(hKEY);
				}
			}
		}
		FreeLibrary(hUser32);
	}
	return desktop_scale;
}
ScreenCapture::ScreenCapture()
{
	capture_data_ = nullptr;
	capture_bitmap_ = nullptr;
	capture_hwnd_ = nullptr;
	capture_width_ = 0;
	capture_height_ = 0;
	cut_screen_ = false;
	cut_rect_ = { 0, 0, 0, 0 };
}

ScreenCapture::~ScreenCapture()
{
	//paint_timer_.Cancel();
	//nbase::NAutoLock auto_lock(&lock_);
	if (capture_bitmap_)
	{
		DeleteObject(capture_bitmap_);
		capture_bitmap_ = nullptr;
	}
}
void ScreenCapture::StartCapture(HWND id, int time_ms)
{
	//nbase::NAutoLock auto_lock(&lock_);
	capture_hwnd_ = id;
	//paint_timer_.Cancel();
	//StdClosure task = nbase::Bind(&ScreenCapture::CustomFrame, this);
	//nbase::ThreadManager::PostRepeatedTask(kThreadScreenCapture, paint_timer_.ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(time_ms));
}
void ScreenCapture::SetCutMode(bool cut, RECT rect)
{
	cut_screen_ = cut;
	cut_rect_ = rect;
}
void ScreenCapture::InitHBitmap(int width, int height)
{
	if (capture_width_ != width || capture_height_ != height)
	{
		if (capture_bitmap_)
		{
			DeleteObject(capture_bitmap_);
			capture_bitmap_ = nullptr;
		}
		capture_width_ = width;
		capture_height_ = height;
		capture_width_ -= capture_width_ % 2;
		capture_height_ -= capture_height_ % 2;
		HDC	hdc = ::GetDC(capture_hwnd_);
		HDC hMemDC = CreateCompatibleDC(hdc);
		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = capture_width_;
		bmi.bmiHeader.biHeight = capture_height_;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = capture_width_*capture_height_ * 4;
		capture_bitmap_ = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS,
			(void**)&capture_data_, NULL, 0);
		DeleteDC(hMemDC);
		::ReleaseDC(capture_hwnd_, hdc);
	}
	else if (capture_data_ != nullptr)
	{
		ZeroMemory(capture_data_, capture_width_*capture_height_ * 4);
	}
}

void ScreenCapture::CustomFrame()
{
	//nbase::NAutoLock auto_lock(&lock_);
	if (IsWindow(capture_hwnd_))
	{
		// 把屏幕设备描述表拷贝到内存设备描述表中
		HDC w_dc = GetDC(capture_hwnd_);
		if (w_dc)
		{
			RECT rcDlg;
			if (cut_screen_)
			{
				rcDlg = cut_rect_;
			} 
			else
			{
				::GetClientRect(capture_hwnd_, &rcDlg);
			}
			int width = rcDlg.right - rcDlg.left;
			int height = rcDlg.bottom - rcDlg.top;
			InitHBitmap(width, height);
			if (capture_bitmap_ == nullptr)
			{
				return;
			}
			HDC mem_dc = CreateCompatibleDC(w_dc);
			HBITMAP old_hbitmap = (HBITMAP)SelectObject(mem_dc, capture_bitmap_);
			//__int64 time0 = get_time_ms();
			if (cut_screen_)
			{
				BitBlt(mem_dc, 0, 0, capture_width_, capture_height_, w_dc, cut_rect_.left, cut_rect_.top, SRCCOPY /*| CAPTUREBLT*/);
			} 
			else
			{
				BitBlt(mem_dc, 0, 0, capture_width_, capture_height_, w_dc, 0, 0, SRCCOPY /*| CAPTUREBLT*/);
			}
			//__int64 time1 = get_time_ms();
			//鼠标
			if (1)
			{
				CURSORINFO pci;
				pci.cbSize = sizeof(CURSORINFO);
				GetCursorInfo(&pci);
				POINT ptCursor = pci.ptScreenPos;
				ICONINFO IconInfo = { 0 };
				if (GetIconInfo(pci.hCursor, &IconInfo))
				{
					ptCursor.x -= IconInfo.xHotspot;
					ptCursor.y -= IconInfo.yHotspot;
					if (NULL != IconInfo.hbmMask)
						DeleteObject(IconInfo.hbmMask);
					if (NULL != IconInfo.hbmColor)
						DeleteObject(IconInfo.hbmColor);
				}
				if (capture_hwnd_ != nullptr)
				{
					//RECT rcDlg;
					//::GetWindowRect(capture_hwnd_, &rcDlg);
					//ptCursor.x -= rcDlg.left;
					//ptCursor.y -= rcDlg.top;
					ScreenToClient(capture_hwnd_, &ptCursor);
				}
				if (cut_screen_)
				{
					ptCursor.x -= cut_rect_.left;
					ptCursor.y -= cut_rect_.top;
				}
				DrawIconEx(mem_dc, ptCursor.x, ptCursor.y, pci.hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);
			}
			SelectObject(mem_dc, old_hbitmap);
			DeleteDC(mem_dc);
			ReleaseDC(capture_hwnd_, w_dc);
			timeb time_now;
			ftime(&time_now); // 秒数
			__int64 cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒
			int wxh = capture_width_ * capture_height_;
			frame_.AddVideoFrame(true, cur_timestamp, (const char*)capture_data_, wxh * 4, \
				capture_width_, capture_height_, "", nim_comp::VideoFrameMng::Ft_ARGB_r);

			//VideoCaptureData(0, capture_data_, wxh * 4, capture_width_, capture_height_, "", nullptr);
			//static __int64 time_custom = get_time_ms();
			//static int num_video = 0;
			//static __int64 time_bitblt = 0; 
			//time_bitblt += time1 - time0;
			//num_video++;
			//__int64 time_cur = get_time_ms();
			//if (time_cur > time_custom + 1000)
			//{
			//	time_custom += 1000;
			//	if (time_cur > time_custom + 2000)
			//	{
			//		time_custom = time_cur;
			//	}
			//	_PRINT("custom video fps %d time %lld", num_video, time_bitblt/num_video);
			//	num_video = 0;
			//	time_bitblt = 0;
			//}
		}
	}
}

}