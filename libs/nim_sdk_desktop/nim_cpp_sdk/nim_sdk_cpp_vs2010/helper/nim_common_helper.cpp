/** @file nim_common_helper.cpp
  * @brief SDK辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/08
  */

#include "nim_common_helper.h"

namespace nim
{

bool StrListToJsonString(const std::list<std::string>& list, std::string& out)
{
	Json::Value json_ids;
	for (auto it = list.begin(); it != list.end(); it++)
	{
		json_ids.append(*it);
	}
	out = GetJsonStringWithNoStyled(json_ids);
	return true;
}

bool JsonStrArrayToList(const Json::Value& array_str, std::list<std::string>& out)
{
	if (array_str.isArray())
	{
		int size = array_str.size();
		for (int index = 0; index < size; index++)
		{
			out.push_back(array_str[index].asString());
		}
		return true;
	}
	return false;
}

std::string PCharToString(const char* str)
{
	std::string res_str;
	if (str) {
		res_str = str;
	}
	else {
		assert(false);
	}
	return res_str;
}

std::string GetJsonStringWithNoStyled(const Json::Value& values)
{
	Json::FastWriter fw;
	return fw.write(values);
}

Json::Value GetJsonValueFromJsonString(const std::string& json_string)
{
	Json::Value value;
	Json::Reader reader;
	if (!reader.parse(json_string, value) || !value.isObject())
		assert(0);

	return value;
}
}