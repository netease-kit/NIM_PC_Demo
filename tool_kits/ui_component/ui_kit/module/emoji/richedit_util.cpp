#include "richedit_util.h"
#include "emoji_info.h"

namespace nim_comp
{
#define EMOJI_SIZE	24

#define MAX_CUSTOM_ITEM_W	150
#define MAX_CUSTOM_ITEM_H	150
#define MAX_CUSTOM_FILE_W	200

#ifdef DEBUG
#define IMAGE_OLE_NAME	L"image_ole_d.dll"
#else
#define IMAGE_OLE_NAME	L"image_ole.dll"
#endif

static HMODULE image_ole_module_ = NULL;
void FreeImageoleModule()
{
	if (image_ole_module_)
	{
		FreeLibrary(image_ole_module_);
		image_ole_module_ = NULL;
	}
}

typedef HRESULT(WINAPI* DLLGETCLASSOBJECTFUNC) (REFCLSID rclsid, REFIID riid, LPVOID* ppv);
bool CreateImageObject(void **ppv)
{
	*ppv = NULL;
	if (image_ole_module_ == NULL)
	{
		std::wstring dll_path = QPath::GetAppPath();
		dll_path.append(IMAGE_OLE_NAME);
		image_ole_module_ = LoadLibrary(dll_path.c_str());
	}
	if (image_ole_module_ == NULL)
	{
		return false;
	}

	DLLGETCLASSOBJECTFUNC pFunc = (DLLGETCLASSOBJECTFUNC)GetProcAddress(image_ole_module_, "DllGetClassObject");
	if (pFunc == NULL)
	{
		FreeImageoleModule();
		return false;
	}
	CComPtr<IClassFactory> pFactory;
	HRESULT hr = (*pFunc)(CLSID_ImageOle/*你的组件CLSID*/, IID_IClassFactory, (void**)&pFactory);
	if (hr == S_OK)
		hr = pFactory->CreateInstance(0, IID_IImageOle, ppv);

	if (hr != S_OK || *ppv == NULL)
	{
		FreeImageoleModule();
		return false;
	}
	return true;
}

IRichEditOle* Re_GetOleInterface(ITextServices *text_service)
{
	IRichEditOle *pRichEditOle = NULL;
	text_service->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle, NULL);
	return pRichEditOle;
}

