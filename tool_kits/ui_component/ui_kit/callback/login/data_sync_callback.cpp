#include "data_sync_callback.h"
#include "export/nim_ui_team_manager.h"

namespace nim_comp
{
void DataSyncCallback::SyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info)
{
	if (sync_type == nim::kNIMDataSyncTypeTeamInfo)
		nim_ui::TeamManager::GetInstance()->InvokeTeamDataSyncCallback(sync_type, status, data_sync_info);
}
}