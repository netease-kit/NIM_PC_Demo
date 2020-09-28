/** @file nim_session_helper.cpp
  * @brief Session 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/22
  */

#include "src/cpp_sdk/nim/helper/nim_session_helper.h"

namespace nim
{
	std::string StickTopSession::ToJsonString(const StickTopSession& stick_session)
	{
		nim_cpp_wrapper_util::Json::Value value;
		ToJsonObject(stick_session,value);
		return nim_cpp_wrapper_util::Json::FastWriter().write(value);
	}
	void StickTopSession::ToJsonObject(const StickTopSession& stick_session,nim_cpp_wrapper_util::Json::Value& value)
	{
		value[kNIMSessionStickTopInfoSessionId] = stick_session.stick_top_info_.top_;
		value[kNIMSessionStickTopInfoSessionId] = stick_session.stick_top_info_.id_;
		value[kNIMSessionStickTopInfoType] = stick_session.stick_top_info_.type_;
		value[kNIMSessionStickTopInfoExt] = stick_session.stick_top_info_.ext_;
		value[kNIMSessionStickTopInfoCreateTime] = stick_session.stick_top_info_.create_time_;
		value[kNIMSessionStickTopInfoUpdateTime] = stick_session.stick_top_info_.update_time_;

		value[kNIMSessionStickTopSessionData][kNIMSessionId] = stick_session.session_data_.id_;
		value[kNIMSessionStickTopSessionData][kNIMSessionType] = stick_session.session_data_.type_;
		value[kNIMSessionStickTopSessionData][kNIMSessionUnreadCount] = stick_session.session_data_.unread_count_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgClientID] = stick_session.session_data_.msg_id_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgFromAccount] = stick_session.session_data_.msg_sender_accid_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgTime] = stick_session.session_data_.msg_timetag_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgType] = stick_session.session_data_.msg_type_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgBody] = stick_session.session_data_.msg_content_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgAttach] = stick_session.session_data_.msg_attach_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgStatus] = stick_session.session_data_.msg_status_;
		value[kNIMSessionStickTopSessionData][kNIMSessionMsgSubStatus] = stick_session.session_data_.msg_sub_status_;
		value[kNIMSessionStickTopSessionData][kNIMSessionLastUpdatedMsg] = stick_session.session_data_.last_updated_msg_;
		value[kNIMSessionStickTopSessionData][kNIMSessionOnTop] = stick_session.session_data_.placed_on_top_;
		value[kNIMSessionStickTopSessionData][kNIMSessionExtendedData] = stick_session.session_data_.extend_data_;
		value[kNIMSessionStickTopSessionData][kNIMSessionRobotFlag] = stick_session.session_data_.is_robot_session_;
	}
	std::string StickTopSessionList::ToJsonString(const StickTopSessionList& stick_session_list)
	{
		nim_cpp_wrapper_util::Json::Value values;
		for (auto it : stick_session_list.sessions_)
		{
			nim_cpp_wrapper_util::Json::Value item;
			it.ToJsonObject(it, item);
			values.append(item);
		}
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
bool ParseSession(const std::string& session_json, SessionData& session)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(session_json, values) && values.isObject())
	{
		ParseSession(values, session);
		return true;
	}
	return false;
}

void ParseSession(const nim_cpp_wrapper_util::Json::Value& session_json, SessionData& session)
{
	session.id_ = session_json[kNIMSessionId].asString();
	session.type_ = (NIMSessionType)session_json[kNIMSessionType].asUInt();
	session.unread_count_ = session_json[kNIMSessionUnreadCount].asUInt();
	session.command_ = (NIMSessionCommand)session_json[kNIMSessionCommand].asUInt();
	session.msg_id_ = session_json[kNIMSessionMsgClientID].asString();
	session.msg_sender_accid_ = session_json[kNIMSessionMsgFromAccount].asString();
	session.msg_timetag_ = session_json[kNIMSessionMsgTime].asUInt64();
	session.msg_type_ = (NIMMessageType)session_json[kNIMSessionMsgType].asUInt();
	session.msg_content_ = session_json[kNIMSessionMsgBody].asString();
	session.msg_attach_ = session_json[kNIMSessionMsgAttach].asString();
	session.msg_status_ = (NIMMsgLogStatus)session_json[kNIMSessionMsgStatus].asUInt();
	session.msg_sub_status_ = (NIMMsgLogSubStatus)session_json[kNIMSessionMsgSubStatus].asUInt();
	session.last_updated_msg_ = session_json[kNIMSessionLastUpdatedMsg].asBool();
	session.placed_on_top_ = session_json[kNIMSessionOnTop].asBool();
	session.extend_data_ = session_json[kNIMSessionExtendedData].asString();
	session.is_robot_session_ = session_json[kNIMSessionRobotFlag].asBool();
	if (session_json.isMember(kNIMSessionStickTopInfo))
	{
		session.stick_top_info_.id_ = session.id_;
		session.stick_top_info_.type_ = session.type_;
		session.stick_top_info_.ext_ = session_json[kNIMSessionStickTopInfo][kNIMSessionStickTopInfoExt].asString();
		session.stick_top_info_.create_time_ = session_json[kNIMSessionStickTopInfo][kNIMSessionStickTopInfoCreateTime].asUInt();
		session.stick_top_info_.update_time_ = session_json[kNIMSessionStickTopInfo][kNIMSessionStickTopInfoUpdateTime].asUInt();
	}	
}

