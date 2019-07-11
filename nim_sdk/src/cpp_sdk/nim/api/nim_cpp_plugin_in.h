/** @file nim_cpp_plugin_in.h
  * @brief 插件接入，接入聊天室需要调用该接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/12/29
  */

#ifndef _NIM_SDK_CPP_PLUGIN_IN_H_
#define _NIM_SDK_CPP_PLUGIN_IN_H_

#include <string>
#include <functional>
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{


/** @class PluginIn
  * @brief NIM SDK 提供的plugin接入接口类
  */
class NIM_SDK_CPPWRAPPER_DLL_API PluginIn
{
public:
	typedef std::function<void(int error_code, const std::string& result)>	ChatRoomRequestEnterCallback;	/**<  获取聊天室登录信息回调*/

public:
	/** @fn void ChatRoomRequestEnter(const int64_t room_id, const ChatRoomRequestEnterCallback &callback, const std::string& json_extension = "")
  * 异步获取聊天室登录信息
  * @param[in] room_id				聊天室ID
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  * @note 错误码	200:成功
  *				414:参数错误
  *				404:聊天室不存在
  *				403:不允许进入该聊天室
  *				415:服务不可用，没有聊天室服务器可分配
  *				13003:在黑名单中
  */
	 static void ChatRoomRequestEnterAsync(const int64_t room_id, const ChatRoomRequestEnterCallback &callback, const std::string& json_extension = "");
};

} 

#endif //_NIM_SDK_CPP_PLUGIN_IN_H_
