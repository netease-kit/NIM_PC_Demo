#pragma once
#include "gui/session/control/unread_item.h"
#include "module/service/photo_service.h"

namespace nim_comp
{
class UnreadForm : public WindowEx
{
public:
	UnreadForm();
	~UnreadForm();

	static void OnTeamEventCallback(const nim::TeamEvent& result);

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
	* 响应查询系统消息的回调函数
	* @param[in] count 查询到的消息数量
	* @param[in] unread 未读消息数
	* @param[in] result 查询到的系统消息列表
	* @return void	无返回值
	*/
	void LoadList(const nim::IMMessage &msg, const std::map<std::string, nim::TeamMemberProperty> &team_members);

	void OnLoadListCallback(const nim::TeamEvent& result);

	void UpdateUnreadCount(const std::string &msg_id, const int unread, const std::string &read_accid);
private:

	/**
	* 向界面中添加一个系统消息项
	* @param[in] msg 系统消息
	* @param[in] first 是否添加到开头
	* @return void	无返回值
	*/
	void AddItem(const nim::TeamMemberProperty &member, bool unread);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] account 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);

	/**
	* 显示或隐藏正在载入消息的提示
	* @param[in] show 显示或有隐藏
	* @return void	无返回值
	*/
	void ShowLoadingTip(bool show);

private:
	void UpdateUnreadCount(int count);
	void UpdateReadCount(int count);
	void DoLoadList();

	

public:
	static const LPCTSTR kClassName;
private:
	ui::ListBox*	unread_list_;
	ui::ListBox*	read_list_;

	ui::Label*	read_title_ =nullptr;
	ui::Label*  unread_title_ = nullptr;
	ui::Label*  form_title_ = nullptr;
	std::map<std::string, nim::TeamMemberProperty> members_;
	AutoUnregister unregister_cb;

	nim::IMMessage msg_;
};
}