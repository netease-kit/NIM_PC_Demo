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
			PostTaskToUIThread(std::bind((*cb_pointer), sync_type, status));
// 			(*cb_pointer)(sync_type, status);
		}
	}
}

static DataSync::DataSyncCallback* g_cb_pointer = nullptr;
void DataSync::RegCompleteCb(const DataSyncCallback& cb)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new DataSyncCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_data_sync_reg_complete_cb)(&CallbackSyncComplete, g_cb_pointer);
}



}