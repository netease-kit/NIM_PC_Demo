#include "config_helper.h"
#include "module/db/public_db.h"


ConfigHelper::ConfigHelper() :
	adapt_dpi_(true)
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
	Json::FastWriter writer;
	Json::Value value;
	value["language"] = language;
	std::string language_config = writer.write(value);
	bool ret = PublicDB::GetInstance()->InsertConfigData("language", language_config);
	if (ret)
		language_ = language;
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
	return "zh_CN"; //缺省为中文
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
}