#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{

/** @class TeamManager
  * @brief 提供群信息获取接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API TeamManager
{
public:
	SINGLETON_DEFINE(TeamManager);

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
	* 获取群名称
	* @param[in] tid 群组id
	* @return wstring 群名称
	*/
	std::wstring GetTeamName(const std::string& tid);

private:
	TeamManager(){};
	~TeamManager(){};
};

}