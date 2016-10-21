#pragma once

typedef std::function<void(const std::string &, bool)> SetStateCallback;
namespace nim_comp
{
/** @class MuteBlackService
  * @brief 静音与黑名单服务
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/14
  */
class MuteBlackService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(MuteBlackService);
private:
	MuteBlackService();
	~MuteBlackService() {}

public:
	/**
	* 获取静音名单
	* @return std::set<std::string> 被静音的用户id列表
	*/
	const std::set<std::string> &GetMuteList();

	/**
	* 获取黑名单
	* @return std::set<std::string> 被拉入黑名单的用户id列表
	*/
	const std::set<std::string> &GetBlackList();

	/**
	* 判断某个用户是否在静音列表中
	* @param[in] id 用户id
	* @return bool true 是，false 否
	*/
	bool IsInMuteList(const std::string& id);

	/**
	* 判断某个用户是否在黑名单中
	* @param[in] id 用户id
	* @return bool true 是，false 否
	*/
	bool IsInBlackList(const std::string& id);

	/**
	* 改变判断某个用户的静音状态
	* @param[in] id 帐号id
	* @param[in] bool 是否设为静音
	* @return void	无返回值
	*/
	void InvokeSetMute(const std::string& id, bool mute);

	/**
	* 改变判断某个用户的黑名单状态
	* @param[in] id 帐号id
	* @param[in] bool 是否拉入黑名单
	* @return void	无返回值
	*/
	void InvokeSetBlack(const std::string& id, bool black);
	
	/**
	* 注册静音事件的响应函数
	* @param[in] cb 回调函数
	* @return UnregisterCallback	反注册对象
	*/
	UnregisterCallback RegSyncSetMuteCallback(const SetStateCallback& cb);

	/**
	* 注册黑名单事件的响应函数
	* @param[in] cb 回调函数
	* @return UnregisterCallback	反注册对象
	*/
	UnregisterCallback RegSyncSetBlackCallback(const SetStateCallback& cb);

private:
	/**
	* 获取到静音列表的回调函数
	* @param[in] res_code 错误码
	* @param[in] lists 静音列表
	* @return void	无返回值
	*/
	void OnGetMuteListCallback(nim::NIMResCode res_code, const std::list<nim::MuteListInfo>& lists);

	/**
	* 获取到黑名单的回调函数
	* @param[in] res_code 错误码
	* @param[in] lists 黑名单
	* @return void	无返回值
	*/
	void OnGetBlackListCallback(nim::NIMResCode res_code, const std::list<nim::BlackListInfo>& lists);

	/**
	* 收到静音或黑名单事件时的回调函数
	* @param[in] change_event 黑名单和静音名单变更通知
	* @return void	无返回值
	*/
	void OnMuteBlackEventCallback(const nim::SpecialRelationshipChangeEvent& change_event);

	/**
	* 修改类内部保存的静音列表
	* @param[in] id 用户id
	* @param[in] mute 是否设为静音
	* @return void	无返回值
	*/
	void ModifyMuteList(const std::string& id, bool mute);

	/**
	* 修改类内部保存的黑名单
	* @param[in] id 用户id
	* @param[in] mute 是否拉入黑名单
	* @return void	无返回值
	*/
	void ModifyBlackList(const std::string& id, bool black);

private:
	std::set<std::string> mute_list_;
	std::set<std::string> black_list_;
	std::map<int, std::unique_ptr<SetStateCallback>> sync_set_mute_cb_list_;
	std::map<int, std::unique_ptr<SetStateCallback>> sync_set_black_cb_list_;
};
}