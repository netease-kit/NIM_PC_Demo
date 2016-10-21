#include "nim_ui_user_config.h"
#include "util/user.h"
#include "module/emoji/richedit_util.h"
#include "module/local/local_helper.h"

namespace nim_ui
{

std::wstring UserConfig::GetUserDataPath()
{
	return nim_comp::GetUserDataPath();
}

std::wstring UserConfig::GetUserImagePath()
{
	return nim_comp::GetUserImagePath();
}

std::wstring UserConfig::GetUserAudioPath()
{
	return nim_comp::GetUserAudioPath();
}

bool UserConfig::GetAppLocalVersion(int &version, std::wstring &version_flag)
{
	return nim_comp::LocalHelper::GetAppLocalVersion(version, version_flag);
}

UINT UserConfig::GetDefaultIcon()
{
	return icon_id_;
}

void UserConfig::SetDefaultIcon(UINT nRes)
{
	if (nRes >= 0)
	{
		icon_id_ = nRes;
	}
}

}

