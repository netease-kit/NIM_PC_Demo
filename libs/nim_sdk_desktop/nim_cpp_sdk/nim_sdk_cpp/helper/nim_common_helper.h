/** @file nim_common_helper.h
  * @brief SDK辅助方法
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/08
  */

#ifndef _NIM_SDK_CPP_COMMON_HELPER_H_
#define _NIM_SDK_CPP_COMMON_HELPER_H_

#include <string>
#include <list>
#include "assert.h"
#include "json.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @enum 自定义的布尔值类型数据的替代数据类型 */
enum BoolStatus
{
	BS_NOT_INIT = -1,	/**< 未初始化 */
	BS_FALSE	= 0,	/**< false */
	BS_TRUE		= 1		/**< true */
};

/** @fn bool StrListToJsonString(const std::list<std::string>& list, std::string& out)
  * @brief 将一个string类型的list组装成一个Json Array
  * @param[in] list string类型的list
  * @param[out] out Json Array
  * @return bool 成功 或失败
  */
bool StrListToJsonString(const std::list<std::string>& list, std::string& out);

/** @fn bool JsonStrArrayToList(const Json::Value& array_str, std::list<std::string>& out)
  * @brief 将一个string类型的Json Array解析成一个string类型的list
  * @param[in] array_str string类型的Json Array
  * @param[out] out string类型的list
  * @return bool 解析成功 或失败
  */
bool JsonStrArrayToList(const Json::Value& array_str, std::list<std::string>& out);

/** @fn std::string PCharToString(const char* str)
  * @brief 字符指针转为字符串，接口内部会对nullpty进行判断
  * @param[in] str 字符指针
  * @return string 字符串
  */
std::string PCharToString(const char* str);

}

#endif //_NIM_SDK_CPP_COMMON_HELPER_H_