bool Re_InsertCustomItem(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file, const std::wstring& face_tag, int ole_type
					, int face_id, bool scale, int scale_width, int scale_height, LONG cp)
{
	if (ole_type >= RE_OLE_TYPE_CUSTOM && Re_GetCustomImageOleCount(text_service) >= MAX_CUSTOM_ITEM_NUM)
	{
		if( callback )
		{
			Post2UI(callback);
		}
		return false;
	}
	if (text_service == NULL)
		return false;

	//m_RichEdit为您的RichEdit对象
	IRichEditOle* lpRichEditOle = Re_GetOleInterface(text_service);

	if (lpRichEditOle == NULL)
		return false;

	//OLE对象
	IOleObject*  lpOleObject = NULL;
	IOleClientSite* lpOleClientSite = NULL; 
	CComPtr<IImageOle> image_ole;
	CLSID   clsid;
	REOBJECT  reobject;
	HRESULT   hr = S_FALSE;
	do 
	{
		{
			if (!CreateImageObject((LPVOID*)&image_ole))
				break;
		}

		//获得数据对象接口
		hr = image_ole->QueryInterface(IID_IOleObject, (void**)&lpOleObject);
		if (hr != S_OK)
			break;

		hr = lpOleObject->GetUserClassID(&clsid);
		if (hr != S_OK)
			break;

		hr = lpRichEditOle->GetClientSite(&lpOleClientSite);
		if (hr != S_OK)
			break;

		//初始化一个对象
		ZeroMemory(&reobject, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.clsid = clsid;
		reobject.cp = cp;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.dwFlags = REO_BELOWBASELINE;
		reobject.poleobj = lpOleObject;
		reobject.polesite = lpOleClientSite;
		reobject.pstg = NULL;// lpStorage;

		lpOleObject->SetClientSite(lpOleClientSite);
		std::wstring image_path = file;
		image_ole->SetFaceTag((BSTR)(face_tag.c_str()));
		image_ole->SetFaceIndex(ole_type);
		image_ole->SetFaceId(face_id);
		if (ole_type == RE_OLE_TYPE_FILE)
		{
			OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
			::GetVersionEx(&os);
			std::wstring fontName = os.dwMajorVersion >= 6 ? L"微软雅黑" : L"新宋体";
			image_ole->SetFont((BSTR)(fontName.c_str()), 20, RGB(0x1a, 0x30, 0x47));
			if (image_path.size() == 0)
			{
				image_path = GetIconByFile(face_tag);
			}
			image_ole->SetBgColor(RGB(255, 255, 255));
		}
		else if (ole_type == RE_OLE_TYPE_IMAGELOADING)
		{
			if (image_path.size() == 0)
			{
				image_path = QPath::GetAppPath();
				image_path.append(L"res\\icons\\loading.gif");
			}
		}
		image_ole->SetScaleSize(scale, scale_width, scale_height);
		image_ole->LoadFromFile((BSTR)(image_path.c_str()));
		std::wstring guid = nbase::UTF8ToUTF16(QString::GetGUID());
		image_ole->SetGUID((BSTR)guid.c_str());
		OleSetContainedObject(lpOleObject, TRUE);

		hr = lpRichEditOle->InsertObject(&reobject);
	} while (0);

	if( lpOleObject  != NULL )
		lpOleObject->Release();
	if( lpOleClientSite != NULL )
		lpOleClientSite->Release();

	lpRichEditOle->Release();

	return hr == S_OK;
}
bool Re_InsertFace(ITextServices *text_service, const std::wstring& file, const std::wstring& face_tag)
{
	int emoji_size = EMOJI_SIZE;
	ui::DpiManager::GetInstance()->ScaleInt(emoji_size);
	return  Re_InsertCustomItem(text_service, InsertCustomItemErrorCallback(), file, face_tag, RE_OLE_TYPE_FACE, 0, true, emoji_size, 0);
}
bool Re_InsertJsb(ITextServices *text_service, const std::wstring& file, const std::wstring& face_tag)
{
	return  Re_InsertCustomItem(text_service, InsertCustomItemErrorCallback(), file, face_tag, RE_OLE_TYPE_FACE, 0, false, MAX_CUSTOM_ITEM_W, MAX_CUSTOM_ITEM_H);
}
bool Re_InsertImage(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file, bool loading, LONG cp)
{
	return  Re_InsertCustomItem(text_service, callback, loading?L"":file, file, loading?RE_OLE_TYPE_IMAGELOADING:RE_OLE_TYPE_IMAGE, 0, false, MAX_CUSTOM_ITEM_W, MAX_CUSTOM_ITEM_H, cp);
}
bool Re_InsertFile(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file)
{
	if (Re_FindFile(text_service, file))
	{
		return false;
	}
	return  Re_InsertCustomItem(text_service, callback, L"", file, RE_OLE_TYPE_FILE, 0, false, MAX_CUSTOM_FILE_W, MAX_CUSTOM_ITEM_H);
}

int Re_GetTextLength(ITextServices * text_service)
{
	LRESULT lRes = 0;
	text_service->TxSendMessage(WM_GETTEXTLENGTH, 0, 0, &lRes);
	return (int)lRes;
}

//文本剩余数， limit-cur+sel
int Re_GetTextLimitSurplus(ITextServices * text_service)
{
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_GETLIMITTEXT, 0, 0, &lRes);
	long text_limit = (int)lRes;
	int text_cur = Re_GetTextLength(text_service);
	std::wstring edit_text;
	CHARRANGE chrg = { 0, text_cur };
	Re_GetTextRange(text_service, &chrg, edit_text);
	text_cur = edit_text.length();
	long start_char = 0, end_char = 0;
	Re_GetSel(text_service, start_char, end_char);
	if (end_char > start_char)
	{
		int text_sel = 0;
		std::wstring text_sel_temp;
		CHARRANGE cr = { start_char, end_char };
		text_service->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
		Re_GetTextRange(text_service, &cr, text_sel_temp);
		text_sel = text_sel_temp.length();
		text_cur -= text_sel;
	}
	return text_limit - text_cur;
}

