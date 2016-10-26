#include "team_info.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "callback/team/team_callback.h"
#include "shared/tool.h"
#include "gui/contact_select_form/contact_select_form.h"
#include "module/login/login_manager.h"
#include "gui/team_info/member_manager.h"
#include "gui/profile_form/head_modify_form.h"

using namespace ui;
using namespace std;

namespace nim_comp
{
const LPCTSTR TeamInfoForm::kClassName = L"TeamInfoFormClassName";
const LPCTSTR TeamInfoForm::kGroupInfoWindowId = L"GroupFormWindowId";
const LPCTSTR TeamInfoForm::kTeamInfoWindowId = L"TeamInfoFormWindowId";

TeamInfoForm::TeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info)
{
	create_or_display_ = create_or_display;
	type_ = type;
	tid_ = team_id;
	team_info_ = team_info;
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
	if (create_or_display_) {
		if (type_ == nim::kNIMTeamTypeNormal)
		{
			return kGroupInfoWindowId;
		}
		else {
			return kTeamInfoWindowId;
		}
	}
	else {
		std::wstring ws_tid = nbase::UTF8ToUTF16(tid_);
		return ws_tid;
	}
}

UINT TeamInfoForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT TeamInfoForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void TeamInfoForm::InitWindow()
{
	unregister_cb.Add(TeamService::GetInstance()->RegAddTeamMember(nbase::Bind(&TeamInfoForm::OnTeamMemberAdd, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeamMember(nbase::Bind(&TeamInfoForm::OnTeamMemberRemove, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMember(nbase::Bind(&TeamInfoForm::OnTeamMemberChange, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegSetTeamAdmin(nbase::Bind(&TeamInfoForm::OnTeamAdminSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamOwner(nbase::Bind(&TeamInfoForm::OnTeamOwnerChange, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&TeamInfoForm::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&TeamInfoForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeam(nbase::Bind(&TeamInfoForm::OnTeamRemove, this, std::placeholders::_1)));

	std::wstring team_type = type_ == nim::kNIMTeamTypeNormal ? L"讨论组" : L"群";
	ui::Label* title_text = (ui::Label*)FindControl(L"title");
	if (create_or_display_) {
		if(type_ == nim::kNIMTeamTypeNormal)
			title_text->SetText(L"创建讨论组");
		else if(type_ == nim::kNIMTeamTypeAdvanced)
			title_text->SetText(L"创建高级群");
	}
	else {
		title_text->SetText(team_type + L"资料");
	}

	((ui::Label*)FindControl(L"team_id_label"))->SetText(team_type + L"ID");
	((ui::Label*)FindControl(L"team_name_label"))->SetText(team_type + L"名称");

	tile_box_ = (ui::ListBox*)FindControl(L"user_list");
	ui::Button* btn_confirm = (ui::Button*)FindControl(L"confirm");
	btn_confirm->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnConfirmClick, this, std::placeholders::_1));
	ui::Button* btn_cancel = (ui::Button*)FindControl(L"cancel");
	btn_cancel->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnCancelClick, this, std::placeholders::_1));
	btn_quit_ = (ui::Button*)FindControl(L"quit");
	btn_quit_->SetText(L"退出" + team_type);
	btn_quit_->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnQuitClick, this, std::placeholders::_1));
	btn_dismiss_ = (ui::Button*)FindControl(L"dismiss");
	btn_dismiss_->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnDissmissClick, this, std::placeholders::_1));

	btn_header_ = (ui::Button*)FindControl(L"head_image");
	if (!create_or_display_)
	{
		nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else {
		//默认开启消息通知 oleg 20161025
		((CheckBox*)FindControl(L"msg_mode_notify"))->Selected(true);
		AddInviteButton();
		btn_quit_->SetVisible(false);
		btn_dismiss_->SetVisible(false);
	}
	re_team_name_ = (RichEdit*)FindControl(L"team_name");
	re_team_intro_ = (RichEdit*)FindControl(L"team_intro");

	if (!create_or_display_) 
	{
		((RichEdit*)FindControl(L"team_id"))->SetUTF8Text(team_info_.GetTeamID());
		re_team_name_->SetUTF8Text(team_info_.GetName());
		if (type_ == nim::kNIMTeamTypeAdvanced)
		{
			btn_header_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(tid_));
			re_team_intro_->SetUTF8Text(team_info_.GetIntro());
			if (team_info_.GetJoinMode() == nim::kNIMTeamJoinModeNoAuth)
				((Option*)FindControl(L"join_mode_any"))->Selected(true);
			else if (team_info_.GetJoinMode() == nim::kNIMTeamJoinModeNeedAuth)
				((Option*)FindControl(L"join_mode_verify"))->Selected(true);
			else if (team_info_.GetJoinMode() == nim::kNIMTeamJoinModeRejectAll)
				((Option*)FindControl(L"join_mode_nobody"))->Selected(true);

			if (team_info_.GetBeInviteMode() == nim::kNIMTeamBeInviteModeNeedAgree)
				((Option*)FindControl(L"be_invite_mode_agree"))->Selected(true);
			else if (team_info_.GetBeInviteMode() == nim::kNIMTeamBeInviteModeNotNeedAgree)
				((Option*)FindControl(L"be_invite_mode_no_need_agree"))->Selected(true);

			if (team_info_.GetInviteMode() == nim::kNIMTeamInviteModeManager)
				((Option*)FindControl(L"invite_mode_manager"))->Selected(true);
			else if (team_info_.GetInviteMode() == nim::kNIMTeamInviteModeEveryone)
				((Option*)FindControl(L"invite_mode_any"))->Selected(true);

			if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateInfoModeManager)
				((Option*)FindControl(L"up_tinfo_mode_manager"))->Selected(true);
			else if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateInfoModeEveryone)
				((Option*)FindControl(L"up_tinfo_mode_any"))->Selected(true);

			if (team_info_.GetUpdateCustomMode() == nim::kNIMTeamUpdateCustomModeManager)
				((Option*)FindControl(L"up_custom_mode_manager"))->Selected(true);
			else if (team_info_.GetUpdateCustomMode() == nim::kNIMTeamUpdateCustomModeEveryone)
				((Option*)FindControl(L"up_custom_mode_any"))->Selected(true);
		}
	}
	else 
	{
		FindControl(L"team_id_panel")->SetVisible(false);

		if (type_ == nim::kNIMTeamTypeAdvanced)
		{
			btn_header_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(tid_));
			((Option*)FindControl(L"join_mode_verify"))->Selected(true);
			((Option*)FindControl(L"be_invite_mode_agree"))->Selected(true);
			((Option*)FindControl(L"invite_mode_manager"))->Selected(true);
			((Option*)FindControl(L"up_tinfo_mode_manager"))->Selected(true);
			((Option*)FindControl(L"up_custom_mode_manager"))->Selected(true);
		}
	}

	if (type_ == nim::kNIMTeamTypeNormal)
	{
		FindControl(L"team_info_fields")->SetVisible(false);
		FindControl(L"team_info_icon")->SetVisible(false);
	}
	else
		btn_header_->AttachClick(nbase::Bind(&TeamInfoForm::OnHeadImageClicked, this, std::placeholders::_1));
}

