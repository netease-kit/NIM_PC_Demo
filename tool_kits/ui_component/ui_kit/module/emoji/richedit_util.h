#pragma once
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
	std::wstring image_src_;
	LONG scale_;
	RE_OLE_ITEM_CONTENT()
	{
		type_ = RE_OLE_TYPE_ERROR;
		scale_ = 0;
	}
};

typedef std::function<void()> InsertCustomItemErrorCallback;

/**
* 静态调用imageoledll，不需要注册
* @return void	无返回值
*/
void FreeImageoleModule();

/**
* 创建一个ImageOle对象
* @param[out] ppv 创建的对象指针
* @return bool true 成功，false 失败
*/
bool CreateImageObject(void **ppv);

/**
* 获取RichEdit内部IRichEditOle*指针
* @param[out] text_service RichEdit控件内部ITextServices*指针
* @return IRichEditOle* IRichEditOle*指针
*/
IRichEditOle* Re_GetOleInterface(ITextServices *text_service);

/**
* 向RichEdit插入一个ImageOle对象
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] callback 插入错误的回调通知函数
* @param[in] file 图片文件路径
* @param[in] face_tag 图片对应的标签
* @param[in] ole_type 插入ole对象的类型
* @param[in] face_id 图片id
* @param[in] scale 是否保持缩放后的比例
* @param[in] scale_width 缩放后的宽度
* @param[in] scale_height 缩放后的高度
* @param[in] cp 图片在RichEdit中插入的位置
* @return bool true 成功，false 失败
*/
bool Re_InsertCustomItem(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file, const std::wstring& face_tag, int ole_type
					, int face_id, bool scale, int scale_width = 0, int scale_height = 0, LONG cp = REO_CP_SELECTION);

/**
* 向RichEdit插入一个表情
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] callback 插入错误的回调通知函数
* @param[in] file 图片文件路径
* @param[in] face_tag 图片对应的标签
* @return bool true 成功，false 失败
*/
bool Re_InsertFace(ITextServices *text_service, const std::wstring& file, const std::wstring& face_tag);

/**
* 向RichEdit插入一个石头剪刀布表情
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] callback 插入错误的回调通知函数
* @param[in] file 图片文件路径
* @param[in] face_tag 图片对应的标签
* @return bool true 成功，false 失败
*/
bool Re_InsertJsb(ITextServices *text_service, const std::wstring& file, const std::wstring& face_tag);

/**
* 向RichEdit插入一个图片
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] callback 插入错误的回调通知函数
* @param[in] file 图片文件路径
* @param[in] loading 是否插入正在加载中的图片
* @param[in] cp 图片在RichEdit中插入的位置
* @return bool true 成功，false 失败
*/
bool Re_InsertImage(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file, bool loading = false, LONG cp=REO_CP_SELECTION);

/**
* 向RichEdit插入一个文件图片
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] callback 插入错误的回调通知函数
* @param[in] file 图片文件路径
* @return bool true 成功，false 失败
*/
bool Re_InsertFile(ITextServices *text_service, InsertCustomItemErrorCallback callback, const std::wstring& file);

/**
* 获取RichEdit中的文字，会自动把控件中的ole对象转换为对应的文字标签
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[out] text 获取到的文字
* @return void 无返回值
*/
void Re_GetText(ITextServices * text_service, std::wstring& text);

/**
* 获取RichEdit中包括ole和文本在内的所有内容的详细信息
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[out] content_list 获取到的内容
* @return void 无返回值
*/
void Re_GetTextEx(ITextServices * text_service, std::vector<RE_OLE_ITEM_CONTENT>& content_list);

/**
* 获取RichEdit某段范围内的文字，以及转换为nim格式后的文字(会自动把控件中的ole对象转换为对应的文字标签)
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] lpchrg 文字范围
* @param[out] text 获取到的文字
* @param[out] nim_text 转换为nim格式的文字
* @return bool true 包含ole对象，false 不包含ole对象
*/
bool Re_GetNimTextInfo(ITextServices * text_service, CHARRANGE *lpchrg, std::wstring& text, std::wstring& nim_text);

/**
* 获取RichEdit某段范围内的文字，以及转换为nim格式后的文字(会自动把控件中的ole对象转换为对应的文字标签)
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[out] nim_text nim格式文字
* @param[in] custom 是否允许插入自定义ole对象
* @param[in] callback 插入错误的回调通知函数
* @return void 无返回值
*/
void Re_InsertNimTextInfo(ITextServices * text_service, std::string& nim_text, bool custom, std::function<void()> callback);

/**
* 获取RichEdit文字长度
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @return int 文字长度
*/
int Re_GetTextLength(ITextServices * text_service);

/**
* 获取RichEdit某段范围内的文字
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] lpchrg 文字范围
* @param[out] text 获取到的文字
* @return int 文字长度
*/
int Re_GetTextRange(ITextServices * text_service, CHARRANGE *lpchrg, std::wstring& text);

/**
* 获取RichEdit可以插入的文本剩余数(limit-cur+sel)
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @return int 文字剩余数
*/
int Re_GetTextLimitSurplus(ITextServices * text_service);

