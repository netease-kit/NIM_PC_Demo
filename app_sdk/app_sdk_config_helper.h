#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "jsoncpp/include/json/json.h"
#include "ui_component\ui_kit\module\runtime_data\runtime_data_manager.h"
#include "app_sdk/app_config/app_sdk_config.h"
namespace app_sdk
{
	enum NimServerConfType
	{
		NimServerConfType_Int = 0,
		NimServerConfType_UInt,
		NimServerConfType_Int64,
		NimServerConfType_UInt64,
		NimServerConfType_Double,
		NimServerConfType_String,
		NimServerConfType_Bool,
		NimServerConfType_Array,
		NimServerConfType_Object,
	};
	static bool GetNimServerConfJsonObject(const std::string& key,Json::Value& value)
	{
		static Json::Value value_config;
		static std::once_flag of;
		bool ret = false;
		std::call_once(of, [](){
			std::string server_conf_path = app_sdk::AppSDKConfig::GetInstance()->GetAppConfigPath();
			std::filebuf file;
			if (!server_conf_path.empty() && file.open(server_conf_path, std::ios::in))
			{
				Json::Reader().parse(std::istream(&file), value_config);
				file.close();
			}				
		});
		if (value_config.isObject() && value_config.isMember(key))
		{
			value = value_config[key];
			ret = true;
		}
		return ret;		
	}
	/**
	* 获取连接服务器的某一个配置信息
	* @param[in] key 需要获取的信息关键字
	* @return string 配置信息
	*/
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_Int, INT>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asInt();
		return 0;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_UInt, UINT>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asUInt();
		return 0;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_Int64, INT64>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asInt64();
		return 0;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_UInt64, UINT64>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asUInt64();
		return 0;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_Double, DOUBLE>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asDouble();
		return 0.0;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_String, std::string>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asString();
		return "";
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_Bool, bool>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value.asBool();
		return false;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_Array, Json::Value>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value;
		return value;
	}
	template <int type>
	static auto GetConfigValueFromNimServerConf(const std::string& key)->
		typename std::enable_if<type == NimServerConfType::NimServerConfType_Object, Json::Value>::type
	{
		Json::Value value;
		if (GetNimServerConfJsonObject(key, value))
			return value;
		return value;
	}
	std::string GetConfigStringValueFromNimServerConf(const std::string &key, NimServerConfType type)
	{
		std::string ret;
		switch (type)
		{
		case app_sdk::NimServerConfType_Int:
			ret = nbase::StringPrintf("%d", GetConfigValueFromNimServerConf<NimServerConfType_Int>(key));
			break;
		case app_sdk::NimServerConfType_UInt:
			ret = nbase::StringPrintf("%u", GetConfigValueFromNimServerConf<NimServerConfType_UInt>(key));
			break;
		case app_sdk::NimServerConfType_Int64:
			ret = nbase::StringPrintf("%ll", GetConfigValueFromNimServerConf<NimServerConfType_Int64>(key));
			break;
		case app_sdk::NimServerConfType_UInt64:
			ret = nbase::StringPrintf("%ull", GetConfigValueFromNimServerConf<NimServerConfType_UInt64>(key));
			break;
		case app_sdk::NimServerConfType_Double:
			ret = nbase::StringPrintf("%f", GetConfigValueFromNimServerConf<NimServerConfType_Double>(key));
			break;
		case app_sdk::NimServerConfType_String:
			ret = GetConfigValueFromNimServerConf<NimServerConfType_String>(key);
			break;
		case app_sdk::NimServerConfType_Bool:
			ret = nbase::StringPrintf("%d", GetConfigValueFromNimServerConf<NimServerConfType_Bool>(key) ? 1 : 0);
			break;
		case app_sdk::NimServerConfType_Array:
			ret = GetConfigValueFromNimServerConf<NimServerConfType_Array>(key).toStyledString();
			break;
		case app_sdk::NimServerConfType_Object:
			ret = GetConfigValueFromNimServerConf<NimServerConfType_Object>(key).toStyledString();
			break;
		default:
			break;
		}
		return ret;
	}
}