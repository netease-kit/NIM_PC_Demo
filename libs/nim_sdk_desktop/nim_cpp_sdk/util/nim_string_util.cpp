/** @file nim_string_util.cpp
  * @brief string操作辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2016/09/08
  */

#include "nim_string_util.h"
#include "third_party/convert_utf/convert_utf.h"

namespace nim
{

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

#ifdef WIN32

#ifndef COUNT_OF
#define COUNT_OF(array)			(sizeof(array)/sizeof(array[0]))
#endif

#define UTF16_STR_BUFF_SIZE		4096
#define UTF8_STR_BUFF_SIZE		8192

void UTF8ToUTF16(const UTF8Char *utf8, size_t length, std::basic_string<UTF16Char> &utf16)
{
	UTF16Char output[UTF16_STR_BUFF_SIZE];
	const UTF8 *src_begin = reinterpret_cast<const UTF8 *>(utf8);
	const UTF8 *src_end = src_begin + length;
	UTF16 *dst_begin = reinterpret_cast<UTF16 *>(output);

	utf16.clear();
	while (src_begin < src_end)
	{
		ConversionResult result = ConvertUTF8toUTF16(&src_begin,
													 src_end,
													 &dst_begin,
													 dst_begin + COUNT_OF(output),
													 lenientConversion);

		utf16.append(output, dst_begin - reinterpret_cast<UTF16 *>(output));
		dst_begin = reinterpret_cast<UTF16 *>(output);
		if (result == sourceIllegal || result == sourceExhausted)
		{
			utf16.clear();
			break;
		}
	}
}

void UTF16ToUTF8(const UTF16Char *utf16, size_t length, std::basic_string<UTF8Char> &utf8)
{
	UTF8Char output[UTF8_STR_BUFF_SIZE];
	const UTF16 *src_begin = reinterpret_cast<const UTF16 *>(utf16);
	const UTF16 *src_end = src_begin + length;
	UTF8 *dst_begin = reinterpret_cast<UTF8 *>(output);

	utf8.clear();
	while (src_begin < src_end)
	{
		ConversionResult result = ConvertUTF16toUTF8(&src_begin,
													 src_end,
													 &dst_begin,
													 dst_begin + COUNT_OF(output),
													 lenientConversion);

		utf8.append(output, dst_begin - reinterpret_cast<UTF8 *>(output));
		dst_begin = reinterpret_cast<UTF8 *>(output);
		if (result == sourceIllegal || result == sourceExhausted)
		{
			utf8.clear();
			break;
		}
	}
}

void UTF8ToUTF16(const std::basic_string<UTF8Char> &utf8, std::basic_string<UTF16Char> &utf16)
{
	UTF8ToUTF16(utf8.c_str(), utf8.length(), utf16);
}

void UTF16ToUTF8(const std::basic_string<UTF16Char> &utf16, std::basic_string<UTF8Char> &utf8)
{
	UTF16ToUTF8(utf16.c_str(), utf16.length(), utf8);
}

#endif

}