bool TeamInfoForm::OnHeadImageClicked(ui::EventArgs* args)
{
	temp_file_path_ = PhotoService::GetInstance()->GetPhotoDir(kTeam);
	std::wstring temp_file_name;
	nbase::Time::TimeStruct now = nbase::Time::Now().ToTimeStruct(true);
	nbase::StringPrintf(temp_file_name, L"temp_head_icon_%d%d%d%d%d%d",
		now.year(), now.month(), now.day_of_month(), now.hour(), now.minute(), now.second());
	temp_file_path_.append(temp_file_name);

	HeadModifyForm* form = (HeadModifyForm*)WindowsManager::GetInstance()->GetWindow(HeadModifyForm::kClassName, HeadModifyForm::kClassName);
	if (form == NULL)
	{
		form = new HeadModifyForm(tid_, temp_file_path_);
		form->Create(NULL, NULL, WS_OVERLAPPED, 0L);
		form->ShowWindow(true);
		form->CenterWindow();
	}
	else
	{
		::SetForegroundWindow(form->GetHWND());
	}
	form->RegCompleteCallback(nbase::Bind(&TeamInfoForm::OnModifyHeaderComplete, this, std::placeholders::_1, std::placeholders::_2));

	return true;
}

void TeamInfoForm::OnModifyHeaderComplete(const std::string& id, const std::string &url)
{
	new_header_url_ = url;
	if (btn_header_)
		btn_header_->SetBkImage(temp_file_path_);
	//nim::TeamInfo tinfo;
	//tinfo.SetTeamID(id);
	//tinfo.SetIcon(url);
	//nim::Team::UpdateTeamInfoAsync(tid_, tinfo, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
}

