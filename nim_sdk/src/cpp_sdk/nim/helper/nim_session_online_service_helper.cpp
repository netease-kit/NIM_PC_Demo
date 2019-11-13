/** @file nim_cpp_session_online_service_helper.cpp
  * @brief SessionOnLineService 辅助方法和数据结构定义
  * @copyright (c) 2015-2019, NetEase Inc. All rights reserved
  * @date 2019/10/22
  */

#include "src/cpp_sdk/nim/helper/nim_session_online_service_helper.h"
#include "public_define/defines/nim_define/nim_session_online_service_def.h"
namespace nim
{
		void SessionOnLineServiceHelper::DeleteSessionParam::AddSession(nim::NIMSessionType to_type, std::string session_id)
		{
			delete_list_.emplace_back(std::make_pair(to_type, session_id));
		}
		bool SessionOnLineServiceHelper::ParseSessionInfo(const std::string& session_json, SessionInfo& session)
		{
			nim_cpp_wrapper_util::Json::Value json_session;
			if (!nim_cpp_wrapper_util::Json::Reader().parse(session_json, json_session))
				return false;
			session.type_ = (NIMSessionType)json_session[kMINSessionOLSRVInfoType].asInt();
			session.id_ = json_session[kMINSessionOLSRVInfoID].asString();
			session.update_time_ = json_session[kMINSessionOLSRVInfoUpdateT].asUInt64();
			session.ext_ = json_session[kMINSessionOLSRVInfoEXT].asString();
			session.last_message_ = json_session[kMINSessionOLSRVInfoLastMSG].asString();
			return true;
		}
		bool SessionOnLineServiceHelper::ParseQureySessionListResult(const std::string& result_json, QureySessionListResult& result)
		{
			nim_cpp_wrapper_util::Json::Value json_result;
			if (!nim_cpp_wrapper_util::Json::Reader().parse(result_json, json_result))
				return false;
			result.res_code = (NIMResCode)json_result[kMINSessionOLSRVQueryListRESCode].asInt();
			result.has_more_ = json_result[kMINSessionOLSRVQueryListHasMore].asBool();
			for (auto & it : json_result[kMINSessionOLSRVQueryListInfos])
			{
				SessionInfo session;
				session.type_ = (NIMSessionType)it[kMINSessionOLSRVInfoType].asInt();
				session.id_ = it[kMINSessionOLSRVInfoID].asString();
				session.update_time_ = it[kMINSessionOLSRVInfoUpdateT].asUInt64();
				session.ext_ = it[kMINSessionOLSRVInfoEXT].asString();
				session.last_message_ = it[kMINSessionOLSRVInfoLastMSG].asString();
				result.session_list_.emplace_back(session);
			}
			return true;
		}
		std::string SessionOnLineServiceHelper::DeleteSessionParamToJsonString(const DeleteSessionParam& param)
		{
			nim_cpp_wrapper_util::Json::Value json_param;
			for (auto& it : param.delete_list_)
			{
				nim_cpp_wrapper_util::Json::Value item;
				item[kMINSessionOLSRVDeleteType] = it.first;
				item[kMINSessionOLSRVDeleteID] = it.second;
				json_param.append(item);
			}
			return nim_cpp_wrapper_util::Json::FastWriter().write(json_param);
		}
}//namespace nim