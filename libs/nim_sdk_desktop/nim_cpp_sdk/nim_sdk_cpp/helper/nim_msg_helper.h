#ifndef _NIM_SDK_CPP_MESSAGE_HELPER_H_
#define _NIM_SDK_CPP_MESSAGE_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"

namespace nim
{
/** @brief Message 通用辅助方法和数据结构定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#include "nim_res_code_def.h"

struct SendMessageArc
{
	std::string talk_id_;
	std::string msg_id_;
	NIMResCode rescode_;
};

bool ParseSendMessageArc(const std::string& arc_json, SendMessageArc& arc);

} //namespace nim

#endif //_NIM_SDK_CPP_MESSAGE_HELPER_H_