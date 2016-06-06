#pragma once
#include "nim_client_helper.h"

namespace nim_comp
{
class DataSyncCallback
{
public:

	static void SyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info);
};
}