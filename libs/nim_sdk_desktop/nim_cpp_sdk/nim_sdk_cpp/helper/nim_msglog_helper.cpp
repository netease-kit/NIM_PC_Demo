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
