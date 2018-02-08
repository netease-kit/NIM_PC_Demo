#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

const std::string kAppServerAddress = "http://app.netease.im";

//好友变化类型
enum FriendChangeType
{
	kChangeTypeAdd,
	kChangeTypeDelete,
	kChangeTypeUpdate
};

//用户性别
enum UserGender
{
	UG_UNKNOWN,
	UG_MALE,
	UG_FEMALE
};

typedef std::function<void(int res, const std::string& err_msg)> OnRegisterAccountCallback;
typedef std::function<void(FriendChangeType change_type, const std::string& accid)> OnFriendListChangeCallback;
typedef std::function<void(const std::list<nim::UserNameCard>&)> OnUserInfoChangeCallback;
typedef std::function<void(const std::list<nim::UserNameCard>&)> OnGetUserInfoCallback;
typedef std::function<void(nim::NIMResCode res)> OnUpdateUserInfoCallback;

const static char* g_AppServerAddress = "kAppServerAddress";
const static char* g_AppKey = "kAppKey";

/**
* 获取连接服务器的某一个配置信息
* @param[in] key 需要获取的信息关键字
* @return string 配置信息
*/
std::string GetConfigValue(const std::string& key);

/**
* 获取应用的app key
* @return string 配置信息app key
*/
std::string GetConfigValueAppKey();

/**
* 应用的app key是否为网易云信Demo本身的app key
* @return bool true:是,false:否
*/
bool IsNimDemoAppKey(const std::string& appkey);

namespace nim_comp
{
/** @class UserService
  * @brief 用户注册、用户信息查询服务
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/14
  */
class UserService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(UserService);
	UserService();

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
	* 向数据库和服务器获取全部好友以及近期获取过的用户信息
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	void InvokeGetAllUserInfo(const OnGetUserInfoCallback& cb);

	/**
	* 向数据库和服务器获取指定用户以及近期获取过的用户信息
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	void InvokeGetAllUserInfo(const std::list<std::string>& accids, const OnGetUserInfoCallback& cb);

	/**
	* 向数据库获取全部机器人以及近期获取过的机器人信息
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	nim::RobotInfos InvokeGetAllRobotsInfoBlock();

	/**
	* 修改用户自己的个人信息
	* @param[in] new_info 新的用户信息
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	void InvokeUpdateMyInfo(const nim::UserNameCard &new_info, const OnUpdateUserInfoCallback& cb);

	/**
	* 修改用户自己的头像
	* @param[in] new_info 新的用户信息
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	void InvokeUpdateMyPhoto(const std::string &url, const OnUpdateUserInfoCallback& cb);

	/**
	* 获取本地保存的所有用户信息
	* @return std::map<std::string, nim::UserNameCard>& 用户信息列表
	*/
	const std::map<std::string, nim::UserNameCard>& GetAllUserInfos();

	/**
	* 获取本地保存的所有好友信息
	* @return std::map<std::string, nim::FriendProfile>& 好友信息列表
	*/
	const std::map<std::string, nim::FriendProfile>& GetAllFriendInfos();

	/**
	* 获取用户信息,如果查询不到则查询服务器
	* @param[in] id 用户id
	* @param[out] info 用户信息
	* @return bool true 查询到，false 没有查询到
	*/
	bool GetUserInfo(const std::string &id, nim::UserNameCard &info);

	/**
	* 获取本地的机器人信息
	* @param[in] id 机器人云信ID
	* @param[out] info 机器人信息
	* @return bool true 查询到，false 没有查询到
	*/
	bool GetRobotInfo(const std::string &id, nim::RobotInfo &info);

	/**
	* 获取本地的所有的机器人信息
	* @param[out] info 机器人信息
	* @return bool true 查询到，false 没有查询到
	*/
	bool GetAllRobotInfo(nim::RobotInfos &info);

	/**
	* 初始化聊天室的所有的机器人信息（匿名聊天室）
	* @param[in] room_id 聊天室ID
	* @return
	*/
	void InitChatroomRobotInfos(long long room_id);

