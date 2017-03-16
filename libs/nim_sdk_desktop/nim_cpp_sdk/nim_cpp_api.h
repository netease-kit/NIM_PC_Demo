/** @file nim_cpp_api.h
  * @brief NIM集合头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg, Harrison
  * @date 2015/10/20
  */

#ifndef _NIM_SDK_API_H_
#define _NIM_SDK_API_H_

//客户端登录，注销以及相关回调接口注册API
#include "nim_cpp_client.h"
//代理设置等工具类API
#include "nim_cpp_global.h"
//数据同步通知API
#include "nim_cpp_data_sync.h"
//好友关系相关API
#include "nim_cpp_friend.h"
//用户信息相关API
#include "nim_cpp_user.h"
//消息历史相关API
#include "nim_cpp_msglog.h"
//会话相关API
#include "nim_cpp_talk.h"
//系统通知相关API
#include "nim_cpp_sysmsg.h"
//群组相关API
#include "nim_cpp_team.h"
//NOS云存储服务相关API
#include "nim_cpp_nos.h"
//会话列表相关API
#include "nim_cpp_session.h"
//插件类相关API(聊天室)
#include "nim_cpp_plugin_in.h"
//工具辅助类API
#include "nim_cpp_tool.h"

#if defined (WIN32)
//数据通道相关API(白板等)
#include "nim_cpp_rts.h"
//音视频,音视频设备相关API
#include "nim_cpp_vchat.h"
//文档共享相关API
#include "nim_cpp_doc_trans.h"
#endif

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @fn void UnregNIMCallback()
* 反注册NIM提供的所有回调
* @return void 无返回值
*/
void UnregNIMCallback();

}

#endif