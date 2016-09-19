/** @file nim_cpp_plugin_in.h
  * @brief NIM SDK 提供的plugin接入接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/12/29
  */

#ifndef _NIM_SDK_CPP_PLUGIN_IN_H_
#define _NIM_SDK_CPP_PLUGIN_IN_H_

#include <string>
#include <functional>

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#include "nim_plugin_in_def.h"

/** @class PluginIn
  * @brief NIM SDK 提供的plugin接入接口类
  */
class PluginIn
{
public:
	typedef std::function<void(int error_code, const std::string& result)>	ChatRoomRequestEnterCallback;	/**<  获取聊天室登录信息回调*/

public:
	/** @fn void ChatRoomRequestEnter(const __int64 room_id, const ChatRoomRequestEnterCallback &callback, const std::string& json_extension = "")
  * 异步获取聊天室登录信息
  * @param[in] room_id				聊天室ID
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
	static void ChatRoomRequestEnterAsync(const __int64 room_id, const ChatRoomRequestEnterCallback &callback, const std::string& json_extension = "");
};

} 

#endif //_NIM_SDK_CPP_PLUGIN_IN_H_