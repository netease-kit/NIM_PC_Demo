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

	}
}