/**
* 替换RichEdit选中文本
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] new_text 替换后的文本
* @param[in] start_indent 文本缩进
* @param[in] undo 是否可以返回
* @return void 无返回值
*/
void Re_ReplaceSel(ITextServices * text_service, const std::wstring& new_text, int start_indent, bool undo = false);

/**
* 替换RichEdit选中文本
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] new_text 替换后的文本
* @param[in] undo 是否可以返回
* @return void 无返回值
*/
void Re_ReplaceSel(ITextServices * text_service, const std::wstring& new_text, bool undo = false);

/**
* 获取RichEdit选中文本的范围
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[out] start_char 选中的起始位置
* @param[out] end_char 选中的末尾位置
* @return void 无返回值
*/
void Re_GetSel(ITextServices * text_service, LONG& start_char, LONG& end_char);

/**
* 设置RichEdit选中文本的范围
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] start_char 选中的起始位置
* @param[in] end_char 选中的末尾位置
* @return int 处理结果
*/
int Re_SetSel(ITextServices * text_service, LONG start_char, LONG end_char);

/**
* 获取RichEdit默认文本样式
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] cf 文本样式
* @return DWORD 处理结果
*/
DWORD Re_GetDefaultCharFormat(ITextServices * text_service, CHARFORMAT& cf);

/**
* 设置RichEditt默认文本样式
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] cf 文本样式
* @return BOOL TRUE 设置成功，FALSE 设置失败
*/
BOOL Re_SetDefaultCharFormat(ITextServices * text_service, CHARFORMAT& cf);

/**
* 设置RichEditt默认字体
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] font_name 字体名字
* @param[in] font_size 字体大小
* @param[in] clr_text 文字颜色
* @param[in] bold 是否加粗
* @param[in] italic 是否斜体
* @param[in] underline 是否加下划线
* @param[in] link 是否为超链接样式
* @return void 无返回值
*/
void Re_SetDefFont(ITextServices * text_service, const std::wstring& font_name, int font_size, COLORREF clr_text, bool bold, 
	bool italic, bool underline, bool link);

/**
* 获取RichEdit当前选中的文本的文本样式
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] cf 文本样式
* @return DWORD 处理结果
*/
DWORD Re_GetSelectionCharFormat(ITextServices * text_service, CHARFORMAT& cf);

/**
* 设置RichEditt当前选中的文本的文本样式
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] cf 文本样式
* @return BOOL TRUE 设置成功，FALSE 设置失败
*/
BOOL Re_SetSelectionCharFormat(ITextServices * text_service, CHARFORMAT& cf);

/**
* 设置RichEditt字体
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] font_name 字体名字
* @param[in] font_size 字体大小
* @param[in] clr_text 文字颜色
* @param[in] bold 是否加粗
* @param[in] italic 是否斜体
* @param[in] underline 是否加下划线
* @param[in] link 是否为超链接样式
* @return void 无返回值
*/
void Re_SetFont(ITextServices * text_service, const std::wstring& font_name, int font_size, COLORREF clr_text, bool bold, bool italic
	, bool underline, bool link);

/**
* 设置RichEditt设置左缩进(单位:缇)
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] size 缩进值
* @return BOOL TRUE 设置成功，FALSE 设置失败
*/
BOOL Re_SetStartIndent(ITextServices * text_service, int size);

/**
* 通知RichEditt中所有ole图片对象换帧(用于gif图片动画)
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @return bool true 设置成功，false 设置失败
*/
bool Re_AllImageOleChangeFrame(ITextServices * text_service);

/**
* 获取RichEditt中ole对象总数
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @return int 总数
*/
int Re_GetCustomImageOleCount(ITextServices * text_service);

/**
* 根据坐标获取RichEditt中ole对象的信息
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] pt 鼠标坐标
* @return RE_OLE_ITEM_CONTENT ole对象信息
*/
RE_OLE_ITEM_CONTENT Re_CustomImageOleHitTest(ITextServices * text_service, POINT pt);

/**
* 通知RichEditt中ole图片对象已经加载完成
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] file_path 图片路径
* @param[in] succeed 是否加载成功
* @return int ole对象的位置
*/
int Re_ImageLoadingFinish(ITextServices * text_service, const std::wstring& file_path, bool succeed);

/**
* 清除RichEditt中所有ole对象
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @return void 无返回值
*/
void Re_RemoveCustomItem(ITextServices * text_service);

/**
* 根据文件扩展名获取对应的图标文件路径
* @param[in] file 文件名
* @return wstring 图标路径
*/
std::wstring GetIconByFile(std::wstring file);

/**
* 查找RichEditt中是否已经插入了某个文件ole对象
* @param[in] text_service RichEdit控件内部ITextServices*指针
* @param[in] file 文件名
* @param[in] check_sel 是否在选中的范围内查找
* @return bool true 存在，false 不存在
*/
bool Re_FindFile(ITextServices * text_service, const std::wstring& file, bool check_sel = false);
}