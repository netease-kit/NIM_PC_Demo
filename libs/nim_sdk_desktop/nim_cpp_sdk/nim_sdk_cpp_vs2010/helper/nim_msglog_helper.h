/** @file nim_msglog_helper.h
  * @brief msglog 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/22
  */

#ifndef _NIM_SDK_CPP_MSGLOG_HELPER_H_
#define _NIM_SDK_CPP_MSGLOG_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_talk_helper.h"

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

/** @brief 查询到消息历史结果 */
struct QueryMsglogResult
{
	int count_;						/**< 消息数 */
	std::list<IMMessage> msglogs_;	/**< 消息 */

	/** 构造函数 */
	QueryMsglogResult() : count_(0) {}
};

/** @brief 发送消息已读回执 */
struct MessageStatusChanged
{
	NIMMsgLogStatus status_;	/**< 变更后的状态 */
	std::string		talk_id_;	/**< 会话ID */
	__int64			msg_timetag_;	/**< 临界的消息的时间戳 */

	MessageStatusChanged() : msg_timetag_(0) {}
};

/** @brief 发送消息已读回执 */
struct MessageStatusChangedResult
{
	NIMResCode rescode_;	/**< 错误码 */
	std::list<MessageStatusChanged> results_;	/**< 结果 */

	MessageStatusChangedResult(int rescode, std::string &result)
	{
		rescode_ = (NIMResCode)rescode;
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(result, values) && values.isArray())
		{
			int size = values.size();
			for (int i = 0; i < size; i++)
			{
				MessageStatusChanged changed;
				changed.status_ = (NIMMsgLogStatus)values[i][kNIMMsglogStatusChangedKeyStatus].asUInt();
				changed.talk_id_ = values[i][kNIMMsglogStatusChangedKeyTalkID].asString();
				changed.msg_timetag_ = values[i][kNIMMsglogStatusChangedKeyMsgTimetag].asInt64();
				results_.push_back(changed);
			}
		}
	}
};

/** @fn bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res)
  * @brief 解析查询到的消息历史结果
  * @param[in] msgs_json 查询到的消息历史(Json Value数据字符串)
  * @param[out] res 消息历史结果
  * @return bool 解析成功 或失败
  */
bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res);

} //namespace nim

#endif //_NIM_SDK_CPP_MSGLOG_HELPER_H_