#pragma once
#include "module/msglog/msglog.h"

#include "module/login/login_manager.h"
#include "module/service/user_service.h"
#include "shared/ui/msgbox.h"


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
	STT_WRITING
};

//会话类型
enum SessionType
{
	kSessionDouble = 0,
	kSessionTeam
};

namespace nim_comp
{
//获取会话类型
SessionType GetSessionType(const MsgData &msg);
//获取会话id
std::string GetSessionId(const MsgData &msg);
//气泡显示位置，左 或 右
bool IsBubbleRight(const MsgData &msg);

//附加可获取表情对应的字符串
std::wstring GetRichText(ui::RichEdit* re);

//计算消息时间：会话窗口abbreviate=false；会话列表abbreviate=true
std::wstring GetMessageTime(const long long t, bool abbreviate);

bool CheckIfShowTime(const long long old_timestamp, const long long new_timestamp);

//计算文件md5
std::string GetFileMD5(const std::wstring &file);

//生成上传图片
void GenerateUploadImage(const std::wstring &src, const std::wstring &dest);

//打开地理位置
void OpenMap(const std::string& title, const std::string& content,  const std::string& point);
}