#include "gui/main/group_list.h"
#include "shared/tool.h"
#include "gui/main/control/friend_item.h"
#include "gui/main/control/session_item.h"
#include "module/service/team_service.h"
#include "module/session/session_manager.h"
#include "gui/profile_form/profile_form.h"

using namespace ui;

namespace nim_comp
{
GroupList::GroupList(ui::TreeView* group_list) :
	group_list_(group_list)
{
	auto add_team_cb = nbase::Bind(&GroupList::OnAddTeam, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(TeamService::GetInstance()->RegAddTeam(add_team_cb));
	auto remove_team_cb = nbase::Bind(&GroupList::OnRemoveTeam, this, std::placeholders::_1);
	unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeam(remove_team_cb));
	auto change_team_name_cb = nbase::Bind(&GroupList::OnTeamNameChanged, this, std::placeholders::_1);
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(change_team_name_cb));
	auto photo_ready_cb = nbase::Bind(&GroupList::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(photo_ready_cb));

	nim::Team::QueryAllMyTeamsInfoAsync(nbase::Bind(&GroupList::OnQueryAllMyTeams, this, std::placeholders::_1, std::placeholders::_2));

	//添加创建和搜索群的listitem，位于列表最开头位置
	ui::TreeNode* create_group_item = (ui::TreeNode*)ui::GlobalManager::CreateBox(L"main/main_create_normal_group_list_item.xml");
	group_list_->GetRootNode()->AddChildNodeAt(create_group_item, 0);

	ui::TreeNode* create_advanced_group_item = (ui::TreeNode*)ui::GlobalManager::CreateBox(L"main/main_create_advanced_group_list_item.xml");
	group_list_->GetRootNode()->AddChildNodeAt(create_advanced_group_item, 1);

	ui::TreeNode* search_group_item = (ui::TreeNode*)ui::GlobalManager::CreateBox(L"main/main_search_group_list_item.xml");
	group_list_->GetRootNode()->AddChildNodeAt(search_group_item, 2);

	//初始化分组项
	ui::TreeNode* tree_node;
	tree_node = ListItemUtil::CreateFirstLetterListItem(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_ADVANCED_TEAM"));
	group_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node_ver_.push_back(tree_node);
	tree_node->SetVisible(false);
	tree_node->SetEnabled(false);

	tree_node = ListItemUtil::CreateFirstLetterListItem(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_NORMAL_TEAM"));
	group_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node_ver_.push_back(tree_node);
	tree_node->SetVisible(false);
	tree_node->SetEnabled(false);

	group_list_->AttachBubbledEvent(kEventReturn, nbase::Bind(&GroupList::OnReturnEventsClick, this, std::placeholders::_1));
}

bool GroupList::OnReturnEventsClick(ui::EventArgs* param)
{
	if (param->Type == kEventReturn)
	{
		FriendItem* item = dynamic_cast<FriendItem*>(param->pSender);
		assert(item);
		if (item)
		{
			SessionManager::GetInstance()->OpenSessionBox(item->GetUTF8DataID(), nim::kNIMSessionTypeTeam);
		}
	}

	return true;
}

void GroupList::OnQueryAllMyTeams(int team_count, const std::list<nim::TeamInfo>& team_info_list)
{
	for (auto it = team_info_list.begin(); it != team_info_list.end(); it++)
	{
		if (it->IsValid() && it->IsMemberValid())
			AddListItem(*it);
	}

	// 批量查询自己在每个群里的成员信息
	if (!team_info_list.empty())
		SessionManager::GetInstance()->QueryMyAllTeamMemberInfos();
}

GroupList::~GroupList()
{

}

void GroupList::AddListItem(const nim::TeamInfo& team_info)
{
	ui::TreeNode* tree_node;
	if (team_info.GetType() == nim::kNIMTeamTypeAdvanced)
	{
		tree_node = tree_node_ver_[0];
	}
	else
	{
		tree_node = tree_node_ver_[1];
	}
	AddListItemInGroup(team_info, tree_node);
}

bool GroupList::OnHeadImageClick(const std::string& uid, ui::EventArgs*)
{
	auto team_info = nim::Team::QueryTeamInfoBlock(uid);
	TeamInfoForm::ShowTeamInfoForm(false, team_info.GetType(), uid, team_info);
	return true;
}

void GroupList::AddListItemInGroup(const nim::TeamInfo& team_info, ui::TreeNode* tree_node)
{
	if (tree_node->GetChildNodeCount() == 0)
	{
		tree_node->SetVisible(true);
	}

	FriendItem* item = new FriendItem;
	ui::GlobalManager::FillBoxWithCache( item, L"main/friend_item.xml" );
	item->Init(kFriendItemTypeTeam, team_info.GetTeamID());
	ui::ButtonBox* head_image = (ui::ButtonBox*)(item->FindSubControl(L"head_image"));
	head_image->AttachClick(nbase::Bind(&GroupList::OnHeadImageClick, this, team_info.GetTeamID(), std::placeholders::_1));
	//tree_node->AddChildNode(item);
	FriendItem* container_element = item;
	std::size_t index = 0;
	for (index = 0; index < tree_node->GetChildNodeCount(); index++)
	{
		FriendItem* temp = (FriendItem*)tree_node->GetChildNode(index);
		if (*container_element == *temp)
		{
			//delete item;
			//ASSERT(FALSE);
			return;
		}
		if (*container_element < *temp)
		{
			tree_node->AddChildNodeAt(container_element, index);
			break;
		}
	}

	if (index == tree_node->GetChildNodeCount())
	{
		tree_node->AddChildNodeAt(container_element, index);
	}
}

void GroupList::OnAddTeam(const std::string& tid, const std::string& tname, nim::NIMTeamType type)
{
	OnRemoveTeam(tid);
	nim::TeamInfo user_info;
	user_info.SetTeamID(tid);
	user_info.SetName(tname);
	user_info.SetType(type);
	AddListItem(user_info);
}

void GroupList::OnRemoveTeam(const std::string& tid)
{
	for (int i = 0; i < group_list_->GetCount(); i++)
	{
		FriendItem* friend_item = dynamic_cast<FriendItem*>(group_list_->GetItemAt(i));
		if (friend_item)
		{
			if (friend_item->GetId() == tid)
			{
				friend_item->GetParentNode()->RemoveChildNode(friend_item);
				break;
			}
		}
	}
}

void GroupList::OnTeamNameChanged(const nim::TeamInfo& team_info)
{
	for (int i = 0; i < group_list_->GetCount(); i++)
	{
		FriendItem* friend_item = dynamic_cast<FriendItem*>(group_list_->GetItemAt(i));
		if (friend_item)
		{
			if (friend_item->GetId() == team_info.GetTeamID())
			{
				friend_item->OnTeamNameChange(team_info);
				break;
			}
		}
	}
}

void GroupList::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	if (type == kTeam)
	{
		FriendItem* friend_item = (FriendItem*)group_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (friend_item)
		{
			if (friend_item->GetId() == accid)
				friend_item->FindSubControl(L"head_image")->SetBkImage(photo_path);
		}
	}
}

}