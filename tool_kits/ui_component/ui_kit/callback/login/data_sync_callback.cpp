#include "data_sync_callback.h"

namespace nim_comp
{
void DataSyncCallback::SyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info)
{
	if (sync_type == nim::kNIMDataSyncTypeTeamInfo)
		nim_comp::TeamService::GetInstance()->OnTeamDataSyncCallback(sync_type, status, data_sync_info);
}
}