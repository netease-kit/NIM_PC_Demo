#pragma once
#include "shared/auto_unregister.h"
#include "shared/ui/msgbox.h"
#include "module/service/photo_service.h"

namespace nim_comp
{
class ProfileForm : public WindowEx
{
public:
	static ProfileForm *ShowProfileForm(UTF8String uid);
	static ProfileForm *ShowProfileForm(UTF8String tid, UTF8String uid, nim::NIMTeamUserType my_type);
private:
	ProfileForm();
	ProfileForm(UTF8String tid, UTF8String uid, nim::NIMTeamUserType my_type)
	{
		tid_ = tid;
		my_team_user_type_ = my_type;
	}
	~ProfileForm();

public:
	//覆盖虚函数
    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;
    virtual std::wstring GetWindowClassName() const override;
    virtual std::wstring GetWindowId() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

public:
	/**
	* 响应多端推送配置改变的回调函数
	* @param[in] switch_on 是否开启多端推送
	* @return void 无返回值
	*/
	void OnMultiportPushConfigChange(bool switch_on);

private:
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 多端推送打开（桌面端在线时移动端不需推送）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMultiPushSwitchSelected(ui::EventArgs* args);

	/**
	* 多端推送关闭（桌面端在线时移动端需推送）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMultiPushSwitchUnSelected(ui::EventArgs* args);

	/**
	* 消息提醒打开（接收该用户的消息）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnNotifySwitchSelected(ui::EventArgs* args);

	/**
	* 消息提醒关闭（屏蔽该用户的消息）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnNotifySwitchUnSelected(ui::EventArgs* args);

	/**
	* 黑名单选项打开（将该用户加入黑名单）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnBlackSwitchSelected(ui::EventArgs* args);

	/**
	* 黑名单选项关闭（将该用户从黑名单移除）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnBlackSwitchUnSelected(ui::EventArgs* args);

	/**
	* 设置禁言选项打开（将该用户加入禁言状态）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMuteSwitchSelected(ui::EventArgs* args);

	/**
	* 设置禁言选项关闭（将该用户从禁言状态中解除除）
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMuteSwitchUnSelected(ui::EventArgs* args);
	void OnNotifyChangeCallback(std::string id, bool mute);
	void OnBlackChangeCallback(std::string id, bool black);

	/**
	* 处理聊天按钮被点击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnStartChatBtnClicked(ui::EventArgs* args);

	/**
	* 处理删除好友按钮被点击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnDeleteFriendBtnClicked(ui::EventArgs* args);

	/**
	* 处理确认删除好友的提示框返回的回调结果
	* @param[in] ret 提示框的返回值
	* @return void 无返回值
	*/
	void OnDeleteFriendMsgBox(MsgBoxRet ret);

	/**
	* 处理添加好友按钮被点击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnAddFriendBtnClicked(ui::EventArgs* args);

	/**
	* 处理设置生日的组合框的选择消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnBirthdayComboSelect(ui::EventArgs* args);

	/**
	* 处理编辑信息或取消编辑按钮被点击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnModifyOrCancelBtnClicked(ui::EventArgs* args, bool to_modify);

	/**
	* 处理保存编辑信息按钮被点击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSaveInfoBtnClicked(ui::EventArgs* args);

	/**
	* 处理备注名编辑框获得焦点消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnAliasEditGetFocus(ui::EventArgs* args);

	/**
	* 处理备注名编辑框失去焦点消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnAliasEditLoseFocus(ui::EventArgs* args);

	/**
	* 处理鼠标进入备注名编辑框消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnAliasEditMouseEnter(ui::EventArgs* args);

	/**
	* 处理鼠标离开备注名编辑框消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnAliasEditMouseLeave(ui::EventArgs* args);

	/**
	* 处理备注名编辑框回车消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnReturnOnAliasEdit(ui::EventArgs* args);

	/**
	* 初始化用户信息，只能使用一次，否则会多次注册回调
	* @param[in] info 用户名片
	* @return void	无返回值
	*/
	void InitUserInfo(const nim::UserNameCard & info);

	/**
	* 打开名片后，初始化各项提示信息
	* @return void	无返回值
	*/
	void InitLabels();

	/**
	* 设置昵称、备注名等
	* @return void	无返回值
	*/
	void SetShowName();

	/**
	* 点击编辑按钮之后，初始化各个下拉框和文本输入框
	* @return void	无返回值
	*/
	void InitEdits();

	/**
	* 初始化生日下拉框
	* @return void	无返回值
	*/
	void InitBirthdayCombo();

	/**
	* 检查该用户是否在自己的静音列表和黑名单中
	* @return void	无返回值
	*/
	void CheckInMuteBlack();

	/**
	* 响应好友列表改变的回调函数
	* @param[in] change_type 好友变化类型
	* @param[in] accid 用户id
	* @return void 无返回值
	*/
	void OnFriendListChange(FriendChangeType change_type, const std::string& accid);

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
	* 响应其他用户信息改变的回调函数
	* @param[in] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void OnMiscUInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* 处理自己名片的头像被点击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnHeadImageClicked(ui::EventArgs* args);
private:
	/**
	* 处理头像修改完毕后的回调函数
	* @param[in] id 用户id
	* @param[in] url 头像的下载地址
	* @return void	无返回值
	*/
	void OnModifyHeaderComplete(const std::string& id, const std::string &url);

	/**
	* 处理头像更新完毕后的回调函数
	* @param[in] res 错误码
	* @return void	无返回值
	*/
	void UpdateUInfoHeaderCallback(int res);

public:
	static const LPCTSTR kClassName;

public:
	std::string	tid_;
	nim::NIMTeamUserType my_team_user_type_;
	bool have_mute_right_ = false;

private:
	nim::UserNameCard	m_uinfo;
	int				user_type; // -1：自己，0：陌生人，1：好友
	AutoUnregister	unregister_cb;

private:
	ui::Button*		head_image_btn = NULL;
	ui::Label*		show_name_label = NULL;
	ui::Label*		user_id_label = NULL;
	ui::Label*		nickname_label = NULL;
	ui::CheckBox*	multi_push_switch = NULL;
	ui::CheckBox*	notify_switch = NULL;
	ui::CheckBox*	black_switch = NULL;
	ui::CheckBox*	mute_switch = NULL;
	ui::Button*		start_chat = NULL;
	ui::TabBox*		add_or_del = NULL;
	ui::Button*		delete_friend = NULL;
	ui::Button*		add_friend = NULL;

	ui::CheckBox*	sex_icon = NULL;
	ui::Button*		btn_modify_info = NULL;
	ui::Button*		btn_cancel_modify = NULL;
	ui::Button*		btn_save_modify = NULL;
	ui::HBox*		alias_box = NULL;
	ui::RichEdit*	alias_edit = NULL;
	ui::HBox*		nickname_box = NULL;
	ui::RichEdit*	nickname_edit = NULL;
	ui::Label*		nickname_error_tip = NULL;
	ui::HBox*		sex_box = NULL;
	ui::Combo*		sex_option = NULL;
	ui::Label*		birthday_label = NULL;
	ui::HBox*		birthday_combo_box = NULL;
	ui::Combo*		birth_year_combo = NULL;
	ui::Combo*		birth_month_combo = NULL;
	ui::Combo*		birth_day_combo = NULL;
	ui::Label*		phone_label = NULL;
	ui::RichEdit*	phone_edit = NULL;
	ui::Label*		email_label = NULL;
	ui::RichEdit*	email_edit = NULL;
	ui::Label*		signature_label = NULL;
	ui::RichEdit*	signature_edit = NULL;
};
}