#pragma once
#include "module/session/session_util.h"

enum BubbleEventType
{
	BET_RESEND,		//重新发送 
	BET_RELOAD,		//重新下载
	BET_DELETE,		//删除
	BET_TRANSFORM,	//转文字
	BET_SHOWPROFILE,//显示资料
	BET_RETWEET,	//转发
	BET_RECALL,		//撤回
	BET_MENUATTA,	//右键菜单at
	BET_UNREAD_COUNT,	//未读列表
};

namespace nim_comp
{
static const std::wstring kBubbleBox = L"BubbleBox";

/** @class MsgBubbleItem
  * @brief 会话窗体中聊天框内的消息项(本类作为各种实际消息项的基类)
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleItem : public ui::ListContainerElement
{
	friend class SessionBox;
public:
	MsgBubbleItem();
	virtual ~MsgBubbleItem();
	typedef std::function<const std::map<std::string,nim::TeamMemberProperty>&()> TeamMemberGetter;
	/**
	* 初始化控件内部指针
	* @param[in] bubble_right 是否显示到右侧
	* @return void 无返回值
	*/
	virtual void InitControl(bool bubble_right);

	/**
	* 初始化控件外观
	* @param[in] msg 消息信息结构体
	* @return void 无返回值
	*/
	virtual void InitInfo(const nim::IMMessage &msg);

	/** 
	* 设置此消息项所属的会话窗体的会话id
	* @param[in] sid 会话id 		
	* @return void 无返回值
	*/
	void SetSessionId(const std::string &sid);

	/**
	* 设置此消息项所属的会话窗体的会话类型
	* @param[in] type 会话类型
	* @return void 无返回值
	*/
	void SetSessionType(const nim::NIMSessionType &type);

	/** 
	* 设置此消息项是否响应右击操作
	* @param[in] action 是否响应右击 
	* @return void 无返回值
	*/
	void SetActionMenu(bool action);

	/** 
	* 获取此消息项所包含的消息内容		
	* @return MsgData 消息信息结构体
	*/
	nim::IMMessage GetMsg();

	/**
	* 获取消息时间戳
	* @return string 用户id
	*/
	int64_t GetMsgTimeTag();

	/**
	* 获取消息发送者id
	* @return string 用户id
	*/
	virtual std::string GetSenderId();

	/**
	* 获取消息类型
	* @return nim::NIMMessageType 消息类型
	*/
	nim::NIMMessageType GetMsgType();

	/**
	* 更新消息时间戳
	* @param[in] timetag 时间戳
	* @return void	无返回值
	*/
	void UpdateMsgTime(__int64 timetag) { msg_.timetag_ = timetag; }

	/**
	* 设置此消息项是否显示接收消息的时间
	* @param[in] show 是否显示时间
	* @return void 无返回值
	*/
	void SetShowTime(bool show);

	/**
	* 显示消息发送者的头像
	* @return void 无返回值
	*/
	virtual void SetShowHeader();

	/**
	* 设置是否显示消息发送者的名字（或昵称）
	* @param[in] show 是否显示名字
	* @param[in] from_nick 指定要显示的名字
	* @return void 无返回值
	*/
	virtual void SetShowName(bool show, const std::string& from_nick);

	/**
	* 设置此消息项的发送状态图标（发送中、发送失败等）
	* @param[in] status 消息状态
	* @return void 无返回值
	*/
	virtual void SetMsgStatus(nim::NIMMsgLogStatus status);

	/**
	* 设置此消息项的接收状态图标（接收中、接收失败等）
	* @param[in] status 消息状态
	* @return void 无返回值
	*/
	void SetLoadStatus(MsgResLoadStatus status);

	/** 
	* 暂时无用
	* @param[in] read
	* @return void 无返回值
	*/
	void SetMsgRead(bool read);

	/**
	* 针对有播放功能的消息，设置此消息的播放状态
	* @param[in] play 是否已经播放
	* @return void 无返回值
	*/
	void SetPlayed(bool play);

	/**
	* 是否需要下载消息项所需的资源
	* @return bool 返回值true: 需要， false: 不需要
	*/ 
	virtual bool NeedDownloadResource(){ return false; }

	/**
	* 此消息项接收的文件（图片、语音等），是否成功下载到本地的回调函数
	* @param[in] success	是否下载成功
	* @param[in] file_path  文件下载路径
	* @return void 无返回值
	*/
	virtual void OnDownloadCallback(bool success, const std::string& file_path){}

	/**
	* 此消息项上传的文件（图片、语音等），上传结果的回调函数
	* @param[in] success	是否上传成功
	* @param[in] url		文件上传地址
	* @return void 无返回值
	*/
	virtual void OnUploadCallback(bool success, const std::string& url){}

	/**
	* 是否为我发送的消息
	* @return bool 返回值true: 是， false: 否
	*/
	bool IsMyMsg() { return my_msg_; }

public:
	void SetUnreadCount(int count);
	/**
	* 设置获取群成员接口，当是群消息时可用 
	*/
	void SetTeamMemberGetter(const TeamMemberGetter& getter) { team_member_getter_ = getter; }
protected:
	/** 
	* 弹出右键菜单
	* @param[in] copy 是否包含复制选项
	* @param[in] recall 是否包含撤销选项
	* @param[in] retweet 是否包含转发选项
	* @return void 无返回值
	*/
	void PopupMenu(bool copy, bool recall, bool retweet = true);

	/**
	* 右键菜单项被单机后的回调函数
	* @param[in] param 被单击的菜单项的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/ 
	bool OnMenu(ui::EventArgs* arg);

	/** 
	* 处理“复制”菜单项的操作		
	* @return void 无返回值
	*/
	virtual void OnMenuCopy(){}

	/**
	* 处理“删除”菜单项的操作
	* @return void 无返回值
	*/
	virtual void OnMenuDelete();

	/**
	* 处理“转换为文本”菜单项的操作
	* @return void 无返回值
	*/
	virtual void OnMenuTransform();
	/**
	* 此条消息是否展示右键菜单的撤回按钮，点对点：自己发送的消息。群：身为群主或管理员，可以撤回其他人发送的消息
	* @return bool
	*/
	virtual bool IsShowRecallButton();
private:

	/** 
	* 响应此消息项里包含的按钮的单击消息
	*@param[in] param 被单击的按钮的相关信息	
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* arg);

	/**
	* 响应此消息项里包含的按钮的右键单击消息
	*@param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnRightClick(ui::EventArgs* param); //右击了某条消息的头像

	/** 
	* 隐藏此消息项的所有发送状态或者所有接收状态图标
	* @param[in] type 1:隐藏所有发送状态图标; 2:隐藏所有接收状态图标; 0:隐藏所有状态图标	
	* @return void 无返回值
	*/
	void HideAllStatus(int type); 
protected:
	ui::Box*		bubble_box_;
	ui::Button*		status_resend_;
	ui::Button*		msg_header_button_;

private:
	ui::Label*		msg_time_;
	ui::Label*		sender_name_;

	ui::Control*	status_sending_;
	ui::Control*	status_loading_;
	ui::Button*		status_reload_;
	ui::Button*		status_read_count_ = nullptr;
	ui::Control*	status_send_failed_;
	ui::Control*	status_load_failed_;
	ui::Label*		status_receipt_;
	ui::Control*	play_status_;
protected:
	nim::IMMessage			msg_;

	std::string		sid_;  //会话窗口id
	nim::NIMSessionType		type_; //会话窗口类型
	
	bool			action_menu_;
	bool			my_msg_;
	TeamMemberGetter team_member_getter_;
};
}