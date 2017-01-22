#pragma once

namespace nim_comp
{

/** @class ForcePushManager
  * @brief 强推消息管理类，用于统一管理强推消息（比如@消息）
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/12/5
  */
class ForcePushManager : public nbase::SupportWeakCallback
{
public:
	struct ForcePushInfo
	{
		std::string msg_body;
		std::string sender_id;
		std::string uuid;
	};

	SINGLETON_DEFINE(ForcePushManager);
	ForcePushManager(){};
	~ForcePushManager(){};

	/**
	* 从本地数据库读取强推信息
	* @return void	无返回值
	*/
	void Load();

	/**
	* 保存强推信息到本地数据库
	* @return void	无返回值
	*/
	void Save();

	/**
	* 会话消息是否包含@me信息
	* @param[in] msg 会话消息
	* @return bool true 包含，false 不包含
	*/
	bool IsAtMeMsg(const nim::IMMessage &msg);

	/**
	* 为某个会话添加@消息
	* @param[in] session_id 会话id
	* @param[in] msg 会话消息
	* @return void 无返回值
	*/
	void AddAtMeMsg(const std::string &session_id, const nim::IMMessage &msg);

	/**
	* 获取某个会话的@消息
	* @param[in] session_id 会话id
	* @param[out] infos 强推信息
	* @return bool true 成功，false 失败
	*/
	bool GetAtMeMsgs(const std::string &session_id, std::vector<ForcePushInfo> &infos);

	/**
	* 重置某个会话的@消息数为0
	* @param[in] session_id 会话id
	* @return void 无返回值
	*/
	void ResetAtMeMsg(const std::string &session_id);

	/**
	* 查找某个会话中是否包含未读的@我的消息
	* @param[in] session_id 会话id
	* @return bool true 包含，false 不包含
	*/
	bool IsContainAtMeMsg(const std::string &session_id);

private:

	std::map<std::string, std::vector<ForcePushInfo>> session_id_atme_;	// 会话是否包含未读的@me消息

};
}