bool ParseSessionList(const std::string& sessions_json, SessionDataList& session_list)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(sessions_json, values) && values.isObject())
	{
		session_list.count_ = values[kNIMSessionListCount].asUInt();
		session_list.unread_count_ = values[kNIMSessionListUnreadCount].asUInt();
		nim_cpp_wrapper_util::Json::Value sessions = values[kNIMSessionListContent];
		int len = sessions.size();
		for (int i = 0; i < len; i++)
		{
			SessionData session;
			ParseSession(sessions[i], session);
			session_list.sessions_.emplace_back(session);
		}
		return true;
	}
	return false;
}
bool ParseStickTopSession(const std::string& session_json, StickTopSession& session)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(session_json, values) && values.isObject())
	{
		ParseStickTopSession(values, session);
		return true;
	}
	return false;
}
void ParseStickTopSession(const nim_cpp_wrapper_util::Json::Value& session_json, StickTopSession& session)
{
	session.stick_top_info_.top_ = session_json[kNIMSessionStickTopInfoTop].asBool();
	session.stick_top_info_.id_ = session_json[kNIMSessionStickTopInfoSessionId].asString();
	session.stick_top_info_.type_ = (NIMSessionType)session_json[kNIMSessionStickTopInfoType].asInt();
    session.stick_top_info_.ext_ = session_json[kNIMSessionStickTopInfoExt].asString();
	session.stick_top_info_.create_time_ = session_json[kNIMSessionStickTopInfoCreateTime].asUInt();
	session.stick_top_info_.update_time_ = session_json[kNIMSessionStickTopInfoUpdateTime].asUInt();
	if (session_json.isMember(kNIMSessionStickTopSessionData))
	{
		session.session_data_.id_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionId].asString();
		session.session_data_.type_ = (NIMSessionType)session_json[kNIMSessionStickTopSessionData][kNIMSessionType].asUInt();
		session.session_data_.unread_count_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionUnreadCount].asUInt();
		session.session_data_.command_ = (NIMSessionCommand)session_json[kNIMSessionStickTopSessionData][kNIMSessionCommand].asUInt();
		session.session_data_.msg_id_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgClientID].asString();
		session.session_data_.msg_sender_accid_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgFromAccount].asString();
		session.session_data_.msg_timetag_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgTime].asUInt64();
		session.session_data_.msg_type_ = (NIMMessageType)session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgType].asUInt();
		session.session_data_.msg_content_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgBody].asString();
		session.session_data_.msg_attach_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgAttach].asString();
		session.session_data_.msg_status_ = (NIMMsgLogStatus)session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgStatus].asUInt();
		session.session_data_.msg_sub_status_ = (NIMMsgLogSubStatus)session_json[kNIMSessionStickTopSessionData][kNIMSessionMsgSubStatus].asUInt();
		session.session_data_.last_updated_msg_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionLastUpdatedMsg].asBool();
		session.session_data_.placed_on_top_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionOnTop].asBool();
		session.session_data_.extend_data_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionExtendedData].asString();
		session.session_data_.is_robot_session_ = session_json[kNIMSessionStickTopSessionData][kNIMSessionRobotFlag].asBool();
	}	
}
bool ParseStickTopSessionList(const std::string& sessions_json, StickTopSessionList& stick_top_session_list)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(sessions_json, values) && values.isArray())
	{
		for (auto it : values)
		{
			StickTopSession session;
			ParseStickTopSession(it, session);
			stick_top_session_list.sessions_.emplace_back(session);
		}	
		return true;
	}
	return false;
}
}