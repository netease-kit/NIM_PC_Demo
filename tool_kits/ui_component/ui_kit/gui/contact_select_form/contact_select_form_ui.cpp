#include "contact_select_form.h"
#include "module/login/login_manager.h"

#include "gui/video/multi_video_form.h"
using namespace ui;
using namespace std;

namespace nim_comp
{
const LPCSTR ContactSelectForm::kCreateGroup = "CreateGroup";
const LPCSTR ContactSelectForm::kCreateTeam = "CreateTeam";
const LPCSTR ContactSelectForm::kRetweetMessage = "RetweetMessage";

const LPCTSTR ContactSelectForm::kClassName = _T("ContactSelectForm");

ContactSelectForm::ContactSelectForm(const UTF8String& uid_or_tid,
	const std::list<UTF8String>& exclude_ids,
	const SelectedCompletedCallback& completedCallback,
	bool need_select_group/* = false*/, bool is_multi_vchat/*=false*/)
	: uid_or_tid_(uid_or_tid)
	, exclude_ids_(exclude_ids)
	, completedCallback_(completedCallback)
	, need_select_group_(need_select_group)
	, is_multi_vchat_(is_multi_vchat)
{
	auto friend_list_change_cb = nbase::Bind(&ContactSelectForm::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));
	auto black_change_cb = nbase::Bind(&ContactSelectForm::OnSetBlackCallback, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetBlackCallback(black_change_cb));
	auto user_info_change_cb = nbase::Bind(&ContactSelectForm::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));
	auto user_photo_ready_cb = nbase::Bind(&ContactSelectForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(user_photo_ready_cb));

	auto add_team_cb = nbase::Bind(&ContactSelectForm::OnAddTeam, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(TeamService::GetInstance()->RegAddTeam(add_team_cb));
	auto remove_team_cb = nbase::Bind(&ContactSelectForm::OnRemoveTeam, this, std::placeholders::_1);
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeam(remove_team_cb));
	auto change_team_name_cb = nbase::Bind(&ContactSelectForm::OnTeamNameChanged, this, std::placeholders::_1);
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(change_team_name_cb));
}

ContactSelectForm::~ContactSelectForm()
{
}

std::wstring ContactSelectForm::GetSkinFolder()
{
	return L"contact_select";
}

std::wstring ContactSelectForm::GetSkinFile()
{
	return L"contact_select_form.xml";
}

std::wstring ContactSelectForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ContactSelectForm::GetWindowId() const
{
	std::wstring uid_or_tid = nbase::UTF8ToUTF16(uid_or_tid_);
	return uid_or_tid;
}

