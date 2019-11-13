/** @file nim_c_api.h
  * @brief NIM集合头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/20
  */

#ifndef _NIM_C_SDK_API_H_
#define _NIM_C_SDK_API_H_

//客户端登录，注销以及相关回调接口注册API
#include "src/c_sdk/nim/api/nim_client.h"
//代理设置等工具类API
#include "src/c_sdk/nim/api/nim_global.h"
//数据同步通知API
#include "src/c_sdk/nim/api/nim_data_sync.h"
//音视频设备相关API
#include "src/c_sdk/nim/api/nim_device.h"
//音视频相关API
#include "src/c_sdk/nim/api/nim_vchat.h"
//数据通道相关API(白板等)
#include "src/c_sdk/nim/api/nim_rts.h"
//独立信令相关API
#include "src/c_sdk/nim/api/nim_avsignal.h"
//文档共享相关API
#include "src/c_sdk/nim/api/nim_doc_trans.h"
//好友关系相关API
#include "src/c_sdk/nim/api/nim_friend.h"
//用户信息相关API
#include "src/c_sdk/nim/api/nim_user.h"
//消息历史相关API
#include "src/c_sdk/nim/api/nim_msglog.h"
//会话相关API
#include "src/c_sdk/nim/api/nim_talk.h"
//系统通知相关API
#include "src/c_sdk/nim/api/nim_sysmsg.h"
//群组相关API
#include "src/c_sdk/nim/api/nim_team.h"
//NOS云存储服务相关API
#include "src/c_sdk/nim/api/nim_nos.h"
//会话列表相关API
#include "src/c_sdk/nim/api/nim_session.h"
//插件类相关API(聊天室)
#include "src/c_sdk/nim/api/nim_plugin_in.h"
//工具辅助类API
#include "src/c_sdk/nim/api/nim_tools.h"
//机器人
#include "src/c_sdk/nim/api/nim_robot.h"
//信令
#include "src/c_sdk/nim/api/nim_signaling.h"
//超大群
#include "src/c_sdk/nim/api/nim_super_team.h"
//事件订阅
#include "src/c_sdk/nim/api/nim_subscribe_event.h"
//会话服务
#include "src/c_sdk/nim/api/nim_session_online_service.h"

#endif