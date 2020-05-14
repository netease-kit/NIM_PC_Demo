/** @file nim_talkex_helper_quick_comment.cpp
  * @brief TalkEx 快捷回复辅助方法和数据结构定义
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#include "src/cpp_sdk/nim/helper/nim_talkex_helper_quick_comment.h"
namespace nim
{
	std::string QuickCommentInfo::ToJsonString(const QuickCommentInfo& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMQuickCommentInfoKeyID] = info.id;
		values[kNIMQuickCommentInfoKeyFromAccID] = info.from_account;
		values[kNIMQuickCommentInfoKeyReplyType] = info.reply_type;
		values[kNIMQuickCommentInfoKeyClinetID] = info.client_id;
		values[kNIMQuickCommentInfoKeyServerID] = info.server_id;
		values[kNIMQuickCommentInfoKeyTime] = info.time;
		values[kNIMQuickCommentInfoKeyExt] = info.ext;
		values[kNIMQuickCommentInfoKeyNeedPush] = info.need_push ? 1 : 0;
		values[kNIMQuickCommentInfoKeyNeedBadge] = info.need_badge ? 1 : 0;
		values[kNIMQuickCommentInfoKeyPushTitle] = info.push_title;
		values[kNIMQuickCommentInfoKeyPushContent] = info.push_content;
		values[kNIMQuickCommentInfoKeyPushPayload] = info.push_payload;
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	void QuickCommentInfo::FromJsonString(const std::string& json_info, QuickCommentInfo& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_info, values))
		{
			FromJsonObject(values, info);
		}
	}
	void QuickCommentInfo::FromJsonObject(const nim_cpp_wrapper_util::Json::Value& json_info, QuickCommentInfo& info)
	{
		info.id = json_info[kNIMQuickCommentInfoKeyID].asString();
		info.from_account = json_info[kNIMQuickCommentInfoKeyFromAccID].asString();
		info.reply_type = json_info[kNIMQuickCommentInfoKeyReplyType].asInt64();
		info.client_id = json_info[kNIMQuickCommentInfoKeyClinetID].asString();
		info.server_id = json_info[kNIMQuickCommentInfoKeyServerID].asUInt64();
		info.time = json_info[kNIMQuickCommentInfoKeyTime].asUInt64();
		info.ext = json_info[kNIMQuickCommentInfoKeyExt].asString();
		info.need_push = json_info[kNIMQuickCommentInfoKeyNeedPush].asInt() != 0;
		info.need_badge = json_info[kNIMQuickCommentInfoKeyNeedBadge].asInt() != 0;
		info.push_title = json_info[kNIMQuickCommentInfoKeyPushTitle].asString();
		info.push_content = json_info[kNIMQuickCommentInfoKeyPushContent].asString();
		info.push_payload = json_info[kNIMQuickCommentInfoKeyPushPayload].asString();
	}
	std::string RemoveQuickCommentParam::ToJsonString(const RemoveQuickCommentParam& param)
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMQuickCommentRemoveParamKeyID] = param.id;
		values[kNIMQuickCommentRemoveParamKeyExt] = param.ext;
		values[kNIMQuickCommentRemoveParamKeyReplyType] = param.reply_type;
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	std::string QueryQuickCommentsParam::ToJsonString(const QueryQuickCommentsParam& param)
	{
		nim_cpp_wrapper_util::Json::Value values;
		for (auto it : param.message_list)
		{
			values.append(it.ToJsonObject(false));
		}
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	void QueryQuickCommentsResponse::FromJsonString(const std::string& json_Response, QueryQuickCommentsResponse& response)
	{
		nim_cpp_wrapper_util::Json::Value values;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_Response, values) && values.isArray())
		{
			QueryQuickCommentsResponseItem item;
			for (auto it : values)
			{
				item.message_client_id = it[kNIMQuickCommentQueryRSPKeyMsgClientID].asString();
				if (it[kNIMQuickCommentQueryRSPKeyInfoList].isArray())
				{
					QuickCommentInfo info;
					for (auto it_info : it[kNIMQuickCommentQueryRSPKeyInfoList])
					{
						QuickCommentInfo::FromJsonObject(it_info,info);
						item.quick_comment_list.emplace_back(info);
					}					
				}
			}
			response.message_quick_comment_list.emplace_back(item);
		}
	}
}