int Re_GetTextRange(ITextServices * text_service, CHARRANGE *lpchrg, std::wstring& text)
{
	text = L"";

	if (NULL == lpchrg || lpchrg->cpMax <= lpchrg->cpMin)
		return 0;

	LONG nLen = lpchrg->cpMax - lpchrg->cpMin;
	wchar_t * temp = new wchar_t[nLen+1];
	if (NULL == temp)
		return 0;

	memset(temp, 0, (nLen+1)*sizeof(wchar_t));

	TEXTRANGE tr = { 0 };
	tr.chrg = *lpchrg;
	tr.lpstrText = temp;

	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, &lRes);

	text = temp;
	delete []temp;

	return (int)lRes;
}

void Re_GetText(ITextServices * text_service, std::wstring& text)
{
	REOBJECT reobject;
	LONG nImageIndex;
	LONG pos = 0;
	std::wstring org_text, temp;

	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};
	Re_GetTextRange(text_service, &chrg, org_text);

	for (LONG i = 0; i < (int)org_text.size(); i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			if (reobject.cp > 0 && reobject.cp > pos)
			{
				temp = org_text.substr(pos, reobject.cp - pos);
				nbase::StringReplaceAll(L"\r", L"\r\n", temp);
				text += temp;
			}
			pos = reobject.cp + 1;

			if (NULL == reobject.poleobj)
				continue;

			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					wchar_t* face_tag;
					pImageOle->GetFaceIndex(&nImageIndex);
					pImageOle->GetFaceTag(&face_tag);
					//COMMENT(litianyi) index==1表示表情 id==2表示图片
					if (nImageIndex == RE_OLE_TYPE_FACE)
					{
						std::wstring tmp = face_tag;
						if (tmp.size() > 0)
						{
							nbase::StringReplaceAll(L"\r", L"\r\n", tmp);
							text += tmp;
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					pImageOle->Release();
				}
			}
			reobject.poleobj->Release();
		}
	}

	if (pos < (int)org_text.size())
	{
		temp = org_text.substr(pos);
		nbase::StringReplaceAll(L"\r", L"\r\n", temp);
		text += temp;
	}

	pRichEditOle->Release();
}
void Re_GetTextEx(ITextServices * text_service, std::vector<RE_OLE_ITEM_CONTENT>& content_list)
{
	content_list.clear();
	REOBJECT reobject;
	LONG nImageId, nImageIndex;
	LONG pos = 0;
	std::wstring org_text, temp;
	std::wstring text;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};
	Re_GetTextRange(text_service, &chrg, org_text);

	for (LONG i = 0; i < (int)org_text.size(); i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			if (reobject.cp > 0 && reobject.cp > pos)
			{
				temp = org_text.substr(pos, reobject.cp - pos);
				nbase::StringReplaceAll(L"\r", L"\r\n", temp);
				text += temp;
			}
			pos = reobject.cp + 1;

			if (NULL == reobject.poleobj)
				continue;

			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					wchar_t* image_info;
					wchar_t* image_src;
					wchar_t* guid;
					LONG scale = 0;
					pImageOle->GetFaceTag(&image_info);
					pImageOle->GetImageFile(&image_src);
					pImageOle->GetFaceIndex(&nImageIndex);
					pImageOle->GetFaceId(&nImageId);
					pImageOle->GetGUID(&guid);
					pImageOle->GetScale(&scale);
					//COMMENT(litianyi) index==0表示表情 id==1表示图片
					if (nImageIndex == RE_OLE_TYPE_FACE)
					{
						std::wstring tmp = image_info;
						if (tmp.size() > 0)
						{
							nbase::StringReplaceAll(L"\r", L"\r\n", tmp);
							text += tmp;
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					else
					{
						if (text.size() > 0)
						{
							RE_OLE_ITEM_CONTENT content;
							content.type_ = RE_OLE_TYPE_TEXT;
							content.content_ = text;
							content_list.push_back(content);
							text.clear();
						}

						RE_OLE_ITEM_CONTENT content2;
						content2.type_ = nImageIndex;
						content2.content_ = image_info;
						content2.image_src_ = image_src;
						content2.guid_ = guid;
						content2.scale_ = scale;
						content_list.push_back(content2);
					}
					pImageOle->Release();
				}
			}
			reobject.poleobj->Release();
		}
	}

	if (pos < (int)org_text.size())
	{
		temp = org_text.substr(pos);
		nbase::StringReplaceAll(L"\r", L"\r\n", temp);
		text += temp;
	}
	if (text.size() > 0)
	{
		RE_OLE_ITEM_CONTENT content;
		content.type_ = RE_OLE_TYPE_TEXT;
		content.content_ = text;
		content_list.push_back(content);
		text.clear();
	}

	pRichEditOle->Release();
}
bool Re_GetNimTextInfo(ITextServices * text_service, CHARRANGE *lpchrg, std::wstring& text, std::wstring& nim_text)
{
	text.clear();
	nim_text.clear();
	bool bCustomObject = false;
	std::string text_info;
	if (text_service)
	{
		std::wstring strText;
		//
		REOBJECT reobject;
		LONG nImageIndex, nPos = 0;
		std::wstring strOrgText, strTemp;

		IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
		if (NULL == pRichEditOle)
			return bCustomObject;

		Re_GetTextRange(text_service, lpchrg, strOrgText);

		TiXmlDocument doc;
		TiXmlElement * decl = new TiXmlElement("RichEditFormat");

		for (LONG i = 0; i < (int)strOrgText.size(); i++)
		{
			memset(&reobject, 0, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);
			reobject.cp = i + lpchrg->cpMin;
			HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
			if (SUCCEEDED(hr))
			{
				if (reobject.cp > 0 && reobject.cp > nPos)
				{
					strTemp = strOrgText.substr(nPos, i - nPos);
					nbase::StringReplaceAll(L"\r", L"\r\n", strTemp);
					strText += strTemp;
					TiXmlElement * item = new TiXmlElement("EditElement");
					item->SetAttribute("type", RE_OLE_TYPE_TEXT);//创建属性
					std::string tmp = nbase::UTF16ToUTF8(strTemp);
					TiXmlText* text_item = new TiXmlText(tmp.c_str());
					item->InsertEndChild(*text_item);
					decl->InsertEndChild(*item);
				}
				nPos = i + 1;

				if (NULL == reobject.poleobj)
					continue;

				if (CLSID_ImageOle == reobject.clsid)
				{
					IImageOle * pImageOle = NULL;
					hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
					if (SUCCEEDED(hr))
					{
						wchar_t* image_info;
						pImageOle->GetFaceTag(&image_info);
						pImageOle->GetFaceIndex(&nImageIndex);
						//COMMENT(litianyi) index==0表示表情 id==1表示图片
						if (nImageIndex == RE_OLE_TYPE_FACE)
						{
							std::wstring text = image_info;
							if (text.size() == 0)
							{
								ASSERT(FALSE);
							}
							nbase::StringReplaceAll(L"\r", L"\r\n", text);
							strText += text;

							TiXmlElement * item = new TiXmlElement("EditElement");
							item->SetAttribute("type", nImageIndex);//创建属性

							std::string tmp = nbase::UTF16ToUTF8(text);
							item->SetAttribute("facetag", tmp.c_str());//创建属性
							decl->InsertEndChild(*item);
							bCustomObject = true;
						}
						else if (nImageIndex == RE_OLE_TYPE_IMAGE)
						{
							std::wstring text = image_info;
							std::string tmp = nbase::UTF16ToUTF8(text);
							TiXmlElement * item = new TiXmlElement("EditElement");
							item->SetAttribute("type", nImageIndex);//创建属性
							item->SetAttribute("filepath", tmp.c_str());//创建属性
							decl->InsertEndChild(*item);
							bCustomObject = true;
							strText += L" ";
						}
						else if (nImageIndex == RE_OLE_TYPE_FILE)
						{
							std::wstring text = image_info;
							std::string tmp = nbase::UTF16ToUTF8(text);
							TiXmlElement * item = new TiXmlElement("EditElement");
							item->SetAttribute("type", nImageIndex);//创建属性
							item->SetAttribute("filepath", tmp.c_str());//创建属性
							decl->InsertEndChild(*item);
							bCustomObject = true;
							strText += L" ";
						}
						pImageOle->Release();
					}
				}
				reobject.poleobj->Release();
			}
		}

		if (nPos < (int)strOrgText.size())
		{
			strTemp = strOrgText.substr(nPos);
			nbase::StringReplaceAll(L"\r", L"\r\n", strTemp);
			strText += strTemp;
			TiXmlElement * item = new TiXmlElement("EditElement");
			item->SetAttribute("type", RE_OLE_TYPE_TEXT);//创建属性
			std::string tmp = nbase::UTF16ToUTF8(strTemp);
			TiXmlText* text_item = new TiXmlText(tmp.c_str());
			item->InsertEndChild(*text_item);
			decl->InsertEndChild(*item);
		}

		text = strText;
		doc.LinkEndChild(decl);
		TiXmlPrinter printer;
		doc.Accept(&printer);
		std::string nim_text_temp = nbase::StringPrintf("%s", printer.CStr());
		nim_text = nbase::UTF8ToUTF16(nim_text_temp);

		pRichEditOle->Release();
	}
	return bCustomObject;
}
void Re_InsertNimTextInfo(ITextServices * text_service, std::string& nim_text, bool custom, std::function<void()> callback)
{
	if (text_service)
	{
		if (nim_text.size() > 0)
		{
			TiXmlDocument doc;
			doc.Parse(nim_text.c_str());
			//test
			TiXmlNode* parent = doc.FirstChild("RichEditFormat");
			if (parent)
			{
				for (TiXmlNode* node = parent->FirstChild(); node != NULL; node = node->NextSibling())
				{
					if (node && strcmp("EditElement", node->Value()) == 0)
					{
						int text_limit_surplus = Re_GetTextLimitSurplus(text_service);
						if (text_limit_surplus <= 0)
						{
							break;
						}
						bool limit_out = false;
						TiXmlElement* element = (TiXmlElement*)node;
						int type = 0;
						element->QueryIntAttribute("type", &type);
						if (custom || type < RE_OLE_TYPE_CUSTOM)
						{
							switch (type)
							{
							case RE_OLE_TYPE_TEXT:
							{
								TiXmlText* text = (TiXmlText*)element->FirstChild();
								if (text)
								{
									std::string text_temp = text->Value();
									std::wstring wtmp = nbase::UTF8ToUTF16(text_temp);
									int text_add = wtmp.length();
									if (text_limit_surplus < text_add)
									{
										limit_out = true;
										wtmp.assign(wtmp, 0, text_limit_surplus);
									}
									Re_ReplaceSel(text_service, wtmp, true);
								}
							}
							break;
							case RE_OLE_TYPE_FACE:
							{
								std::string tmp = element->Attribute("facetag");
								std::wstring face_tag = nbase::UTF8ToUTF16(tmp);

								std::wstring file_path;
								emoji::GetEmojiFileByTag(face_tag, file_path);

								if (file_path.size() > 0)
								{
									std::wstring emoji_path = QPath::GetAppPath();
									emoji_path.append(L"res\\emoji\\" + file_path);
									Re_InsertFace(text_service, emoji_path, face_tag);
								}
							}
							break;
							case RE_OLE_TYPE_IMAGE:
							{
								std::string tmp = element->Attribute("filepath");
								std::wstring file_path = nbase::UTF8ToUTF16(tmp);
								Re_InsertImage(text_service, callback, file_path);
							}
							break;
							case RE_OLE_TYPE_FILE:
							{
								std::string tmp = element->Attribute("filepath");
								std::wstring file_path = nbase::UTF8ToUTF16(tmp);
								Re_InsertFile(text_service, callback, file_path);
							}
							break;
							}
						}
						if (limit_out)
						{
							break;
						}
					}
				}
			}
		}
	}
}

