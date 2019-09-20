#include "capture_manager.h"
#include "base/encrypt/encrypt.h"
#include "base/encrypt/encrypt_impl.h"
#include "base/memory/singleton.h"
#include "base/thread/thread_manager.h"
#include "shared/threads.h"

using namespace nbase;

CaptureManager::CaptureManager()
{
	capture_tracking_ = false;
}

CaptureManager::~CaptureManager()
{
}

bool CaptureManager::StartCapture(CaptureCallback callback, const std::wstring& user_data_dir, const std::wstring& send_info)
{
	if (!capture_tracking_)
	{
		capture_tracking_ = true;
		capture_callback_ = callback;
		CaptureWindow* capture_window  = new CaptureWindow(user_data_dir);
		capture_window->SetSendInfo(send_info);
		if (capture_window->Create(NULL, L"CaptureWindow", WS_POPUP, 0, false))
		{
			capture_window->ShowWindow();
			return true;
		}
		else
		{
			delete capture_window;
			capture_window = NULL;
		}
	}
	return false;
}
bool CaptureManager::EndCapture(std::wstring track_image_path)
{
	if (capture_tracking_)
	{
		capture_tracking_ = false;
		if (capture_callback_)
		{
			capture_callback_(track_image_path.size() > 0, track_image_path);
			
			return true;
		}
	}
	return false;
}

//void CaptureManager::DoClipByHotkey()
//{
//	auto it = capture_hotkey_cb_list_.begin();
//	while (it != capture_hotkey_cb_list_.end())
//	{
//		if (!it->flag_.expired())
//		{
//			if (it->cb_())
//			{
//				return;
//			}
//			it++;
//		}
//		else
//		{
//			it = capture_hotkey_cb_list_.erase(it);
//		}
//	}
//
//	std::wstring send_info = L"";
//	StartCapture(CaptureCallback(), send_info);
//}
//void CaptureManager::AddClipHotkeyCallback(CaptureHotkeyCallback cb, std::weak_ptr<nbase::WeakFlag> flag)
//{
//	Hotkey_Callback_Info info;
//	info.cb_ = cb;
//	info.flag_ = flag;
//	capture_hotkey_cb_list_.push_back(info);
//}