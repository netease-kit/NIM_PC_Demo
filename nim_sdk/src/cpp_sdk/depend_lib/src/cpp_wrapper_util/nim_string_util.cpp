/** @file nim_string_util.cpp
  * @brief string操作辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2016/09/08
  */

#include "nim_string_util.h"
#include "convert_utf.h"
std::string PCharToString(const char* str)
{
	std::string res_str;
	if (str) {
		res_str = str;
	}
	else {
		//assert(false);
	}
	return res_str;
}