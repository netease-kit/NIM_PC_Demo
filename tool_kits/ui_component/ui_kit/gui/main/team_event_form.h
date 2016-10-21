#pragma once
#include "gui/main/control/event_item.h"
#include "module/service/photo_service.h"

namespace nim_comp
{
/** @class TeamEventForm
  * @brief 点击主界面会话列表中“消息中心”时弹出的消息中心窗口，用列表形式显示系统消息（如xxx加你为好友，xxx邀请你加入xxx群）。
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class TeamEventForm : public WindowEx
{
public:
	TeamEventForm();
	~TeamEventForm();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnClicked(ui::EventArgs* msg);

	/**
	* 重新向sdk查询所有的系统消息，用于已经
	* @return void	无返回值
	*/
	void ReloadEvents();

	/**
	* 响应系统消息事件的回调函数
	* @param[in] msg_id 消息id
	* @param[in] status 系统消息状态
	* @return void 无返回值
	*/
	void OnTeamEventCb(__int64 msg_id, nim::NIMSysMsgStatus status);

	/**
	* 响应删除全部系统消息的回调函数
	* @param[in] res_code 错误码
	* @param[in] unread 未读系统消息数
	* @return void 无返回值
	*/
	static void DeleteAllCb(nim::NIMResCode res_code, int unread);

	/**
	* 响应设置全部系统消息为已读的回调函数
	* @param[in] code 错误码
	* @param[in] unread 未读系统消息数
	* @return void 无返回值
	*/
	static void SysMsgReadAllCb(nim::NIMResCode code, int unread);

	/**
	* 响应收到一条系统消息的回调函数
	* @param[in] msg 系统消息
	* @return void 无返回值
	*/
	void OnOneTeamEvent(const nim::SysMessage &msg);

	/**
	* 响应收到一条自定义通知的回调函数
	* @param[in] msg 系统消息
	* @return void 无返回值
	*/
	void OnOneCustomMsg(const nim::SysMessage& msg);

	/**
	* 当前页面是否为自定义通知页面
	* @return bool true 是，false 否
	*/
	bool IsCustomList() { return custom_list_->IsVisible(); }
private:
	/**
	* 获取最后一条查询到的消息的时间戳
	* @return void	无返回值
	*/
	void UpdateFarstTime();

	/**
	* 向sdk查询一部分的系统消息
	* @return void	无返回值
	*/
	void InvokeLoadEvents();

	/**
	* 响应查询系统消息的回调函数
	* @param[in] count 查询到的消息数量
	* @param[in] unread 未读消息数
	* @param[in] result 查询到的系统消息列表
	* @return void	无返回值
	*/
	void LoadEventsCb(int count, int unread, const std::list<nim::SysMessage> &result);

	/**
	* 向界面中添加一个系统消息项
	* @param[in] msg 系统消息
	* @param[in] first 是否添加到开头
	* @return void	无返回值
	*/
	void AddEvent(const nim::SysMessage &msg, bool first);

	/**
	* 向界面中添加一个自定义通知项
	* @param[in] msg 系统消息
	* @param[in] first 是否添加到开头
	* @return void	无返回值
	*/
	void AddCustomMsg(const nim::SysMessage& msg, bool first);

	/**
	* 显示或隐藏正在载入消息的提示
	* @param[in] show 显示或有隐藏
	* @return void	无返回值
	*/
	void ShowLoadingTip(bool show);

	/**
	* 获取更多自定义通知消息
	* @return void	无返回值
	*/
	void GetMoreCustomMsg();

	/**
	* 响应用户信息改变的回调函数
	* @param[in] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] account 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);

	/**
	* 响应群名称改变的回调函数
	* @param[in] team_info 群信息
	* @return void 无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);

public:
	static const LPCTSTR kClassName;
private:
	ui::Button*		btn_recycle_;
	ui::ListBox*	event_list_;
	ui::ListBox*	custom_list_;

	__int64			farst_time_;
	std::map<__int64,TeamEventItem*>	id_item_pair_;

	bool has_more_;
	bool is_loading_;

	__int64 last_custom_msg_time_;//早的一条消息时间
	bool has_more_custom_;

	AutoUnregister unregister_cb;
};


/**
* 全局函数，更新未读系统消息条数
* @param[in]	count 未读条数
* @return void	无返回值
*/
void UpdateSysmsgUnread(int count);

/**
* 全局函数，更新未读自定义消息条数
* @param[in]	add 是否新增了未读消息，是则未读条数加1，否则未读条数清0。
* @return void	无返回值
*/
void UpdateCustomSysmsgUnread(bool add);

}