#ifndef MEMBER_MANAGER_FORM_H_
#define MEMBER_MANAGER_FORM_H_

#include "util/window_ex.h"
#include "module/service/user_service.h"

namespace nim_comp
{
/** @class MemberManagerForm
  * @brief 群成员管理窗口，发起聊天窗口和邀请群成员共用
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/21
  */
class MemberManagerForm : public WindowEx
{
public:
	/**
	* 构造函数
	* @param[in] team_id 群组id
	* @param[in] member_info 群成员信息
	* @param[in] show_privilege_panel 是否显示权限修改控件
	*/
	MemberManagerForm(const std::string& team_id, const nim::TeamMemberProperty& member_info, bool show_privilege_panel);
	virtual ~MemberManagerForm();
	
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	virtual void InitWindow() override;
	static const LPCTSTR kClassName;

private:
	/**
	* 处理确认按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnBtnConfirmClick(ui::EventArgs* param);

	/**
	* 处理取消按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnBtnCancelClick(ui::EventArgs* param);

private:
	ui::RichEdit* re_team_card_;

	std::string tid_;
	nim::TeamMemberProperty member_info_;
	bool show_privilege_panel_;

	AutoUnregister unregister_cb;
};
}
#endif // MEMBER_MANAGER_FORM_H_
