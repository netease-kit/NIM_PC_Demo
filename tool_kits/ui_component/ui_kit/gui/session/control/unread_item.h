#pragma once

namespace nim_comp
{

class UnreadItem : public ui::ListContainerElement
{
public:
	UnreadItem();
	virtual ~UnreadItem();

	/**
	* 初始化控件
	* @return void	无返回值
	*/
	void InitCtrl();

	/**
	* 初始化控件界面效果
	* @param[in] json 系统消息和自定义通知数据
	* @return void	无返回值
	*/
	bool InitInfo(const nim::TeamMemberProperty &json);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] account 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	bool OnUserPhotoReady(const std::string& accid, const std::wstring& photo_path);

	/**
	* 处理好友控件头像单击消息
	* @param[in] msg 消息的相关信息
	* @param[in] type 类型
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnHeadImageClick(const std::string& uid, ui::EventArgs*);

private:
	ui::Button*		btn_head_;
	ui::Label*		member_name_ = nullptr;
	ui::Label*		member_org_ = nullptr;

	nim::TeamMemberProperty member_;
	AutoUnregister unregister_cb;
};
}