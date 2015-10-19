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
	MemberManagerForm(const std::string& team_id, const std::string& user_id, bool show_privilege_panel);
	virtual ~MemberManagerForm();
	
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	virtual void MemberManagerForm::InitWindow() override;
	static const LPCTSTR kClassName;

private:
	void OnQueryTeamMembers(const std::string& tid, int member_count, const std::list<nim::TeamMemberInfo>& team_member_info_list);
	bool OnBtnConfirmClick(ui::EventArgs* param);
	bool OnBtnCancelClick(ui::EventArgs* param);

	std::string tid_;
	std::string user_id_;
	bool show_privilege_panel_;
	nim::NIMTeamUserType user_type_;
	ui::RichEdit* re_team_card_;
	std::string team_card_;
	AutoUnregister unregister_cb;
};
}
#endif // MEMBER_MANAGER_FORM_H_
