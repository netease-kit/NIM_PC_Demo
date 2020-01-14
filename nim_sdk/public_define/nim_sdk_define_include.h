/** @file nim_sdk_define_include.h
  * @brief nim im 公共数据类型定义总的包含文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2017/08/03
  */

#ifndef _NIM_SDK_CPP_NIM_SDK_DEFINES_H_
#define _NIM_SDK_CPP_NIM_SDK_DEFINES_H_

#if !defined(_WIN32) && !defined(WIN32)
#include <pthread.h>
#endif

#ifndef DEF_UNDER_NO_NAMESPACE
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
#endif //DEF_UNDER_NO_NAMESPACE

#include "public_define/defines/nim_define/nim_res_code_def.h"
#include "public_define/defines/nim_define/nim_data_sync_def.h"
#include "public_define/defines/nim_define/nim_friend_def.h"
#include "public_define/defines/nim_define/nim_session_def.h"
#include "public_define/defines/nim_define/nim_client_def.h"
#include "public_define/defines/nim_define/nim_user_def.h"
#include "public_define/defines/nim_define/nim_team_def.h"
#include "public_define/defines/nim_define/nim_msglog_def.h"
#include "public_define/defines/nim_define/nim_talk_def.h"
#include "public_define/defines/nim_define/nim_sysmsg_def.h"
#include "public_define/defines/nim_define/nim_subscribe_event_def.h"
#include "public_define/defines/nim_define/nim_robot_def.h"
#include "public_define/defines/nim_define/nim_global_def.h"
#include "public_define/defines/nim_define/nim_nos_def.h"
#include "public_define/defines/nim_define/nim_plugin_in_def.h"
#include "public_define/defines/nim_define/nim_tools_def.h"
#include "public_define/defines/nim_define/nim_user_def.h"

#include "public_define/defines/nim_define/nim_doc_trans_def.h"
#include "public_define/defines/nim_define/nim_vchat_def.h"
#include "public_define/defines/nim_define/nim_device_def.h"
#include "public_define/defines/nim_define/nim_rts_def.h"
#include "public_define/defines/nim_define/nim_signaling_def.h"

#include "public_define/defines/nim_define/nim_super_team_def.h"
#include "public_define/defines/nim_define/nim_session_online_service_def.h"
#include "public_define/defines/nim_define/nim_pass_through_proxy_def.h"

#ifndef DEF_UNDER_NO_NAMESPACE
}
#endif //DEF_UNDER_NO_NAMESPACE

#endif //_NIM_SDK_CPP_NIM_SDK_DEFINES_H_
