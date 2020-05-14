#include "stdafx.h"
#include "team_info_box.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "callback/team/team_callback.h"
#include "shared/tool.h"
#include "gui/contact_select_form/contact_select_form.h"
#include "module/login/login_manager.h"
#include "module/session/session_manager.h"
#include "gui/team_info/member_manager.h"
#include "gui/profile_form/head_modify_form.h"
#include "gui/team_info/team_info.h"
using namespace nim_comp;
const std::vector<std::wstring> TeamInfoBox::options_name_list_ = {
	L"join_mode_any",					L"join_mode_verify",				L"join_mode_nobody",
	L"be_invite_mode_agree",			L"be_invite_mode_no_need_agree",
	L"invite_mode_manager",				L"invite_mode_any",
	L"up_tinfo_mode_manager",			L"up_tinfo_mode_any",
	L"up_custom_mode_manager",			L"up_custom_mode_any",
	L"mute_mode_allow_manager",			L"mute_mode_allow_any",
	L"notify_mode_on",					L"notify_mode_admin",				L"notify_mode_off"

};
TeamInfoBox::TeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info, bool value)
{
	create_or_display_ = create_or_display;
	type_ = type;
	tid_ = team_id;
	team_info_ = team_info;
	view_mode_ = value;
}

TeamInfoBox::~TeamInfoBox()
{

}
void TeamInfoBox::DoInit()
{
	QLOG_APP(L"TeamInfoBox::DoInit enter.");

    this->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&TeamInfoBox::OnClicked, this, std::placeholders::_1));

	unregister_cb.Add(TeamService::GetInstance()->RegAddTeamMember(nbase::Bind(&TeamInfoBox::OnTeamMemberAdd, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeamMember(nbase::Bind(&TeamInfoBox::OnTeamMemberRemove, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMember(nbase::Bind(&TeamInfoBox::OnTeamMemberChange, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegSetTeamAdmin(nbase::Bind(&TeamInfoBox::OnTeamAdminSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamOwner(nbase::Bind(&TeamInfoBox::OnTeamOwnerChange, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&TeamInfoBox::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&TeamInfoBox::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeam(nbase::Bind(&TeamInfoBox::OnTeamRemove, this, std::placeholders::_1)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&TeamInfoBox::OnTeamNameChange, this, std::placeholders::_1)));
    unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMute(nbase::Bind(&TeamInfoBox::OnTeamMuteChange, this, std::placeholders::_1, std::placeholders::_2)));
    unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamNotification(nbase::Bind(&TeamInfoBox::OnTeamNotificationModeChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));

	QLOG_APP(L"TeamInfoBox::DoInit MutiLanSupport* mls.");

	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	
	ui::Label* team_id_label = (ui::Label*)FindSubControl(L"team_id_label");
	ui::Label* team_name_label = (ui::Label*)FindSubControl(L"team_name_label");
	if (type_ == nim::kNIMTeamTypeNormal)
	{
		team_id_label->SetText(mls->GetStringViaID(L"STRID_TEAM_INFO_GROUP_ID"));
		team_name_label->SetText(mls->GetStringViaID(L"STRID_TEAM_INFO_GROUP_NAME"));
	}
	else if (type_ == nim::kNIMTeamTypeAdvanced)
	{
		team_id_label->SetText(mls->GetStringViaID(L"STRID_TEAM_INFO_TEAM_ID"));
		team_name_label->SetText(mls->GetStringViaID(L"STRID_TEAM_INFO_TEAM_NAME"));
	}

	tile_box_ = (ui::VirtualTileBox*)FindSubControl(L"user_list");
	tile_box_->SetDataProvider(this);	
	ui::Button* btn_confirm = (ui::Button*)FindSubControl(L"confirm");
	btn_confirm->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnConfirmClick, this, std::placeholders::_1));
	ui::Button* btn_cancel = (ui::Button*)FindSubControl(L"cancel");
	btn_cancel->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnCancelClick, this, std::placeholders::_1));
	btn_quit_ = (ui::Button*)FindSubControl(L"quit");
	if (type_ == nim::kNIMTeamTypeNormal)
		btn_quit_->SetText(mls->GetStringViaID(L"STRID_TEAM_INFO_QUIT_GROUP"));
	else if (type_ == nim::kNIMTeamTypeAdvanced)
		btn_quit_->SetText(mls->GetStringViaID(L"STRID_TEAM_INFO_QUIT_TEAM"));
	btn_quit_->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnQuitClick, this, std::placeholders::_1));
	btn_dismiss_ = (ui::Button*)FindSubControl(L"dismiss");
	btn_dismiss_->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnDissmissClick, this, std::placeholders::_1));

	btn_header_ = (ui::Button*)FindSubControl(L"head_image");

	QLOG_APP(L"TeamInfoBox::DoInit create_or_display_ = {0}") << create_or_display_;

	if (!create_or_display_)
	{
		nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoBox::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else
    {
		//默认开启消息通知 oleg 20161025
		((ui::Option*)FindSubControl(L"notify_mode_on"))->Selected(true);
		AddInviteButton();
		btn_quit_->SetVisible(false);
		btn_dismiss_->SetVisible(false);
        FindSubControl(L"team_mute_fields")->SetEnabled(false);
        FindSubControl(L"team_notify_fields")->SetEnabled(false);
	}
	re_team_name_ = (ui::RichEdit*)FindSubControl(L"team_name");
	re_team_name_->SetLimitText(50);
	re_team_intro_ = (ui::RichEdit*)FindSubControl(L"team_intro");
	re_team_intro_->SetLimitText(250);

	QLOG_APP(L"TeamInfoBox::DoInit RefreshInfo = {0}") << nbase::UTF8ToUTF16(team_info_.ToJsonString());
	RefreshInfo(&team_info_);

	if (type_ == nim::kNIMTeamTypeNormal)
	{
		FindSubControl(L"team_info_fields")->SetVisible(false);
		FindSubControl(L"team_info_icon")->SetVisible(false);
		FindSubControl(L"team_notify_fields")->SetVisible(false);
	}
    else
    {
        btn_header_->AttachClick(nbase::Bind(&TeamInfoBox::OnHeadImageClicked, this, std::placeholders::_1));
    }

    if (!view_mode_)
    {
        FindSubControl(L"option_bar")->SetVisible(true);
    }

	auto edit_button = dynamic_cast<ui::ButtonBox*>(FindSubControl(L"edit"));
    if (edit_button != nullptr)
    {
        edit_button->AttachClick([this](ui::EventArgs* param) {
            TeamInfoForm::ShowTeamInfoForm(create_or_display_, type_, tid_, team_info_);
            return true;
        });
    }

	auto start_chat = dynamic_cast<ui::Button*>(FindSubControl(L"start_chat"));
    if (start_chat != nullptr)
    {
        start_chat->AttachClick([this](ui::EventArgs* param) {
            nim_comp::SessionManager::GetInstance()->OpenSessionBox(tid_, nim::NIMSessionType::kNIMSessionTypeTeam);
            return true;
        });
    }

	for (auto it : options_name_list_)
	{
		auto option = dynamic_cast<ui::Option*>(FindSubControl(it));
		if (option != nullptr)
		{
			option->SetGroup(option->GetGroup().append(nbase::UTF8ToUTF16(team_info_.GetTeamID())).append(view_mode_?L"true":L"false"));
		}
	}
}

