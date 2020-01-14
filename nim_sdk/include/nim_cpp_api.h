/** @file nim_cpp_api.h
  * @brief NIM集合头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/20
  */

#ifndef _NIM_SDK_API_H_
#define _NIM_SDK_API_H_

//客户端登录，注销以及相关回调接口注册API
#include "src/cpp_sdk/nim/api/nim_cpp_client.h"
#include "src/cpp_sdk/nim/helper/nim_client_helper.h"
//代理设置等工具类API
#include "src/cpp_sdk/nim/api/nim_cpp_global.h"
//数据同步通知API
#include "src/cpp_sdk/nim/api/nim_cpp_data_sync.h"
//好友关系相关API
#include "src/cpp_sdk/nim/api/nim_cpp_friend.h"
#include "src/cpp_sdk/nim/helper/nim_friend_helper.h"
//用户信息相关API
#include "src/cpp_sdk/nim/api/nim_cpp_user.h"
#include "src/cpp_sdk/nim/helper/nim_user_helper.h"
//消息历史相关API
#include "src/cpp_sdk/nim/api/nim_cpp_msglog.h"
#include "src/cpp_sdk/nim/helper/nim_msglog_helper.h"
//会话相关API
#include "src/cpp_sdk/nim/api/nim_cpp_talk.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
//系统通知相关API
#include "src/cpp_sdk/nim/api/nim_cpp_sysmsg.h"
#include "src/cpp_sdk/nim/helper/nim_sysmsg_helper.h"
//群组相关API
#include "src/cpp_sdk/nim/api/nim_cpp_team.h"
#include "src/cpp_sdk/nim/helper/nim_team_helper.h"
//NOS云存储服务相关API
#include "src/cpp_sdk/nim/api/nim_cpp_nos.h"
#include "src/cpp_sdk/nim/helper/nim_nos_helper.h"
//会话列表相关API
#include "src/cpp_sdk/nim/api/nim_cpp_session.h"
#include "src/cpp_sdk/nim/helper/nim_session_helper.h"
//插件类相关API(聊天室)
#include "src/cpp_sdk/nim/api/nim_cpp_plugin_in.h"
//工具辅助类API
#include "src/cpp_sdk/nim/api/nim_cpp_tool.h"
//以dll方式使用时的导出定义
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
//超大群
#include "src/cpp_sdk/nim/api/nim_cpp_super_team.h"
#include "src/cpp_sdk/nim/helper/nim_super_team_helper.h"
//文档共享相关API
#include "src/cpp_sdk/nim/api/nim_cpp_doc_trans.h"
#include "src/cpp_sdk/nim/helper/nim_doc_trans_helper.h"
//事件订阅
#include "src/cpp_sdk/nim/api/nim_cpp_subscribe_event.h"
#include "src/cpp_sdk/nim/helper/nim_subscribe_event_helper.h"
//独立信令
#include "src/cpp_sdk/nim/api/nim_cpp_signaling.h"
#include "src/cpp_sdk/nim/helper/nim_signaling_helper.h"
//会话服务
#include "src/cpp_sdk/nim/api/nim_cpp_session_online_service.h"
#include "src/cpp_sdk/nim/helper/nim_session_online_service_helper.h"
//透传服务
#include "src/cpp_sdk/nim/api/nim_cpp_pass_through_proxy.h"
#include "src/cpp_sdk/nim/helper/nim_pass_through_proxy_helper.h"
/*仅在 Windows 平台下使用的头*/
#if defined (WIN32)
    //数据通道相关API(白板等)
    #include "src/cpp_sdk/nim/api/nim_cpp_rts.h"
    //音视频,音视频设备相关API
    #include "src/cpp_sdk/nim/api/nim_cpp_vchat.h"
    //机器人
    #include "src/cpp_sdk/nim/api/nim_cpp_robot.h"
    #include "src/cpp_sdk/nim/helper/nim_robot_helper.h"
#endif

#include "src/cpp_sdk/nim/helper/nim_msg_helper.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "src/cpp_sdk/nim/helper/nim_tool_helper.h"

#endif
