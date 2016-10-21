#pragma once
#include "nim_client_helper.h"

namespace nim_comp
{
/** @class DataSyncCallback
  * @brief 数据同步事件回调类
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/13
  */
class DataSyncCallback
{
public:

	/**
	* 注册数据同步完成的回调函数
	* @param[in] sync_type 数据同步类型
	* @param[in] status 数据同步状态
	* @param[in] data_sync_info 数据数据
	* @return void 无返回值
	*/
	static void SyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info);
};
}