bool TeamInfoBox::IsCreateOrDisplay() const
{
	return create_or_display_;
}

std::wstring TeamInfoBox::GetTeamID() const
{
	return nbase::UTF8ToUTF16(tid_);
}

nim::NIMTeamType TeamInfoBox::GetTeamType() const
{
	return type_;
}

bool TeamInfoBox::OnHeadImageClicked(ui::EventArgs* args)
{
	temp_file_path_ = PhotoService::GetInstance()->GetPhotoDir(kTeam);
	std::wstring temp_file_name;
	nbase::Time::TimeStruct now = nbase::Time::Now().ToTimeStruct(true);
	nbase::StringPrintf(temp_file_name, L"temp_head_icon_%d%d%d%d%d%d",
		now.year(), now.month(), now.day_of_month(), now.hour(), now.minute(), now.second());
	temp_file_path_.append(temp_file_name);

	HeadModifyForm* form = (HeadModifyForm*)WindowsManager::GetInstance()->GetWindow(HeadModifyForm::kClassName, nbase::UTF8ToUTF16(tid_));
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
	form->RegCompleteCallback(nbase::Bind(&TeamInfoBox::OnModifyHeaderComplete, this, std::placeholders::_1, std::placeholders::_2));

	return true;
}

void TeamInfoBox::OnModifyHeaderComplete(const std::string& id, const std::string &url)
{
	new_header_url_ = url;
	if (btn_header_)
		btn_header_->SetBkImage(temp_file_path_);
	//nim::TeamInfo tinfo;
	//tinfo.SetTeamID(id);
	//tinfo.SetIcon(url);
	//nim::Team::UpdateTeamInfoAsync(tid_, tinfo, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
}

bool TeamInfoBox::OnInviteUesrBtnClick(ui::EventArgs *param)
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

	if (!contact_select_form)
	{
		std::list<UTF8String> exnclude_ids;
		for (int i = 0; i < tile_box_->GetCount(); i++)
			exnclude_ids.push_back(tile_box_->GetItemAt(i)->GetUTF8DataID());

		contact_select_form = new ContactSelectForm(wnd_id, exnclude_ids, nbase::Bind(&TeamInfoBox::SelectedCompleted, this, std::placeholders::_1, std::placeholders::_2, create_or_display_));
		contact_select_form->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L);
		contact_select_form->CenterWindow();
	}

	contact_select_form->ActiveWindow();

	return true;
}

