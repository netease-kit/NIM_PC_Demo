/** @file nim_string_util.h
  * @brief string操作辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2016/09/08
  */

#ifndef _NIM_SDK_CPP_NIM_STRING_UTIL_H_
#define _NIM_SDK_CPP_NIM_STRING_UTIL_H_

#include <string>

/** @fn std::string PCharToString(const char* str)
  * @brief 字符指针转为字符串，接口内部会对nullpty进行判断
  * @param[in] str 字符指针
  * @return string 字符串
  */
std::string PCharToString(const char* str);

#endif //_NIM_SDK_CPP_NIM_STRING_UTIL_H_
