/** @file nim_cpp_data_sync.cpp
  * @brief NIM SDK提供的数据同步相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/09/23
  */

#include "nim_cpp_data_sync.h"
#include "nim_sdk_helper.h"

namespace nim
{

typedef void(*nim_data_sync_reg_complete_cb)(nim_data_sync_cb_func cb, const void *user_data);


static void CallbackSyncComplete(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const char *json_attachment, const void *user_data)
{
	if (user_data)
	{
		DataSync::DataSyncCallback* cb_pointer = (DataSync::DataSyncCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(sync_type, status);
		}
	}
}

static DataSync::DataSyncCallback* g_cb_data_sync_ = nullptr;
void DataSync::RegCompleteCb(const DataSyncCallback& cb)
{
	if (g_cb_data_sync_)
	{
		delete g_cb_data_sync_;
		g_cb_data_sync_ = nullptr;
	}
	g_cb_data_sync_ = new DataSyncCallback(cb);
	return NIM_SDK_GET_FUNC(nim_data_sync_reg_complete_cb)(&CallbackSyncComplete, g_cb_data_sync_);
}

void DataSync::UnregDataSyncCb()
{
	if (g_cb_data_sync_)
	{
		delete g_cb_data_sync_;
		g_cb_data_sync_ = nullptr;
	}
}



}