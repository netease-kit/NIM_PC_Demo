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

bool UserConfig::GetAppLocalVersion(std::wstring &version)
{
	return nim_comp::LocalHelper::GetSDKVersion(version);
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

