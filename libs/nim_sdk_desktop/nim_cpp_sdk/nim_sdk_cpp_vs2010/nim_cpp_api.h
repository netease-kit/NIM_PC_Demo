/** @file nim_cpp_api.h
  * @brief NIM cpp sdk 头文件
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#ifndef _NIM_SDK_API_H_
#define _NIM_SDK_API_H_

#include "nim_cpp_client.h"
#include "nim_cpp_data_sync.h"
#include "nim_cpp_friend.h"
#include "nim_cpp_global.h"
#include "nim_cpp_msglog.h"
#include "nim_cpp_nos.h"
#include "nim_cpp_rts.h"
#include "nim_cpp_session.h"
#include "nim_cpp_sysmsg.h"
#include "nim_cpp_talk.h"
#include "nim_cpp_team.h"
#include "nim_cpp_tool.h"
#include "nim_cpp_user.h"
#include "nim_cpp_vchat.h"
#include "nim_cpp_plugin_in.h"

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

/** @fn void UnregNIMCallback()
* 反注册NIM提供的所有回调
* @return void 无返回值
*/
void UnregNIMCallback()
{
	Client::UnregClientCb();
	DataSync::UnregDataSyncCb();
	Friend::UnregFriendCb();
	Session::UnregSessionCb();
	SystemMsg::UnregSysmsgCb();
	Talk::UnregTalkCb();
	Team::UnregTeamCb();
	User::UnregUserCb();
}

}

#endif