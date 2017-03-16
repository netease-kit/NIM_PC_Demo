#ifndef UI_UTILS_APIWRAPPER_H_
#define UI_UTILS_APIWRAPPER_H_

#pragma once

namespace ui
{
#define WM_TOUCH                        0x0240
#define WM_GESTURE                      0x0119
#define WM_GESTURENOTIFY                0x011A

/*
 * RegisterTouchWindow flag values
 */
#define TWF_FINETOUCH       (0x00000001)
#define TWF_WANTPALM        (0x00000002)

/*
 * Conversion of touch input coordinates to pixels
 */
#define TOUCH_COORD_TO_PIXEL(l)         ((l) / 100)

/*
 * Touch input flag values (TOUCHINPUT.dwFlags)
 */
#define TOUCHEVENTF_MOVE            0x0001
#define TOUCHEVENTF_DOWN            0x0002
#define TOUCHEVENTF_UP              0x0004
#define TOUCHEVENTF_INRANGE         0x0008
#define TOUCHEVENTF_PRIMARY         0x0010
#define TOUCHEVENTF_NOCOALESCE      0x0020
#define TOUCHEVENTF_PEN             0x0040
#define TOUCHEVENTF_PALM            0x0080

/*
 * Touch input mask values (TOUCHINPUT.dwMask)
 */
#define TOUCHINPUTMASKF_TIMEFROMSYSTEM  0x0001  // the dwTime field contains a system generated value
#define TOUCHINPUTMASKF_EXTRAINFO       0x0002  // the dwExtraInfo field is valid
#define TOUCHINPUTMASKF_CONTACTAREA     0x0004  // the cxContact and cyContact fields are valid

	typedef enum PROCESS_DPI_AWARENESS {
		PROCESS_DPI_UNAWARE = 0,
		PROCESS_SYSTEM_DPI_AWARE = 1,
		PROCESS_PER_MONITOR_DPI_AWARE = 2
	} PROCESS_DPI_AWARENESS;

	typedef enum MONITOR_DPI_TYPE {
		MDT_EFFECTIVE_DPI = 0,
		MDT_ANGULAR_DPI = 1,
		MDT_RAW_DPI = 2,
		MDT_DEFAULT = MDT_EFFECTIVE_DPI
	} MONITOR_DPI_TYPE;

	DECLARE_HANDLE(HTOUCHINPUT);
	typedef struct tagTOUCHINPUT {
		LONG x;
		LONG y;
		HANDLE hSource;
		DWORD dwID;
		DWORD dwFlags;
		DWORD dwMask;
		DWORD dwTime;
		ULONG_PTR dwExtraInfo;
		DWORD cxContact;
		DWORD cyContact;
	} TOUCHINPUT, *PTOUCHINPUT;
	typedef TOUCHINPUT const * PCTOUCHINPUT;


	bool GetDpiForSystemWrapper(UINT& dpi);
	bool GetDpiForMonitorWrapper(HMONITOR hMonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);

	bool SetProcessDpiAwarenessWrapper(PROCESS_DPI_AWARENESS value);
	bool SetProcessDPIAwareWrapper();

	bool GetProcessDpiAwarenessWrapper(PROCESS_DPI_AWARENESS& awareness);
	bool IsProcessDPIAwareWrapper(bool& bAware);

	bool RegisterTouchWindowWrapper(HWND hwnd, ULONG ulFlags);
	bool UnregisterTouchWindowWrapper(HWND hwnd);

	bool GetTouchInputInfoWrapper(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize);
	bool CloseTouchInputHandleWrapper(HTOUCHINPUT hTouchInput); 
}
#endif //UI_UTILS_APIWRAPPER_H_