void Re_ReplaceSel(ITextServices * text_service, const std::wstring& new_text, bool undo /*= false*/)
{
	BOOL bUndo = undo ? TRUE : FALSE;
	text_service->TxSendMessage(EM_REPLACESEL, (WPARAM)bUndo, (LPARAM)new_text.c_str(), NULL);
}

void Re_ReplaceSel(ITextServices * text_service, const std::wstring& new_text, int start_indent, bool undo/* = false*/)
{
	long start_char = 0, end_char = 0;
	Re_GetSel(text_service, start_char, end_char);
	Re_ReplaceSel(text_service, new_text, undo);
	end_char = start_char + new_text.length();
	Re_SetSel(text_service, start_indent, end_char);
	Re_SetStartIndent(text_service, start_indent);
	Re_SetSel(text_service, end_char, end_char);
}

void Re_GetSel(ITextServices * text_service, LONG& start_char, LONG& end_char)
{
	CHARRANGE cr = { 0, 0 };
	text_service->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, NULL);
	start_char = cr.cpMin;
	end_char = cr.cpMax;
}

int Re_SetSel(ITextServices * text_service, LONG start_char, LONG end_char)
{
	CHARRANGE cr = { start_char, end_char };
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lRes);
	return (int)lRes;
}

DWORD Re_GetDefaultCharFormat(ITextServices * text_service, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
	return (DWORD)lRes;
}

