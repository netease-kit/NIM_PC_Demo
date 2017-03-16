#include "data_sync_callback.h"
#include "module/session/session_manager.h"

namespace nim_comp
{
void DataSyncCallback::SyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info)
{
	if (sync_type == nim::kNIMDataSyncTypeTeamInfo)
		TeamService::GetInstance()->OnTeamDataSyncCallback(sync_type, status, data_sync_info);
	else if (sync_type == nim::kNIMDataSyncTypeAllTeamUserList)
		SessionManager::GetInstance()->QueryMyAllTeamMemberInfos();
}
}