void TeamInfoBox::SelectedCompleted(const std::list<UTF8String>& friend_list, const std::list<UTF8String>& team_list, bool delete_enable)
{
	if (create_or_display_) {
		nim::TeamMemberProperty team_member;
		for (auto it = friend_list.begin(); it != friend_list.end(); it++)
		{
			team_member.SetAccountID(*it);
			team_member.SetNick(nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(*it)));
			team_member.SetUserType(nim::kNIMTeamUserTypeLocalWaitAccept);
			tile_box_->Add(CreateTeamMemberListItem(team_member, delete_enable));
		}
	}
	else {
		nim::Team::InviteAsync2(tid_, friend_list, "This is invitation_postscript","This is invitation_attachment", std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
}

void TeamInfoBox::AddInviteButton()
{
	
	ui::HBox* box = new ui::HBox();
	ui::GlobalManager::FillBoxWithCache(box, L"team_info/inviteicon.xml");	
	invitebtn_ = (ui::Button*)box->FindSubControl(L"InviteBtn");
	invitebtn_->AttachClick(nbase::Bind(&TeamInfoBox::OnInviteUesrBtnClick, this, std::placeholders::_1));
	if (!view_mode_)
	{
		ui::Box* user_list_container = (ui::Box*)FindSubControl(L"user_list_container");
		invitebtn_->SetVerAlignType(ui::VerAlignType::kVerAlignBottom);
		invitebtn_->SetHorAlignType(ui::HorAlignType::kHorAlignRight);
		user_list_container->Add(invitebtn_);
	}	
}

void TeamInfoBox::OnGetTeamMembers(const std::string& team_id, int count, const std::list<nim::TeamMemberProperty>& team_member_list)
{
	tile_box_->RemoveAll();
	team_member_list_.clear();
	team_member_sort_list_.clear();
	team_member_sort_list_.reserve(team_member_list.size());
	for (auto it : team_member_list)
	{
		team_member_list_.insert(std::make_pair(it.GetAccountID(),std::make_shared<nim::TeamMemberProperty>(it)));
		team_member_sort_list_.emplace_back(team_member_list_[it.GetAccountID()]);
	}
	auto member = team_member_list_[LoginManager::GetInstance()->GetAccount()];
	{
		((ui::Option*)FindSubControl(L"notify_mode_off"))->SetEnabled(true);
		((ui::Option*)FindSubControl(L"notify_mode_admin"))->SetEnabled(true);
		((ui::Option*)FindSubControl(L"notify_mode_on"))->SetEnabled(true);

		my_property_ = *member;
		auto bits = member->GetBits();
		if ((bits & nim::kNIMTeamBitsConfigMaskMuteNotify) == nim::kNIMTeamBitsConfigMaskMuteNotify)
		{
			((ui::Option*)FindSubControl(L"notify_mode_off"))->Selected(true);
		}
		else if ((bits & nim::kNIMTeamBitsConfigMaskOnlyAdmin) == nim::kNIMTeamBitsConfigMaskOnlyAdmin)
		{
			((ui::Option*)FindSubControl(L"notify_mode_admin"))->Selected(true);
		}
		else
		{
			((ui::Option*)FindSubControl(L"notify_mode_on"))->Selected(true);
		}
	}
	AddInviteButton();//cqu227hk
	UpdateUIByIdentity();
	RefreshMemberList(true);
}

ui::HBox* TeamInfoBox::CreateTeamMemberListItem(const nim::TeamMemberProperty& member_info, bool delelte_enable/* = false*/)
{
	ui::HBox* container_element = new ui::HBox();
	ui::GlobalManager::FillBoxWithCache(container_element, L"team_info/start_chat_friend.xml");
	container_element->SetUTF8DataID(member_info.GetAccountID());
	container_element->SetUTF8Name(member_info.GetAccountID());

	nim::NIMTeamUserType user_type = member_info.GetUserType();
	bool is_me = member_info.GetAccountID() == LoginManager::GetInstance()->GetAccount();
	bool has_authority = (my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || (my_property_.GetUserType() == nim::kNIMTeamUserTypeManager && user_type == nim::kNIMTeamUserTypeNomal));

	ui::Button* head_image_button = (ui::Button*)container_element->FindSubControl(L"head_image");
	head_image_button->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(member_info.GetAccountID()));
	if (!create_or_display_ && team_info_.GetType() == nim::kNIMTeamTypeAdvanced && (is_me || has_authority))
		head_image_button->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnHeadImageClick, this, member_info.GetAccountID(), std::placeholders::_1));
	else
		head_image_button->SetEnabled(false);

	ui::Label* show_name_label = (ui::Label*)container_element->FindSubControl(L"show_name");
	std::string team_card = member_info.GetNick();
	if (!team_card.empty()) {
		show_name_label->SetUTF8Text(team_card);
	}
	else {
		show_name_label->SetText(UserService::GetInstance()->GetUserName(member_info.GetAccountID()));
	}

	ui::Control* team_admin = container_element->FindSubControl(L"admin");
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

	if (!is_me && (delelte_enable || has_authority))
	{
		container_element->AttachBubbledEvent(ui::kEventMouseEnter, nbase::Bind(&TeamInfoBox::OnTeamMemberItemMouseEnter, this, container_element, std::placeholders::_1));
		container_element->AttachBubbledEvent(ui::kEventMouseLeave, nbase::Bind(&TeamInfoBox::OnTeamMemberItemMouseLeave, this, container_element, std::placeholders::_1));
		ui::Button* delete_btn = (ui::Button*)container_element->FindSubControl(L"btn_delete");
		delete_btn->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnDeleteClick, this, container_element, member_info.GetAccountID(), std::placeholders::_1));
	}

	return container_element;
}

bool TeamInfoBox::OnBtnDeleteClick(ui::Box* container_element, const UTF8String& user_id, ui::EventArgs* args)
{
	if (!create_or_display_) {
		auto cb = nbase::Bind(&TeamInfoBox::OnMsgbox, this, user_id, std::placeholders::_1);
		ShowMsgBox(GetWindow()->GetHWND(), cb, L"STRID_TEAM_INFO_DELETE_MEMBER_TIP", true, L"STRING_TIPS", true, L"STRING_OK", true, L"STRING_NO", true);
	}
	else {
		tile_box_->Remove(container_element);
	}

	return true;
}

