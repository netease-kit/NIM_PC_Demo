#ifndef CAPTURE_MANAGER_H
#define CAPTURE_MANAGER_H

#include "capture_wnd.h"
#include "base/memory/singleton.h"

class CaptureManager
{
public:
	SINGLETON_DEFINE(CaptureManager);
	typedef std::function<void(bool, const std::wstring&)> CaptureCallback;

	CaptureManager();
	~CaptureManager();

	bool StartCapture(CaptureCallback callback, const std::wstring& user_data_dir, const std::wstring& send_info = L"");
	bool EndCapture(std::wstring track_image_path);

	bool IsCaptureTracking() {return capture_tracking_;}
	//void DoClipByHotkey();
	//void AddClipHotkeyCallback(CaptureHotkeyCallback cb, std::weak_ptr<nbase::WeakFlag> flag);

private:
	bool capture_tracking_;//ÕýÔÚ½ØÍ¼
	CaptureCallback capture_callback_;
	//std::vector<Hotkey_Callback_Info> capture_hotkey_cb_list_;
};

#endif //CAPTURE_MANAGER_H