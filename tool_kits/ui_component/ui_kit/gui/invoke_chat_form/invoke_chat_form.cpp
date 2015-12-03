#include "invoke_chat_form.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "shared/tool.h"
#include "module/login/login_manager.h"
#include "module/service/mute_black_service.h"

using namespace ui;
using namespace std;

namespace nim_comp
{
const LPCTSTR InvokeChatForm::kClassName			= _T("InvokeChatForm");



InvokeChatForm::InvokeChatForm(const UTF8String& uid_or_tid, const std::list<UTF8String>& exclude_ids, const SelectedCompletedCallback& completedCallback) :
	uid_or_tid_(uid_or_tid),
	exclude_ids_(exclude_ids),
	completedCallback_(completedCallback)
{
	auto friend_list_change_cb = nbase::Bind(&InvokeChatForm::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));

	auto black_change_cb = nbase::Bind(&InvokeChatForm::OnSetBlackCallback, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetBlackCallback(black_change_cb));
}

InvokeChatForm::~InvokeChatForm()
{
}

std::wstring InvokeChatForm::GetSkinFolder()
{
	return L"invokechat";
}

std::wstring InvokeChatForm::GetSkinFile()
{
	return L"invoke_chat_form.xml";
}

std::wstring InvokeChatForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring InvokeChatForm::GetWindowId() const
{
	std::wstring uid_or_tid = nbase::UTF8ToUTF16(uid_or_tid_);
	return uid_or_tid;
}

UINT InvokeChatForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT InvokeChatForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void InvokeChatForm::InitWindow()
{
	std::wstring title_id;
	if(uid_or_tid_ == "CreateGroupWnd")
		title_id = L"STRING_INVITEUSERFORM_CREATE_GROUP";
	else if (uid_or_tid_ == "CreateTeamWnd")
		title_id = L"STRING_INVITEUSERFORM_CREATE_TEAM";
	else
		title_id = L"STRING_INVOKECHATFORM_CAPTION";
	std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(title_id);
	SetTaskbarTitle(title);
	((Label*)FindControl(L"title"))->SetText(title);

	friend_list_ = (ui::TreeView*)FindControl(L"user_list");
	friend_list_->AttachBubbledEvent(kEventClick, nbase::Bind(&InvokeChatForm::OnListItemClick, this, std::placeholders::_1));
	search_result_list_ = static_cast<ui::ListBox*>(FindControl(L"search_result"));
	search_result_list_->AttachBubbledEvent(kEventClick, nbase::Bind(&InvokeChatForm::OnSearchResultListItemClick, this, std::placeholders::_1));
	selected_user_list_ = (ui::ListBox*)FindControl(L"selected_user_list");

	TreeNode* tree_node = ListItemUtil::CreateFirstLetterListItem(L"*");
	AddTreeNode(tree_node);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"群组");
	AddTreeNode(tree_node);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"#");
	AddTreeNode(tree_node);

	for (int i = 0; i < 26; i++)
	{
		wchar_t letter = L'A' + i;
		wstring letter_str;
		letter_str += letter;
		tree_node = ListItemUtil::CreateFirstLetterListItem(letter_str);
		AddTreeNode(tree_node);
	}

	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	UserService* user_service = UserService::GetInstance();
	const std::map<std::string, nim::UserNameCard>& users = user_service->GetAllUserInfos();
	MuteBlackService* mb_service = MuteBlackService::GetInstance();
	for(auto it = users.begin(); it != users.end(); it++) {
		if (it->first != current_user_id && user_service->GetUserType(it->second.GetAccId()) == nim::kNIMFriendFlagNormal && !mb_service->IsInBlackList(it->first)) {
			AddListItem(it->second, true);
		}
	}

	search_edit_ = (RichEdit*) FindControl( L"search" );
	search_edit_->AttachTextChange(nbase::Bind(&InvokeChatForm::OnSearchEditChange, this, std::placeholders::_1));
	search_edit_->SetLimitText( 30 );
	btn_clear_input_ = (Button*) FindControl(L"clear_input");
	btn_clear_input_->SetNoFocus();
	btn_clear_input_->AttachClick(nbase::Bind(&InvokeChatForm::OnClearBtnClick, this, std::placeholders::_1));

	tool_tip_content_ = static_cast<ui::Label*>(FindControl(L"tool_tip"));
	tool_tip_content_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INVOKECHATFORM_SEARCH_TOOLTIP").c_str());

	ui::Button* btn_confirm = (ui::Button*)FindControl(L"confirm");
	btn_confirm->AttachClick(nbase::Bind(&InvokeChatForm::OnBtnConfirmClick, this, std::placeholders::_1));
	ui::Button* btn_cancel = (ui::Button*)FindControl(L"cancel");
	btn_cancel->AttachClick(nbase::Bind(&InvokeChatForm::OnBtnCancelClick, this, std::placeholders::_1));
}

