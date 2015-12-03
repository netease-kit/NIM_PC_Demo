#ifndef _NIM_SDK_CPP_SESSION_HELPER_H_
#define _NIM_SDK_CPP_SESSION_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"

namespace nim
{
/** @brief Session 辅助方法和数据结构定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/22
  */

#include "nim_session_def.h"
#include "nim_msglog_def.h"

struct SessionData
{
	std::string		id_;
	NIMSessionType	type_;
	int				unread_count_;
	NIMSessionCommand	command_;
	std::string		msg_id_;
	std::string		msg_sender_accid_;
	__int64			msg_timetag_;
	NIMMessageType	msg_type_;
	std::string		msg_content_;
	std::string		msg_attach_;
	NIMMsgLogStatus	msg_status_;
	NIMMsgLogSubStatus	msg_sub_status_;

	SessionData() : unread_count_(0)
				, msg_timetag_(0)
				, type_(kNIMSessionTypeP2P)
				, command_(kNIMSessionCommandAdd)
				, msg_type_(kNIMMessageTypeUnknown)
				, msg_status_(kNIMMsgLogStatusNone)
				, msg_sub_status_(kNIMMsgLogSubStatusNone){}
};

struct SessionDataList
{
	int			count_;
	int			unread_count_;
	std::list<SessionData> sessions_;

	SessionDataList() : count_(0), unread_count_(0) {}
};

bool ParseSession(const std::string& session_json, SessionData& session);
void ParseSession(const Json::Value& session_json, SessionData& session);
bool ParseSessionList(const std::string& sessions_json, SessionDataList& session_list);
} //namespace nim

#endif //_NIM_SDK_CPP_SESSION_HELPER_H_