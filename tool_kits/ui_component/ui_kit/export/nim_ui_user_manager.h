#pragma once

#include "nim_ui_dll.h"
#include "module/service/user_service.h"
#include "tool_kits/base/memory/singleton.h"

namespace nim_ui
{

/** @class UserManager
  * @brief 提供用户数据获取接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API UserManager
{
public:
	SINGLETON_DEFINE(UserManager);
	UserManager(){};
	~UserManager(){};
public:
	/**
	* 注册一个新账号
	* @param[in] username 要注册的用户名
	* @param[in] password 用户密码
	* @param[in] nickname 用户昵称
	* @param[in] cb 注册完毕的回调通知函数
	* @return void	无返回值
	*/
	void InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb);

	/**
	* 获取本地保存的所有用户信息
	* @return std::map<std::string, nim::UserNameCard>& 用户信息列表
	*/
	const std::map<std::string, nim::UserNameCard>& GetAllUserInfos();
	/**
	* 查询本地保存的用户信息
	* @param[in] id 用户id
	* @param[out] info 用户信息
	* @return bool true 查询到，false 没有查询到
	*/
	bool GetUserInfo(const std::string &id, nim::UserNameCard &info);

	/**
	* 查询本地保存的用户信息
	* @param[in] ids 用户id列表
	* @param[out] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void GetUserInfos(const std::list<std::string> &ids, std::list<nim::UserNameCard>&uinfos);

	/**
	* 获取某个用户的好友类型
	* @param[in] id 用户id
	* @return NIMFriendFlag 好友类型
	*/
	nim::NIMFriendFlag GetUserType(const std::string &id);

	/**
	* 获取好友昵称或者备注名
	* @param[in] id 用户id
	* @param[in] alias_prior 是否有限查备注名
	* @return wstring 用户昵称或备注名
	*/
	std::wstring GetUserName(const std::string &id, bool alias_prior = true);

	/**
	* 获取好友备注名
	* @param[in] id 用户id
	* @return wstring 用户备注名
	*/
	std::wstring GetFriendAlias(const std::string &id);

	/**
	* 注册好友列表改变的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegFriendListChange(const OnFriendListChangeCallback& callback);

	/**
	* 注册用户名、头像改变的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegUserInfoChange(const OnUserInfoChangeCallback& callback);

	/**
	* 注册用户其他信息改变的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegMiscUInfoChange(const OnUserInfoChangeCallback& callback);
};

}