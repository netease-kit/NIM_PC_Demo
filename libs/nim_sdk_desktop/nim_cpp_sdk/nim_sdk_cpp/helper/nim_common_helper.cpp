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
	out = json_ids.toStyledString();
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


}