#ifndef _NIM_SDK_CPP_MSGLOG_HELPER_H_
#define _NIM_SDK_CPP_MSGLOG_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_talk_helper.h"

namespace nim
{
/** @brief msglog 辅助方法和数据结构定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/22
  */

struct QueryMsglogResult
{
	int count_;
	std::list<IMMessage> msglogs_;

	QueryMsglogResult() : count_(0) {}
};

bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res);

} //namespace nim

#endif //_NIM_SDK_CPP_MSGLOG_HELPER_H_