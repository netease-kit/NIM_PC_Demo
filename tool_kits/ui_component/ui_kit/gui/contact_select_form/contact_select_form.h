#ifndef INVOKE_CHAT_FORM_H_
#define INVOKE_CHAT_FORM_H_

#include "util/window_ex.h"
#include "module/service/user_service.h"
#include "module/service/photo_service.h"
#include "shared/pin_yin_helper.h"
#include "shared/list_item_util.h"

namespace nim_comp
{
class SelectedContactItemUI : public ui::HBox
{
public:
	SelectedContactItemUI()
	{
		ui::GlobalManager::FillBoxWithCache(this, L"invokechat/user_photo.xml");
	}
	~SelectedContactItemUI(){};

	void Init(const std::string& accid, bool is_team)
	{
		is_team_ = is_team;
		accid_ = accid;

		SetUTF8Name(accid);
		SetUTF8DataID(accid);
		ui::Label* show_name_label = (ui::Label*)FindSubControl(L"show_name");
		if (is_team)
			show_name_label->SetText(TeamService::GetInstance()->GetTeamName(accid));
		else
			show_name_label->SetText(UserService::GetInstance()->GetUserName(accid));
	}

	bool IsTeam() const { return is_team_; }
	std::string GetAccountID() const { return accid_; }

private:
	std::string accid_;
	bool is_team_;
};


class ContactListItemUI : public ui::ListContainerElement, public ListItemUserData
{
public:
	ContactListItemUI()
	{
		ui::GlobalManager::FillBoxWithCache(this, L"invokechat/start_chat_friend.xml");
	}

	void Init(const std::string& accid, bool is_team)
	{
		is_team_ = is_team;
		accid_ = accid;

		SetUTF8Name(accid);
		SetUTF8DataID(accid);

		ui::Button* head_image_button = (ui::Button*)FindSubControl(L"head_image");
		ui::Control* head_image_mask = (ui::Control*)FindSubControl(L"headmask");
		head_image_mask->SetClass(L"checkbox_headimage_mask_40x40");
		if (is_team)
			head_image_button->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(accid, false));
		else
			head_image_button->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(accid));

		ui::Label* show_name_label = (ui::Label*)FindSubControl(L"show_name");
		if (is_team)
			nick_name = TeamService::GetInstance()->GetTeamName(accid);
		else
			nick_name = UserService::GetInstance()->GetUserName(accid);
		show_name_label->SetText(nick_name);

		nick_name = nbase::MakeLowerString(nick_name);
		nick_name_full_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name));
		nick_name_simple_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name));
	}

	bool IsTeam() const { return is_team_; }
	std::string GetAccountID() const { return accid_; }

	bool Match(const UTF8String& search_key)
	{
		std::wstring ws_search_key = nbase::UTF8ToUTF16(search_key);
		ws_search_key = nbase::MakeLowerString(ws_search_key);
		if (nick_name.find(ws_search_key) != std::wstring::npos
			|| nick_name_full_spell.find(search_key) != UTF8String::npos
			|| nick_name_simple_spell.find(search_key) != UTF8String::npos)
		{
			return true;
		}
		return false;
	}

private:
	std::string accid_;
	bool is_team_;
};

class ContactTileListUI : public ui::ListBox
{
public:
	ContactTileListUI() : ListBox(new ui::TileLayout)
	{
	}
};

class ContactSelectForm : public WindowEx
{
public:
	typedef std::function<void(const std::list<UTF8String>& selected_friends, const std::list<UTF8String>& selected_teams)> SelectedCompletedCallback;
	static const LPCSTR kCreateGroup;	//创建讨论组
	static const LPCSTR kCreateTeam;	//创建高级群
	static const LPCSTR kRetweetMessage;//转发消息

	//窗口类型，发起聊天窗口和邀请群成员共用
	ContactSelectForm(const UTF8String& uid_or_tid, const std::list<UTF8String>& exclude_ids, const SelectedCompletedCallback& completedCallback, bool need_select_group = false);
	virtual ~ContactSelectForm();
	
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;

private:
	void AddGroup(ui::TreeNode* tree_node);

	void AddFriendListItem(const std::string& accid, bool is_enable);
	void RemoveFriendListItem(const std::string& accid);
	
	void AddTeamListItem(const std::string& teamid, bool is_enable);
	void RemoveTeamListItem(const std::string& teamid);

	ui::Box* AddListItemInGroup(const std::string& accid, bool is_team, ContactTileListUI* tile_layout);
	bool RemoveListItemInGroup(const std::string& accid, ContactTileListUI* tile_layout);

	ContactListItemUI* CreateListItem(const std::string& accid, bool is_team);
	SelectedContactItemUI* CreateSelectedListItem(const std::string& accid, bool is_team);
	
	ContactTileListUI* GetGroup(GroupType groupType, wchar_t letter = L'');

private:
	bool OnBtnDeleteClick(const UTF8String& user_id, ui::EventArgs* param);
	bool OnBtnConfirmClick(ui::EventArgs* param);
	bool OnBtnCancelClick(ui::EventArgs* param);
	bool OnSearchEditChange(ui::EventArgs* param);
	bool OnClearBtnClick(ui::EventArgs* param);
	bool OnListItemClick(ui::EventArgs* param);
	bool OnSearchResultListItemClick(ui::EventArgs* param);
	void OnCheckBox(UTF8String id, bool is_team, bool check);

private:
	//好友名单改变的回调
	void OnFriendListChange(FriendChangeType change_type, const std::string& accid);
	//黑名单改变时的回调
	void OnSetBlackCallback(const std::string& id, bool black);
	//用户名片改变回调
	void OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos);
	//用户头像下载完回调
	void OnUserPhotoReady(PhotoType type, const std::string & accid, const std::wstring & photo_path);

	//新增群或者讨论组回调
	void OnAddTeam(const std::string& teamid, const std::string& tname, nim::NIMTeamType type);
	//移除群或者讨论组回调
	void OnRemoveTeam(const std::string& teamid);
	//群或者讨论组名称改变回调
	void OnTeamNameChanged(const nim::TeamInfo& team_info);

public:
	static const LPCTSTR kClassName;
private:
	UTF8String	uid_or_tid_;
	bool		need_select_group_;
	std::list<UTF8String>		exclude_ids_;
	SelectedCompletedCallback	completedCallback_;
	
	ui::Label		*tool_tip_content_;
	ui::Button		*btn_clear_input_;	
	ui::TreeView	*friend_list_;
	vector<ContactTileListUI*>  tree_node_ver_;
	ui::RichEdit	*search_edit_;
	ui::ListBox		*selected_user_list_;
	ui::ListBox		*search_result_list_;
	
	AutoUnregister	unregister_cb;
};
}
#endif // INVOKE_CHAT_FORM_H_
