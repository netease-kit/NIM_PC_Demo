#pragma once
#include "module/session/session_util.h"
#include "module/login/login_manager.h"
#include "module/service/user_service.h"
#include "shared/ui/msgbox.h"

namespace nim_comp
{
//会话窗口提示类型
//消息顶部浮层：网络连接提示 
#define SESSION_TIP_LINK				L"SESSION_TIP_LINK"

//消息顶部浮层：不在群中提示 
#define SESSION_TIP_LEAVE				L"SESSION_TIP_LEAVE"

//消息顶部浮层：不是好友提示 
#define SESSION_TIP_NOT_FRIEND			L"SESSION_TIP_NOT_FRIEND"

//消息内容顶部嵌入：加载更多消息历史
#define CELL_LOAD_MORE_MSG				L"CELL_LOAD_MORE_MSG"
#define CELL_BTN_LOAD_MORE_MSG			L"CELL_BTN_LOAD_MORE_MSG"

//消息内容底部嵌入：正在输入状态
#define CELL_WRITING					L"CELL_WRITING"

enum SessionTipType
{
	STT_LINK,
	STT_LEAVE,
	STT_NOT_FRIEND,
	STT_LOAD_MORE_MSG,
	STT_WRITING,
	STT_DISMISS,
};

//会话类型
enum SessionType
{
	kSessionDouble = 0,
	kSessionTeam
};

//消息加载状态
enum MsgResLoadStatus
{
	RS_LOADING,
	RS_RELOAD,
	RS_LOAD_OK,
	RS_LOAD_NO
};

//自定义消息的类型
enum CustomMsgType
{
	CustomMsgType_Jsb = 1,	//石头剪子布
	CustomMsgType_SnapChat,	//阅后即焚
	CustomMsgType_Sticker,	//贴图
	CustomMsgType_Rts,		//白板的发起结束消息
	CustomMsgType_Meeting = 10,		//多人会议控制协议
};

/**
* 获取会话类型
* @param[in] msg 会话消息
* @return SessionType	会话类型
*/
SessionType GetSessionType(const nim::IMMessage &msg);

/**
* 获取会话id
* @param[in] msg 会话消息
* @return string	会话id
*/
std::string GetSessionId(const nim::IMMessage &msg);

/**
* 获取气泡显示位置，左或右
* @param[in] msg 会话消息
* @return bool	true 右，false 左
*/
bool IsBubbleRight(const nim::IMMessage &msg);

/**
* 获取RichEdit控件中的字符串，会自动转换表情对象为对应的字符串
* @param[in] re RichEdit控件
* @return wstring	字符串
*/
std::wstring GetRichText(ui::RichEdit* re);

/**
* 计算消息的显示时间：
* @param[in] t 消息时间
* @param[in] 会话窗口abbreviate=false，会话列表abbreviate=true
* @return wstring	显示的时间
*/
std::wstring GetMessageTime(const long long t, bool abbreviate);

/**
* 对文字消息进行检查
* @param[in] content 需要检查的消息
* @return wstring	过滤后的文字
*/
std::wstring CheckMsgContent(const std::wstring &content);

/**
* 检测会话窗口中是否需要显示时间提示
* @param[in] old_timestamp 旧的时间戳
* @param[in] new_timestamp 新的时间戳
* @return bool true 需要，false 不需要
*/
bool CheckIfShowTime(const long long old_timestamp, const long long new_timestamp);

/**
* 计算文件md5
* @param[in] file 文件路径
* @return string	MD5
*/
std::string GetFileMD5(const std::wstring &file);

/**
* 生成用于上传的图片
* @param[in] src 原文件路径
* @param[in] dest 目标输出文件路径
* @return void	无返回值
*/
void GenerateUploadImage(const std::wstring &src, const std::wstring &dest);

/**
* 插入表情到RichEdit
* @param[in] edit RichEdit控件指针
* @param[in] file_name 表情文件名
* @param[in] tag 表情名称
* @return void 无返回值
*/
void InsertFaceToEdit(ui::RichEdit* edit, const std::wstring &file_name, const std::wstring &tag);

/**
* 将一段文字（其中可能包含表情）插入到RichEdit中
* @param[in] edit RichEdit控件指针
* @param[in] str 文字
* @return void 无返回值
*/
void InsertTextToEdit(ui::RichEdit* edit, const std::wstring &str);

/**
* 插入图片到RichEdit
* @param[in] edit RichEdit控件指针
* @param[in] image_src 图片路径
* @param[in] loading 是否显示正在加载中的图片外观
* @return void 无返回值
*/
void InsertImageToEdit(ui::RichEdit* edit, const std::wstring& image_src, bool loading);

/**
* 插入文件到RichEdit
* @param[in] edit RichEdit控件指针
* @param[in] file_path 文件路径
* @return void 无返回值
*/
void InsertFileToEdit(ui::RichEdit* edit, const std::wstring& file_path);

/**
* 显示地理位置
* @param[in] title 标题
* @param[in] content 内容
* @param[in] point 包含坐标信息的字符串
* @return void	无返回值
*/
void OpenMap(const std::string& title, const std::string& content,  const std::string& point);

/**
* 判断消息中对应的资源文件是否存在（图片、语音）
* @param[in] msg 会话消息
* @return void	true 存在，false 不存在
*/
bool IsResourceExist(const nim::IMMessage &msg);

/**
* 自定义消息转换为会话消息
* @param[in] sys_msg 自定义消息
* @param[out] msg 会话消息
* @return void	无返回值
*/
void CustomSysMessageToIMMessage(const nim::SysMessage &sys_msg, nim::IMMessage &msg);

/**
* Json字符串解析为Json对象
* @param[in] str 字符串
* @param[out] json Json对象
* @return bool true 成功，false 失败
*/
bool StringToJson(const std::string &str, Json::Value &json);

/**
* 会话消息是否为通知类型的消息
* @param[in] msg 会话消
* @return bool true 包含，false 不包含
*/
bool IsNoticeMsg(const nim::IMMessage& msg);

/**
* 获取通知消息显示的文本
* @param[in] msg_attach 会话消息附加字段
* @param[in] from_account 发送者用户id
* @param[in] to_account 接收者用户id
* @param[out] show_text 显示的文本
* @param[in] session_id 会话id
* @return void	无返回值
*/
void GetNotifyMsg(const std::string& msg_attach, const std::string& from_account, const std::string& to_account, std::wstring &show_text, const std::string& session_id);

/**
* 判断是否是语音通话的话单消息
* @param[in] msg_type 消息类型
* @param[in] msg_attach 会话消息附加字段
* @return bool true 成功，false 失败
*/
bool IsNetCallMsg(nim::NIMMessageType msg_type, const std::string& msg_attach);

/**
* 获取自定义消息的提示内容
* @param[in] sender_accid 发送者帐号
* @param[in] msg_attach 会话消息附加字段
* @return wstring 提示内容
*/
std::wstring GetCustomMsg(const std::string &sender_accid, const std::string &msg_attach);

/**
* 获取回撤消息的提示文本
* @param[in] session_id 会话id
* @param[in] session_type 会话类型
* @param[in] msg_from_id 消息发送者的id
* @param[in] msg_from_nick 消息发送者的昵称
* @return std::wstring 提示文本
*/
std::wstring GetRecallNotifyText(const std::string& session_id, nim::NIMSessionType session_type, const std::string& msg_from_id, const std::string& msg_from_nick);

}