void InvokeChatForm::AddTreeNode(ui::TreeNode* tree_node)
{
	tree_node->SetEnabled(false);
	friend_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node->SetVisible(false);
	TreeNode* tree_node2 = new TreeNode();
	//SelectItemCallback callback = nbase::Bind(&InvokeChatForm::SelectItem, this, std::placeholders::_1);
	//auto createControlCallback = nbase::Bind(&InvokeChatListCreateControl, std::placeholders::_1);
	CInvokeChatTileListUI* tile_layout = new CInvokeChatTileListUI;
	ui::GlobalManager::FillBox(tile_layout, L"invokechat/InvokeChatTileList.xml");
	//tile_layout->SetSelectItemCallback(callback);
	RECT rect = { 5, 5, 5, 5 };
	tree_node2->GetLayout()->SetPadding(rect);
	tree_node2->Add(tile_layout);
	tree_node2->SetFixedWidth(DUI_LENGTH_AUTO);
	tree_node2->SetFixedHeight(DUI_LENGTH_AUTO);

	tree_node->AddChildNode(tree_node2);
	tree_node_ver_.push_back(tile_layout);
}


void InvokeChatForm::AddListItem(const nim::UserNameCard& all_info, bool is_enable)
{
	wstring ws_show_name = UserService::GetInstance()->GetUserName(all_info.GetAccId());
	string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	wstring ws_spell = nbase::UTF8ToUTF16(spell);
	CInvokeChatTileListUI* tile_layout;
	if (!ws_spell.empty())
	{
		tile_layout = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tile_layout = GetGroup(GT_COMMON_NUMBER);
	}

	ListContainerElement* contain = (ListContainerElement*)AddListItemInGroup(all_info, tile_layout);
	if (contain)
	{
		//contain->AttachSelect(notify);
		CheckBox* ckb = (CheckBox*)contain->FindSubControl(L"checkbox");
		if (!is_enable)
		{
			ckb->Selected(true);
			contain->SetEnabled(false);
		}
	}
}

void InvokeChatForm::RemoveListItem(const nim::UserNameCard& all_info)
{
	wstring ws_show_name = UserService::GetInstance()->GetUserName(all_info.GetAccId());
	string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	wstring ws_spell = nbase::UTF8ToUTF16(spell);
	CInvokeChatTileListUI* tile_layout;
	if (!ws_spell.empty())
	{
		tile_layout = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tile_layout = GetGroup(GT_COMMON_NUMBER);
	}
	RemoveListItemInGroup(all_info, tile_layout);

	//从已选列表和搜索列表中删除
	auto selected_item = selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(all_info.GetAccId()));
	if (selected_item != NULL)
		selected_user_list_->Remove(selected_item);
	auto search_item = search_result_list_->FindSubControl(nbase::UTF8ToUTF16(all_info.GetAccId()));
	if (search_item != NULL)
		search_result_list_->Remove(search_item);
}

Box* InvokeChatForm::AddListItemInGroup(const nim::UserNameCard& all_info, CInvokeChatTileListUI* tile_layout)
{
	if (tile_layout->GetCount() == 0)
	{
		((TreeNode*)tile_layout->GetParent())->GetParentNode()->SetVisible(true);
	}
	CInvokeChatListItemUI* container_element = CreateStartChatListItem(all_info);
	int index = 0;
	for (index = 0; index < tile_layout->GetCount(); index++)
	{
		CInvokeChatListItemUI* temp = (CInvokeChatListItemUI*)tile_layout->GetItemAt(index);
		if (container_element->GetDataID() == temp->GetDataID())
		{
			//ASSERT(FALSE);
			return NULL;
		}
		if (*((ListItemUserData*)container_element) < *((ListItemUserData*)temp))
		{
			tile_layout->AddAt(container_element, index);
			break;
		}
	}

	if (index == tile_layout->GetCount())
	{
		tile_layout->AddAt(container_element, index);
	}

	return container_element;
}


bool InvokeChatForm::RemoveListItemInGroup(const nim::UserNameCard& all_info, CInvokeChatTileListUI* tile_layout)
{
	bool ret = false;
	int index = 0;
	for (index = 0; index < tile_layout->GetCount(); index++)
	{
		CInvokeChatListItemUI* temp = (CInvokeChatListItemUI*)tile_layout->GetItemAt(index);
		if (all_info.GetAccId() == temp->GetUTF8DataID())
		{
			OnCheckBox(all_info.GetAccId(), false); //同时从已选择列表中删除这个帐号
			tile_layout->RemoveAt(index);
			ret = true;
			break;
		}
	}

	if (tile_layout->GetCount() == 0)
	{
		((TreeNode*)tile_layout->GetParent())->GetParentNode()->SetVisible(false);
	}

	return ret;
}


CInvokeChatListItemUI* InvokeChatForm::CreateStartChatListItem(const nim::UserNameCard& user_info)
{
	CInvokeChatListItemUI* container_element = new CInvokeChatListItemUI();
	container_element->Init(user_info);
	
	if (std::find(exclude_ids_.begin(), exclude_ids_.end(), user_info.GetAccId()) != exclude_ids_.end())
	{
		((ui::CheckBox*)container_element->FindSubControl(L"checkbox"))->Selected(true);
		container_element->SetEnabled(false);
	}

	return container_element;
}

