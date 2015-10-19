#pragma once


#include <string>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_data_sync_def.h"

/** @class DataSync
  * @brief NIM SDK提供的数据同步相关接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class DataSync
{

public:
	typedef std::function<void(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status)> DataSyncCallback;
	
	/** 
	* 注册数据同步完成的回调函数
	* @param[in] cb 数据同步完成的回调函数
	* @return void 无返回值
	*/
	static void RegCompleteCb(const DataSyncCallback& cb);
};

} 