bool TeamInfoForm::OnInviteUesrBtnClick(ui::EventArgs *param)
{
	std::string wnd_id;
	if (create_or_display_)
	{
		switch (type_)
		{
		case nim::kNIMTeamTypeNormal:
			wnd_id = ContactSelectForm::kCreateGroup;
			break;
		case nim::kNIMTeamTypeAdvanced:
			wnd_id = ContactSelectForm::kCreateTeam;
			break;
		default:
			break;
		}
	}
	else
		wnd_id = tid_;

	if (wnd_id.empty())
		return false;

	ContactSelectForm *contact_select_form = (ContactSelectForm *)WindowsManager::GetInstance()->GetWindow\
		(ContactSelectForm::kClassName, nbase::UTF8ToUTF16(wnd_id));

	if(!contact_select_form)
	{
		std::list<UTF8String> exnclude_ids;
		for (int i = 0; i < tile_box_->GetCount(); i++)
			exnclude_ids.push_back(tile_box_->GetItemAt(i)->GetUTF8DataID());

		contact_select_form = new ContactSelectForm(wnd_id, exnclude_ids, nbase::Bind(&TeamInfoForm::SelectedCompleted, this, std::placeholders::_1, std::placeholders::_1));
		contact_select_form->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L);
		contact_select_form->CenterWindow();
	}

	contact_select_form->ActiveWindow();

	return true;
}

void TeamInfoForm::SelectedCompleted(const std::list<UTF8String>& friend_list, const std::list<UTF8String>& team_list)
{
	if (create_or_display_) {
		nim::TeamMemberProperty team_member;
		for (auto it = friend_list.begin(); it != friend_list.end(); it++)
		{
			team_member.SetAccountID(*it);
			team_member.SetNick(nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(*it)));
			team_member.SetUserType(nim::kNIMTeamUserTypeLocalWaitAccept);
			tile_box_->Add(CreateTeamMemberListItem(team_member));
		}
	}
	else {
		nim::Team::InviteAsync(tid_, friend_list, "", nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
}

void TeamInfoForm::AddInviteButton()
{
	HBox* box = new HBox();
	GlobalManager::FillBoxWithCache(box, L"team_info/inviteicon.xml");
	tile_box_->AddAt(box, 0);
	invitebtn_ = (Button*)box->FindSubControl(L"InviteBtn");
	invitebtn_->AttachClick(nbase::Bind(&TeamInfoForm::OnInviteUesrBtnClick, this, std::placeholders::_1));
}

void TeamInfoForm::OnGetTeamMembers(const std::string& team_id, int count, const std::list<nim::TeamMemberProperty>& team_member_list)
{
	tile_box_->RemoveAll();
	team_member_list_.clear();

	AddInviteButton();

	for (const auto& member : team_member_list) //创建其他item之前，先需要知道自己的身份
	{
		if (LoginManager::GetInstance()->GetAccount() == member.GetAccountID())
		{
			my_property_ = member;
			auto bits = member.GetBits();
			bool msg_notify = (bits & nim::kNIMTeamBitsConfigMaskMuteNotify) == 0;
			((CheckBox*)FindControl(L"msg_mode_notify"))->Selected(msg_notify);
			((CheckBox*)FindControl(L"msg_mode_notify"))->SetEnabled(true);

			break;
		}
	}

	for (auto it = team_member_list.begin(); it != team_member_list.end(); it++) 
	{
		if (IsTeamMemberType(it->GetUserType()))
		{
			team_member_list_[it->GetAccountID()] = *it;
			tile_box_->Add(CreateTeamMemberListItem(*it));
		}
	}

	UpdateUIByIdentity();
}

ui::HBox* TeamInfoForm::CreateTeamMemberListItem(const nim::TeamMemberProperty& member_info)
{
	ui::HBox* container_element = new HBox();
	GlobalManager::FillBoxWithCache(container_element, L"team_info/start_chat_friend.xml");
	container_element->SetUTF8DataID(member_info.GetAccountID());
	container_element->SetUTF8Name(member_info.GetAccountID());

	nim::NIMTeamUserType user_type = member_info.GetUserType();
	bool is_me = member_info.GetAccountID() == LoginManager::GetInstance()->GetAccount();
	bool has_authority = (my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || (my_property_.GetUserType() == nim::kNIMTeamUserTypeManager && user_type == nim::kNIMTeamUserTypeNomal));
	
	Button* head_image_button = (Button*)container_element->FindSubControl(L"head_image");
	head_image_button->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(member_info.GetAccountID()));
	if (!create_or_display_ && team_info_.GetType() == nim::kNIMTeamTypeAdvanced && (is_me || has_authority))
		head_image_button->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnHeadImageClick, this, member_info.GetAccountID(), std::placeholders::_1));
	else
		head_image_button->SetEnabled(false);

	Label* show_name_label = (Label*)container_element->FindSubControl(L"show_name");
	std::string team_card = member_info.GetNick();
	if (!team_card.empty()) {
		show_name_label->SetUTF8Text(team_card);
	}
	else {
		show_name_label->SetText(UserService::GetInstance()->GetUserName(member_info.GetAccountID()));
	}

	Control* team_admin = container_element->FindSubControl(L"admin");
	if (user_type == nim::kNIMTeamUserTypeCreator)
	{
		team_admin->SetVisible(true);
		team_admin->SetBkImage(L"..\\public\\icon\\team_creator.png");
	}
	else if (user_type == nim::kNIMTeamUserTypeManager)
	{
		team_admin->SetVisible(true);
		team_admin->SetBkImage(L"..\\public\\icon\\team_manager.png");
	}

	if (!is_me && has_authority)
	{
		container_element->AttachBubbledEvent(ui::kEventMouseEnter, nbase::Bind(&TeamInfoForm::OnTeamMemberItemMouseEnter, this, container_element, std::placeholders::_1));
		container_element->AttachBubbledEvent(ui::kEventMouseLeave, nbase::Bind(&TeamInfoForm::OnTeamMemberItemMouseLeave, this, container_element, std::placeholders::_1));
		ui::Button* delete_btn = (ui::Button*)container_element->FindSubControl(L"btn_delete");
		delete_btn->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnDeleteClick, this, container_element, member_info.GetAccountID(), std::placeholders::_1));
	}

	return container_element;
}

