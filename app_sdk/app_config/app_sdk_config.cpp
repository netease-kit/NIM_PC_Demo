#include "stdafx.h"
#include "app_sdk_config.h"
namespace app_sdk
{
	std::string AppSDKConfig::GetAppConfigPath() const
	{
		if (app_config_path_.empty())
		{
			std::string server_conf_path = nbase::UTF16ToUTF8(QPath::GetAppPath()).append("nim_server.conf");
			if (shared::FilePathIsExist(server_conf_path, false))
				app_config_path_ = server_conf_path;
		}
		return app_config_path_;
	}

	void AppSDKConfig::SetAppConfigPath(const std::string& path)
	{
		app_config_path_ = path;
	}
}