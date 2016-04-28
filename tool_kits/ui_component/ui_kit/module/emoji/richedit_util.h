#ifndef RICHEDIT_UTIL_H
#define RICHEDIT_UTIL_H
#include <atlbase.h>
#include "shared/image_ole_i.h"
#include <richedit.h>
#include <richole.h>
#include <textserv.h>

namespace nim_comp
{
#define EMOJI_PREFIX L"_nim_emoticon_emoji_"

#define MAX_CUSTOM_ITEM_NUM	9

enum
{
	RE_OLE_TYPE_TEXT = 0,
	RE_OLE_TYPE_FACE,
	RE_OLE_TYPE_CUSTOM,
	RE_OLE_TYPE_IMAGE = RE_OLE_TYPE_CUSTOM,
	RE_OLE_TYPE_IMAGELOADING,//正在加载的图片
	RE_OLE_TYPE_FILE,
	RE_OLE_TYPE_ERROR,
};
struct RE_OLE_ITEM_CONTENT 
{
	int type_;
	std::wstring guid_;
	std::wstring content_;
	LONG scale_;
	RE_OLE_ITEM_CONTENT()
	{
		type_ = RE_OLE_TYPE_ERROR;
		scale_ = 0;
	}
};
//静态调用imageoledll，不需要注册，在最后再free
void FreeImageoleModule();
bool CreateImageObject(void **ppv);

typedef std::function<void()> InsertCustomItemErrorCallback;

IRichEditOle* Re_GetOleInterface(ITextServices *text_service);
bool Re_InsertCustomItem(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file, const std::wstring& face_tag, int face_index
					, int face_id, bool scale, int scale_width = 0, int scale_height = 0, LONG cp=REO_CP_SELECTION);

bool Re_InsertFace(ITextServices *text_service, const std::wstring& file, const std::wstring& face_tag);
bool Re_InsertJsb(ITextServices *text_service, const std::wstring& file, const std::wstring& face_tag);
bool Re_InsertImage(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file, bool loading = false, LONG cp=REO_CP_SELECTION);
bool Re_InsertFile(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file);

// 获取文本
void Re_GetText(ITextServices * text_service, std::wstring& text);
void Re_GetTextEx(ITextServices * text_service, std::vector<RE_OLE_ITEM_CONTENT>& content_list);
bool Re_GetNimTextInfo(ITextServices * text_service, CHARRANGE *lpchrg, std::wstring& text, std::wstring& nim_text);
void Re_InsertNimTextInfo(ITextServices * text_service, std::string& nim_text, bool custom, std::function<void()> callback);
int Re_GetWindowTextLength(ITextServices * text_service);
int Re_GetTextRange(ITextServices * text_service, CHARRANGE *lpchrg, std::wstring& text);
int Re_GetWindowText(ITextServices * text_service, wchar_t *buff, int max_count);
//文本剩余数， limit-cur+sel
int Re_GetTextLimitSurplus(ITextServices * text_service);

// 替换选中文本
void Re_ReplaceSel(ITextServices * text_service, const std::wstring& new_text, const std::wstring& font_name, int font_size
	,COLORREF clr_text, bool bold, bool italic, bool underline, bool link, int start_indent, bool undo = false);
void Re_ReplaceSel(ITextServices * text_service, const std::wstring& new_text, BOOL undo = false);

void Re_GetSel(ITextServices * text_service, LONG& start_char, LONG& end_char);
int Re_SetSel(ITextServices * text_service, LONG start_char, LONG end_char);

DWORD Re_GetDefaultCharFormat(ITextServices * text_service, CHARFORMAT& cf);
BOOL Re_SetDefaultCharFormat(ITextServices * text_service, CHARFORMAT& cf);
//设置默认字体
void Re_SetDefFont(ITextServices * text_service, const std::wstring& font_name, int font_size, COLORREF clr_text, bool bold, 
	bool italic, bool underline, bool link);

DWORD Re_GetSelectionCharFormat(ITextServices * text_service, CHARFORMAT& cf);
BOOL Re_SetSelectionCharFormat(ITextServices * text_service, CHARFORMAT& cf);
// 设置字体
void Re_SetFont(ITextServices * text_service, const std::wstring& font_name, int font_size, COLORREF clr_text, bool bold, bool italic
	, bool underline, bool link);

// 设置左缩进(单位:缇)
BOOL Re_SetStartIndent(ITextServices * text_service, int size);

bool Re_AllImageOleChangeFrame(ITextServices * text_service);
int Re_GetCustomImageOleCount(ITextServices * text_service);
RE_OLE_ITEM_CONTENT Re_CustomImageOleHitTest(ITextServices * text_service, POINT pt);
int Re_ImageLoadingFinish(ITextServices * text_service, const std::wstring& file_path, bool succeed);
int Re_RemoveCustomItem(ITextServices * text_service);
std::wstring GetIconByFile(std::wstring file);
bool Re_FindFile(ITextServices * text_service, const std::wstring& file, bool check_sel = false);
}
#endif //RICHEDIT_UTIL_H