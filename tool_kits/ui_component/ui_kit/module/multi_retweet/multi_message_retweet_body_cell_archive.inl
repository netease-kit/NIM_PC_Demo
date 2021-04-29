namespace nim_comp
{
	inline std::string BodyCellArchive::LogTokenToStringKey(LogToken token)
	{
		return std::to_string((int)token);
	}
	inline nim::IMMessage BodyCellArchive::ToMessageLog(const std::string& data)
	{
		Json::Value json_msg;
		Json::Reader().parse(data, json_msg);
		return ToMessageLog(json_msg);
	}
	inline std::string BodyCellArchive::ToString(const nim::IMMessage& msg)
	{
		std::string ret;
		Json::Value&& logs = std::forward<Json::Value>(ToJsonValue(msg));		
		ret = Json::FastWriter().write(logs);
		return ret;
	}
	inline nim::IMMessage BodyCellArchive::ToMessageLog(const Json::Value& json_msg)
	{
		nim::IMMessage msg;
		msg.session_type_ = (nim::NIMSessionType)json_msg[LogTokenToStringKey(LogToken::ReceiverType)].asInt();
		msg.receiver_accid_ = json_msg[LogTokenToStringKey(LogToken::ReceiverAccount)].asString();
		msg.sender_accid_ = json_msg[LogTokenToStringKey(LogToken::fromAccount)].asString();
		msg.readonly_sender_client_type_ = (nim::NIMClientType)json_msg[LogTokenToStringKey(LogToken::clientType)].asInt();
		msg.timetag_ = json_msg[LogTokenToStringKey(LogToken::time)].asInt64();
		msg.type_ = (nim::NIMMessageType)json_msg[LogTokenToStringKey(LogToken::type)].asInt();
		msg.client_msg_id_ = json_msg[LogTokenToStringKey(LogToken::msgid_client)].asString();
		if (json_msg.isMember(LogTokenToStringKey(LogToken::deviceId)))
			msg.readonly_sender_device_id_ = json_msg[LogTokenToStringKey(LogToken::deviceId)].asString();
		if (json_msg.isMember(LogTokenToStringKey(LogToken::fromNick)))
			msg.readonly_sender_nickname_ = json_msg[LogTokenToStringKey(LogToken::fromNick)].asString();
		if (json_msg.isMember(LogTokenToStringKey(LogToken::body)))
			msg.content_ = json_msg[LogTokenToStringKey(LogToken::body)].asString();
		if (json_msg.isMember(LogTokenToStringKey(LogToken::attach)))
			msg.attach_ = json_msg[LogTokenToStringKey(LogToken::attach)].asString();
		if (json_msg.isMember(LogTokenToStringKey(LogToken::msgid_server)))
			msg.readonly_server_id_ = std::atoi(json_msg[LogTokenToStringKey(LogToken::msgid_server)].asString().c_str());
		if (json_msg.isMember(LogTokenToStringKey(LogToken::pushPayload)))
			nim_cpp_wrapper_util::Json::Reader().parse(json_msg[LogTokenToStringKey(LogToken::pushPayload)].toStyledString(), msg.msg_setting_.push_payload_);
		if (json_msg.isMember(LogTokenToStringKey(LogToken::pushContent)))
			msg.msg_setting_.push_content_ = json_msg[LogTokenToStringKey(LogToken::pushContent)].asString();
		return msg;
	}
	inline Json::Value BodyCellArchive::ToJsonValue(const nim::IMMessage& msg)
	{
		Json::Value logs;
		logs[LogTokenToStringKey(LogToken::ReceiverType)] = msg.session_type_;
		logs[LogTokenToStringKey(LogToken::ReceiverAccount)] = msg.receiver_accid_;
		logs[LogTokenToStringKey(LogToken::fromAccount)] = msg.sender_accid_;
		logs[LogTokenToStringKey(LogToken::clientType)] = msg.readonly_sender_client_type_;
		logs[LogTokenToStringKey(LogToken::deviceId)] = msg.readonly_sender_device_id_;
		logs[LogTokenToStringKey(LogToken::fromNick)] = msg.readonly_sender_nickname_;
		logs[LogTokenToStringKey(LogToken::time)] = msg.timetag_;
		logs[LogTokenToStringKey(LogToken::type)] = msg.type_;
		if (!msg.content_.empty())
			logs[LogTokenToStringKey(LogToken::body)] = msg.content_;
		if (!msg.attach_.empty())
		{
			nim_cpp_wrapper_util::Json::Value attachment;
			if (nim_cpp_wrapper_util::Json::Reader().parse(msg.attach_, attachment))
				logs[LogTokenToStringKey(LogToken::attach)] = nim_cpp_wrapper_util::Json::FastWriter().write(attachment);
		}
		logs[LogTokenToStringKey(LogToken::msgid_client)] = msg.client_msg_id_;
		logs[LogTokenToStringKey(LogToken::msgid_server)] = msg.readonly_server_id_;
		if (msg.msg_setting_.push_payload_.isObject())
			logs[LogTokenToStringKey(LogToken::pushPayload)] = nim_cpp_wrapper_util::Json::FastWriter().write(msg.msg_setting_.push_payload_);
		if (!msg.msg_setting_.push_content_.empty())
			logs[LogTokenToStringKey(LogToken::pushContent)] = msg.msg_setting_.push_content_;
		return logs;
	}
}