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
    //接口实现
    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;

    virtual std::wstring GetWindowClassName() const override;
    virtual std::wstring GetWindowId() const override;

	virtual void InitWindow() override;

public:
	void OnMultiportPushConfigChange(bool switch_on);

private:
	bool Notify(ui::EventArgs* msg);
	bool OnMultiPushSwitchSelected(ui::EventArgs* args); //多端推送打开（桌面端在线时移动端不需推送）
	bool OnMultiPushSwitchUnSelected(ui::EventArgs* args); //多端推送关闭（桌面端在线时移动端需推送）
	bool OnNotifySwitchSelected(ui::EventArgs* args); //消息提醒打开（接收该用户的消息）
	bool OnNotifySwitchUnSelected(ui::EventArgs* args); //消息提醒关闭（屏蔽该用户的消息）
	bool OnBlackSwitchSelected(ui::EventArgs* args); //黑名单选项打开（将该用户加入黑名单）
	bool OnBlackSwitchUnSelected(ui::EventArgs* args); //黑名单选项关闭（将该用户从黑名单移除）
	bool OnMuteSwitchSelected(ui::EventArgs* args); //设置禁言选项打开（将该用户加入禁言状态）
	bool OnMuteSwitchUnSelected(ui::EventArgs* args); //设置禁言选项关闭（将该用户从禁言状态中解除除）
	void OnNotifyChangeCallback(std::string id, bool mute);
	void OnBlackChangeCallback(std::string id, bool black);
	bool OnStartChatBtnClicked(ui::EventArgs* args); //聊天按钮被点击
	bool OnDeleteFriendBtnClicked(ui::EventArgs* args); //删除好友按钮被点击
	void OnDeleteFriendMsgBox(MsgBoxRet ret);
	bool OnAddFriendBtnClicked(ui::EventArgs* args); //添加好友按钮被点击
	bool OnBirthdayComboSelect(ui::EventArgs* args);

	bool OnModifyOrCancelBtnClicked(ui::EventArgs* args, bool to_modify); //编辑信息或取消编辑按钮被点击
	bool OnSaveInfoBtnClicked(ui::EventArgs* args); //保存编辑信息按钮被点击
	bool OnAliasEditGetFocus(ui::EventArgs* args); //备注名编辑框获得焦点
	bool OnAliasEditLoseFocus(ui::EventArgs* args); //备注名编辑框失去焦点
	bool OnAliasEditMouseEnter(ui::EventArgs* args); //鼠标进入备注名编辑框
	bool OnAliasEditMouseLeave(ui::EventArgs* args); //鼠标离开备注名编辑框
	bool OnReturnOnAliasEdit(ui::EventArgs* args); //备注名编辑框回车

	void InitUserInfo(const nim::UserNameCard & info); //只能使用一次，否则会多次注册回调。
	void InitLabels(); //打开名片后，初始化各项信息
	void SetShowName();
	void InitEdits(); //点击编辑按钮之后，初始化各个下拉框和文本输入框
	void InitBirthdayCombo(); //初始化生日下拉框
	void CheckInMuteBlack(); //检查该用户是否在自己的静音列表和黑名单中

	void OnFriendListChange(FriendChangeType change_type, const std::string& accid);
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);
	void OnMiscUInfoChange(const std::list<nim::UserNameCard> &uinfos);

	bool OnHeadImageClicked(ui::EventArgs* args); //自己名片的头像被点击
private:
	void OnModifyHeaderComplete(const std::string& id, const std::string &url);
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