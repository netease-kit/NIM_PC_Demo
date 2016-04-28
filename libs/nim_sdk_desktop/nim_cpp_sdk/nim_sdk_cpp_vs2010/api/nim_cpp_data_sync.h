/** @file nim_cpp_data_sync.h
  * @brief NIM SDK提供的数据同步相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/09/23
  */

#ifndef _NIM_SDK_CPP_DATASYNC_H_
#define _NIM_SDK_CPP_DATASYNC_H_

#include <string>
#include <functional>

/**
* @namespace nim
* @brief IM
*/
namespace nim
{
#include "nim_data_sync_def.h"

/** @class DataSync
  * @brief NIM SDK提供的数据同步相关接口
  */
class DataSync
{

public:
	typedef std::function<void(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status)> DataSyncCallback;	/**< 数据同步通知回调模板 */
	
public:
	/** @fn static void RegCompleteCb(const DataSyncCallback& cb)
	* 注册数据同步完成的回调函数
	* @param[in] cb 数据同步完成的回调函数
	* @return void 无返回值
	*/
	static void RegCompleteCb(const DataSyncCallback& cb);

	/** @fn void UnregDataSyncCb()
	* 反注册DataSync提供的所有回调
	* @return void 无返回值
	*/
	static void UnregDataSyncCb();

};

} 

#endif //_NIM_SDK_CPP_DATASYNC_H_