BOOL Re_SetDefaultCharFormat(ITextServices * text_service, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRes);
	return (BOOL)lRes;
}

void Re_SetDefFont(ITextServices * text_service, const std::wstring& font_name, int font_size, COLORREF clr_text, bool bold, 
	bool italic, bool underline, bool link)
{
	CHARFORMAT cf;
	memset(&cf, 0, sizeof(cf));
	Re_GetDefaultCharFormat(text_service, cf);

	// 设置字体名称
	if (!font_name.empty())
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		wcscpy_s(cf.szFaceName, font_name.c_str());
	}

	// 设置字体高度
	if (font_size > 0)
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = font_size * 20;
	}

	cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_LINK;

	// 设置字体颜色
	cf.crTextColor = clr_text;
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	// 设置粗体
	if (bold)
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;

	// 设置倾斜
	if (italic)
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	// 设置下划线
	if (underline)
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	// 设置超链接
	if (link)
		cf.dwEffects |= CFE_LINK;
	else
		cf.dwEffects &= ~CFE_LINK;

	Re_SetDefaultCharFormat(text_service, cf);
}

DWORD Re_GetSelectionCharFormat(ITextServices * text_service, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
	return (DWORD)lRes;
}

BOOL Re_SetSelectionCharFormat(ITextServices * text_service, CHARFORMAT& cf)
{
	cf.cbSize = sizeof(CHARFORMAT);
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
	return (BOOL)lRes;
}

