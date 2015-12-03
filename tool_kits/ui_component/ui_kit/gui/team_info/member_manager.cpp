#include "member_manager.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "callback/team/team_callback.h"
#include "shared/tool.h"
#include "gui/invoke_chat_form/invoke_chat_form.h"
#include "shared/ui/msgbox.h"
#include "module/login/login_manager.h"
#include "module/service/user_service.h"
#include "module/service/team_service.h"

using namespace ui;
using namespace std;

namespace nim_comp
{
const LPCTSTR MemberManagerForm::kClassName			= _T("MemberManagerForm");

MemberManagerForm::MemberManagerForm(const std::string& team_id, const std::string& user_id, bool show_privilege_panel)
{
	tid_ = team_id;
	user_id_ = user_id;
	show_privilege_panel_ = show_privilege_panel;
}

MemberManagerForm::~MemberManagerForm()
{

}

std::wstring MemberManagerForm::GetSkinFolder()
{
	return L"team_info";
}

std::wstring MemberManagerForm::GetSkinFile()
{
	return L"member_manager.xml";
}

std::wstring MemberManagerForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MemberManagerForm::GetWindowId() const
{
	std::wstring ws_user_id_ = nbase::UTF8ToUTF16(user_id_);
	return ws_user_id_;
}

UINT MemberManagerForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MemberManagerForm::InitWindow()
{
	FindControl(L"privilege_panel")->SetVisible(show_privilege_panel_);
	nim::UserNameCard user_info;
	UserService::GetInstance()->GetUserInfo(user_id_, user_info);
	Button* head_image_button = (Button*)FindControl(L"head_image");
	head_image_button->SetBkImage(UserService::GetInstance()->GetUserPhoto(user_info.GetAccId()));
	Label* show_name_label = (Label*)FindControl(L"show_name");
	show_name_label->SetText(UserService::GetInstance()->GetUserName(user_info.GetAccId()));
	re_team_card_ = (ui::RichEdit*)FindControl(L"team_card");
	re_team_card_->SetLimitText(10);
	ui::Button* btn_confirm = (ui::Button*)FindControl(L"confirm");
	btn_confirm->AttachClick(nbase::Bind(&MemberManagerForm::OnBtnConfirmClick, this, std::placeholders::_1));
	ui::Button* btn_cancel = (ui::Button*)FindControl(L"cancel");
	btn_cancel->AttachClick(nbase::Bind(&MemberManagerForm::OnBtnCancelClick, this, std::placeholders::_1));

	nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&MemberManagerForm::OnQueryTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	OnUserPhotoReadyCallback cb = ToWeakCallback([this](const std::string& accid, const std::wstring& photo_path) {
		if (user_id_ == accid) FindControl(L"head_image")->SetBkImage(photo_path); });
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(cb));
}

void MemberManagerForm::OnQueryTeamMembers(const std::string& tid, int member_count, const std::list<nim::TeamMemberProperty>& team_member_info_list)
{
	for (auto& team_member_info : team_member_info_list)
	{
		if (user_id_ == team_member_info.GetAccountID())
		{
			user_type_ = team_member_info.GetUserType();
			if (user_type_ == nim::kNIMTeamUserTypeCreator)
			{
				((Option*)FindControl(L"owner"))->Selected(true);
			}
			else if (user_type_ == nim::kNIMTeamUserTypeManager)
			{
				((Option*)FindControl(L"manager"))->Selected(true);
			}
			else if (user_type_ == nim::kNIMTeamUserTypeNomal)
			{
				((Option*)FindControl(L"member"))->Selected(true);
			}

			std::string team_card = team_member_info.GetNick();
			re_team_card_->SetUTF8Text(team_card);
			team_card_ = team_card;

			break;
		}
	}
}

static void OnTeamEventCallback(const std::string& uid, const std::string& team_card, const nim::TeamEvent& team_event)
{
	if (team_event.res_code_ == nim::kNIMResSuccess)
	{
		if (team_event.notification_id_ == nim::kNIMNotificationIdLocalUpdateOtherNick || team_event.notification_id_ == nim::kNIMNotificationIdLocalUpdateTlist)
		{
			TeamService::GetInstance()->InvokeChangeTeamMember(team_event.team_id_, uid, team_card);
		}
	}
}

bool MemberManagerForm::OnBtnConfirmClick(ui::EventArgs* param)
{
	if (((Option*)FindControl(L"owner"))->IsSelected())
	{
		nim::Team::TransferTeamAsync(tid_, user_id_, false, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
	else if (((Option*)FindControl(L"manager"))->IsSelected() && user_type_ != nim::kNIMTeamUserTypeManager)
	{
		std::list<std::string> uids_list;
		uids_list.push_back(user_id_);
		nim::Team::AddManagersAsync(tid_, uids_list, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
	else if (((Option*)FindControl(L"member"))->IsSelected() && user_type_ != nim::kNIMTeamUserTypeNomal)
	{
		std::list<std::string> uids_list;
		uids_list.push_back(user_id_);
		nim::Team::RemoveManagersAsync(tid_, uids_list, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
	
	std::string new_team_card = nbase::StringTrim(re_team_card_->GetUTF8Text());
	if (!team_card_.empty() && new_team_card.empty())
	{
		MsgboxCallback cb = ToWeakCallback([this](MsgBoxRet ret) {
			this->ActiveWindow();
		});
		ShowMsgBox(m_hWnd, L"群昵称不能为空", ToWeakCallback(cb), L"提示", L"确定", L"");
		return true;
	}

	if (new_team_card != team_card_)
	{
		nim::TeamMemberProperty values(tid_, user_id_);
		values.SetNick(new_team_card);
		if (user_id_ != LoginManager::GetInstance()->GetAccount()) 
			nim::Team::UpdateOtherNickAsync(values, nbase::Bind(&OnTeamEventCallback, user_id_, new_team_card, std::placeholders::_1));
		else 
			nim::Team::UpdateMyPropertyAsync(values, nbase::Bind(&OnTeamEventCallback, user_id_, new_team_card, std::placeholders::_1));
	}

	Close();
	return true; 
}

bool MemberManagerForm::OnBtnCancelClick(ui::EventArgs* param)
{
	Close();
	return true;
}
}