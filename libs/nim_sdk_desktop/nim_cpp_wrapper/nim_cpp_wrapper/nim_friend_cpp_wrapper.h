#pragma once

#include <string>
#include <list>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_friend_def.h"

struct UserProfile
{
	std::string accid;
	NIMFriendFlag flag;
};

/** @class Friend
  * @brief NIM 好友相关接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author caowei
  * @date 2015/8/17
  */
class Friend
{
public:
	typedef std::function<void(NIMFriendChangeType type, const std::list<nim::UserProfile>& user_profile_list)> FriendChangeCallback;
	typedef std::function<void(int res_code)> FriendOptCallback;
	typedef std::function<void(int res_code, const std::list<nim::UserProfile>& user_profile_list)> GetFriendsListCallback;
	typedef std::function<void(const std::string &accid, const std::string &result_json)> QueryFriendInfoCallback;

public:
	/** 
	* 统一注册好友变更通知回调函数（多端同步添加、删除、更新，好友列表同步）
	* @param[in] cb	好友变更通知回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegChangeCb(const FriendChangeCallback &cb, const std::string& json_extension = "");

	/**
	* 添加、验证好友
	* @param[in] accid	对方账号
	* @param[in] verify_type 好友验证类型
	* @param[in] msg 接口扩展字段
	* @param[in] cb	好友请求回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Request(const std::string &accid, NIMVerifyType verify_type, const std::string &msg, const FriendOptCallback &cb, const std::string& json_extension = "");
	
	/**
	* 删除好友
	* @param[in] accid	对方账号
	* @param[in] cb	删除好友回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Delete(const std::string &accid, const FriendOptCallback &cb, const std::string& json_extension = "");
	
	/**
	* 更新好友资料（自己）
	* @param[in] friend_json 好友资料json字符串
	* @param[in] cb	更新好友资料回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Update(const std::string &friend_json, const FriendOptCallback &cb, const std::string& json_extension = "");
	
	/**
	* 增量获取好友列表
	* @param[in] cb	获取好友列表回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void GetList(const GetFriendsListCallback& cb, const std::string& json_extension = "");
	
};

}