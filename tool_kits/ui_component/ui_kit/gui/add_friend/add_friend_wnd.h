// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: pqk
// Date: 2013/8/14
#pragma once

#include "util/window_ex.h"
#include "module/service/photo_service.h"

namespace nim_comp
{
const LPCTSTR g_ADDFRIEND_SEARCH_PAGE	  = L"search_page";
const LPCTSTR g_ADDFRIEND_USERINFO_PAGE	  = L"userinfo_page";
const LPCTSTR g_ADDFRIEND_NOTFOUND_PAGE	  = L"notfound_page";
const LPCTSTR g_ADDFRIEND_SENDFRIENDAPPLY_PAGE	  = L"send_friend_apply_page";
const LPCTSTR g_ADDFRIEND_SENDCOMPLETE_PAGE	  = L"send_complete_page";
const LPCTSTR g_ADDFRIEND_SENDTOOFREQUENT_PAGE	  = L"send_too_frequent_page";
const LPCTSTR g_ADDFRIEND_ADDSUCCESS_PAGE	  = L"add_success_page";
const LPCTSTR g_ADDFRIEND_NET_ABNORMAL_PAGE = L"net_abnormal";

/** @class AddFriendWindow
  * @brief 添加好友窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author pqk
  * @date 2016/09/19
  */
class AddFriendWindow : public WindowEx
{
public:
	AddFriendWindow();
	virtual ~AddFriendWindow();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	/**
	* 处理搜索按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Search(ui::EventArgs* param);

	/**
	* 处理取消按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Cancel(ui::EventArgs* param);

	/**
	* 根据用户名片设置并切换用户信息展示页面
	* @param[in] user_info 用户名片
	* @return void	无返回值
	*/
	void InitUserProfile(const nim::UserNameCard& user_info);

	/**
	* 清空输入框中的文本
	* @return void	无返回值
	*/
	void InitEdit();

	enum INIT_TYPE
	{
		NONE,
		SearchAndSendApplyEdit,
		SendApplyEdit,
	};

	/**
	* 处理上一步或者下一步按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @param[in] page_name 要跳转的目标页面
	* @param[in] init_edit 要清理的输入框的类型
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool PreOrNextClick(ui::EventArgs* param, const std::wstring& page_name, INIT_TYPE init_edit);

public:
	static const LPCTSTR kClassName;

private:
	/**
	* 处理添加好友按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool AddFriendClick(ui::EventArgs* param);

	/**
	* 处理开始聊天按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool ChatClick(ui::EventArgs* param);

	/**
	* 处理从黑名单移除按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool RemoveFromBlack(ui::EventArgs* args);

	/**
	* 处理发送添加好友请求按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool SendFriendApplyClick(ui::EventArgs* param);

	/**
	* 处理输入框获取焦点的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSearchKeyEditSetFocus(void* param);

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

private:
	ui::Button* headimage_;
	ui::Label* nick_name_;
	ui::RichEdit* search_key_;
	ui::RichEdit* apply_words_;
	ui::TabBox*	tablayout_;
	ui::TabBox* addfriend_or_chat_;
	ui::Button* userinfo_page_add_friend_;

private:
	UTF8String id_;
	bool da_key_;
	AutoUnregister unregister_cb;
};
}
