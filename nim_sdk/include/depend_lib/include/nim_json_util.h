/** @file nim_json_util.h
  * @brief JSON辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/08
  */

#ifndef _NIM_SDK_CPP_NIM_JSON_UTIL_H_
#define _NIM_SDK_CPP_NIM_JSON_UTIL_H_

#include <string>
#include <list>
#include "assert.h"
#include "include/depend_lib/include/json/json.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @fn bool StrListToJsonString(const std::list<std::string>& list, std::string& out)
  * @brief 将一个string类型的list组装成一个Json Array
  * @param[in] list string类型的list
  * @param[out] out Json Array
  * @return bool 成功 或失败
  */
	 bool StrListToJsonString(const std::list<std::string>& list, std::string& out);

	          
/** @fn bool JsonStrArrayToList(const nim_cpp_wrapper_util::Json::Value& array_str, std::list<std::string>& out)
  * @brief 将一个string类型的Json Array解析成一个string类型的list
  * @param[in] array_str string类型的Json Array
  * @param[out] out string类型的list
  * @return bool 解析成功 或失败
  */
	 bool JsonStrArrayToList(const nim_cpp_wrapper_util::Json::Value& array_str, std::list<std::string>& out);

/** @fn bool JsonArrayStringToList(const std::string& array_str, std::list<std::string>& out)
  * @brief 将一个string类型的Json Array解析成一个string类型的list
  * @param[in] array_str string类型的Json Array
  * @param[out] out string类型的list
  * @return bool 解析成功 或失败
  */
	 bool JsonArrayStringToList(const std::string& array_str, std::list<std::string>& out);

/** @fn bool ParseJsonValue(const std::string &content, nim_cpp_wrapper_util::Json::Value &values);
  * @brief 解析JsonString
  * @param[in] content JsonString
  * @param[out] values JsonValue
  * @return bool 解析成功失败
  */
	 bool ParseJsonValue(const std::string &content, nim_cpp_wrapper_util::Json::Value &values);

/** @fn std::string GetJsonStringWithNoStyled(const nim_cpp_wrapper_util::Json::Value& values);
  * @brief 获得非格式化的Json string,传入SDK的json string格式要求为非格式化的，如果是格式化的json string可能会影响功能
  * @param[in] values Json Value
  * @return std::string 非格式化的json string
  */
	 std::string GetJsonStringWithNoStyled(const nim_cpp_wrapper_util::Json::Value& values);

/** @fn Json::Value GetJsonValueFromJsonString(const std::string& json_string);
  * @brief 解析JsonString
  * @return JsonValue
  */
	 nim_cpp_wrapper_util::Json::Value GetJsonValueFromJsonString(const std::string& json_string);

}

#endif //_NIM_SDK_CPP_NIM_JSON_UTIL_H_