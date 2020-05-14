/** @file nim_talkex_helper_collect.cpp
  * @brief TalkEx 收藏辅助方法和数据结构定义
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#include "src/cpp_sdk/nim/helper/nim_talkex_helper_collect.h"
namespace nim
{
	void CollectInfo::FromJsonString(const std::string& json_string, CollectInfo& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_string, values))
		{
			info.id = values[kNIMCollectInfoKeyID].asUInt64();
			info.type = values[kNIMCollectInfoKeyType].asUInt64();
			info.data = values[kNIMCollectInfoKeyData].asString();
			info.ext = values[kNIMCollectInfoKeyExt].asString();
			info.unique_id = values[kNIMCollectInfoKeyUniqueID].asString();
			info.create_time = values[kNIMCollectInfoKeyCreateTime].asUInt64();
			info.update_time = values[kNIMCollectInfoKeyUpdateTime].asUInt64();
		}
	}
	std::string CollectInfo::ToJsonString(const CollectInfo& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMCollectInfoKeyID] = info.id;
		values[kNIMCollectInfoKeyType] = info.type;
		values[kNIMCollectInfoKeyData] = info.data;
		values[kNIMCollectInfoKeyExt] = info.ext;
		values[kNIMCollectInfoKeyUniqueID] = info.unique_id;
		values[kNIMCollectInfoKeyCreateTime] = info.create_time;
		values[kNIMCollectInfoKeyUpdateTime] = info.update_time;
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	void CollectInfoList::FromJsonString(const std::string& json_string, CollectInfoList& list)
	{
		nim_cpp_wrapper_util::Json::Value values;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_string, values))
		{
			for (auto it : values)
			{
				CollectInfo info;
				info.id = it[kNIMCollectInfoKeyID].asUInt64();
				info.type = it[kNIMCollectInfoKeyType].asUInt64();
				info.data = it[kNIMCollectInfoKeyData].asString();
				info.ext = it[kNIMCollectInfoKeyExt].asString();
				info.unique_id = it[kNIMCollectInfoKeyUniqueID].asString();
				info.create_time = it[kNIMCollectInfoKeyCreateTime].asUInt64();
				info.update_time = it[kNIMCollectInfoKeyUpdateTime].asUInt64();
				list.list.emplace_back(info);
			}
		}
	}
	std::string MatchCollectParm::ToJsonString(const MatchCollectParm& param)
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMMatchCollectInfoKeyID] = param.id;
		values[kNIMMatchCollectInfoKeyCreateTime] = param.create_time;
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	std::string RemoveCollectsParm::ToJsonString(const RemoveCollectsParm& param)
	{
		nim_cpp_wrapper_util::Json::Value values;
		for (auto it : param.list)
		{
			nim_cpp_wrapper_util::Json::Value item;
			item[kNIMMatchCollectInfoKeyID] = it.id;
			item[kNIMMatchCollectInfoKeyCreateTime] = it.create_time;
			values.append(item);
		}
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
	std::string QueryCollectsParm::ToJsonString(const QueryCollectsParm& info)
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMQueryCollectKeyFromTime] = info.from_time;
		values[kNIMQueryCollectKeyToTime] = info.to_time;
		values[kNIMQueryCollectKeyExcludeID] = info.exclude_id;
		values[kNIMQueryCollectKeyLimit] = info.limit;
		values[kNIMQueryCollectKeyReverse] = info.reverse;
		values[kNIMQueryCollectKeyType] = info.type;
		return nim_cpp_wrapper_util::Json::FastWriter().write(values);
	}
}