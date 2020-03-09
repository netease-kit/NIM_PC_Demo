#include "config_helper.h"
#include "module/db/public_db.h"


ConfigHelper::ConfigHelper() :
adapt_dpi_(true), uistyle_id_(1), 
private_settings_enable_(false),
private_settings_url_("")
{
	ReadConfig();
}

ConfigHelper::~ConfigHelper()
{

}

void ConfigHelper::SetAdaptDpi(bool adapt_dpi)
{
	adapt_dpi_ = adapt_dpi;

	Json::FastWriter writer;
	Json::Value value;
	value["adapt_dpi"] = adapt_dpi_;

	std::string dpi_config = writer.write(value);
	PublicDB::GetInstance()->InsertConfigData("adapt_dpi", dpi_config);
}

bool ConfigHelper::IsAdaptDpi()
{
	return adapt_dpi_;
}

void ConfigHelper::SetLanguage(const std::string& language)
{
	std::string full_language = "lang\\" + language;

	Json::FastWriter writer;
	Json::Value value;
	value["language"] = full_language;
	std::string language_config = writer.write(value);
	bool ret = PublicDB::GetInstance()->InsertConfigData("language", language_config);
	if (ret)
		language_ = full_language;
	else
	{
		ASSERT(false);
		QLOG_ERR(L"Set language failed!");
	}
}

std::string ConfigHelper::GetLanguage()
{
	if (!language_.empty())
		return language_;
	LANGID lid = GetSystemDefaultLangID();
	switch (lid)
	{
	case 0x0404://Chinese   (Taiwan   Region) 
	case 0X0804://Chinese(PRC)
	case 0x0c04://Chinese(Hong   Kong   SAR, PRC)
	case 0x1004://Chinese(Singapore)
		return "lang\\zh_CN"; 
	default:
		return "lang\\en_US";
	}
}
int ConfigHelper::GetUIStyle()
{
	return uistyle_id_;
}
void ConfigHelper::SetUIStyle(int style_id)
{
	Json::FastWriter writer;
	Json::Value value;
	value["uistyle"] = style_id;
	std::string uistyle_config = writer.write(value);
	bool ret = PublicDB::GetInstance()->InsertConfigData("uistyle", uistyle_config);
	if (ret)
		uistyle_id_ = style_id;
	else
	{
		ASSERT(false);
		QLOG_ERR(L"Set uistyle failed!");
	}
}
bool ConfigHelper::UsePrivateSettings(std::string& url) const
{
	url = private_settings_url_;
	return private_settings_enable_;
}
void ConfigHelper::UsePrivateSettings(bool use, const std::string& url)
{
	Json::FastWriter writer;
	Json::Value value;
	value["enable"] = use;
	value["url"] = url;
	std::string private_setting_config = writer.write(value);
	bool ret = PublicDB::GetInstance()->InsertConfigData("private_settings", private_setting_config);
	if (ret)
	{
		private_settings_enable_ = use;
		private_settings_url_ = url;
	}
	else
	{
		ASSERT(false);
		QLOG_ERR(L"Set private settings failed!");
	}
}

bool ConfigHelper::UsePrivateSettings() const
{
	return private_settings_enable_;
}

void ConfigHelper::UsePrivateSettings(bool use)
{
	UsePrivateSettings(use, private_settings_url_);
}

void ConfigHelper::ReadConfig()
{
	Json::Reader reader;
	Json::Value value;

	//dpi
	std::string dpi_config;
	PublicDB::GetInstance()->QueryConfigData("adapt_dpi", dpi_config);
	if (reader.parse(dpi_config, value))
	{
		if (value.isMember("adapt_dpi"))
			adapt_dpi_ = value["adapt_dpi"].asBool();
	}

	//语言
	std::string language_config;
	PublicDB::GetInstance()->QueryConfigData("language", language_config);
	if (reader.parse(language_config, value))
	{
		if (value.isMember("language"))
			language_ = value["language"].asString();
	}

	//uistyle
	std::string uistyle_config;
	PublicDB::GetInstance()->QueryConfigData("uistyle", uistyle_config);
	if (reader.parse(uistyle_config, value))
	{
		if (value.isMember("uistyle"))
			uistyle_id_ = value["uistyle"].asInt();
	}
	//private settings
	std::string private_setting_config;
	PublicDB::GetInstance()->QueryConfigData("private_settings", private_setting_config);
	if (reader.parse(private_setting_config, value))
	{
		if (value.isMember("enable"))
			private_settings_enable_ = value["enable"].asBool();
		if(value.isMember("url"))
			private_settings_url_ = value["url"].asString();
	}
}