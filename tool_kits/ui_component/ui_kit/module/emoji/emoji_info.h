#pragma once

#include "shared/util.h"
#include "module/emoji/richedit_util.h"
#include "module/emoji/richedit_olecallback.h"

namespace nim_comp
{
	namespace emoji
	{
		struct Emoticon
		{
			std::wstring  tag;  //such as "[大笑]"
			std::wstring  file; //"emoji_00.png"
			std::wstring  path; //full path
		};

		/**
		* 加载表情信息
		* @return void 无返回值
		*/
		void LoadEmoji();

		/**
		* 获取表情信息
		* @param[out] emoticon 表情信息列表
		* @return void 无返回值
		*/
		void GetEmoticon(std::vector<Emoticon> &emoticon);

		/**
		* 获取表情文件名
		* @param[in] tag 表情名称(如:[大笑])
		* @param[out] file 表情文件名
		* @return bool true 获取成功，false 获取失败
		*/
		bool GetEmojiFileByTag(const std::wstring tag, std::wstring& file);

	}
}