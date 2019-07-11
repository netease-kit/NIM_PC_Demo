/** @file nim_json_util.cpp
  * @brief JSON辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/08
  */
#include "convert_utf.h"
#include "nim_json_util.h"
namespace nim
{

bool StrListToJsonString(const std::list<std::string>& list, std::string& out)
{
	nim_cpp_wrapper_util::Json::Value json_ids;
	for (auto it = list.begin(); it != list.end(); it++)
	{
		json_ids.append(*it);
	}
	out = GetJsonStringWithNoStyled(json_ids);
	return true;
}

bool JsonStrArrayToList(const nim_cpp_wrapper_util::Json::Value& array_str, std::list<std::string>& out)
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

bool JsonArrayStringToList(const std::string& array_str, std::list<std::string>& out)
{
	nim_cpp_wrapper_util::Json::Value value;
	if (ParseJsonValue(array_str, value) && value.isArray())
	{
		return JsonStrArrayToList(value, out);
	}
	return false;
}

bool ParseJsonValue(const std::string &content, nim_cpp_wrapper_util::Json::Value &values)
{
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(content, values))
	{
		return true;
	}
	return false;
}

std::string GetJsonStringWithNoStyled(const nim_cpp_wrapper_util::Json::Value& values)
{
	nim_cpp_wrapper_util::Json::FastWriter fw;
	return fw.write(values);
}

nim_cpp_wrapper_util::Json::Value GetJsonValueFromJsonString(const std::string& json_string)
{
	nim_cpp_wrapper_util::Json::Value value;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (!json_string.empty() && !reader.parse(json_string, value) /*|| !value.isArray()*/)
		assert(0);

	return value;
}
}