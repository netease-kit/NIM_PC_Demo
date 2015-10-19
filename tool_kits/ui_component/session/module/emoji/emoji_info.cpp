#include "emoji_info.h"
#include "richedit_util.h"


#define EMOTICON_EMOTICON_TAG	"Tag"
#define EMOTICON_EMOTICON_FILE	"File"

namespace nim_comp
{
	namespace emoji
	{
		std::vector<Emoticon> g_emoticon; //for display
		std::map<std::wstring, std::wstring> g_emoticon_pair; //for find

		void ParseSingleEmoji(const TiXmlElement *element, Emoticon &emoticon)
		{
			static std::wstring dir = QPath::GetAppPath();

			std::string tag = element->Attribute(EMOTICON_EMOTICON_TAG);
			emoticon.tag = nbase::UTF8ToUTF16(tag);

			std::string file = element->Attribute(EMOTICON_EMOTICON_FILE);
			emoticon.file = nbase::UTF8ToUTF16(file);

			emoticon.path = dir + L"res\\emoji\\" + emoticon.file;
		}

		void ParseEmoji(const TiXmlElement* element, bool for_display)
		{
			if (element)
			{
				const TiXmlElement* child = element->FirstChildElement();
				while (child)
				{
					Emoticon emo;
					ParseSingleEmoji(child, emo);

					if (for_display)
						g_emoticon.push_back(emo);

					g_emoticon_pair[emo.tag] = emo.file;

					child = child->NextSiblingElement();
				}
			}
		}

		void LoadEmoji()
		{
			std::wstring xml_file = QPath::GetAppPath();
			xml_file.append(L"res\\emoji.xml");

			std::unique_ptr<FILE, nbase::DeleterFileClose> fp;
			FILE* fp_file = nullptr;
			errno_t err = _wfopen_s(&fp_file, xml_file.c_str(), L"rb");
			fp.reset(fp_file);
			if (fp)
			{
				TiXmlDocument document;
				if (document.LoadFile(fp.get()))
				{
					TiXmlElement *root = document.RootElement();
					if (root)
					{
						TiXmlElement *child = root->FirstChildElement();
						ParseEmoji(child, true);

						while (child)
						{
							ParseEmoji(child, false);
							child = child->NextSiblingElement();
						}
					}
				}
				else
				{
					QLOG_ERR(L"load xml fail: {0}") << xml_file;
				}
			}
			else
			{
				QLOG_ERR(L"open xml file fail: {0}") << err;
			}
		}

		void GetEmoticon(std::vector<Emoticon> &emoticon)
		{
			emoticon = g_emoticon;
		}

		bool GetEmojiFileByTag(const std::wstring tag, std::wstring& file)
		{
			std::map<std::wstring, std::wstring>::const_iterator i = g_emoticon_pair.find(tag);
			if (i == g_emoticon_pair.end())
				return false;
			else
			{
				file = i->second;
				return true;
			}
		}

		void InsertFaceToEdit(ui::RichEdit* edit, const std::wstring &file_name, const std::wstring &tag)
		{
			std::wstring file = QPath::GetAppPath();
			file.append(L"res\\emoji\\" + file_name);
			if (nbase::FilePathIsExist(file, false))
			{
				ITextServices* service = edit->GetTextServices();
				if (!Re_InsertFace(service, file, tag))
				{
					QLOG_ERR(L"insert emoj {0} {1} fail") << tag << file;
				}
				service->Release();
			}
			else
			{
				QLOG_ERR(L"emoj {0} {1} miss") << tag << file;
			}
		}

		//把纯文本插入到RichEdit中
		void _InsertTextToEdit(ui::RichEdit* edit, std::wstring &text)
		{
			if (text.empty())
				return;

			ITextServices* service = edit->GetTextServices();
			Re_ReplaceSel(service, text, false);
			service->Release();

			text.clear();
		}

		//将表情插入到RichEdit中
		void _InsertEmojiToEdit(ui::RichEdit* edit, std::wstring &emoji_file, std::wstring tag)
		{
			if (emoji_file.empty())
				return;

			InsertFaceToEdit(edit, emoji_file, tag);

			emoji_file.clear();
		}

		//处理可能内嵌的情况，"[[[大笑]"
		void _FindEmoji(ui::RichEdit* edit, const std::wstring &str)
		{
			const wchar_t kCp = L'[';

			std::wstring txt, emo, file;

			size_t p = str.rfind(kCp);
			assert(p != str.npos);
			if (p == 0)
				emo = str;
			else
			{
				txt = str.substr(0, p);
				emo = str.substr(p);
			}

			_InsertTextToEdit(edit, txt);
			if (GetEmojiFileByTag(emo, file))
				_InsertEmojiToEdit(edit, file, emo);
			else
				_InsertTextToEdit(edit, emo);
		}

		//表情格式 "[大笑]"
		void InsertToEdit(ui::RichEdit* edit, const std::wstring &str)
		{
			const wchar_t kCp = L'[', kCq = L']';

			size_t p1 = 0, p2 = 0, q = 0, len = str.size();
			std::wstring emo, file, txt;
			while (p1 < len)
			{
				//查找"["
				p2 = str.find(kCp, p1);
				if (p2 == str.npos)
				{
					txt = str.substr(p1);
					_InsertTextToEdit(edit, txt);
					break;
				}
				else
				{
					if (p2 > p1)
					{
						txt = str.substr(p1, p2 - p1);
						_InsertTextToEdit(edit, txt);
					}
					//查找"]"
					q = str.find(kCq, p2);
					if (q == str.npos)
					{
						txt = str.substr(p2);
						_InsertTextToEdit(edit, txt);
						break;
					}
					else
					{
						emo = str.substr(p2, q - p2 + 1);
						_FindEmoji(edit, emo);

						p1 = q + 1;
					}
				}
			}
		}

	}
}