#pragma once

#include "util/window_ex.h"
#include "module/service/photo_service.h"

namespace nim_comp
{
/** @class BlackListWindow
  * @brief 黑名单窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class BlackListWindow : public WindowEx
{
public:
	BlackListWindow();
	virtual ~BlackListWindow();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 刷新黑名单窗口
	* @return void	无返回值
	*/
	void RefreshBlackList();

private:
	/**
	* 处理解除按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnRemoveBtnClicked(ui::EventArgs *args);
	
	/**
	* 根据用户名片重置列表中对应的黑名单项
	* @param[in] info 用户名片
	* @return void	无返回值
	*/
	void ResetUserInfo(const nim::UserNameCard &info);

	/**
	* 根据用户名片向列表中添加黑名单项
	* @param[in] info 用户名片
	* @return void	无返回值
	*/
	void AddBlackListMember(const nim::UserNameCard &info);

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
	* 响应黑名单改变的回调函数
	* @param[in] id 用户id
	* @param[in] black 是否加入黑名单
	* @return void 无返回值
	*/
	void OnSetBlackCallback(const std::string& id, bool black);
public:
	static const LPCTSTR kClassName;

private:
	ui::ListBox*	m_black_list = NULL;
	ui::Button*		m_add_black = NULL;
	ui::TreeView*	m_friend_list = NULL;
	ui::TabBox*		m_page_switch = NULL;
	AutoUnregister	unregister_cb;
};
}