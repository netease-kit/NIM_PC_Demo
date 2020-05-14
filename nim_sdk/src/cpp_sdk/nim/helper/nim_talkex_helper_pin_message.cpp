/** @file nim_talkex_helper_pin_message.cpp
  * @brief TalkEx Pin Message 辅助方法和数据结构定义
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#include "src/cpp_sdk/nim/helper/nim_talkex_helper_pin_message.h"
namespace nim
{
	std::string PinMessageInfo::ToJsonString(const PinMessageInfo& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		ToJsonObject(info, values);
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	void PinMessageInfo::ToJsonObject(const PinMessageInfo& info, nim_cpp_wrapper_util::Json::Value& json_info)
	{
		json_info[kNIMPinMessageInfoKeyID] = info.id;
		json_info[kNIMPinMessageInfoKeySessionID] = info.session_id;
		json_info[kNIMPinMessageInfoKeyServerID] = info.server_id;
		json_info[kNIMPinMessageInfoKeyClientID] = info.client_id;
		json_info[kNIMPinMessageInfoKeyToType] = info.to_type;
		json_info[kNIMPinMessageInfoKeyFromAccID] = info.from_account;
		json_info[kNIMPinMessageInfoKeyToAccID] = info.to_account;
		json_info[kNIMPinMessageInfoKeyMSGTime] = info.message_time;
		json_info[kNIMPinMessageInfoKeyOPAccID] = info.operator_account;
		json_info[kNIMPinMessageInfoKeyExt] = info.ext;
		json_info[kNIMPinMessageInfoKeyCreateTime] = info.create_time;
		json_info[kNIMPinMessageInfoKeyUpdateTime] = info.update_time;
	}
	void PinMessageInfo::FromJsonString(const std::string& json_info, PinMessageInfo& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_info, values))
		{
			FromJsonObject(values, info);
		}
	}
	void PinMessageInfo::FromJsonObject(const nim_cpp_wrapper_util::Json::Value& json_info, PinMessageInfo& info)
	{
		info.id = json_info[kNIMPinMessageInfoKeyID];
		info.session_id = json_info[kNIMPinMessageInfoKeySessionID];
		info.server_id = json_info[kNIMPinMessageInfoKeyServerID];
		info.client_id = json_info[kNIMPinMessageInfoKeyClientID];
		info.to_type = json_info[kNIMPinMessageInfoKeyToType];
		info.from_account = json_info[kNIMPinMessageInfoKeyFromAccID];
		info.to_account = json_info[kNIMPinMessageInfoKeyToAccID];
		info.message_time = json_info[kNIMPinMessageInfoKeyMSGTime];
		info.operator_account = json_info[kNIMPinMessageInfoKeyOPAccID];
		info.ext = json_info[kNIMPinMessageInfoKeyExt];
		info.create_time = json_info[kNIMPinMessageInfoKeyCreateTime];
		info.update_time = json_info[kNIMPinMessageInfoKeyUpdateTime];
	}
	std::string ModifyPinMessageParam::ToJsonString(const ModifyPinMessageParam& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMPinMessageModifyParamKeyID] = info.id;
		values[kNIMPinMessageModifyParamKeySession] = info.session;
		values[kNIMPinMessageModifyParamKeyToType] = info.to_type;
		values[kNIMPinMessageModifyParamKeyExt] = info.ext;
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	void QueryAllPinMessageResponse::FromJsonString(const std::string& json_response, QueryAllPinMessageResponse& response)
	{
		nim_cpp_wrapper_util::Json::Value values_response;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_response, values_response))
		{
			for (auto it : values_response)
			{
				PinMessageInfo info;
				PinMessageInfo::FromJsonObject(it, info);
				response.pin_list.emplace_back(info);
			}
		}
	}
	std::string QueryAllPinMessageResponse::ToJsonString(const QueryAllPinMessageResponse& rsp)
	{
		nim_cpp_wrapper_util::Json::Value values;
		for (auto it : rsp.pin_list)
		{
			nim_cpp_wrapper_util::Json::Value item;
			PinMessageInfo::ToJsonObject(it, item);
			values.append(item);
		}
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
}