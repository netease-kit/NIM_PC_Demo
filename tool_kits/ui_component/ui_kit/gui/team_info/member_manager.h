#ifndef MEMBER_MANAGER_FORM_H_
#define MEMBER_MANAGER_FORM_H_

#include "util/window_ex.h"
#include "module/service/user_service.h"

namespace nim_comp
{
class MemberManagerForm : public WindowEx
{
public:
	//窗口类型，发起聊天窗口和邀请群成员共用
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
	bool OnBtnConfirmClick(ui::EventArgs* param);
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
