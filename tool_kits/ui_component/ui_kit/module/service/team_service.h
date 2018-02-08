#pragma once


//tid, tname
typedef std::function<void(const std::string&, const std::string&, nim::NIMTeamType)> OnTeamAdd;
typedef std::function<void(const std::string&)> OnTeamRemove;
typedef std::function<void(const nim::TeamInfo& team_info)> OnTeamNameChange;

//tid,uid
typedef std::function<void(const std::string&, const nim::TeamMemberProperty&)> OnTeamMemberAdd;
typedef std::function<void(const std::string&, const std::string&)> OnTeamMemberRemove;
typedef std::function<void(const std::string&, const std::string&)> OnTeamMemberChange;

//tid,uid,admin
typedef std::function<void(const std::string&, const std::string&, bool)> OnTeamAdminSet;
typedef std::function<void(const std::string&, const std::string&)> OnTeamOwnerChange;

typedef std::function<void(const nim::TeamInfo& team_info)> OnGetTeamInfo;

typedef OnTeamAdminSet OnMuteMember;

//tid, bits
typedef std::function<void(const std::string&, const int64_t)> OnTeamNotificationModeChange;


namespace nim_comp
{
/** @class TeamService
  * @brief 群组相关服务
  * @copyright (c) 2016, NetEase Inc. All rights reserved

  * @date 2016/09/14
  */
class TeamService : public nbase::SupportWeakCallback
{
public:
	friend void UITeamEventCallback(nim::TeamEvent &info, const std::string& tid, const nim::TeamInfo& team_info);
	friend class TeamCallback;
	SINGLETON_DEFINE(TeamService);
	TeamService() {}
public:

	/**
	* 注册增加群的回调(比如新加入群或者创建了群)
	* @param[in] add 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegAddTeam(OnTeamAdd add);

	/**
	* 注册移除群的回调(比如离开了群)
	* @param[in] remove 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegRemoveTeam(OnTeamRemove remove);

	/**
	* 注册群名称改变的回调
	* @param[in] change 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegChangeTeamName(OnTeamNameChange change);

	/**
	* 注册新增群成员的回调
	* @param[in] add 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegAddTeamMember(OnTeamMemberAdd add);

	/**
	* 注册移除群成员的回调
	* @param[in] remove 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegRemoveTeamMember(OnTeamMemberRemove remove);

	/**
	* 注册群成员信息改变的回调
	* @param[in] change 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegChangeTeamMember(OnTeamMemberChange change);

	/**
	* 注册设置群管理员的回调
	* @param[in] admin 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegSetTeamAdmin(OnTeamAdminSet admin);

	/**
	* 注册群主改变的回调
	* @param[in] set_team_owner 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegChangeTeamOwner(OnTeamOwnerChange set_team_owner);

	/**
	* 注册群成员被静音(或接触静音)改变的回调
	* @param[in] mute 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegMuteMember(OnMuteMember mute);

	/**
	* 注册群消息通知模式改变的回调
	* @param[in] mute 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegChangeTeamNotification(OnTeamNotificationModeChange mute);

	/**
	* 触发增加群的的回调
	* @param[in] tid 群组
	* @param[in] tinfo 群信息
	* @return void	无返回值
	*/
	void InvokeAddTeam(const std::string& tid, const nim::TeamInfo& tinfo);

	/**
	* 触发移除群的的回调
	* @param[in] tid 群组id
	* @return void	无返回值
	*/
	void InvokeRemoveTeam(const std::string& tid);

	/**
	* 触发群名称改变的的回调
	* @param[in] team_info 群信息
	* @return void	无返回值
	*/
	void InvokeChangeTeamName(const nim::TeamInfo& team_info);

	/**
	* 触发新增群成员的的回调
	* @param[in] tid 群组id
	* @param[in] team_member 群成员信息
	* @return void	无返回值
	*/
	void InvokeAddTeamMember(const std::string& tid, const nim::TeamMemberProperty& team_member);

	/**
	* 触发移除群成员的的回调
	* @param[in] tid 群组id
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void InvokeRemoveTeamMember(const std::string& tid, const std::string& uid);

	/**
	* 触发群成员信息改变的的回调
	* @param[in] tid 群组id
	* @param[in] uid 用户id
	* @param[in] team_card 群昵称
	* @return void	无返回值
	*/
	void InvokeChangeTeamMember(const std::string& tid, const std::string& uid, const std::string& team_card);

	/**
	* 触发群管理员改变的的回调
	* @param[in] tid 群组id
	* @param[in] uid 用户id
	* @param[in] admin 是否设置为管理员
	* @return void	无返回值
	*/
	void InvokeChangeTeamAdmin(const std::string& tid, const std::string& uid, bool admin);

