#include "nim_ui_user_config.h"
#include "util/user.h"
#include "module/emoji/richedit_util.h"
#include "module/local/local_helper.h"

namespace nim_ui
{

UserConfig::UserConfig()
{
	icon_id_ = 0;
}

UserConfig::~UserConfig()
{

}

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



void UserConfig::Re_GetText(ITextServices * text_service, std::wstring& text)
{
	nim_comp::Re_GetText(text_service, text);
}

bool UserConfig::GetAppLocalVersion(int &version, std::wstring &version_flag)
{
	return nim_comp::LocalHelper::GetAppLocalVersion(version, version_flag);
}

UINT UserConfig::GetIcon()
{
	return icon_id_;
}

void UserConfig::SetIcon(UINT nRes)
{
	if (nRes >= 0)
	{
		icon_id_ = nRes;
	}
}

}

