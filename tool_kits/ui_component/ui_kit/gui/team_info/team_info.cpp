#include "team_info.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "callback/team/team_callback.h"
#include "shared/tool.h"
#include "gui/contact_select_form/contact_select_form.h"
#include "module/login/login_manager.h"
#include "gui/team_info/member_manager.h"
#include "gui/profile_form/head_modify_form.h"
#include "team_info_box.h"
#include "gui/plugins/team/team_plugin.h"
#include "module/plugins/main_plugins_manager.h"
using namespace ui;
using namespace std;

namespace nim_comp
{
const LPCTSTR TeamInfoForm::kClassName = L"TeamInfoFormClassName";
const LPCTSTR TeamInfoForm::kGroupInfoWindowId = L"GroupFormWindowId";
const LPCTSTR TeamInfoForm::kTeamInfoWindowId = L"TeamInfoFormWindowId";

TeamInfoForm::TeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info)
{
	team_info_box_ = new TeamInfoBox(create_or_display,type,team_id,team_info);
	team_info_box_->AttachClose(std::bind(&TeamInfoForm::Close, this, std::placeholders::_1));
	team_info_box_->AttachSetTaskbarTitle(std::bind(&TeamInfoForm::SetTaskbarTitle, this, std::placeholders::_1));
	team_info_box_->AttachActive(std::bind(&TeamInfoForm::ActiveWindow, this));
}

TeamInfoForm::~TeamInfoForm()
{

}

std::wstring TeamInfoForm::GetSkinFolder()
{
	return L"team_info";
}

std::wstring TeamInfoForm::GetSkinFile()
{
	return L"team_info.xml";
}

std::wstring TeamInfoForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring TeamInfoForm::GetWindowId() const
{
	if (team_info_box_->IsCreateOrDisplay()) {
		if (team_info_box_->GetTeamType() == nim::kNIMTeamTypeNormal)
		{
			return kGroupInfoWindowId;
		}
		else {
			return kTeamInfoWindowId;
		}
	}
	else {	
		return team_info_box_->GetTeamID();
	}
}

UINT TeamInfoForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void TeamInfoForm::InitWindow()
{
	ui::GlobalManager::FillBoxWithCache(team_info_box_, L"team_info/team_info_box.xml");
	dynamic_cast<ui::Box*>(FindControl(L"container"))->Add(team_info_box_);
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	std::wstring title_text;
	ui::Label* label_title = (ui::Label*)FindControl(L"title");
	if (team_info_box_->IsCreateOrDisplay()) {
		if (team_info_box_->GetTeamType() == nim::kNIMTeamTypeNormal)
			title_text = mls->GetStringViaID(L"STRID_TEAM_INFO_CREATE_GROUP");
		else if (team_info_box_->GetTeamType() == nim::kNIMTeamTypeAdvanced)
			title_text = mls->GetStringViaID(L"STRID_TEAM_INFO_CREATE_ADVANCED_TEAM");
	}
	else {
		if (team_info_box_->GetTeamType() == nim::kNIMTeamTypeNormal)
			title_text = mls->GetStringViaID(L"STRID_TEAM_INFO_GROUP_INFO");
		else if (team_info_box_->GetTeamType() == nim::kNIMTeamTypeAdvanced)
			title_text = mls->GetStringViaID(L"STRID_TEAM_INFO_TEAM_INFO");
	}
	label_title->SetText(title_text);
	SetTaskbarTitle(title_text);
}
void TeamInfoForm::SetTaskbarTitle(const std::wstring &title)
{
	WindowEx::SetTaskbarTitle(title);
}
void TeamInfoForm::Close(UINT nRet/* = 0*/)
{
	WindowEx::Close(nRet);
}
void TeamInfoForm::ActiveWindow()
{
	WindowEx::ActiveWindow();
}
void TeamInfoForm::UpdateTeamMember()
{
	team_info_box_->UpdateTeamMember();
}
void TeamInfoForm::ShowTeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info, bool just_box)
{
	if (!just_box)
		ShowTeamInfoForm(create_or_display, type, team_id, team_info);
	else
		MainPluginsManager::GetInstance()->GetPlugin<TeamPlugin>(TeamPlugin::kPLUGIN_NAME)->ShowTeamInfoBox(create_or_display, type, team_id, team_info);
}
TeamInfoForm *TeamInfoForm::ShowTeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info)
{	
	std::wstring session_id = nbase::UTF8ToUTF16(team_id);
	TeamInfoForm* team_info_form = (TeamInfoForm*)WindowsManager::GetInstance()->GetWindow\
		(TeamInfoForm::kClassName, session_id);
	if (team_info_form == NULL)
	{
		team_info_form = new TeamInfoForm(false, team_info.GetType(), team_id, team_info);
		team_info_form->Create(NULL, L"", WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
		team_info_form->CenterWindow();
		team_info_form->ShowWindow(true);
	}
	else
	{
		team_info_form->ActiveWindow();
	}
	return team_info_form;
}
void TeamInfoForm::UpdateTeamMember(const std::wstring& tid)
{
	TeamInfoForm* team_info_form = (TeamInfoForm*)WindowsManager::GetInstance()->GetWindow(TeamInfoForm::kClassName, tid);
	if (team_info_form != nullptr)
	{
		team_info_form->UpdateTeamMember();
	}

}

}