	/**
	* 触发群主改变的的回调
	* @param[in] tid 群组id
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void InvokeSetTeamOwner(const std::string& tid, const std::string& uid);

	/**
	* 触发群成员静音的的回调
	* @param[in] tid 群组id
	* @param[in] uid 用户id
	* @param[in] set_mute 是否设置为静音
	* @return void	无返回值
	*/
	void InvokeMuteMember(const std::string& tid, const std::string& uid, bool set_mute);

	/**
	* 触发群消息通知模式的的回调
	* @param[in] tid 群组id
	* @param[in] set_mute 是否设置为静音
	* @return void	无返回值
	*/
	void InvokeChangeNotificationMode(const std::string& tid, const int64_t bits);

	/**
	* 所有群信息同步完成后的回调函数
	* @param[in] sync_type 数据同步类型
	* @param[in] status 数据同步状态
	* @param[in] data_sync_info 数据数据
	* @return void	无返回值
	*/
	void OnTeamDataSyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info);

	/**
	* 获取本地缓存好的所有群的信息
	* @return std::list<nim::TeamInfo> 群信息
	*/
	std::list<nim::TeamInfo> GetCachedTeamsInfo();

	/**
	* 获取群名称
	* @param[in] tid 群组id
	* @return wstring 群名称
	*/
	std::wstring GetTeamName(const std::string& tid);

	/**
	* 获取群头像
	* @param[in] tid 群组id
	* @param[out] icon 群头像
	* @return bool	true 获取成功，false 获取失败
	*/
	bool GetTeamIcon(const std::string& tid, std::string& icon);

	/**
	* 获取群类型
	* @param[in] tid 群组id
	* @return int	0: 普通群, 1: 高级群, -1: 获取不到
	*/
	int GetTeamType(const std::string& tid);

	/**
	* 查询群信息
	* @param[in] tid 群组id
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	void QueryTeamInfo(const std::string& tid, OnGetTeamInfo cb);

	/**
	* 从本地数据库获取所有群信息
	* @return void	无返回值
	*/
	void QueryAllTeamInfo();

	/**
	* 获取所有群信息的回调函数
	* @param[in] team_count 群组数量
	* @param[in] team_info_list 群信息列表
	* @return void	无返回值
	*/
	void QueryAllTeamInfoCb(int team_count, const std::list<nim::TeamInfo>& team_info_list);

	/**
	* 获取所有群信息的回调函数转发到UI线程
	* @param[in] team_count 群组数量
	* @param[in] team_info_list 群信息列表
	* @return void	无返回值
	*/
	void UIQueryAllTeamInfoCb(int team_count, const std::list<nim::TeamInfo>& team_info_list);

	/**
	* 从服务器获取群信息
	* @param[in] tid 群组id
	* @return void	无返回值
	*/
	void GetTeamInfo(const std::string& tid);

	/**
	* 从服务器获取群信息的回调函数
	* @param[in] team_event 群组事件通知
	* @return void	无返回值
	*/
	void GetTeamInfoCb(const nim::TeamEvent& team_event);

	/**
	* 从服务器查询群信息的回调函数
	* @param[in] tid 群组id
	* @param[in] result 群信息
	* @return void	无返回值
	*/
	void GetLocalTeamInfoCb(const std::string& tid, const nim::TeamInfo& result);

	/**
	* 从服务器查询群信息的回调函数转发到UI线程
	* @param[in] tid 群组id
	* @param[in] result 群信息
	* @return void	无返回值
	*/
	void UIGetLocalTeamInfoCb(const std::string& tid, const nim::TeamInfo& result);


private:
	std::map<int, std::unique_ptr<OnTeamAdd>>			add_team_cb_;
	std::map<int, std::unique_ptr<OnTeamRemove>>		remove_team_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberAdd>>		add_team_member_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberRemove>>	remove_team_member_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberChange>>	change_team_member_cb_;
	std::map<int, std::unique_ptr<OnTeamNameChange>>	change_team_name_cb_;
	std::map<int, std::unique_ptr<OnTeamAdminSet>>		change_team_admin_cb_;
	std::map<int, std::unique_ptr<OnTeamOwnerChange>>	set_team_owner_cb_;
	std::map<int, std::unique_ptr<OnMuteMember>>		mute_member_cb_;
	std::map<int, std::unique_ptr<OnTeamNotificationModeChange>>		notification_change_cb_;

	std::map<std::string, nim::TeamInfo> cached_tinfo_;
	std::set<std::string> on_query_tids_; //正在查询其群信息，但还没返回的群id
};
}