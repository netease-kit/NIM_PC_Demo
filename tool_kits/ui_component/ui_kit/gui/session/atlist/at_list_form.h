#pragma once
#include "util/window_ex.h"
#include "at_list_item.h"

typedef std::function<void(const std::string& uid, bool is_robot)> OnSelectAtItem;

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
	/**
	* 构造函数
	* @param[in] session_id 会话ID
	* @param[in] session_type 会话类型
	* @param[in] cb 某项被选中的回调函数
	* @return void	无返回值
	*/
	AtlistForm(std::string session_id, nim::NIMSessionType session_type, OnSelectAtItem cb);
	virtual ~AtlistForm();

	//覆盖虚函数
    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;
    virtual std::wstring GetWindowClassName() const override;
    virtual std::wstring GetWindowId() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;
   
	/**
	* 为@列表初始化群成员信息
	* @param[in] team_member_info_list 群成员信息表
	* @return void 无返回值
	*/
	void InitTeamMembers(const std::map<std::string, nim::TeamMemberProperty>& team_member_info_list);

	/**
	* 为@列表初始化机器人信息
	* @param[in] infos 机器人信息
	* @return void 无返回值
	*/
	void InitRobotInfos(const nim::RobotInfos &infos);

	/**
	* 根据关键字去显示或隐藏@列表项
	* @param[in] search_key 关键字
	* @param[in] match_visible true 只遍历列表中已经显示的项目（当匹配关键字增加时使用）,false 重新遍历所有项目（当匹配的关键字减少时使用）
	* @return bool true 匹配成功，false 匹配失败
	*/
	bool Match(const std::wstring& search_key, bool match_visible);

	/**
	* 输入@后或者删除得只剩下了一个@，重新显示所有的列表项
	* @param[in] last_five 最后5个发言人
	* @return void	无返回值
	*/
	void ShowMemberItems(std::list<std::string> &last_five);

	/**
	* 重置@列表显示的位置，同时具有显示和刷新@列表的功能
	* @param[in] pt 显示坐标
	* @return void	无返回值
	*/
	void SetShowPos(POINT pt);

	/**
	* 会话窗体收到了键盘和鼠标消息后，转给AtListForm处理关心Enter键消息
	* @return bool true 已处理，false 没有处理
	*/
	bool HandleKeyEnter();

	/**
	* 会话窗体收到了键盘和鼠标消息后，转给AtListForm处理关心Down键消息
	* @return bool true 已处理，false 没有处理
	*/
	bool HandleKeyDown();

	/**
	* 会话窗体收到了键盘和鼠标消息后，转给AtListForm处理关心Up键消息
	* @return bool true 已处理，false 没有处理
	*/
	bool HandleKeyUp();

	/**
	* 会话窗体收到了键盘和鼠标消息后，转给AtListForm处理关心鼠标滚轮消息
	* @param[in] is_up 是否向上滚动
	* @return bool true 已处理，false 没有处理
	*/
	bool HandleMouseWheel(bool is_up);

	/**
	* 直接销毁窗口
	* @return void	无返回值
	*/
	void CloseForm();

private:
	/**
	* 拦截并处理窗口关闭消息，让他自动隐藏，调用CloseForm函数直接销毁窗体
	* @param[in] bHandled 是否处理了消息，为true则不继续传递
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;

	/**
	* 创建一个列表项控件
	* @param[in] uid 用户id
	* @param[in] is_last_five 是否为最近发言人
	* @return AtListItem* 列表项控件
	*/
	AtListItem* CreateAtListItem(const std::string& uid, bool is_last_five, bool is_robot);

	/**
	* 添加一个列表项控件到列表
	* @param[in] uid 用户id
	* @param[in] index 插入位置，-1代表插入到末尾
	* @param[in] is_last_five 是否为最近发言人
	* @return AtListItem* 列表项控件
	*/
	AtListItem* AddListItem(const std::string& uid, int index, bool is_last_five, bool is_robot = false);

	/**
	* 移除一个列表项控件
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void RemoveListItem(const std::string& uid);

	/**
	* 刷新某个列表项的头像显示
	* @param[in] uid 用户id
	* @param[in] photo_path 头像路径
	* @return void	无返回值
	*/
	void CheckListItemHeadIcon(const std::string& uid, const std::wstring& photo_path);

	/**
	* 删除最近发言人
	* @return void	无返回值
	*/
	void RemoveLastFiveSender();

	/**
	* 从最近5个发言人里查找
	* @param[in] uid 用户id
	* @return AtListItem* 列表项控件
	*/
	AtListItem* FindInLastFiveSender(const std::wstring& uid);

private:
	/**
	* 获取群昵称
	* @param[in] uid 用户id
	* @return std::wstring	群昵称
	*/
	std::wstring GetTeamCardName(const std::string& uid);

	/**
	* 获取实际显示的名字
	* @param[in] uid 用户id
	* @return std::wstring	名字
	*/
	std::wstring GetShowName(const std::string& uid);

	/**
	* 处理所有控件的选中消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSelectItem(ui::EventArgs *param);
private:
	/**
	* 响应用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] accid 用户id
	* @param[in] photo_path 头像路径
	* @return void	无返回值
	*/
	void OnUserPhotoChange(PhotoType type, const std::string& accid, const std::wstring &photo_path);

	/**
	* 响应用户信息改变的回调函数
	* @param[in] uinfos 用户信息列表
	* @return void	无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* 响应好友改变的回调函数
	* @param[in] change_type 好友改变类型
	* @param[in] accid 用户id
	* @return void	无返回值
	*/
	void OnFriendInfoChange(FriendChangeType change_type, const std::string& accid);

	/**
	* 响应群成员昵称改变的回调函数
	* @param[in] tid_uid 包含群组id和用户id的字符串
	* @param[in] team_card 群昵称
	* @return void	无返回值
	*/
	void OnTeamCardChange(const std::string& tid_uid, const std::string& team_card);

	/**
	* 响应群成员增加的回调函数
	* @param[in] tid 群组id
	* @param[in] team_member_info 群成员信息
	* @return void	无返回值
	*/
	void OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info);

	/**
	* 响应群成员减少的回调函数
	* @param[in] tid 群组id
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void OnTeamMemberRemove(const std::string& tid, const std::string& uid);

public:
    static const LPCTSTR kClassName;

private:
    std::string	session_id_;
	nim::NIMSessionType session_type_;

	nim::RobotInfos robots_info_;
	std::map<std::string, nim::TeamMemberProperty> team_member_info_list_;
	ui::ListBox				*robot_members_container_ = nullptr;
	ui::ListBox				*team_members_container_ = nullptr;

	std::list<std::string>	uid_last_five_;	//最近发消息的5个人（不包括自己）,最新发言的在列表最后

	OnSelectAtItem			callback_select_;//选中某项后的回调函数
	AutoUnregister	unregister_cb;
};
}