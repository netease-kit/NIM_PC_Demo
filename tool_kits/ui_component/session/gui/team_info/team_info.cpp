#include "team_info.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "callback/team/team_callback.h"
#include "shared/tool.h"
#include "gui/invoke_chat_form/invoke_chat_form.h"
#include "module/login/login_manager.h"
#include "gui/team_info/member_manager.h"

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
	user_type_ = nim::kNIMTeamUserTypeCreator;
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
	if (type_ == nim::kNIMTeamTypeNormal)
	{
		FindControl(L"team_info_fields")->SetVisible(false);
	}
	unregister_cb.Add(TeamService::GetInstance()->RegAddTeamMember(nbase::Bind(&TeamInfoForm::OnTeamMemberAdd, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeamMember(nbase::Bind(&TeamInfoForm::OnTeamMemberRemove, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMember(nbase::Bind(&TeamInfoForm::OnTeamMemberChange, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamAdmin(nbase::Bind(&TeamInfoForm::OnTeamMemberAdmin, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	unregister_cb.Add(TeamService::GetInstance()->RegSetTeamOwner(nbase::Bind(&TeamInfoForm::OnSetTeamOwner, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&TeamInfoForm::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(nbase::Bind(&TeamInfoForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2)));

	ui::Label* title_text = (ui::Label*)FindControl(L"title");
	if (create_or_display_) {
		title_text->SetText(L"创建群组");
	}
	else {
		title_text->SetText(L"群资料");
	}
	tile_box_ = (ui::ListBox*)FindControl(L"user_list");
	
	ui::Button* btn_confirm = (ui::Button*)FindControl(L"confirm");
	btn_confirm->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnConfirmClick, this, std::placeholders::_1));
	ui::Button* btn_cancel = (ui::Button*)FindControl(L"cancel");
	btn_cancel->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnCancelClick, this, std::placeholders::_1));
	btn_quit_ = (ui::Button*)FindControl(L"quit");
	btn_quit_->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnQuitClick, this, std::placeholders::_1));
	btn_dismiss_ = (ui::Button*)FindControl(L"dismiss");
	btn_dismiss_->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnDissmissClick, this, std::placeholders::_1));

	if (!create_or_display_)
	{
		if (team_info_.readonly) {
			btn_quit_->SetVisible(false);
			btn_dismiss_->SetVisible(false);
		}
		else {
			ChangeUIByIdentity();
		}
		nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::Init, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else {
		AddInviteButton();
		btn_quit_->SetVisible(false);
		btn_dismiss_->SetVisible(false);
	}
	re_team_name_ = (RichEdit*)FindControl(L"team_name");
	re_team_intro_ = (RichEdit*)FindControl(L"team_intro");

	if (!create_or_display_) {
		((RichEdit*)FindControl(L"team_id"))->SetUTF8Text(team_info_.id);
		re_team_name_->SetUTF8Text(team_info_.name);
		re_team_intro_->SetUTF8Text(team_info_.intro);
		if (team_info_.join_mode == nim::kNIMTeamJoinModeNoAuth)
		{
			((Option*)FindControl(L"join_mode_any"))->Selected(true);
		}
		else if (team_info_.join_mode == nim::kNIMTeamJoinModeNeedAuth)
		{
			((Option*)FindControl(L"join_mode_verify"))->Selected(true);
		}
		else if (team_info_.join_mode == nim::kNIMTeamJoinModeRejectAll)
		{
			((Option*)FindControl(L"join_mode_nobody"))->Selected(true);
		}
	}
	else {
		FindControl(L"team_id_panel")->SetVisible(false);
		((Option*)FindControl(L"join_mode_verify"))->Selected(true);
	}
}

bool TeamInfoForm::OnInviteUesrBtnClick(ui::EventArgs *param)
{
	InvokeChatForm *invite_user_form = (InvokeChatForm *)WindowsManager::GetInstance()->GetWindow\
		(InvokeChatForm::kClassName, L"CreateTeamWnd");
	std::wstring caption = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INVITEUSERFORM_INVITE_JOINCHAT");
	if(!invite_user_form)
	{
		std::list<UTF8String> ids_;
		for (int i = 0; i < tile_box_->GetCount(); i++)
		{
			ids_.push_back(tile_box_->GetItemAt(i)->GetUTF8DataID());
		}
		invite_user_form = new InvokeChatForm("CreateTeamWnd", ids_, nbase::Bind(&TeamInfoForm::SelectedCompleted, this, std::placeholders::_1));
		invite_user_form->Create(NULL, caption.c_str(), UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L);
		invite_user_form->CenterWindow();
	}

	invite_user_form->ActiveWindow();
	::SetForegroundWindow(invite_user_form->GetHWND());

	return true;
}

void TeamInfoForm::SelectedCompleted(const std::list<UTF8String>& id_list)
{
	if (create_or_display_) {
		std::list<nim::TeamMemberInfo> team_member_vec;
		nim::TeamMemberInfo team_member;
		for (auto it = id_list.begin(); it != id_list.end(); it++)
		{
			UserInfo userinfo;
			UserService::GetInstance()->GetUserInfo(*it, userinfo);
			team_member.account = userinfo.account;
			team_member.nick = userinfo.name;
			team_member.type = nim::kNIMTeamUserTypeLocalWaitAccept;
			team_member_vec.push_back(team_member);
		}
		AddTeamMembersInfo(team_member_vec);
	}
	else {
		nim::Team::InviteAsync(tid_, id_list, "", nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
void TeamInfoForm::Init(const std::string& team_id, int count, const std::list<nim::TeamMemberInfo>& team_member_list)
{
	tile_box_->RemoveAll();
	AddInviteButton();
	nim::NIMTeamUserType user_type = nim::kNIMTeamUserTypeCreator;
	auto login_account = LoginManager::GetInstance()->GetAccount();
	for (auto it = team_member_list.begin(); it != team_member_list.end(); it++) {
		if (login_account == it->account) {
			user_type = it->type;
			my_tInfo_ = *it;
			bool msg_notify = (my_tInfo_.bits&nim::kNIMTeamBitsConfigMaskMuteNotify) == 0;
			((CheckBox*)FindControl(L"msg_mode_notify"))->Selected(msg_notify);
			((CheckBox*)FindControl(L"msg_mode_notify"))->SetEnabled(true);
			break;
		}
	}
	user_type_ = user_type;
	if (team_info_.type == nim::kNIMTeamTypeAdvanced)
	{
		invitebtn_->SetEnabled((!team_info_.readonly) && (user_type_ == nim::kNIMTeamUserTypeCreator || user_type_ == nim::kNIMTeamUserTypeManager));
	}
	FindControl(L"team_owner_section")->SetEnabled((!team_info_.readonly) && (user_type_ == nim::kNIMTeamUserTypeCreator || user_type_ == nim::kNIMTeamUserTypeManager));
	if (team_info_.type == nim::kNIMTeamTypeNormal)
	{
		FindControl(L"team_name_panel")->SetEnabled(true);
	}
	ChangeUIByIdentity();
	AddTeamMembersInfo(team_member_list);
}

void TeamInfoForm::AddTeamMembersInfo(const std::list<nim::TeamMemberInfo>& team_member_list)
{
	for(auto it = team_member_list.begin(); it != team_member_list.end(); it++) {
		// 要查看群名片窗口，需先打开群聊天窗口，再点击群头像。而打开聊天窗口之时已经把所有群成员信息获取到内存了，因此此处用GetUserInfo即可。
		UserInfo userInfo;
		UserService::GetInstance()->GetUserInfo(it->account, userInfo); 
		HBox* listitem = CreateTeamMemberListItem(it->type, it->nick, userInfo);
		if (create_or_display_)
		{
			listitem->FindSubControl(L"head_image")->SetEnabled(false);
		}
		else {
			if (it->account != LoginManager::GetInstance()->GetAccount()) {
				if (team_info_.readonly
					|| !(user_type_ == nim::kNIMTeamUserTypeCreator || (user_type_ == nim::kNIMTeamUserTypeManager && it->type == nim::kNIMTeamUserTypeNomal)))
				{
					listitem->SetMouseEnabled(false);
					listitem->SetMouseChildEnabled(false);
				}
			}
		}
		tile_box_->Add(listitem);
	}
}

HBox* TeamInfoForm::CreateTeamMemberListItem(nim::NIMTeamUserType user_type, const std::string& team_card, const UserInfo& user_info)
{
	ui::HBox* container_element = new HBox();
	GlobalManager::FillBoxWithCache(container_element, L"team_info/start_chat_friend.xml");
	container_element->SetUTF8DataID(user_info.account);
	container_element->SetUTF8Name(user_info.account);
	Button* head_image_button = (Button*)container_element->FindSubControl(L"head_image");
	head_image_button->SetBkImage(UserService::GetInstance()->GetUserPhoto(user_info.account));
	head_image_button->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnHeadImageClick, this, user_info.account, std::placeholders::_1));
	if (team_info_.type == nim::kNIMTeamTypeNormal)
	{
		head_image_button->SetEnabled(false);
	}
	Label* show_name_label = (Label*)container_element->FindSubControl(L"show_name");
	if (!team_card.empty()) {
		show_name_label->SetUTF8Text(team_card);
		show_name_label->SetUTF8DataID(team_card);	//把群昵称保存到DataId里面
	}
	else {
		show_name_label->SetUTF8Text(user_info.name);
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

	if (user_info.account != LoginManager::GetInstance()->GetAccount())
	{
		container_element->AttachBubbledEvent(ui::kEventMouseEnter, nbase::Bind(&TeamInfoForm::OnTeamMemberItemMouseEnter, this, container_element, std::placeholders::_1));
		container_element->AttachBubbledEvent(ui::kEventMouseLeave, nbase::Bind(&TeamInfoForm::OnTeamMemberItemMouseLeave, this, container_element, std::placeholders::_1));
	}
	ui::Button* delete_btn = (ui::Button*)container_element->FindSubControl(L"btn_delete");
	delete_btn->AttachClick(nbase::Bind(&TeamInfoForm::OnBtnDeleteClick, this, container_element, user_info.account, std::placeholders::_1));

	return container_element;
}

void TeamInfoForm::UpdateTeamMemberListItem(ui::Box* container_element, const std::string& team_card, const UserInfo& user_info)
{
	Button* head_image_button = (Button*)container_element->FindSubControl(L"head_image");
	head_image_button->SetBkImage(UserService::GetInstance()->GetUserPhoto(user_info.account));

	Label* show_name_label = (Label*)container_element->FindSubControl(L"show_name");
	if (!team_card.empty()) 
	{
		show_name_label->SetUTF8Text(team_card);
		show_name_label->SetUTF8DataID(team_card);//群昵称保存到控件DataId
	}
	else if (!show_name_label->GetUTF8Text().empty() && show_name_label->GetUTF8Text() == show_name_label->GetUTF8DataID())
	{
		//如果之前设置过群昵称，而且群昵称和当前显示的名字一致，就不再设置名字了，防止user_info.name覆盖掉群昵称
	}
	else if (show_name_label->GetUTF8DataID().empty() )
	{
		show_name_label->SetUTF8Text(user_info.name);
	}

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
		nim::Team::KickAsync(tid_, uids_list, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
	MemberManagerForm* team_info_form = (MemberManagerForm*)WindowsManager::GetInstance()->GetWindow\
		(MemberManagerForm::kClassName, ws_user_id);
	if(team_info_form == NULL)
	{
		bool show_privilege_panel = user_type_ == nim::kNIMTeamUserTypeCreator && user_id != LoginManager::GetInstance()->GetAccount();
		team_info_form = new MemberManagerForm(tid_, user_id, show_privilege_panel);
		team_info_form->Create(NULL, L"群资料", WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
		team_info_form->CenterWindow();
		team_info_form->ShowWindow(true);
	}
	else
	{
		team_info_form->ActiveWindow();
	}

	return true;
}


bool TeamInfoForm::OnBtnConfirmClick(ui::EventArgs* param)
{
	param->pSender->SetEnabled(false);
	UTF8String team_name = ((RichEdit*)FindControl(L"team_name"))->GetUTF8Text();
	UTF8String team_intro = ((RichEdit*)FindControl(L"team_intro"))->GetUTF8Text();

	Json::Value tinfo;
	if (team_name != team_info_.name)
	{
		tinfo[nim::kNIMTeamInfoKeyName] = team_name.c_str();
	}
	if (type_ == nim::kNIMTeamTypeAdvanced) {
		tinfo[nim::kNIMTeamInfoKeyType] = nim::kNIMTeamTypeAdvanced;
		if (team_intro != team_info_.intro)
		{
			tinfo[nim::kNIMTeamInfoKeyIntro] = team_intro.c_str();
		}
		nim::NIMTeamJoinMode join_mode = nim::kNIMTeamJoinModeNeedAuth;
		if (((Option*)FindControl(L"join_mode_any"))->IsSelected())
		{
			join_mode = nim::kNIMTeamJoinModeNoAuth;
		}
		else if (((Option*)FindControl(L"join_mode_verify"))->IsSelected())
		{
			join_mode = nim::kNIMTeamJoinModeNeedAuth;
		}
		else if (((Option*)FindControl(L"join_mode_nobody"))->IsSelected())
		{
			join_mode = nim::kNIMTeamJoinModeRejectAll;
		}
		if (join_mode != team_info_.join_mode)
		{
			tinfo[nim::kNIMTeamInfoKeyJoinMode] = join_mode;
		}
	}
	else {
		tinfo[nim::kNIMTeamInfoKeyType] = nim::kNIMTeamTypeNormal;
	}

	if (create_or_display_)
	{
		std::list<UTF8String> id_list;
		for (int i = 1; i < tile_box_->GetCount(); i++)
		{
			id_list.push_back(tile_box_->GetItemAt(i)->GetUTF8DataID());
		}
		nim::Team::CreateTeamAsync(tinfo.toStyledString(), id_list, "", nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
	else {

		nim::Team::UpdateTeamInfoAsync(tid_, tinfo.toStyledString(), nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		if (((CheckBox*)FindControl(L"msg_mode_notify"))->IsEnabled())
		{
			bool msg_notify = (my_tInfo_.bits&nim::kNIMTeamBitsConfigMaskMuteNotify) == 0;
			if (msg_notify != ((Option*)FindControl(L"msg_mode_notify"))->IsSelected())
			{
				Json::Value values;
				values[nim::kNIMTeamUserKeyID] = tid_;
				uint64_t bits = my_tInfo_.bits;
				if (msg_notify)
				{
					bits |= nim::kNIMTeamBitsConfigMaskMuteNotify;
				}
				else
				{
					bits &= ~nim::kNIMTeamBitsConfigMaskMuteNotify;
				}
				values[nim::kNIMTeamUserKeyBits] = bits;
				nim::Team::UpdateMyPropertyAsync(values.toStyledString(), nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
	nim::Team::DismissAsync(tid_, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	Close();
	return true;
}

bool TeamInfoForm::OnBtnQuitClick(ui::EventArgs* param)
{
	nim::Team::LeaveAsync(tid_, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	Close();
	return true;
}

void TeamInfoForm::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberInfo& team_member_info)
{
	for (int i = 0; i < tile_box_->GetCount(); i++)
	{
		if (team_member_info.account == tile_box_->GetItemAt(i)->GetUTF8DataID())
		{
			return;
		}
	}

	UserInfo user_info;
	UserService::GetInstance()->GetUserInfo(team_member_info.account, user_info);
	if (!team_member_info.nick.empty())
	{
		user_info.name = team_member_info.nick;
	}
	HBox* hbox = CreateTeamMemberListItem(nim::kNIMTeamUserTypeNomal, "", user_info);
	tile_box_->Add(hbox);
}

void TeamInfoForm::OnTeamMemberRemove(const std::string& tid, const std::string& uid)
{
	if(tid == tid_)
	{
		for (int i = 0; i < tile_box_->GetCount(); i++)
		{
			if (tile_box_->GetItemAt(i)->GetUTF8DataID() == uid)
			{
				tile_box_->RemoveAt(i);
				break;
			}
		}
	}
}

void TeamInfoForm::OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card)
{
	size_t splitter = tid_uid.find_first_of('#');
	std::string tid = tid_uid.substr(0, splitter), uid = tid_uid.substr(splitter + 1);
	if(tid == tid_)
	{
		for (int i = 0; i < tile_box_->GetCount(); i++)
		{
			if (tile_box_->GetItemAt(i)->GetUTF8DataID() == uid)
			{
				UserInfo user_info;
				UserService::GetInstance()->GetUserInfo(uid, user_info);
				std::string new_team_card = team_card;

				UpdateTeamMemberListItem(((ui::Box*)tile_box_->GetItemAt(i)), new_team_card, user_info);
				break;
			}
		}
	}
}

void TeamInfoForm::OnTeamMemberAdmin(const std::string& tid, const std::string& uid, bool admin)
{
	if (tid == tid_)
	{
		nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::Init, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
}

void TeamInfoForm::OnSetTeamOwner(const std::string& tid, const std::string& uid)
{
	if (tid == tid_)
	{
		nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::Init, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
}

void TeamInfoForm::OnUserInfoChange(const std::list<UserInfo>& uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	{
		HBox* item = (HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(iter->account));
		if(item == NULL) continue;
		if((iter->field_avail_flag & kUInfoFlagHeadImage) != 0)
			item->FindSubControl(L"head_image")->SetBkImage(UserService::GetInstance()->GetUserPhoto(iter->account));
		Label* show_name_label = (Label*)item->FindSubControl(L"show_name");
		if (show_name_label->GetDataID().empty() && (iter->field_avail_flag & kUInfoFlagName) != 0) // 如果没有设置群昵称，则显示用户自己的昵称。如果设置了群昵称，就不变。
			show_name_label->SetUTF8Text(iter->name);
	}
}

void TeamInfoForm::OnUserPhotoReady(const std::string & accid, const std::wstring & photo_path)
{
	HBox* item = (HBox*)tile_box_->FindSubControl(nbase::UTF8ToUTF16(accid));
	if (item != NULL)
		item->FindSubControl(L"head_image")->SetBkImage(photo_path);
}

void TeamInfoForm::UpdateTeamMember()
{
	nim::Team::QueryTeamMembersAsync(tid_, nbase::Bind(&TeamInfoForm::Init, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TeamInfoForm::ChangeUIByIdentity()
{
	if (type_ == nim::kNIMTeamUserTypeNomal || user_type_ != nim::kNIMTeamUserTypeCreator) {
		btn_dismiss_->SetVisible(false);
		btn_quit_->SetVisible(true);
	}
	else {
		btn_dismiss_->SetVisible(true);
		btn_quit_->SetVisible(false);
	}
}
}