void Re_SetFont(ITextServices * text_service, const std::wstring& font_name, int font_size, COLORREF clr_text, bool bold, bool italic
	, bool underline, bool link)
{
	CHARFORMAT cf;
	memset(&cf, 0, sizeof(cf));
	Re_GetSelectionCharFormat(text_service, cf);

	// 设置字体名称
	if (!font_name.empty())
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		wcscpy_s(cf.szFaceName, font_name.c_str());
	}

	// 设置字体高度
	if (font_size > 0)
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = font_size * 20;
	}

	cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_LINK;

	// 设置字体颜色
	cf.crTextColor = clr_text;
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	// 设置粗体
	if (bold)
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;

	// 设置倾斜
	if (italic)
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	// 设置下划线
	if (underline)
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	// 设置超链接
	if (link)
		cf.dwEffects |= CFE_LINK;
	else
		cf.dwEffects &= ~CFE_LINK;

	Re_SetSelectionCharFormat(text_service, cf);
}

BOOL Re_SetStartIndent(ITextServices * text_service, int size)
{
	PARAFORMAT2 pf2;
	memset(&pf2, 0, sizeof(pf2));
	pf2.cbSize = sizeof(PARAFORMAT2);
	pf2.dwMask = PFM_STARTINDENT;
	pf2.dxStartIndent = size;
	LRESULT lRes = 0;
	text_service->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf2, &lRes);
	return (BOOL)lRes;
}
bool Re_AllImageOleChangeFrame(ITextServices * text_service)
{
	bool ret = false;
	REOBJECT reobject;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return ret;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};

	for (LONG i = 0; i < chrg.cpMax; i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			i = reobject.cp;

			if (NULL == reobject.poleobj)
				continue;

			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					hr = pImageOle->Draw();
					if (hr == S_OK)
					{
						ret = true;
					}
					pImageOle->Release();
				}
			}
			reobject.poleobj->Release();
		}
	}

	pRichEditOle->Release();
	return ret;
}
int Re_GetCustomImageOleCount(ITextServices * text_service)
{
	int ret = 0;
	REOBJECT reobject;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return ret;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};

	for (LONG i = 0; i < chrg.cpMax; i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			i = reobject.cp;

			if (NULL == reobject.poleobj)
				continue;

			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					long nImageIndex;
					pImageOle->GetFaceIndex(&nImageIndex);
					if (nImageIndex >= RE_OLE_TYPE_CUSTOM)
					{
						++ret;
					}
					pImageOle->Release();
				}
			}
			reobject.poleobj->Release();
		}
	}
	pRichEditOle->Release();

	return ret;
}
RE_OLE_ITEM_CONTENT Re_CustomImageOleHitTest(ITextServices * text_service, POINT pt)
{
	RE_OLE_ITEM_CONTENT ret;
	ret.type_ = RE_OLE_TYPE_ERROR;
	REOBJECT reobject;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return ret;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};

	for (LONG i = 0; i < chrg.cpMax && ret.type_ == RE_OLE_TYPE_ERROR; i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			i = reobject.cp;

			if (NULL == reobject.poleobj)
				continue;

			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					long nImageIndex;
					pImageOle->GetFaceIndex(&nImageIndex);
					RECT rc;
					pImageOle->GetImageRect(&rc);
					if (nImageIndex >= RE_OLE_TYPE_CUSTOM && PtInRect(&rc, pt))
					{
						wchar_t* image_info;
						wchar_t* image_src;
						wchar_t* guid;
						pImageOle->GetFaceTag(&image_info);
						pImageOle->GetImageFile(&image_src);
						pImageOle->GetGUID(&guid);
						pImageOle->GetScale(&ret.scale_);
						ret.type_ = nImageIndex;
						ret.content_ = image_info;
						ret.image_src_ = image_src;
						ret.guid_ = guid;
					}
					pImageOle->Release();
				}
			}
			reobject.poleobj->Release();
		}
	}
	pRichEditOle->Release();

	return ret;
}
int Re_ImageLoadingFinish(ITextServices * text_service, const std::wstring& file_path, bool succeed)
{
	int ret = 0;
	REOBJECT reobject;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return ret;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};

	for (LONG i = 0; i < chrg.cpMax; i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			i = reobject.cp;

			if (NULL == reobject.poleobj)
				continue;
			bool find = false;
			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					long nImageIndex;
					pImageOle->GetFaceIndex(&nImageIndex);
					if (nImageIndex == RE_OLE_TYPE_IMAGELOADING)
					{
						wchar_t* image_info;
						pImageOle->GetFaceTag(&image_info);
						std::wstring image_tag = image_info;
						if (image_tag == file_path)
						{
							find = true;
							++ret;
						}
					}
					if (pImageOle)
					{
						pImageOle->Release();
					}
				}
			}
			reobject.poleobj->Release();
			if (find)
			{
				long start_char = 0, end_char = 0;
				Re_GetSel(text_service,start_char, end_char);
				Re_SetSel(text_service, i, i+1);
				Re_ReplaceSel(text_service, L"");
				if (succeed)
				{
					Re_InsertImage(text_service, InsertCustomItemErrorCallback(), file_path, false, i);
				} 
				else
				{
					if (i < start_char)
						start_char--;
					if (i < end_char)
						end_char--;
				}
				Re_SetSel(text_service, start_char, end_char);
			}
		}
	}
	pRichEditOle->Release();

	return ret;
}
void Re_RemoveCustomItem(ITextServices * text_service)
{
	REOBJECT reobject;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return;

	CHARRANGE chrg = { 0, Re_GetTextLength(text_service) };

	for (LONG i = 0; i < chrg.cpMax; i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			i = reobject.cp;

			if (NULL == reobject.poleobj)
				continue;
			bool find = false;
			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					long nImageIndex;
					pImageOle->GetFaceIndex(&nImageIndex);
					find = nImageIndex >= RE_OLE_TYPE_CUSTOM;
					if (pImageOle)
					{
						pImageOle->Release();
					}
				}
			}
			reobject.poleobj->Release();
			if (find)
			{
				Re_SetSel(text_service, i, i + 1);
				Re_ReplaceSel(text_service, L"");
			}
		}
	}
	pRichEditOle->Release();
	Re_SetSel(text_service, -1, -1);
}
std::wstring GetIconByFile(std::wstring file)
{
	std::wstring app_w_path = QPath::GetAppPath();
	std::wstring image_path = app_w_path + L"res\\icons\\";

	std::wstring file_exten;
	std::wstring ext;
	nbase::FilePathExtension(file, ext);
	if (ext.size() > 1)
	{
		file_exten = ext.substr(1);
	}
	if (file_exten == L"doc" || file_exten == L"docx")
	{
		image_path += L"file_doc.png";
	}
	else if (file_exten == L"ppt" || file_exten == L"pptx")
	{
		image_path += L"file_ppt.png";
	}
	else if (file_exten == L"xls" || file_exten == L"xlsx")
	{
		image_path += L"file_excel.png";
	}
	else if (file_exten == L"mp3")
	{
		image_path += L"file_mp3.png";
	}
	else if (file_exten == L"htm" || file_exten == L"html")
	{
		image_path += L"file_html.png";
	}
	else if (file_exten == L"txt" || file_exten == L"text")
	{
		image_path += L"file_txt.png";
	}
	else if (file_exten == L"pdf")
	{
		image_path += L"file_pdf.png";
	}
	else if (file_exten == L"zip")
	{
		image_path += L"file_zip.png";
	}
	else if (file_exten == L"rar")
	{
		image_path += L"file_rar.png";
	}
	else if (file_exten == L"zip")
	{
		image_path += L"file_zip.png";
	}
	else
	{
		image_path += L"file_default.png";
	}
	return image_path;
}