void TeamInfoBox::OnMsgbox(const UTF8String& user_id, MsgBoxRet ret)
{
	if (ret == MB_YES)
	{
		std::list<std::string> uids_list;
		uids_list.push_back(user_id);
		nim::Team::KickAsync(tid_, uids_list, std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
		MemberManagerForm* member_manager_form = (MemberManagerForm*)WindowsManager::GetInstance()->GetWindow\
			(MemberManagerForm::kClassName, nbase::UTF8ToUTF16(user_id));
		if (member_manager_form)
			member_manager_form->Close();
	}
}

bool TeamInfoBox::OnTeamMemberItemMouseEnter(ui::Box* team_member_item, ui::EventArgs* args)
{
	team_member_item->FindSubControl(L"btn_delete")->SetVisible(true);
	return true;
}

bool TeamInfoBox::OnTeamMemberItemMouseLeave(ui::Box* team_member_item, ui::EventArgs* args)
{
	team_member_item->FindSubControl(L"btn_delete")->SetVisible(false);
	return true;
}

bool TeamInfoBox::OnBtnHeadImageClick(const UTF8String& user_id, ui::EventArgs* args)
{
	std::wstring ws_user_id = nbase::UTF8ToUTF16(user_id);
	MemberManagerForm* member_manager_form = (MemberManagerForm*)WindowsManager::GetInstance()->GetWindow\
		(MemberManagerForm::kClassName, ws_user_id);
	if (member_manager_form == NULL)
	{
		bool show_privilege_panel = (my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator && user_id != LoginManager::GetInstance()->GetAccount());
		member_manager_form = new MemberManagerForm(tid_, *(team_member_list_[user_id]), show_privilege_panel);
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

bool TeamInfoBox::OnBtnConfirmClick(ui::EventArgs* param)
{
    saving_settings_ = true;

	UTF8String team_name = ((ui::RichEdit*)FindSubControl(L"team_name"))->GetUTF8Text();
	UTF8String team_intro = ((ui::RichEdit*)FindSubControl(L"team_intro"))->GetUTF8Text();

	MsgboxCallback cb = [this](MsgBoxRet ret){
		this->ActiveWindow();
	};

	team_name = ui::StringHelper::Trim(team_name);
	if (team_name.empty())
	{

		ShowMsgBox(GetWindow()->GetHWND(), ToWeakCallback(cb), L"STRID_TEAM_INFO_SHOULD_SET_TEAM_NAME");
		((ui::RichEdit*)FindSubControl(L"team_name"))->SetText(L"");
		return true;
	}

	//高级群不需要一定邀请
	if (type_ == nim::kNIMTeamTypeNormal && tile_box_->GetCount() <= 1)	//tile_box_中包含一个添加按钮
	{
		ShowMsgBox(GetWindow()->GetHWND(), ToWeakCallback(cb), L"STRID_TEAM_INFO_PLEASE_INVITE_FRIEND");
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
		if (((ui::Option*)FindSubControl(L"join_mode_any"))->IsSelected())
			join_mode = nim::kNIMTeamJoinModeNoAuth;
		else if (((ui::Option*)FindSubControl(L"join_mode_verify"))->IsSelected())
			join_mode = nim::kNIMTeamJoinModeNeedAuth;
		else if (((ui::Option*)FindSubControl(L"join_mode_nobody"))->IsSelected())
			join_mode = nim::kNIMTeamJoinModeRejectAll;
		if (join_mode != team_info_.GetJoinMode())
			tinfo.SetJoinMode(join_mode);

		nim::NIMTeamBeInviteMode be_invite_mode = nim::kNIMTeamBeInviteModeNeedAgree;
		if (((ui::Option*)FindSubControl(L"be_invite_mode_agree"))->IsSelected())
			be_invite_mode = nim::kNIMTeamBeInviteModeNeedAgree;
		else if (((ui::Option*)FindSubControl(L"be_invite_mode_no_need_agree"))->IsSelected())
			be_invite_mode = nim::kNIMTeamBeInviteModeNotNeedAgree;
		if (be_invite_mode != team_info_.GetBeInviteMode())
			tinfo.SetBeInviteMode(be_invite_mode);

		nim::NIMTeamInviteMode invite_mode = nim::kNIMTeamInviteModeManager;
		if (((ui::Option*)FindSubControl(L"invite_mode_manager"))->IsSelected())
			invite_mode = nim::kNIMTeamInviteModeManager;
		else if (((ui::Option*)FindSubControl(L"invite_mode_any"))->IsSelected())
			invite_mode = nim::kNIMTeamInviteModeEveryone;
		if (invite_mode != team_info_.GetInviteMode())
			tinfo.SetInviteMode(invite_mode);

		nim::NIMTeamUpdateInfoMode up_info_mode = nim::kNIMTeamUpdateInfoModeManager;
		if (((ui::Option*)FindSubControl(L"up_tinfo_mode_manager"))->IsSelected())
			up_info_mode = nim::kNIMTeamUpdateInfoModeManager;
		else if (((ui::Option*)FindSubControl(L"up_tinfo_mode_any"))->IsSelected())
			up_info_mode = nim::kNIMTeamUpdateInfoModeEveryone;
		if (up_info_mode != team_info_.GetUpdateInfoMode())
			tinfo.SetUpdateInfoMode(up_info_mode);

		nim::NIMTeamUpdateCustomMode up_custom_mode = nim::kNIMTeamUpdateCustomModeManager;
		if (((ui::Option*)FindSubControl(L"up_custom_mode_manager"))->IsSelected())
			up_custom_mode = nim::kNIMTeamUpdateCustomModeManager;
		else if (((ui::Option*)FindSubControl(L"up_custom_mode_any"))->IsSelected())
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
		nim::Team::CreateTeamAsync(tinfo, id_list, "", std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
	else
    {
        // 群消息通知方式
        if (((ui::Option*)FindSubControl(L"notify_mode_on"))->IsEnabled())
        {
            auto my_member_info = team_member_list_.at(LoginManager::GetInstance()->GetAccount());
            long long bits = my_member_info->GetBits();
            long long new_bits = 0;
            if (((ui::Option*)FindSubControl(L"notify_mode_on"))->IsSelected())
                new_bits &= ~nim::kNIMTeamBitsConfigMaskMuteNotify;
            else if (((ui::Option*)FindSubControl(L"notify_mode_off"))->IsSelected())
                new_bits |= nim::kNIMTeamBitsConfigMaskMuteNotify;
            else
                new_bits |= nim::kNIMTeamBitsConfigMaskOnlyAdmin;
            if (new_bits != bits)
            {
                nim::TeamMemberProperty values(tid_, LoginManager::GetInstance()->GetAccount(), my_member_info->GetUserType());
                values.SetBits(new_bits);
                nim::Team::UpdateMyPropertyAsync(values, std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
            }
        }

        if (FindSubControl(L"team_owner_section")->IsEnabled() || FindSubControl(L"team_name_panel")->IsEnabled())
        {
            // 群组禁言
            nim::NIMTeamMuteType mute_mode = nim::kNIMTeamMuteTypeNone;
            if (((ui::Option*)FindSubControl(L"mute_mode_allow_manager"))->IsSelected())
                mute_mode = nim::kNIMTeamMuteTypeNomalMute;
            else if (((ui::Option*)FindSubControl(L"mute_mode_allow_any"))->IsSelected())
                mute_mode = nim::kNIMTeamMuteTypeNone;
            if (mute_mode != team_info_.GetMuteType())
            {
                nim::Team::MuteAsync(tid_, mute_mode != nim::kNIMTeamMuteTypeNone, std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1), "");
            }

            // 更新群组基本信息
			nim::Team::UpdateTeamInfoAsync(tid_, tinfo, std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
		}
	}

	Close();
	return true;
}

bool TeamInfoBox::OnBtnCancelClick(ui::EventArgs* param)
{
	Close();
	return true;
}

bool TeamInfoBox::OnBtnDissmissClick(ui::EventArgs* param)
{
	nim::Team::DismissAsync(tid_, std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	Close();
	return true;
}

bool TeamInfoBox::OnBtnQuitClick(ui::EventArgs* param)
{
	nim::Team::LeaveAsync(tid_, std::bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	Close();
	return true;
}

void TeamInfoBox::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
{
	if (team_info_.GetTeamID().compare(tid) != 0)
		return;
	if (!IsTeamMemberType(team_member_info.GetUserType()))
		return;
	AddTeamMemberData(team_member_info);
	RefreshMemberList(true);	
}

void TeamInfoBox::OnTeamMemberRemove(const std::string& tid, const std::string& uid)
{
	if (tid == tid_)
	{
		RemoveTeamMemberData(uid);
		RefreshMemberList(false);
	}
}

void TeamInfoBox::OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card)
{
	size_t splitter = tid_uid.find_first_of('#');
	std::string tid = tid_uid.substr(0, splitter), uid = tid_uid.substr(splitter + 1);
	if (tid == tid_)
	{
		auto iter = team_member_list_.find(uid);
		if (iter != team_member_list_.end())
			iter->second->SetNick(team_card);

		ui::HBox* member_item = (ui::HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(uid));
		if (member_item)
		{
			ui::Button* head_image_button = (ui::Button*)member_item->FindSubControl(L"head_image");
			head_image_button->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(uid));

			ui::Label* show_name_label = (ui::Label*)member_item->FindSubControl(L"show_name");
			if (!team_card.empty())
				show_name_label->SetUTF8Text(team_card);
			else
				show_name_label->SetText(UserService::GetInstance()->GetUserName(uid));
		}
	}
}

void TeamInfoBox::OnTeamAdminSet(const std::string& tid, const std::string& uid, bool admin)
{
	if (tid == tid_)
	{
		SetTeamMemberDataAdmin(uid, admin);
		RefreshMemberList(true);
		if (uid == LoginManager::GetInstance()->GetAccount())
		{
			UpdateUIByIdentity();
		}
		
	}
}

void TeamInfoBox::OnTeamOwnerChange(const std::string& tid, const std::string& uid)
{
	if (tid == tid_)
	{
		if (uid == LoginManager::GetInstance()->GetAccount() || my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator)
			nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoBox::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		else
		{
			team_member_list_.at(uid)->SetUserType(nim::kNIMTeamUserTypeCreator);			
		}
		RefreshMemberList();
		if (uid == LoginManager::GetInstance()->GetAccount())
		{
			UpdateUIByIdentity();
		}
	}
}

void TeamInfoBox::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	{
		ui::HBox* item = (ui::HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(iter->GetAccId()));
		if (item)
		{
			if (iter->ExistValue(nim::kUserNameCardKeyIconUrl))
				item->FindSubControl(L"head_image")->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(iter->GetAccId()));

			// 如果没有设置群昵称，则显示用户的备注名或昵称。如果设置了群昵称，就不变。
			ui::Label* show_name_label = (ui::Label*)item->FindSubControl(L"show_name");
			if (iter->ExistValue(nim::kUserNameCardKeyName) && team_member_list_.at(iter->GetAccId())->GetNick().empty())
				show_name_label->SetText(UserService::GetInstance()->GetUserName(iter->GetAccId()));
		}
	}
}

void TeamInfoBox::OnUserPhotoReady(PhotoType type, const std::string & tid, const std::wstring & photo_path)
{
	if (tid != tid_)
		return;

	if (type == kUser)
	{
		ui::HBox* item = (ui::HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(tid));
		if (item != NULL)
			item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
	else if (type == kTeam)
	{
		if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
			btn_header_->SetBkImage(photo_path);
	}
}

void TeamInfoBox::OnTeamRemove(const std::string & tid)
{
	if (tid == tid_)
		Close();
}

void TeamInfoBox::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (team_info.GetTeamID() == tid_)
	{
		auto&& team_info = nim::Team::QueryTeamInfoBlock(tid_);
		RefreshInfo(&team_info);
	}
}

void TeamInfoBox::UpdateTeamMember()
{
	nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoBox::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

ui::Control* nim_comp::TeamInfoBox::CreateElement()
{
	ui::HBox* container_element = new ui::HBox();
	ui::GlobalManager::FillBoxWithCache(container_element, L"team_info/start_chat_friend.xml");
	return container_element;
}

void nim_comp::TeamInfoBox::FillElement(ui::Control* control, int index)
{
	auto member_info = team_member_sort_list_.at(index);
	ui::HBox* container_element = (ui::HBox*)control;
	container_element->SetUTF8DataID(member_info->GetAccountID());
	container_element->SetUTF8Name(member_info->GetAccountID());

	nim::NIMTeamUserType user_type = member_info->GetUserType();
	bool is_me = member_info->GetAccountID().compare(LoginManager::GetInstance()->GetAccount()) == 0;
	bool has_authority = (my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || (my_property_.GetUserType() == nim::kNIMTeamUserTypeManager && user_type == nim::kNIMTeamUserTypeNomal));

	ui::Button* head_image_button = (ui::Button*)container_element->FindSubControl(L"head_image");
	head_image_button->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(member_info->GetAccountID()));
	head_image_button->DetachEvent(ui::EventType::kEventClick);
	if (!create_or_display_ && team_info_.GetType() == nim::kNIMTeamTypeAdvanced && (is_me || has_authority))
		head_image_button->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnHeadImageClick, this, member_info->GetAccountID(), std::placeholders::_1));
	else
		head_image_button->SetEnabled(false);

	ui::Label* show_name_label = (ui::Label*)container_element->FindSubControl(L"show_name");
	std::string team_card = member_info->GetNick();
	if (!team_card.empty()) {
		show_name_label->SetUTF8Text(team_card);
	}
	else {
		show_name_label->SetText(UserService::GetInstance()->GetUserName(member_info->GetAccountID()));
	}

	ui::Control* team_admin = container_element->FindSubControl(L"admin");
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
	else
	{
		team_admin->SetVisible(false);
	}

	if (!is_me && (/*delelte_enable ||*/ has_authority))
	{
		container_element->DetachEvent(ui::kEventMouseEnter);
		container_element->DetachEvent(ui::kEventMouseLeave);
		container_element->AttachBubbledEvent(ui::kEventMouseEnter, nbase::Bind(&TeamInfoBox::OnTeamMemberItemMouseEnter, this, container_element, std::placeholders::_1));
		container_element->AttachBubbledEvent(ui::kEventMouseLeave, nbase::Bind(&TeamInfoBox::OnTeamMemberItemMouseLeave, this, container_element, std::placeholders::_1));
		ui::Button* delete_btn = (ui::Button*)container_element->FindSubControl(L"btn_delete");
		delete_btn->DetachEvent(ui::EventType::kEventClick);
		delete_btn->AttachClick(nbase::Bind(&TeamInfoBox::OnBtnDeleteClick, this, container_element, member_info->GetAccountID(), std::placeholders::_1));
	}
}

int nim_comp::TeamInfoBox::GetElementtCount()
{
	return team_member_sort_list_.size();
}

void TeamInfoBox::UpdateUIByIdentity()
{
	FindSubControl(L"team_owner_section")->SetEnabled(!view_mode_ && (my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || my_property_.GetUserType() == nim::kNIMTeamUserTypeManager));
	FindSubControl(L"team_notify_fields")->SetEnabled(!view_mode_);
	if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
	{
		if (team_info_.GetInviteMode() == nim::kNIMTeamInviteModeEveryone)
		{
			invitebtn_->SetVisible(true);
		}			
		else
		{
			invitebtn_->SetVisible(my_property_.GetUserType() == nim::kNIMTeamUserTypeCreator || my_property_.GetUserType() == nim::kNIMTeamUserTypeManager);
		}
		if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateInfoModeEveryone)
		{
			FindSubControl(L"team_info_icon")->SetEnabled(true);
			FindSubControl(L"team_name_panel")->SetEnabled(true);
			FindSubControl(L"team_info_panel")->SetEnabled(true);
		}
	}
	if (team_info_.GetType() == nim::kNIMTeamTypeNormal)
	{
		FindSubControl(L"team_name_panel")->SetEnabled(true);
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

bool TeamInfoBox::IsTeamMemberType(const nim::NIMTeamUserType user_type)
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
void TeamInfoBox::SetTaskbarTitle(const std::wstring &title)
{
	if (taskbar_title_function_ != nullptr)
		taskbar_title_function_(title);
}
void TeamInfoBox::Close(UINT nRet/* = 0*/)
{
	if (!temp_file_path_.empty())
	{
		StdClosure closure = nbase::Bind(&DeleteFileCallback, temp_file_path_);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, closure);
	}
	if (close_function_ != nullptr)
		close_function_(nRet);
}
void TeamInfoBox::ActiveWindow()
{
	if (active_function_ != nullptr)
		active_function_();
}
void TeamInfoBox::AttachSetTaskbarTitle(const std::function<void(const std::wstring&)>& cb)
{
	taskbar_title_function_ = cb;
}
void TeamInfoBox::AttachClose(const std::function<void(UINT)>& cb)
{
	close_function_ = cb;
}
void TeamInfoBox::AttachActive(const std::function<void()>& cb)
{
	active_function_ = cb;
}
void TeamInfoBox::RefreshInfo(nim::TeamInfo* team_info)
{
    // 如果正在保存数据，不刷新界面
    if (saving_settings_)
        return;

	if (team_info_.GetTeamID().compare(team_info->GetTeamID()) != 0)
		return;
	if (team_info != &team_info_)
		team_info_ = *team_info;
	if (!create_or_display_)
	{
		((ui::RichEdit*)FindSubControl(L"team_id"))->SetUTF8Text(team_info_.GetTeamID());
		re_team_name_->SetUTF8Text(team_info_.GetName());
		if (type_ == nim::kNIMTeamTypeAdvanced)
		{
			btn_header_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(tid_));
			re_team_intro_->SetUTF8Text(team_info_.GetIntro());
			if (team_info_.GetJoinMode() == nim::kNIMTeamJoinModeNoAuth)
				((ui::Option*)FindSubControl(L"join_mode_any"))->Selected(true);
			else if (team_info_.GetJoinMode() == nim::kNIMTeamJoinModeNeedAuth)
				((ui::Option*)FindSubControl(L"join_mode_verify"))->Selected(true);
			else if (team_info_.GetJoinMode() == nim::kNIMTeamJoinModeRejectAll)
				((ui::Option*)FindSubControl(L"join_mode_nobody"))->Selected(true);

			if (team_info_.GetBeInviteMode() == nim::kNIMTeamBeInviteModeNeedAgree)
				((ui::Option*)FindSubControl(L"be_invite_mode_agree"))->Selected(true);
			else if (team_info_.GetBeInviteMode() == nim::kNIMTeamBeInviteModeNotNeedAgree)
				((ui::Option*)FindSubControl(L"be_invite_mode_no_need_agree"))->Selected(true);

			if (team_info_.GetInviteMode() == nim::kNIMTeamInviteModeManager)
				((ui::Option*)FindSubControl(L"invite_mode_manager"))->Selected(true);
			else if (team_info_.GetInviteMode() == nim::kNIMTeamInviteModeEveryone)
				((ui::Option*)FindSubControl(L"invite_mode_any"))->Selected(true);

			if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateInfoModeManager)
				((ui::Option*)FindSubControl(L"up_tinfo_mode_manager"))->Selected(true);
			else if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateInfoModeEveryone)
				((ui::Option*)FindSubControl(L"up_tinfo_mode_any"))->Selected(true);

			if (team_info_.GetUpdateCustomMode() == nim::kNIMTeamUpdateCustomModeManager)
				((ui::Option*)FindSubControl(L"up_custom_mode_manager"))->Selected(true);
			else if (team_info_.GetUpdateCustomMode() == nim::kNIMTeamUpdateCustomModeEveryone)
				((ui::Option*)FindSubControl(L"up_custom_mode_any"))->Selected(true);

			if (team_info_.GetMuteType() != nim::kNIMTeamMuteTypeNone)
				((ui::Option*)FindSubControl(L"mute_mode_allow_manager"))->Selected(true);
			else
				((ui::Option*)FindSubControl(L"mute_mode_allow_any"))->Selected(true);
		}
	}
	else
	{
		FindSubControl(L"team_id_panel")->SetVisible(false);

		if (type_ == nim::kNIMTeamTypeAdvanced)
		{
			btn_header_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(tid_));
			((ui::Option*)FindSubControl(L"join_mode_verify"))->Selected(true);
			((ui::Option*)FindSubControl(L"be_invite_mode_agree"))->Selected(true);
			((ui::Option*)FindSubControl(L"invite_mode_manager"))->Selected(true);
			((ui::Option*)FindSubControl(L"up_tinfo_mode_manager"))->Selected(true);
			((ui::Option*)FindSubControl(L"up_custom_mode_manager"))->Selected(true);
			((ui::Option*)FindSubControl(L"mute_mode_allow_any"))->Selected(true);
		}
	}
}

