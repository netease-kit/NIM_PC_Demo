/** @file browser_handler.h
  * @brief 定义Render进程与Browser进程通信所用常量
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/7/19
*/
#pragma once

namespace nim_cef
{
	static const char kFocusedNodeChangedMessage[] = "FocusedNodeChanged";	// web页面中获取焦点的元素改变
	static const char kCallCppFunctionMessage[] = "JsCallback";					// web调用C++接口接口的通知
}