bool Re_FindFile(ITextServices * text_service, const std::wstring& file, bool check_sel)
{
	bool ret = false;
	REOBJECT reobject;
	IRichEditOle * pRichEditOle = Re_GetOleInterface(text_service);
	if (NULL == pRichEditOle)
		return ret;

	CHARRANGE chrg = {0, Re_GetTextLength(text_service)};
	long start_char = 0, end_char = 0;
	Re_GetSel(text_service, start_char, end_char);

	for (LONG i = 0; i < chrg.cpMax && !ret; i++)
	{
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.cp = i;
		HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
		if (SUCCEEDED(hr))
		{
			i = reobject.cp;

			if (!check_sel && i>=start_char && i<end_char)
			{
				continue;
			}

			if (NULL == reobject.poleobj)
				continue;
			if (CLSID_ImageOle == reobject.clsid)
			{
				IImageOle * pImageOle = NULL;
				hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
				if (SUCCEEDED(hr))
				{
					long nImageIndex;
					pImageOle->GetFaceIndex(&nImageIndex);
					if (nImageIndex == RE_OLE_TYPE_FILE)
					{
						wchar_t* image_info;
						pImageOle->GetFaceTag(&image_info);
						std::wstring image_tag = image_info;
						if (image_tag == file)
						{
							ret = true;
						}
					}
					if (pImageOle)
					{
						pImageOle->Release();
					}
				}
			}
			reobject.poleobj->Release();
		}
	}
	pRichEditOle->Release();

	return ret;
}
}