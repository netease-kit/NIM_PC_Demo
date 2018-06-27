#include "config_helper.h"
#include "module/db/public_db.h"


ConfigHelper::ConfigHelper() :
adapt_dpi_(true), uistyle_id_(1)
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
}