/** @file nim_cpp_win32_demo_helper.h
  * @brief Win32 Desktop辅助方法（不属于SDK本身，用于Win32 Desktop Demo的辅助接口定义）
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/09/08
  */

#ifndef _NIM_SDK_CPP_NIM_WIN32_DEMO_HELPER_H_
#define _NIM_SDK_CPP_NIM_WIN32_DEMO_HELPER_H_

#ifndef NIM_SDK
#include "..\..\..\tool_kits/base/thread/thread_manager.h"
#include "..\..\..\tool_kits/base/callback/callback.h"
#endif

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#ifndef NIM_SDK
inline void PostTaskToUIThread(const StdClosure &closure)
{
	nbase::ThreadManager::PostTask(0, closure);
}
#endif

}

#endif //_NIM_SDK_CPP_NIM_WIN32_DEMO_HELPER_H_