UINT ContactSelectForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void ContactSelectForm::InitWindow()
{
	std::wstring title_id;
	if (uid_or_tid_ == kCreateGroup)
		title_id = L"STRING_INVITEUSERFORM_CREATE_GROUP";
	else if (uid_or_tid_ == kCreateTeam)
		title_id = L"STRING_INVITEUSERFORM_CREATE_TEAM";
	else if (uid_or_tid_ == kRetweetMessage)
		title_id = L"STRING_INVITEUSERFORM_RETWEET_MSG";
	else
		title_id = L"STRING_INVOKECHATFORM_CAPTION";

	std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(title_id);
	SetTaskbarTitle(title);
	((Label*)FindControl(L"title"))->SetText(title);

	contract_list_ = (ui::TreeView*)FindControl(L"user_list");
	contract_list_->AttachBubbledEvent(kEventClick, nbase::Bind(&ContactSelectForm::OnListItemClick, this, std::placeholders::_1));
	search_result_list_ = static_cast<ui::ListBox*>(FindControl(L"search_result"));
	search_result_list_->AttachBubbledEvent(kEventClick, nbase::Bind(&ContactSelectForm::OnSearchResultListItemClick, this, std::placeholders::_1));
	selected_user_list_ = (ui::ListBox*)FindControl(L"selected_user_list");

	TreeNode* tree_node = ListItemUtil::CreateFirstLetterListItem(L"*");
	AddGroup(tree_node);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"群组");
	AddGroup(tree_node);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"#");
	AddGroup(tree_node);

	for (int i = 0; i < 26; i++)
	{
		wchar_t letter = L'A' + i;
		wstring letter_str;
		letter_str += letter;
		tree_node = ListItemUtil::CreateFirstLetterListItem(letter_str);
		AddGroup(tree_node);
	}

	tool_tip_content_ = static_cast<ui::Label*>(FindControl(L"tool_tip"));
	search_edit_ = (RichEdit*)FindControl(L"search");
	search_edit_->AttachTextChange(nbase::Bind(&ContactSelectForm::OnSearchEditChange, this, std::placeholders::_1));
	search_edit_->SetLimitText(30);
	btn_clear_input_ = (Button*)FindControl(L"clear_input");
	btn_clear_input_->SetNoFocus();
	btn_clear_input_->AttachClick(nbase::Bind(&ContactSelectForm::OnClearBtnClick, this, std::placeholders::_1));

	ui::Button* btn_confirm = (ui::Button*)FindControl(L"confirm");
	btn_confirm->AttachClick(nbase::Bind(&ContactSelectForm::OnBtnConfirmClick, this, std::placeholders::_1));
	ui::Button* btn_cancel = (ui::Button*)FindControl(L"cancel");
	btn_cancel->AttachClick(nbase::Bind(&ContactSelectForm::OnBtnCancelClick, this, std::placeholders::_1));

	if (is_multi_vchat_)
	{
		tool_tip_content_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOCHATFORM_SEARCH_ORG_EMPTY").c_str());

		std::wstring title_id;
		title_id = L"STRING_INVITEUSERFORM_MULTI_VCHAT";
		std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(title_id);
		SetTaskbarTitle(title);
		((Label*)FindControl(L"title"))->SetText(title);
		title_id = L"STRING_INVITEUSERFORM_TEAM_MEMBERS";
		title = ui::MutiLanSupport::GetInstance()->GetStringViaID(title_id);
// 		((Label*)FindControl(L"label_contact"))->SetText(title);
		title_id = L"STRING_INVITEUSERFORM_START_VCHAT";
		title = ui::MutiLanSupport::GetInstance()->GetStringViaID(title_id);
		btn_confirm->SetText(title);
		TeamService* team_service = TeamService::GetInstance();
		nim::Team::QueryTeamMembersAsync(uid_or_tid_, nbase::Bind(&ContactSelectForm::OnGetTeamMembers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else
	{
		tool_tip_content_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INVOKECHATFORM_SEARCH_TOOLTIP").c_str());
		// 添加好友
		UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
		UserService* user_service = UserService::GetInstance();
		const std::map<std::string, nim::UserNameCard>& users = user_service->GetAllUserInfos();
		MuteBlackService* mb_service = MuteBlackService::GetInstance();
		for (auto &it : users)
		{
			if (it.first != current_user_id &&
				user_service->GetUserType(it.second.GetAccId()) == nim::kNIMFriendFlagNormal &&
				!mb_service->IsInBlackList(it.first))
			{
				AddFriendListItem(it.second.GetAccId(), true);
			}
		}

		// 添加讨论组和高级群
		if (need_select_group_)
		{
			std::list<nim::TeamInfo> teams = TeamService::GetInstance()->GetCachedTeamsInfo();
			for (auto &it : teams)
			{
				AddTeamListItem(it.GetTeamID(), true);
			}
		}
	}
}

void ContactSelectForm::OnGetTeamMembers(const std::string& team_id, int count, const std::list<nim::TeamMemberProperty>& team_member_list)
{
	auto cb = [=]()
	{
		UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
		for (const auto& member : team_member_list)
		{
			std::string member_id = member.GetAccountID();
			if (current_user_id != member_id)
			{
				MultiVideoChatForm *multi_window = (MultiVideoChatForm*)(WindowsManager::GetInstance()->GetWindow(MultiVideoChatForm::kClassName, MultiVideoChatForm::kClassName));
				if (multi_window)
				{
					std::set<std::string> talking_members = multi_window->GetTalkingMember();
					bool is_enable = true;
					std::set<std::string>::iterator it = talking_members.find(member.GetAccountID());
					if (it == talking_members.end())
					{
						AddFriendListItem(member.GetAccountID(), is_enable);
					}
				}
				else
					AddFriendListItem(member.GetAccountID(), true);;
			}
		}
	};
	Post2UI(cb);
}


bool ContactSelectForm::OnBtnDeleteClick(const UTF8String& user_id, ui::EventArgs* param)
{
	for (int i = 0; i < selected_user_list_->GetCount(); i++)
	{
		Control* item = selected_user_list_->GetItemAt(i);
		if (item->GetUTF8DataID() == user_id)
		{
			selected_user_list_->RemoveAt(i);
		}
	}

	if (search_result_list_->IsVisible())
	{
		for (int i = 0; i < search_result_list_->GetCount(); i++)
		{
			Control* item = search_result_list_->GetItemAt(i);
			if (item->GetUTF8DataID() == user_id)
			{
				EventArgs args;
				args.pSender = item;
				OnSearchResultListItemClick(&args);
			}
		}
	}
	else{
		for (auto& it : tree_node_ver_)
		{
			for (int i = 0; i < it->GetCount(); i++)
			{
				ContactListItemUI* item = (ContactListItemUI*)it->GetItemAt(i);
				if (item->GetUTF8DataID() == user_id)
				{
					EventArgs args;
					args.pSender = item;
					OnListItemClick(&args);
				}
			}
		}
	}
	return true;
}

bool ContactSelectForm::OnBtnConfirmClick(ui::EventArgs* param)
{
	std::list<UTF8String> friend_list;
	std::list<UTF8String> team_list;
	for (int i = 0; i < selected_user_list_->GetCount(); i++)
	{
		SelectedContactItemUI* select_item = dynamic_cast<SelectedContactItemUI*>(selected_user_list_->GetItemAt(i));
		UTF8String id = select_item->GetAccountID();
		if (select_item->IsTeam())
			team_list.push_back(id);
		else
			friend_list.push_back(id);
	}

	completedCallback_(friend_list, team_list);
	Close();
	return true;
}

bool ContactSelectForm::OnBtnCancelClick(ui::EventArgs* param)
{
	Close();
	return true;
}

bool ContactSelectForm::OnSearchEditChange(ui::EventArgs* param)
{
	UTF8String search_key = search_edit_->GetUTF8Text();
	if(search_key.empty())
	{
		search_result_list_->RemoveAll();
		search_result_list_->SetVisible(false);
		btn_clear_input_->SetVisible(false);
		tool_tip_content_->SetVisible(false);
	}
	else
	{
		search_result_list_->RemoveAll();

		for (auto& it : tree_node_ver_)
		{
			for (int i = 0; i < it->GetCount(); i++)
			{
				ContactListItemUI* user_listitem = (ContactListItemUI*)it->GetItemAt(i);
				if (user_listitem->Match(search_key))
				{
					ui::CheckBox* checkbox = (ui::CheckBox*)user_listitem->FindSubControl(L"checkbox");
					auto search_listitem = CreateListItem(user_listitem->GetAccountID(), user_listitem->IsTeam());
					ui::CheckBox* search_checkbox = (ui::CheckBox*)search_listitem->FindSubControl(L"checkbox");
					search_checkbox->Selected(checkbox->IsSelected());
					search_result_list_->Add(search_listitem);
				}
			}
		}

		search_result_list_->SetVisible(true);
		int count = search_result_list_->GetCount();
		if(count > 0)
			tool_tip_content_->SetVisible(false);
		else
			tool_tip_content_->SetVisible(true);

		btn_clear_input_->SetVisible(true);
	}

	return true;
}

bool ContactSelectForm::OnClearBtnClick(ui::EventArgs* param)
{
	btn_clear_input_->SetVisible(false);
	tool_tip_content_->SetVisible(false);

	search_edit_->SetText(L"");

	return false;
}

bool ContactSelectForm::OnListItemClick(ui::EventArgs* param)
{
	ContactListItemUI* listitem = dynamic_cast<ContactListItemUI*>(param->pSender);
	if (listitem)
	{
		UTF8String id = listitem->GetUTF8DataID();
		ui::CheckBox* checkbox = (ui::CheckBox*)listitem->FindSubControl(L"checkbox");
		checkbox->Selected(!checkbox->IsSelected(), false);
		if (!checkbox->IsSelected()) {
			listitem->Selected(false, true);
		}
		OnCheckBox(id, listitem->IsTeam(), checkbox->IsSelected());
	}

	return true;
}

bool ContactSelectForm::OnSearchResultListItemClick(ui::EventArgs* param)
{
	OnListItemClick(param);

	for (auto& it : tree_node_ver_)
	{
		for (int i = 0; i < it->GetCount(); i++)
		{
			auto listitem = (ContactListItemUI*)(it->GetItemAt(i));
			if (listitem->GetUTF8DataID() == param->pSender->GetUTF8DataID())
			{
				ui::CheckBox* checkbox = (ui::CheckBox*)listitem->FindSubControl(L"checkbox");
				checkbox->Selected(!checkbox->IsSelected(), false);
				if (!checkbox->IsSelected()) {
					listitem->Selected(false, true);
				}
			}
		}
	}

	return true;
}

void ContactSelectForm::OnCheckBox(UTF8String id, bool is_team, bool check)
{
	if (check)
	{
		SelectedContactItemUI* selected_listitem = CreateSelectedListItem(id, is_team);
		selected_user_list_->Add(selected_listitem);
		selected_user_list_->EndDown();
	}
	else
	{
		for (int i = 0; i < selected_user_list_->GetCount(); i++)
		{
			SelectedContactItemUI* listitem = (SelectedContactItemUI*)selected_user_list_->GetItemAt(i);
			if (listitem->GetUTF8DataID() == id)
			{
				selected_user_list_->RemoveAt(i);
				break;
			}
		}
	}
}

}