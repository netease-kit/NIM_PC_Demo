/** @file nim_sdk_api.h
  * @brief 接口头文件集合，在需要使用即时通讯 SDK 的地方包含该头文件即可
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2017/02/01
  */

//客户端登录，注销以及相关回调接口注册API
#include "nim_client.h"
//代理设置等工具类API
#include "nim_global.h"
//数据同步通知API
#include "nim_data_sync.h"
//音视频设备相关API
#include "nim_devie.h"
//音视频相关API
#include "nim_vchat.h"
//数据通道相关API(白板等)
#include "nim_rts.h"
//文档共享相关API
#include "nim_doc_trans.h"
//好友关系相关API
#include "nim_friend.h"
//用户信息相关API
#include "nim_user.h"
//消息历史相关API
#include "nim_msglog.h"
//会话相关API
#include "nim_talk.h"
//系统通知相关API
#include "nim_sysmsg.h"
//群组相关API
#include "nim_team.h"
//NOS云存储服务相关API
#include "nim_nos.h"
//会话列表相关API
#include "nim_session.h"
//插件类相关API(聊天室)
#include "nim_plugin_in.h"
//工具辅助类API
#include "nim_tools.h"
//机器人
#include "nim_robot.h"