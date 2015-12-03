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

		void LoadEmoji(); //加载表情信息

		void GetEmoticon(std::vector<Emoticon> &emoticon); //获取表情信息

		// tag=/大笑 file=emoji_00.png
		bool GetEmojiFileByTag(const std::wstring tag, std::wstring& file);

		// 插入表情到RichEdit
		void InsertFaceToEdit(ui::RichEdit* edit, const std::wstring &file_name, const std::wstring &tag);

		// 将一段文字（其中可能包含表情）插入到RichEdit中
		void InsertToEdit(ui::RichEdit* edit, const std::wstring &str);

	}
}