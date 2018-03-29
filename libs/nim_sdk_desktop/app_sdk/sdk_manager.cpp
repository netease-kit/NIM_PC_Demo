#include "sdk_manager.h"
#include "base/util/string_util.h"
#include "nim_win32_demo_helper.h"
#include "base/util/base64.h"
#include "base/encrypt/encrypt_impl.h"
#include "base/util/string_util.h"
#include "shared/log.h"
#include "base/file/file_util.h"
#include "base/win32/platform_string_util.h"
#include "shared/threads.h"

namespace app_sdk
{
	SDKManager::SDKManager()
	{
	}

	SDKManager::~SDKManager()
	{
	}

	static const std::vector<std::string> g_appkeys = { "45c6af3c98409b18a84451215d0bdd6e", "", "", "45c6af3c98409b18a84451215d0bdd6e" };
	static const std::vector<std::string> g_hosts = { "", "http://10.100.2.173", "http://10.100.7.226", "https://portal.etransfar.com" };

	void SDKManager::SetServerType(SERVER_TYPE type)
	{
		if (type == kST_DEFAULT)
			server_type_ = kST_ONLINE;
		else
			server_type_ = type;
	}

	std::string SDKManager::GetAppkey(SERVER_TYPE type/* = kST_DEFAULT*/)
	{
		if (type == kST_DEFAULT)
			type = server_type_;
		return g_appkeys[type];
	}

	std::string SDKManager::GetHost(SERVER_TYPE type/* = kST_DEFAULT*/)
	{
		if (type == kST_DEFAULT)
			type = server_type_;
		return g_hosts[type];
	}

}