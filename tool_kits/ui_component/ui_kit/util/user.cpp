#include "user.h"
#include "module/login/login_manager.h"

namespace
{
	static const std::wstring kResImage = L"image\\";
	static const std::wstring kResAudio = L"audio\\";
	static const std::wstring kResOther = L"res\\";
}

namespace nim_comp
{
std::wstring GetUserDataPath()
{
	std::string account = LoginManager::GetInstance()->GetAccount();
	assert( !account.empty() );
	std::wstring user_app_data_dir = QPath::GetUserAppDataDir( account );
	return user_app_data_dir;
}

std::wstring GetUserImagePath()
{
	std::wstring dir = GetUserDataPath();
	dir.append(kResImage);
	return dir;
}

std::wstring GetUserAudioPath()
{
	std::wstring dir = GetUserDataPath();
	dir.append(kResAudio);
	return dir;
}

std::wstring GetUserOtherResPath()
{
	std::wstring dir = GetUserDataPath();
	dir.append(kResOther);
	return dir;
}

}