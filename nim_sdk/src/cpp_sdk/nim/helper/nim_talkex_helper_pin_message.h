/** @file nim_talkex_helper_pin_message.h
  * @brief Talkex Pin Message 辅助方法和数据结构定义
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#ifndef _NIM_SDK_CPP_TALKEX_HELPER_PIN_MESSAGE_H_
#define _NIM_SDK_CPP_TALKEX_HELPER_PIN_MESSAGE_H_

#include <string>
#include <list>
#include <functional>
#include <algorithm>
#include "include/depend_lib/include/nim_json_util.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
	/** @brief Pin消息属性 */
	struct NIM_SDK_CPPWRAPPER_DLL_API PinMessageInfo
	{
		std::string id; /**< Pin Message的ID 客户端生成服务端没有*/
		std::string session_id; /**< Pin Message对应的会话ID (客户端数据)*/		
		uint64_t server_id; /**< 被 Pin 消息的服务端ID*/
		std::string client_id;/**< 被 pin 消息的客户商dID*/
		int to_type;/**< 被 pin 消息的会话类型*/
		std::string from_account;/**< 被 pin 消息的发送方ID*/
		std::string to_account;/**< 被 pin 消息的接收方ID*/
		uint64_t message_time;/**< 被 pin 消息的时间戳*/
		std::string operator_account;/**< Pin Message的操作者ID*/
		std::string ext;/**< Pin Message的扩展字段*/
		uint64_t	create_time;/**< Pin Message的创建时间戳*/
		uint64_t	update_time;/**< Pin Message的更新时间戳*/

		static std::string ToJsonString(const PinMessageInfo& info);
		static void ToJsonObject(const PinMessageInfo& info, nim_cpp_wrapper_util::Json::Value& json_info);
		static void FromJsonString(const std::string& json_info, PinMessageInfo& info);
		static void FromJsonObject(const nim_cpp_wrapper_util::Json::Value& json_info, PinMessageInfo& info);
	};

	/** @brief 修改、取消Pin Message参数定义 */
	struct NIM_SDK_CPPWRAPPER_DLL_API ModifyPinMessageParam
	{
		std::string session;/**< UnPin 消息所属会话*/
		int to_type;/**< UnPin 消息所属会话类型*/
		std::string id;/**< Pin Message 的ID 客户端生成服务端没有*/
		std::string ext;/**< 扩展字段*/
		static std::string ToJsonString(const ModifyPinMessageParam& info);
	};

	/** @brief 查询Pin Message应答参数定义 */
	struct NIM_SDK_CPPWRAPPER_DLL_API QueryAllPinMessageResponse
	{
		std::list< PinMessageInfo> pin_list;
		static void FromJsonString(const std::string& json_info, QueryAllPinMessageResponse& info);
		static std::string ToJsonString(const QueryAllPinMessageResponse& rsp);
	};
} //namespace nim


#endif //_NIM_SDK_CPP_TALKEX_HELPER_PIN_MESSAGE_H_