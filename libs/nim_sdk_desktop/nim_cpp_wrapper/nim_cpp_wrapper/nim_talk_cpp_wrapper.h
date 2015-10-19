#pragma once


#include <string>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_talk_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_http_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_sysmsg_def.h"

/** @class Talk
  * @brief 聊天功能；主要包括发送消息、接收消息等功能
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Talk
{

public:
	typedef std::function<void(const std::string&)>	SendMsgCallback;
	typedef std::function<void(const std::string&)>	SendCustomSysmsgCallback;
	typedef std::function<void(const std::string&)>	ReveiveMsgCallback;
	typedef std::function<void(__int64, __int64)>	FileUpPrgCallback;

	/** 
	* 注册发送消息回调函数 （必须全局注册，统一接受回调后分发消息到具体的会话。注意：客户端发包之后，服务器不一定会返回！！！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		发送消息的回调函数
	* @return void 无返回值
	*/
	static void RegSendMsgCb(const SendMsgCallback& cb, const std::string& json_extension = "");

	/** 
	* 发送消息
	* @param[in] json_msg		消息体Json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] prg_cb		上传进度的回调函数, 如果发送的消息里包含了文件资源，则通过此回调函数通知上传进度
	* @return void 无返回值
	*/
	static void SendMsg(const std::string& json_msg, std::string msg_id = "", FileUpPrgCallback* pcb = nullptr);

	/** 
	* 停止正在发送中的消息（目前只支持发送文件消息时的终止）
	* @param[in] json_msg		消息体Json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void StopSendMsg(const std::string& json_msg, const std::string& json_extension = "");

	/** 
	* 注册发送透传消息回调函数 （必须全局注册，统一接受回调后分发消息到具体的会话。注意：客户端发包之后，服务器不一定会返回！！！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		发送透传消息的回调函数
	* @return void 无返回值
	*/
	static void RegSendCustomSysmsgCb(const SendCustomSysmsgCallback& cb, const std::string& json_extension = "");

	/** 
	* 发送透传消息（统一以系统消息的形式通知）
	* @param[in] json_msg		消息体Json string (Keys SEE MORE `nim_sysmsg_def.h` 『系统消息结构 Json Keys』)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void SendCustomSysmsg(const std::string& json_msg);

	/** 
	* 注册接收消息回调 （建议全局注册，统一接受回调后分发消息到具体的会话）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		接收消息的回调函数
	* @return void 无返回值
	*/
	static void RegReceiveCb(const ReveiveMsgCallback& cb, const std::string& json_extension = "");
};

} 