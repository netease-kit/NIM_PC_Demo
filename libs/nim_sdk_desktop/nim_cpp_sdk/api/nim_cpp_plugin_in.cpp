/** @file nim_cpp_plugin_in.cpp
  * @brief NIM SDK 提供的plugin接入接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/12/29
  */

#include "nim_cpp_plugin_in.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"
#include "callback_proxy.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_plugin_chatroom_request_enter_async)(const int64_t room_id, const char *json_extension, nim_plugin_chatroom_request_enter_cb_func cb, const void *user_data);
#else
#include "nim_plugin_in.h"
#endif

static void CallbackRequestChatRoomEnter(int error_code, const char *result, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<PluginIn::ChatRoomRequestEnterCallback>(user_data, [=](const PluginIn::ChatRoomRequestEnterCallback& cb){

		CallbackProxy::Invoke(cb, error_code, nim::PCharToString(result));
	});

}

 void PluginIn::ChatRoomRequestEnterAsync(const int64_t room_id, const ChatRoomRequestEnterCallback &callback, const std::string& json_extension/* = ""*/)
{
	ChatRoomRequestEnterCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new ChatRoomRequestEnterCallback(callback);

	NIM_SDK_GET_FUNC(nim_plugin_chatroom_request_enter_async)(room_id, json_extension.c_str(), &CallbackRequestChatRoomEnter, cb_pointer);
}

}