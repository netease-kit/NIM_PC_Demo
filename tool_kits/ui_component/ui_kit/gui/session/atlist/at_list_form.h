#pragma once
#include "util/window_ex.h"
#include "at_list_item.h"

typedef std::function<void(const std::string& uid)> OnSelectAtItem;

namespace nim_comp
{
/** @class AtlistForm
* @brief @列表窗口
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2016/9/6
*/
class AtListItem;
class AtlistForm : public WindowEx
{
public:
	AtlistForm(std::string tid, OnSelectAtItem cb);
	virtual ~AtlistForm();

    //接口实现
    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;
    virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
    virtual std::wstring GetZIPFileName() const;
    //覆盖虚函数
    virtual std::wstring GetWindowClassName() const override;
    virtual std::wstring GetWindowId() const override;

	virtual void InitWindow() override;
   
	// @param match_visible:为true时只遍历列表中已经显示的项目（当匹配关键字增加时使用）
	//						为false时重新遍历所有项目（当匹配的关键字减少时使用）
	// 返回值：当存在匹配项时返回true
	bool Match(const std::wstring& search_key, bool match_visible);

	// 输入@后或者删除得只剩下了一个@，就重新显示所有的列表项
	void ShowAllItems(std::list<std::string> &last_five);

	// 重置@列表显示的位置，同时具有显示和刷新@列表的功能
	void SetShowPos(POINT pt);

	// 会话窗体收到了键盘和鼠标消息后，转给AtListForm处理关心的键盘消息
	bool HandleKeyEnter();
	bool HandleKeyDown();
	bool HandleKeyUp();
	bool HandleMouseWheel(bool is_up);

	// 直接销毁窗口
	void CloseForm();

private:
	// 重写了WM_CLOSE消息响应，让他自动隐藏，调用CloseForm函数直接销毁窗体
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;
	
	/**
	* 获取了群成员信息后自动调用的回调函数
	* @param[in] tid	群ID
	* @param[in] count	群成员数
	* @param[in] team_member_info_list 群成员信息表
	* @return void 无返回值
	*/
	void OnGetTeamMemberCb(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list);

	// @param is_last_five:是否创建的是最近5个发言人的item
	AtListItem* CreateAtListItem(const std::string& uid, bool is_last_five = false);
	// @param is_last_five : 是否创建的是最近5个发言人的item
	// @param index : -1代表插入到末尾
	AtListItem* AddListItem(const std::string& uid, int index = -1, bool is_last_five = false);
	void RemoveListItem(const std::string& uid);

	// 刷新某个列表项的头像显示
	void CheckListItemHeadIcon(const std::string& uid, const std::wstring& photo_path);

	// 删除最近发言人
	void RemoveLastFiveSender();
	// 从最近5个发言人里查找
	AtListItem* FindInLastFiveSender(const std::wstring& uid);

private:
	std::wstring GetTeamCardName(const std::string& uid);
	std::wstring GetShowName(const std::string& uid);
	bool OnSelectItem(ui::EventArgs *param);
private:
	// 监听群成员和好友信息的改变
	void OnUserPhotoChange(PhotoType type, const std::string& accid, const std::wstring &photo_path);

	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnFriendInfoChange(FriendChangeType change_type, const std::string& accid);
	void OnTeamCardChange(const std::string& tid_uid, const std::string& team_card);

	void OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info);
	void OnTeamMemberRemove(const std::string& tid, const std::string& uid);

public:
    static const LPCTSTR kClassName;

private:
    std::string	team_id_;

	std::map<std::string, nim::TeamMemberProperty> team_member_info_list_;
	ui::ListBox				*team_members_container_;

	std::list<std::string>	uid_last_five_;	//最近发消息的5个人（不包括自己）,最新发言的在列表最后

	OnSelectAtItem			callback_select_;//选中某项后的回调函数
	AutoUnregister	unregister_cb;
};
}