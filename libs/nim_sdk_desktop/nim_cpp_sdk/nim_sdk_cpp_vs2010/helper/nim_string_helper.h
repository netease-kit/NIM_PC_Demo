/** @file nim_string_helper.h
  * @brief string操作辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2016/09/08
  */

#ifndef _NIM_SDK_CPP_STRING_HELPER_H_
#define _NIM_SDK_CPP_STRING_HELPER_H_

#include <string>

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
#ifdef WIN32

typedef wchar_t UTF16Char;
typedef char UTF8Char;
void UTF8ToUTF16(const UTF8Char *utf8, size_t length, std::basic_string<UTF16Char> &utf16);
void UTF16ToUTF8(const UTF16Char *utf16, size_t length, std::basic_string<UTF8Char> &utf8);
void UTF8ToUTF16(const std::basic_string<UTF8Char> &utf8, std::basic_string<UTF16Char> &utf16);
void UTF16ToUTF8(const std::basic_string<UTF16Char> &utf16, std::basic_string<UTF8Char> &utf8);

#endif
}

#endif //_NIM_SDK_CPP_STRING_HELPER_H_