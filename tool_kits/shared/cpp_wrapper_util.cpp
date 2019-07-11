#include "stdafx.h"
#include "cpp_wrapper_util.h"
namespace shared
{
	namespace tools
	{
		Json::Value NimCppWrapperJsonValueToJsonValue(const nim_cpp_wrapper_util::Json::Value& param)
		{
			Json::Value ret;
			Json::Reader().parse(nim_cpp_wrapper_util::Json::FastWriter().write(param), ret);
			return ret;
		}
		nim_cpp_wrapper_util::Json::Value JsonValueToNimCppWrapperJsonValue(const Json::Value& param)
		{
			nim_cpp_wrapper_util::Json::Value ret;
			nim_cpp_wrapper_util::Json::Reader().parse(Json::FastWriter().write(param), ret);
			return ret;
		}
	}
}
