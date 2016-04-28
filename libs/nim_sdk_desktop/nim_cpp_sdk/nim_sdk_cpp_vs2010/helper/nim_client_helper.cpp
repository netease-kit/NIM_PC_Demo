/** @file nim_client_helper.cpp
  * @brief Client 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/07
  */

#include "nim_client_helper.h"

namespace nim
{

bool ParseOtherClientsPres(const Json::Value array_objs, std::list<OtherClientPres> &outs)
{
	if (array_objs.isArray())
	{
		int size = array_objs.size();
		for (int index = 0; index < size; index++)
		{
			OtherClientPres pres;
			pres.app_account_ = array_objs[index][kNIMPresAppAccount].asString();
			pres.client_type_ = (NIMClientType)array_objs[index][kNIMPresClientType].asUInt();
			pres.client_os_ = array_objs[index][kNIMPresOS].asString();
			pres.mac_address_ = array_objs[index][kNIMPresMac].asString();
			pres.device_id_ = array_objs[index][kNIMPresDeviceID].asString();
			pres.login_time_ = array_objs[index][kNIMPresLoginTime].asInt64();
			outs.push_back(pres);
		}
		return true;
	}
	return false;
}
}