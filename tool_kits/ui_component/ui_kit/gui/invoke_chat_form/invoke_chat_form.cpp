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

	auto user_info_change_cb = nbase::Bind(&InvokeChatForm::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));

	auto user_photo_ready_cb = nbase::Bind(&InvokeChatForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(user_photo_ready_cb));
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
			AddListItem(it->second.GetAccId(), true);
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


void InvokeChatForm::AddListItem(const std::string& accid, bool is_enable)
{
	wstring ws_show_name = UserService::GetInstance()->GetUserName(accid);
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

	ListContainerElement* contain = (ListContainerElement*)AddListItemInGroup(accid, tile_layout);
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

void InvokeChatForm::RemoveListItem(const std::string& accid)
{
	wstring ws_show_name = UserService::GetInstance()->GetUserName(accid);
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
	RemoveListItemInGroup(accid, tile_layout);

	//从已选列表和搜索列表中删除
	auto selected_item = selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
	if (selected_item != NULL)
		selected_user_list_->Remove(selected_item);
	auto search_item = search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
	if (search_item != NULL)
		search_result_list_->Remove(search_item);
}

ui::Box* InvokeChatForm::AddListItemInGroup(const std::string& accid, CInvokeChatTileListUI* tile_layout)
{
	if (tile_layout->GetCount() == 0)
	{
		((TreeNode*)tile_layout->GetParent())->GetParentNode()->SetVisible(true);
	}
	CInvokeChatListItemUI* container_element = CreateStartChatListItem(accid);
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


bool InvokeChatForm::RemoveListItemInGroup(const std::string& accid, CInvokeChatTileListUI* tile_layout)
{
	bool ret = false;
	int index = 0;
	for (index = 0; index < tile_layout->GetCount(); index++)
	{
		CInvokeChatListItemUI* temp = (CInvokeChatListItemUI*)tile_layout->GetItemAt(index);
		if (accid == temp->GetUTF8DataID())
		{
			OnCheckBox(accid, false); //同时从已选择列表中删除这个帐号
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

CInvokeChatListItemUI* InvokeChatForm::CreateStartChatListItem(const std::string& accid)
{
	CInvokeChatListItemUI* container_element = new CInvokeChatListItemUI();
	container_element->Init(accid);
	
	if (std::find(exclude_ids_.begin(), exclude_ids_.end(), accid) != exclude_ids_.end())
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
					auto search_listitem = CreateStartChatListItem(user_listitem->accid_);
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
	if(check) {
		ui::HBox* selected_listitem = CreateSelectedListItem(id);
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

ui::HBox* InvokeChatForm::CreateSelectedListItem(const std::string& accid)
{
	ui::HBox* container_element = (ui::HBox*)GlobalManager::CreateBoxWithCache(L"invokechat/user_photo.xml");
	container_element->SetUTF8Name(accid);
	container_element->SetUTF8DataID(accid);
	Label* show_name_label = (Label*)container_element->FindSubControl(L"show_name");
	show_name_label->SetText(UserService::GetInstance()->GetUserName(accid));
	Button* btn_delete = (Button*)container_element->FindSubControl(L"delete");
	btn_delete->AttachClick(nbase::Bind(&InvokeChatForm::OnBtnDeleteClick, this, accid, std::placeholders::_1));

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

void InvokeChatForm::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	if (current_user_id == accid)
		return;

	if (change_type == kChangeTypeAdd)
	{
		if (UserService::GetInstance()->GetUserType(accid) == nim::kNIMFriendFlagNormal
			&& !MuteBlackService::GetInstance()->IsInBlackList(accid))
		{
			AddListItem(accid, true);
		}
	}
	else if (change_type == kChangeTypeDelete)
	{
		RemoveListItem(accid);
	}
}

void InvokeChatForm::OnSetBlackCallback(const std::string& id, bool black)
{
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	if (current_user_id == id)
		return;

	if (black)
		RemoveListItem(id);
	else
		AddListItem(id, true);
}

void InvokeChatForm::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	UserService* user_service = UserService::GetInstance();
	for (auto info : uinfos)
	{
		std::string accid = info.GetAccId();
		if (user_service->GetUserType(accid) == nim::kNIMFriendFlagNormal && !MuteBlackService::GetInstance()->IsInBlackList(accid))
		{
			//好友列表项
			CInvokeChatListItemUI* friend_item = dynamic_cast<CInvokeChatListItemUI*>(friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
			if(!friend_item)
				continue;

			//搜索列表项
			CInvokeChatListItemUI* search_item = NULL;
			if(search_result_list_->IsVisible())
				search_item = dynamic_cast<CInvokeChatListItemUI*>(search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));

			//选中列表项
			ui::HBox* select_item = dynamic_cast<ui::HBox*>(selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
			
			if (info.ExistValue(nim::kUserNameCardKeyName))
			{
				bool enabled = friend_item->IsEnabled();
				bool checked = ((CheckBox*)friend_item->FindSubControl(L"checkbox"))->IsSelected();
				Box* tile_box = friend_item->GetParent();
				tile_box->Remove(friend_item); //删除
				if (tile_box->GetCount() == 0) //该字母下没有item了，隐藏bar
					dynamic_cast<TreeNode*>(tile_box->GetParent())->GetParentNode()->SetVisible(false);
				AddListItem(accid, enabled); //重新添加
				friend_item = dynamic_cast<CInvokeChatListItemUI*>(friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
				((CheckBox*)friend_item->FindSubControl(L"checkbox"))->Selected(checked);
				
				if (search_item)
				{
					UTF8String search_key = search_edit_->GetUTF8Text();
					if (friend_item->Match(search_key)) //名字变了但依然符合搜索条件
						search_item->Init(accid);
					else
						search_result_list_->Remove(search_item);
				}

				if (select_item)
					((Label*)select_item->FindSubControl(L"show_name"))->SetText(user_service->GetUserName(accid));
			}
			
			if(info.ExistValue(nim::kUserNameCardKeyIconUrl))
			{
				friend_item->FindSubControl(L"head_image")->SetBkImage(user_service->GetUserPhoto(accid));

				if(search_item)
					search_item->FindSubControl(L"head_image")->SetBkImage(user_service->GetUserPhoto(accid));
			}
		}
	}
}

void InvokeChatForm::OnUserPhotoReady(const std::string & accid, const std::wstring & photo_path)
{
	UserService* user_service = UserService::GetInstance();
	if (user_service->GetUserType(accid) == nim::kNIMFriendFlagNormal && !MuteBlackService::GetInstance()->IsInBlackList(accid))
	{
		CInvokeChatListItemUI* friend_item = dynamic_cast<CInvokeChatListItemUI*>(friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
		if (friend_item)
			friend_item->FindSubControl(L"head_image")->SetBkImage(photo_path);

		if (search_result_list_->IsVisible())
		{
			CInvokeChatListItemUI* search_item = dynamic_cast<CInvokeChatListItemUI*>(search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
			if(search_item)
				search_item->FindSubControl(L"head_image")->SetBkImage(photo_path);
		}
	}
}

}