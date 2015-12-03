#ifndef INVOKE_CHAT_FORM_H_
#define INVOKE_CHAT_FORM_H_

#include "util/window_ex.h"
#include "module/service/user_service.h"
#include "shared/pin_yin_helper.h"
#include "shared/list_item_util.h"

namespace nim_comp
{
class CInvokeChatListItemUI : public ui::ListContainerElement, public ListItemUserData
{
public:
	CInvokeChatListItemUI()
	{
		ui::GlobalManager::FillBoxWithCache(this, L"invokechat/start_chat_friend.xml");
	}

	void Init(const nim::UserNameCard& _user_info)
	{
		user_info = _user_info;

		SetUTF8Name(user_info.GetAccId());
		SetUTF8DataID(user_info.GetAccId());

		ui::Button* head_image_button = (ui::Button*)FindSubControl(L"head_image");
		ui::Control* head_image_mask = (ui::Control*)FindSubControl(L"headmask");
		head_image_mask->SetClass(L"checkbox_headimage_mask_40x40");
		head_image_button->SetBkImage(UserService::GetInstance()->GetUserPhoto(user_info.GetAccId()));

		ui::Label* show_name_label = (ui::Label*)FindSubControl(L"show_name");
		nick_name = UserService::GetInstance()->GetUserName(user_info.GetAccId());
		show_name_label->SetText(nick_name);

		nick_name = nbase::MakeLowerString(nick_name);
		nick_name_full_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name));
		nick_name_simple_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name));
	}

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

	nim::UserNameCard user_info;
};

class CInvokeChatTileListUI : public ui::ListBox
{
public:
	CInvokeChatTileListUI() : ListBox(new ui::TileLayout)
	{
	}
};

class InvokeChatForm : public WindowEx
{
public:
	typedef std::function<void(const std::list<UTF8String>& selected_ids)> SelectedCompletedCallback;
	//窗口类型，发起聊天窗口和邀请群成员共用
	InvokeChatForm(const UTF8String& uid_or_tid, const std::list<UTF8String>& exclude_ids, const SelectedCompletedCallback& completedCallback);
	virtual ~InvokeChatForm();
	
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	static const LPCTSTR kClassName;
	
private:
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;
	void AddTreeNode(ui::TreeNode* tree_node);
	void AddListItem(const nim::UserNameCard& all_info, bool is_enable);
	void RemoveListItem(const nim::UserNameCard& all_info);
	ui::Box* AddListItemInGroup(const nim::UserNameCard& all_info, CInvokeChatTileListUI* tile_layout);
	bool RemoveListItemInGroup(const nim::UserNameCard& all_info, CInvokeChatTileListUI* tile_layout);
	CInvokeChatListItemUI* CreateStartChatListItem(const nim::UserNameCard& user_info);
	ui::HBox* CreateSelectedListItem(const nim::UserNameCard& user_info);
	bool OnBtnDeleteClick(const UTF8String& user_id, ui::EventArgs* param);
	bool OnBtnConfirmClick(ui::EventArgs* param);
	bool OnBtnCancelClick(ui::EventArgs* param);
	CInvokeChatTileListUI* GetGroup(GroupType groupType, wchar_t letter = L'');
	bool OnSearchEditChange(ui::EventArgs* param);
	bool OnClearBtnClick(ui::EventArgs* param);
	bool OnListItemClick(ui::EventArgs* param);
	bool OnSearchResultListItemClick(ui::EventArgs* param);
	void OnCheckBox(UTF8String id, bool check);

	//好友名单改变的回调
	void OnFriendListChange(FriendChangeType change_type, const nim::UserNameCard& user);
	//黑名单改变时的回调
	void OnSetBlackCallback(const std::string& id, bool black);
	
private:
	UTF8String uid_or_tid_;
	ui::Label *tool_tip_content_;
	ui::Button*   btn_clear_input_;
	std::list<UTF8String> exclude_ids_;
	ui::TreeView* friend_list_;
	vector<CInvokeChatTileListUI*>  tree_node_ver_;
	ui::RichEdit*	search_edit_;
	ui::ListBox* selected_user_list_;
	ui::ListBox* search_result_list_;
	SelectedCompletedCallback completedCallback_;
	AutoUnregister unregister_cb;
};
}
#endif // INVOKE_CHAT_FORM_H_