CInvokeChatTileListUI* InvokeChatForm::GetGroup(GroupType groupType, wchar_t letter)
{
	return tree_node_ver_[ListItemUtil::GetGroup(groupType, letter)];
}

bool InvokeChatForm::OnSearchEditChange(ui::EventArgs* param)
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
				CInvokeChatListItemUI* user_listitem = (CInvokeChatListItemUI*)it->GetItemAt(i);
				if (user_listitem->Match(search_key))
				{
					ui::CheckBox* checkbox = (ui::CheckBox*)user_listitem->FindSubControl(L"checkbox");
					auto search_listitem = CreateStartChatListItem(user_listitem->user_info);
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

bool InvokeChatForm::OnClearBtnClick(ui::EventArgs* param)
{
	btn_clear_input_->SetVisible(false);
	tool_tip_content_->SetVisible(false);

	search_edit_->SetText(L"");

	return false;
}

bool InvokeChatForm::OnListItemClick(ui::EventArgs* param)
{
	CInvokeChatListItemUI* listitem = dynamic_cast<CInvokeChatListItemUI*>(param->pSender);
	if (listitem)
	{
		UTF8String id = listitem->GetUTF8DataID();
		ui::CheckBox* checkbox = (ui::CheckBox*)listitem->FindSubControl(L"checkbox");
		checkbox->Selected(!checkbox->IsSelected(), false);
		if (!checkbox->IsSelected()) {
			listitem->Selected(false, true);
		}
		OnCheckBox(id, checkbox->IsSelected());
	}

	return true;
}

bool InvokeChatForm::OnSearchResultListItemClick(ui::EventArgs* param)
{
	OnListItemClick(param);

	for (auto& it : tree_node_ver_)
	{
		for (int i = 0; i < it->GetCount(); i++)
		{
			auto listitem = (CInvokeChatListItemUI*)(it->GetItemAt(i));
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

void InvokeChatForm::OnCheckBox(UTF8String id, bool check)
{
	nim::UserNameCard userInfo;
	UserService::GetInstance()->GetUserInfo(id, userInfo);
	if(check) {
		ui::HBox* selected_listitem = CreateSelectedListItem(userInfo);
		selected_user_list_->Add(selected_listitem);
		selected_user_list_->EndDown();
	}
	else {
		for (int i = 0; i < selected_user_list_->GetCount(); i++)
		{
			ui::HBox* listitem = (ui::HBox*)selected_user_list_->GetItemAt(i);
			if (listitem->GetUTF8DataID() == id)
			{
				selected_user_list_->RemoveAt(i);
				break;
			}
		}
	}
}

ui::HBox* InvokeChatForm::CreateSelectedListItem(const nim::UserNameCard& user_info)
{
	ui::HBox* container_element = (ui::HBox*)GlobalManager::CreateBoxWithCache(L"invokechat/user_photo.xml");
	container_element->SetUTF8Name(user_info.GetAccId());
	container_element->SetUTF8DataID(user_info.GetAccId());
	Label* show_name_label = (Label*)container_element->FindSubControl(L"show_name");
	show_name_label->SetText(UserService::GetInstance()->GetUserName(user_info.GetAccId()));
	Button* btn_delete = (Button*)container_element->FindSubControl(L"delete");
	btn_delete->AttachClick(nbase::Bind(&InvokeChatForm::OnBtnDeleteClick, this, user_info.GetAccId(), std::placeholders::_1));

	return container_element;
}

bool InvokeChatForm::OnBtnDeleteClick(const UTF8String& user_id, ui::EventArgs* param)
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
				CInvokeChatListItemUI* item = (CInvokeChatListItemUI*)it->GetItemAt(i);
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

bool InvokeChatForm::OnBtnConfirmClick(ui::EventArgs* param)
{
	std::list<UTF8String> id_list;
	for (int i = 0; i < selected_user_list_->GetCount(); i++)
	{
		UTF8String id = selected_user_list_->GetItemAt(i)->GetUTF8DataID();
		id_list.push_back(id);
	}

	completedCallback_(id_list);
	Close();
	return true;
}

bool InvokeChatForm::OnBtnCancelClick(ui::EventArgs* param)
{
	Close();
	return true;
}

void InvokeChatForm::OnFriendListChange(FriendChangeType change_type, const nim::UserNameCard& user)
{
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	if (current_user_id == user.GetAccId())
		return;

	if (change_type == kChangeTypeAdd)
	{
		if (UserService::GetInstance()->GetUserType(user.GetAccId()) == nim::kNIMFriendFlagNormal 
			&& !MuteBlackService::GetInstance()->IsInBlackList(user.GetAccId()))
		{
			AddListItem(user, true);
		}
	}
	else if (change_type == kChangeTypeDelete)
	{
		RemoveListItem(user);
	}
}

void InvokeChatForm::OnSetBlackCallback(const std::string& id, bool black)
{

	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	if (current_user_id == id)
		return;

	nim::UserNameCard user;
	UserService::GetInstance()->GetUserInfo(id, user);

	if (black)
	{
		RemoveListItem(user);
	}
	else
	{
		AddListItem(user, true);
	}
}
}