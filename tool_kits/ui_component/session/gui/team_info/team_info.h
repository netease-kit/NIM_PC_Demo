#ifndef TEAM_INFO_FORM_H_
#define TEAM_INFO_FORM_H_

#include "util/window_ex.h"
#include "module/service/user_service.h"
#include "module/service/team_service.h"
#include "shared/ui/msgbox.h"

namespace nim_comp
{
class TeamInfoForm : public WindowEx
{
public:
	//窗口类型，发起聊天窗口和邀请群成员共用
	TeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info);
	virtual ~TeamInfoForm();
	
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	static const LPCTSTR kClassName;
	static const LPCTSTR kGroupInfoWindowId;
	static const LPCTSTR kTeamInfoWindowId;
	
	void Init(const std::string& team_id, int count, const std::list<nim::TeamMemberInfo>& team_member_list);
	void AddTeamMembersInfo(const std::list<nim::TeamMemberInfo>& team_member_list);
	void UpdateTeamMember();

private:
	void AddInviteButton();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;
	bool OnInviteUesrBtnClick(ui::EventArgs *param);
	void SelectedCompleted(const std::list<UTF8String>& id_list);
	ui::HBox* CreateTeamMemberListItem(nim::NIMTeamUserType user_type, const std::string& team_card, const UserInfo& user_info);
	void UpdateTeamMemberListItem(ui::Box* container_element, const std::string& team_card, const UserInfo& user_info);
	void ChangeUIByIdentity();
	bool OnBtnDeleteClick(ui::Box* container_element, const UTF8String& user_id, ui::EventArgs* args);
	void OnMsgbox(const UTF8String& user_id, MsgBoxRet ret);
	bool OnTeamMemberItemMouseEnter(ui::Box* team_member_item, ui::EventArgs* args);
	bool OnTeamMemberItemMouseLeave(ui::Box* team_member_item, ui::EventArgs* args);
	bool OnBtnHeadImageClick(const UTF8String& user_id, ui::EventArgs* args);
	bool OnBtnConfirmClick(ui::EventArgs* param);
	bool OnBtnCancelClick(ui::EventArgs* param);
	bool OnBtnDissmissClick(ui::EventArgs* param);
	bool OnBtnQuitClick(ui::EventArgs* param);
	void OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberInfo& team_member_info);
	void OnTeamMemberRemove(const std::string& tid, const std::string& uid);
	void OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card);
	void OnTeamMemberAdmin(const std::string& tid, const std::string& uid, bool admin);
	void OnSetTeamOwner(const std::string& tid, const std::string& uid);
	void OnUserInfoChange(const std::list<UserInfo>& uinfos);
	void OnUserPhotoReady(const std::string& accid, const std::wstring &photo_path);

private:
	ui::ListBox* tile_box_;
	nim::NIMTeamUserType user_type_;
	bool create_or_display_;
	nim::NIMTeamType type_;
	std::string tid_;
	nim::TeamInfo team_info_;
	nim::TeamMemberInfo my_tInfo_;
	ui::RichEdit* re_team_name_;
	ui::RichEdit* re_team_intro_;
	ui::Button* invitebtn_;
	ui::Button* btn_dismiss_;
	ui::Button* btn_quit_;

private:
	AutoUnregister unregister_cb;
};
}
#endif // TEAM_INFO_FORM_H_
