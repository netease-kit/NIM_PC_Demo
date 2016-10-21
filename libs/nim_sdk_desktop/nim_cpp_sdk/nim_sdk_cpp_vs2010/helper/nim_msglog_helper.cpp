/** @file nim_msglog_helper.cpp
  * @brief msglog 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/22
  */

#include "nim_msglog_helper.h"

namespace nim
{

bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(msgs_json, values) && values.isObject())
	{
		res.count_ = values[kNIMMsglogQueryKeyCount].asUInt();
		res.source_ = (NIMMsglogQuerySource)values[kNIMMsglogQueryKeySource].asUInt();
		Json::Value msgs = values[kNIMMsglogQueryKeyContent];
		int len = msgs.size();
		for (int i = 0; i < len; i++)
		{
			IMMessage msg;
			ParseMessage(msgs[i], msg);
			res.msglogs_.push_back(msg);
		}
		return true;
	}
	return false;
}

}
