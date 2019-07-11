/** @file nim_cpp_data_sync.cpp
  * @brief NIM SDK提供的数据同步相关接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/23
  */

#include "src/cpp_sdk/nim/api//nim_cpp_data_sync.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_data_sync_reg_complete_cb)(nim_data_sync_cb_func cb, const void *user_data);
#else
#include "nim_data_sync.h"
#endif

static void CallbackSyncComplete(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const char *json_attachment, const void *user_data)
{
	CallbackProxy::DoSafeCallback<DataSync::DataSyncCallback>(user_data, [=](const DataSync::DataSyncCallback& cb_pointer){

		CallbackProxy::Invoke(cb_pointer, sync_type, status, PCharToString(json_attachment));
	});
}

static DataSync::DataSyncCallback g_cb_data_sync_ = nullptr;
void DataSync::RegCompleteCb(const DataSyncCallback& cb)
{
	g_cb_data_sync_ = cb;
	NIM_SDK_GET_FUNC(nim_data_sync_reg_complete_cb)(&CallbackSyncComplete, &g_cb_data_sync_);
}

void DataSync::UnregDataSyncCb()
{
	g_cb_data_sync_ = nullptr;
}



}