bool TeamInfoForm::OnBtnDeleteClick(ui::Box* container_element, const UTF8String& user_id, ui::EventArgs* args)
{
	if (!create_or_display_) {
		auto cb = nbase::Bind(&TeamInfoForm::OnMsgbox, this, user_id, std::placeholders::_1);
		ShowMsgBox(m_hWnd, L"确实要删除该成员吗？", cb, L"提示", L"确定", L"取消");
	}
	else {
		tile_box_->Remove(container_element);
	}

	return true;
}

void TeamInfoForm::OnMsgbox(const UTF8String& user_id, MsgBoxRet ret)
{
	if (ret == MB_YES)
	{
		std::list<std::string> uids_list;
		uids_list.push_back(user_id);
		nim::Team::KickAsync(tid_, uids_list, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
}

bool TeamInfoForm::OnTeamMemberItemMouseEnter(ui::Box* team_member_item, ui::EventArgs* args)
{
	team_member_item->FindSubControl(L"btn_delete")->SetVisible(true);
	return true;
}

bool TeamInfoForm::OnTeamMemberItemMouseLeave(ui::Box* team_member_item, ui::EventArgs* args)
{
	team_member_item->FindSubControl(L"btn_delete")->SetVisible(false);
	return true;
}

bool TeamInfoForm::OnBtnHeadImageClick(const UTF8String& user_id, ui::EventArgs* args)
{
	std::wstring ws_user_id = nbase::UTF8ToUTF16(user_id);
	MemberManagerForm* member_manager_form = (MemberManagerForm*)WindowsManager::GetInstance()->GetWindow\
		(MemberManagerForm::kClassName, ws_user_id);
	if(member_manager_form == NULL)
	{
		bool show_privilege_panel = (my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator && user_id != LoginManager::GetInstance()->GetAccount());
		member_manager_form = new MemberManagerForm(tid_, team_member_list_[user_id], show_privilege_panel);
		member_manager_form->Create(NULL, L"", WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
		member_manager_form->CenterWindow();
		member_manager_form->ShowWindow(true);
	}
	else
	{
		member_manager_form->ActiveWindow();
	}

	return true;
}

bool TeamInfoForm::OnBtnConfirmClick(ui::EventArgs* param)
{
	UTF8String team_name = ((RichEdit*)FindControl(L"team_name"))->GetUTF8Text();
	UTF8String team_intro = ((RichEdit*)FindControl(L"team_intro"))->GetUTF8Text();

	MsgboxCallback cb = [this](MsgBoxRet ret){
		this->ActiveWindow();
	};

	team_name = ui::StringHelper::Trim(team_name);
	if (team_name.empty())
	{

		ShowMsgBox(m_hWnd, L"请设置群名称", ToWeakCallback(cb), L"提示", L"确定", L"");
		((RichEdit*)FindControl(L"team_name"))->SetText(L"");
		return true;
	}

	//高级群不需要一定邀请
	if (type_ == nim::kNIMTeamTypeNormal && tile_box_->GetCount() <= 1)	//tile_box_中包含一个添加按钮
	{
		ShowMsgBox(m_hWnd, L"创建失败，请邀请好友", ToWeakCallback(cb), L"提示", L"确定", L"");
		return true;
	}

	param->pSender->SetEnabled(false);

	nim::TeamInfo tinfo;
	tinfo.SetTeamID(tid_);

	if (team_name != team_info_.GetName())
	{
		tinfo.SetName(team_name);
	}
	if (type_ == nim::kNIMTeamTypeAdvanced) 
	{
		tinfo.SetType(nim::kNIMTeamTypeAdvanced);
		if (team_intro != team_info_.GetIntro())
		{
			tinfo.SetIntro(team_intro.c_str());
		}
		if (!new_header_url_.empty() && team_info_.GetIcon() != new_header_url_)
			tinfo.SetIcon(new_header_url_);

		nim::NIMTeamJoinMode join_mode = nim::kNIMTeamJoinModeNeedAuth;
		if (((Option*)FindControl(L"join_mode_any"))->IsSelected())
			join_mode = nim::kNIMTeamJoinModeNoAuth;
		else if (((Option*)FindControl(L"join_mode_verify"))->IsSelected())
			join_mode = nim::kNIMTeamJoinModeNeedAuth;
		else if (((Option*)FindControl(L"join_mode_nobody"))->IsSelected())
			join_mode = nim::kNIMTeamJoinModeRejectAll;
		if (join_mode != team_info_.GetJoinMode())
			tinfo.SetJoinMode(join_mode);

		nim::NIMTeamBeInviteMode be_invite_mode = nim::kNIMTeamBeInviteModeNeedAgree;
		if (((Option*)FindControl(L"be_invite_mode_agree"))->IsSelected())
			be_invite_mode = nim::kNIMTeamBeInviteModeNeedAgree;
		else if (((Option*)FindControl(L"be_invite_mode_no_need_agree"))->IsSelected())
			be_invite_mode = nim::kNIMTeamBeInviteModeNotNeedAgree;
		if (be_invite_mode != team_info_.GetBeInviteMode())
			tinfo.SetBeInviteMode(be_invite_mode);

		nim::NIMTeamInviteMode invite_mode = nim::kNIMTeamInviteModeManager;
		if (((Option*)FindControl(L"invite_mode_manager"))->IsSelected())
			invite_mode = nim::kNIMTeamInviteModeManager;
		else if (((Option*)FindControl(L"invite_mode_any"))->IsSelected())
			invite_mode = nim::kNIMTeamInviteModeEveryone;
		if (invite_mode != team_info_.GetInviteMode())
			tinfo.SetInviteMode(invite_mode);

		nim::NIMTeamUpdateInfoMode up_info_mode = nim::kNIMTeamUpdateInfoModeManager;
		if (((Option*)FindControl(L"up_tinfo_mode_manager"))->IsSelected())
			up_info_mode = nim::kNIMTeamUpdateInfoModeManager;
		else if (((Option*)FindControl(L"up_tinfo_mode_any"))->IsSelected())
			up_info_mode = nim::kNIMTeamUpdateInfoModeEveryone;
		if (up_info_mode != team_info_.GetUpdateInfoMode())
			tinfo.SetUpdateInfoMode(up_info_mode);

		nim::NIMTeamUpdateCustomMode up_custom_mode = nim::kNIMTeamUpdateCustomModeManager;
		if (((Option*)FindControl(L"up_custom_mode_manager"))->IsSelected())
			up_custom_mode = nim::kNIMTeamUpdateCustomModeManager;
		else if (((Option*)FindControl(L"up_custom_mode_any"))->IsSelected())
			up_custom_mode = nim::kNIMTeamUpdateCustomModeEveryone;
		if (up_custom_mode != team_info_.GetUpdateCustomMode())
			tinfo.SetUpdateCustomMode(up_custom_mode);
	}
	else {
		tinfo.SetType(nim::kNIMTeamTypeNormal);
	}

	if (create_or_display_)
	{
		std::list<UTF8String> id_list;
		for (int i = 1; i < tile_box_->GetCount(); i++)
		{
			id_list.push_back(tile_box_->GetItemAt(i)->GetUTF8DataID());
		}

		if (!tinfo.GetIcon().empty())
			PhotoService::GetInstance()->DownloadTeamIcon(tinfo);

		nim::Team::CreateTeamAsync(tinfo, id_list, "", nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
	else {

		nim::Team::UpdateTeamInfoAsync(tid_, tinfo, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
		if (((CheckBox*)FindControl(L"msg_mode_notify"))->IsEnabled())
		{
			auto my_member_info = team_member_list_.at(LoginManager::GetInstance()->GetAccount());
			long long bits = my_member_info.GetBits();
			bool msg_notify = (bits & nim::kNIMTeamBitsConfigMaskMuteNotify) == 0;
			if (msg_notify != ((Option*)FindControl(L"msg_mode_notify"))->IsSelected())
			{
				nim::TeamMemberProperty values(tid_, LoginManager::GetInstance()->GetAccount(), my_member_info.GetUserType());
				if (msg_notify)
				{
					bits |= nim::kNIMTeamBitsConfigMaskMuteNotify;
				}
				else
				{
					bits &= ~nim::kNIMTeamBitsConfigMaskMuteNotify;
				}
				values.SetBits(bits);
				nim::Team::UpdateMyPropertyAsync(values, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
			}
		}
	}

	Close();
	return true; 
}

bool TeamInfoForm::OnBtnCancelClick(ui::EventArgs* param)
{
	Close();
	return true;
}

bool TeamInfoForm::OnBtnDissmissClick(ui::EventArgs* param)
{
	nim::Team::DismissAsync(tid_, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	Close();
	return true;
}

bool TeamInfoForm::OnBtnQuitClick(ui::EventArgs* param)
{
	nim::Team::LeaveAsync(tid_, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	Close();
	return true;
}

void TeamInfoForm::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
{
	if (!IsTeamMemberType(team_member_info.GetUserType()))
		return;

	team_member_list_[team_member_info.GetAccountID()] = team_member_info;

	HBox* hbox = (HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(team_member_info.GetAccountID()));
	if (hbox)
		tile_box_->Remove(hbox);
	tile_box_->Add(CreateTeamMemberListItem(team_member_info));
}

void TeamInfoForm::OnTeamMemberRemove(const std::string& tid, const std::string& uid)
{
	if(tid == tid_)
	{
		Control* item = tile_box_->FindSubControl(nbase::UTF8ToUTF16(uid));
		if(item)
			tile_box_->Remove(item);

		team_member_list_.erase(uid);
	}
}

void TeamInfoForm::OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card)
{
	size_t splitter = tid_uid.find_first_of('#');
	std::string tid = tid_uid.substr(0, splitter), uid = tid_uid.substr(splitter + 1);
	if(tid == tid_)
	{
		auto iter = team_member_list_.find(uid);
		if (iter != team_member_list_.end())
			iter->second.SetNick(team_card);

		ui::HBox* member_item = (ui::HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(uid));
		if (member_item)
		{
			Button* head_image_button = (Button*)member_item->FindSubControl(L"head_image");
			head_image_button->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(uid));

			Label* show_name_label = (Label*)member_item->FindSubControl(L"show_name");
			if (!team_card.empty())
				show_name_label->SetUTF8Text(team_card);
			else
				show_name_label->SetText(UserService::GetInstance()->GetUserName(uid));
		}
	}
}

void TeamInfoForm::OnTeamAdminSet(const std::string& tid, const std::string& uid, bool admin)
{
	if (tid == tid_)
	{
		if(uid == LoginManager::GetInstance()->GetAccount())
			nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		else
		{
			team_member_list_.at(uid).SetUserType(admin ? nim::kNIMTeamUserTypeManager : nim::kNIMTeamUserTypeNomal);
			ui::HBox* item = (ui::HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(uid));
			if (item)
			{
				int index = tile_box_->GetItemIndex(item);
				tile_box_->Remove(item);
				tile_box_->AddAt(CreateTeamMemberListItem(team_member_list_.at(uid)), index);
			}
		}
	}
}

void TeamInfoForm::OnTeamOwnerChange(const std::string& tid, const std::string& uid)
{
	if (tid == tid_)
	{
		if (uid == LoginManager::GetInstance()->GetAccount() || my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator)
			nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		else
		{
			team_member_list_.at(uid).SetUserType(nim::kNIMTeamUserTypeCreator);
			ui::HBox* item = (ui::HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(uid));
			if (item)
			{
				int index = tile_box_->GetItemIndex(item);
				tile_box_->Remove(item);
				tile_box_->AddAt(CreateTeamMemberListItem(team_member_list_.at(uid)), index);
			}
		}
	}
}

void TeamInfoForm::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	{
		HBox* item = (HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(iter->GetAccId()));
		if(item == NULL) continue;
		if(iter->ExistValue(nim::kUserNameCardKeyIconUrl))
			item->FindSubControl(L"head_image")->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(iter->GetAccId()));
		
		// 如果没有设置群昵称，则显示用户的备注名或昵称。如果设置了群昵称，就不变。
		Label* show_name_label = (Label*)item->FindSubControl(L"show_name");
		if (iter->ExistValue(nim::kUserNameCardKeyName) && team_member_list_.at(iter->GetAccId()).GetNick().empty())
			show_name_label->SetText(UserService::GetInstance()->GetUserName(iter->GetAccId()));
	}
}

void TeamInfoForm::OnUserPhotoReady(PhotoType type, const std::string & accid, const std::wstring & photo_path)
{
	if (type == kUser)
	{
		HBox* item = (HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item != NULL)
			item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
	else if (type == kTeam)
	{
		if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
			btn_header_->SetBkImage(photo_path);
	}
}

void TeamInfoForm::OnTeamRemove(const std::string & tid)
{
	if (tid == tid_)
		Close();
}

void TeamInfoForm::UpdateTeamMember()
{
	nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TeamInfoForm::UpdateUIByIdentity()
{
	FindControl(L"team_owner_section")->SetEnabled((my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || my_property_.GetUserType() == nim::kNIMTeamUserTypeManager));
	if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
	{
		//invitebtn_->SetEnabled((!team_info_.readonly) && (user_type_ == nim::kNIMTeamUserTypeCreator || user_type_ == nim::kNIMTeamUserTypeManager));
		if (team_info_.GetInviteMode() == nim::kNIMTeamInviteModeEveryone)
			invitebtn_->SetEnabled(true);
		else
			invitebtn_->SetEnabled(my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || my_property_.GetUserType() == nim::kNIMTeamUserTypeManager);

		if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateInfoModeEveryone)
		{
			FindControl(L"team_info_icon")->SetEnabled(true);
			FindControl(L"team_name_panel")->SetEnabled(true);
			FindControl(L"team_info_panel")->SetEnabled(true);
		}
	}
	if (team_info_.GetType() == nim::kNIMTeamTypeNormal)
	{
		FindControl(L"team_name_panel")->SetEnabled(true);
	}

	if (type_ == nim::kNIMTeamTypeNormal || my_property_.GetUserType() != nim::kNIMTeamUserTypeCreator) 
	{
		btn_dismiss_->SetVisible(false);
		btn_quit_->SetVisible(true);
	}
	else 
	{
		btn_dismiss_->SetVisible(true);
		btn_quit_->SetVisible(false);
	}
}

bool TeamInfoForm::IsTeamMemberType(const nim::NIMTeamUserType user_type)
{
	if (user_type == nim::kNIMTeamUserTypeNomal ||
		user_type == nim::kNIMTeamUserTypeManager ||
		user_type == nim::kNIMTeamUserTypeCreator)
		return true;

	return false;
}


void DeleteFileCallback(const std::wstring& tmp_head_image_path)
{
	if (nbase::FilePathIsExist(tmp_head_image_path, false))
	{
		nbase::DeleteFile(tmp_head_image_path); // 删除临时文件
	}
}

LRESULT TeamInfoForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	if (!temp_file_path_.empty())
	{
		StdClosure closure = nbase::Bind(&DeleteFileCallback, temp_file_path_);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, closure);
	}
	return __super::OnClose(u, w, l, bHandled);
}

}