	/**
	* 获取Id对应的用户是不是机器人
	* @param[in] accid 机器人云信ID
	* @return bool true 是，false 不是
	*/
	bool IsRobotAccount(const std::string &accid);

	/**
	* 获取用户信息,如果查询不到则查询服务器
	* @param[in] ids 用户id列表
	* @param[out] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void GetUserInfos(const std::list<std::string> &ids, std::list<nim::UserNameCard>&uinfos);

	/**
	* 执行批量查询用户信息操作,在可能要使用某些用户信息前进行批量查询,用于优化用户信息查询操作
	* @param[in] ids 用户id列表
	* @return void 无返回值
	*/
	void DoQueryUserInfos(const std::set<std::string>& ids);

	/**
	* 执行批量查询用户信息操作,在可能要使用某些用户信息前进行批量查询,用于优化用户信息查询操作
	* @param[in] ids 用户id列表
	* @return void 无返回值
	*/
	void DoQueryUserInfos(const std::list<std::string>& ids);

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
	* 获取好友扩展字段
	* @param[in] id 用户id
	* @return Json::Value 用户扩展字段
	*/
	Json::Value GetUserCustom(const std::string &id);

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
	* 注册机器人列表改变的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegRobotListChange(const nim::Robot::RobotChangedCallback& callback);

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

	/**
	* 好友列表改变的回调
	* @param[in] change_event 好友变更事件
	* @return void 无返回值
	*/
	void OnFriendListChangeBySDK(const nim::FriendChangeEvent& change_event);

	/**
	* 同步好友列表的回调
	* @param[in] change_event 同步好友列表事件
	* @return void 无返回值
	*/
	void OnSyncFriendList(const nim::FriendChangeEvent& change_event);

	/**
	* 用户名、头像改变的回调(由SDk触发)
	* @param[in] uinfo_list 用户名片列表
	* @return void 无返回值
	*/
	void OnUserInfoChangeBySDK(const std::list<nim::UserNameCard> &uinfo_list);

	/**
	* 用户名、头像改变的回调（由Demo触发）
	* @param[in] uinfo_list 用户名片列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfo_list);

	/**
	* 响应机器人信息改变的回调函数
	* @param[in] rescode 错误码
	* @param[in] type 类型
	* @param[in] robots 机器人列表
	* @return void 无返回值
	*/
	void OnRobotChange(nim::NIMResCode rescode, nim::NIMRobotInfoChangeType type, const nim::RobotInfos& robots);
private:
	/**
	* 向数据库和服务器获取指定id的用户信息
	* @param[in] account_list 用户id列表
	* @return void	无返回值
	*/
	void InvokeGetUserInfo(const std::list<std::string>& account_list);

	/**
	* 触发好友列表变更的的回调
	* @param[in] change_type 好友变化类型
	* @param[in] accid 用户id
	* @return void	无返回值
	*/
	void InvokeFriendListChangeCallback(FriendChangeType change_type, const std::string& accid);

private:
	std::map<std::string, nim::UserNameCard> all_user_; //好友+陌生人
	std::set<std::string> post_ids_; //已经发出请求的id们
	std::map<std::string, nim::FriendProfile> friend_list_; //好友列表
	std::map<std::string, nim::RobotInfo> robot_list_; //机器人列表
	std::map<int, std::unique_ptr<OnFriendListChangeCallback>> friend_list_change_cb_list_; //有好友增减回调列表
	std::map<int, std::unique_ptr<OnUserInfoChangeCallback>> uinfo_change_cb_list_; //用户名、头像变化回调列表
	std::map<int, std::unique_ptr<OnUserInfoChangeCallback>> misc_uinfo_change_cb_list_; //用户其他信息变化回调列表
	std::map<int, std::unique_ptr<nim::Robot::RobotChangedCallback>> robot_change_cb_list_; //机器人变化回调列表

	std::set<std::string> on_query_list_; //已经要求查询，但还未返回结果的
};

}