bool nim_comp::TeamInfoBox::OnClicked(ui::EventArgs* args)
{
    std::wstring name = args->pSender->GetName();
    return true;
}

void nim_comp::TeamInfoBox::OnTeamMuteChange(const std::string& tid, bool mute_all)
{
    if (tid == tid_)
    {
        auto&& team_info = nim::Team::QueryTeamInfoBlock(tid_);
        RefreshInfo(&team_info);
    }
}

void nim_comp::TeamInfoBox::OnTeamNotificationModeChangeCallback(const std::string& id, int64_t bits)
{
    switch (bits)
    {
    case nim::kNIMTeamBitsConfigMaskOnlyAdmin:
        dynamic_cast<ui::Option*>(FindSubControl(L"notify_mode_admin"))->Selected(true);
        break;
    case nim::kNIMTeamBitsConfigMaskMuteNotify:
        dynamic_cast<ui::Option*>(FindSubControl(L"notify_mode_off"))->Selected(true);
        break;
    default:
        dynamic_cast<ui::Option*>(FindSubControl(L"notify_mode_on"))->Selected(true);
        break;
    }	
}
void nim_comp::TeamInfoBox::SortTeamMembers()
{
	std::sort(team_member_sort_list_.begin(), team_member_sort_list_.end(), [](const std::shared_ptr<nim::TeamMemberProperty>& item1, const std::shared_ptr<nim::TeamMemberProperty>& item2) {
		auto get_user_type = [](const std::shared_ptr<nim::TeamMemberProperty>& itme) {
			switch (itme->GetUserType())
			{
			case nim::kNIMTeamUserTypeNomal:
				return 0;
			case nim::kNIMTeamUserTypeManager:
				return 1;
			case nim::kNIMTeamUserTypeCreator:
				return 3;
			default:
				return -1;
			}
		};
		auto user_type1 = get_user_type(item1);
		auto user_type2 = get_user_type(item2);
		if (user_type1 == user_type2)
			return std::less<std::string>()(item1->GetAccountID(), item2->GetAccountID());
		else
			return user_type2 < user_type1;
		});
}
void nim_comp::TeamInfoBox::RefreshMemberList(bool sort/* = false*/)
{
	if (sort)
		SortTeamMembers();
	if (tile_box_ != nullptr)
		tile_box_->Refresh();
}
void nim_comp::TeamInfoBox::AddTeamMemberData(const nim::TeamMemberProperty& data)
{
	auto ptr_data = std::make_shared<nim::TeamMemberProperty>(data);
	team_member_list_[data.GetAccountID()] = ptr_data;
	team_member_sort_list_.emplace_back(ptr_data);
	if (data.GetUserType() == nim::kNIMTeamUserTypeCreator)
		team_owner_accid_ = data.GetAccountID();
}
void nim_comp::TeamInfoBox::RemoveTeamMemberData(const std::string uid)
{
	team_member_list_.erase(uid);
	auto it = std::find_if(team_member_sort_list_.begin(), team_member_sort_list_.end(), [&](const std::shared_ptr<nim::TeamMemberProperty>& item) {
		return uid.compare(item->GetAccountID()) == 0;
		});
	if (it != team_member_sort_list_.end())
		team_member_sort_list_.erase(it);
}
void nim_comp::TeamInfoBox::SetTeamMemberDataAdmin(const std::string uid, bool admin)
{
	if (team_member_list_.find(uid) != team_member_list_.end())
		team_member_list_[uid]->SetUserType(admin ? nim::kNIMTeamUserTypeManager : nim::kNIMTeamUserTypeNomal);
	auto it = std::find_if(team_member_sort_list_.begin(), team_member_sort_list_.end(), [&](const std::shared_ptr<nim::TeamMemberProperty>& item) {
		return uid.compare(item->GetAccountID()) == 0;
		});
	if (it != team_member_sort_list_.end())
		(*it)->SetUserType(admin ? nim::kNIMTeamUserTypeManager : nim::kNIMTeamUserTypeNomal);
}
void nim_comp::TeamInfoBox::TeamOwnerChanged(const std::string new_owner_accid)
{
	if (team_owner_accid_ == new_owner_accid)
		return;
	auto info_list_it = team_member_list_.find(team_owner_accid_);
	if (info_list_it != team_member_list_.end())
		info_list_it->second->SetUserType(nim::kNIMTeamUserTypeNomal);
	auto sort_list_it = std::find_if(team_member_sort_list_.begin(), team_member_sort_list_.end(), [&](const std::shared_ptr<nim::TeamMemberProperty>& item) {
		return team_owner_accid_.compare(item->GetAccountID()) == 0;
		});
	if (sort_list_it != team_member_sort_list_.end())
		(*sort_list_it)->SetUserType(nim::kNIMTeamUserTypeNomal);
	team_owner_accid_ = new_owner_accid;
	info_list_it = team_member_list_.find(team_owner_accid_);
	if (info_list_it != team_member_list_.end())
		info_list_it->second->SetUserType(nim::kNIMTeamUserTypeCreator);
	sort_list_it = std::find_if(team_member_sort_list_.begin(), team_member_sort_list_.end(), [&](const std::shared_ptr<nim::TeamMemberProperty>& item) {
		return team_owner_accid_.compare(item->GetAccountID()) == 0;
		});
	if (sort_list_it != team_member_sort_list_.end())
		(*sort_list_it)->SetUserType(nim::kNIMTeamUserTypeCreator);
}