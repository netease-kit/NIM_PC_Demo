#pragma once
namespace app_sdk
{
	class AppSDKConfig : public nbase::Singleton<AppSDKConfig>
	{
		SingletonHideConstructor(AppSDKConfig)
	private:
		AppSDKConfig() = default;
		~AppSDKConfig() = default;;
	public:
		std::string GetAppConfigPath() const;
		void SetAppConfigPath(const std::string& path);
	private:
		mutable std::string app_config_path_;
	};
}
