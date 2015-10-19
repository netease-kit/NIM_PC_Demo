#pragma once


#include <string>
#include <list>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_user_def.h"

/** @class User
  * @brief NIM SDK提供的用户相关接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/8/17
  */
class User
{

public:
	typedef std::function<void(const std::string& json_params)> SetStatCallback;
	typedef std::function<void(NIMUserSpecialRelationshipChangeType type, const std::string& result_json)> SpecialRelationshipChangedCallback;
	typedef std::function<void(int res_code, const std::string& accid, bool opt)> SetBlackCallback;
	typedef std::function<void(int res_code, const std::string& accid, bool opt)> SetMuteCallback;
	typedef std::function<void(int res_code, const std::string& black_list_json)> GetMuteAndBlacklistCallback;

	typedef std::function<void(const std::string& result_json)> UserInfoChangedCallback;
	typedef std::function<void(const std::string& result_json)> GetUserInfoCallback;
	typedef std::function<void(int res_code)> UpdateUserInfoCallback;

	/** 
	* 顺风环境专用更新用户自定义在线状态（公网版不开发该接口），传入的JSON参数json_extension定义见nim_user_def.h
	* @param[in] user_stat 自定义用户状态，值意义由第三方自定义
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 更新用户自定义在线状态的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
	* @return void 无返回值
	*/
	static void SetStat(unsigned int user_stat, const SetStatCallback& cb, const std::string& json_extension = "");

	/** 
	* 统一注册用户属性变更通知回调函数（多端同步黑名单、静音名单变更）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void RegSpecialRelationshipChangedCb(const SpecialRelationshipChangedCallback& cb, const std::string& json_extension = "");

	/** 
	* 设置、取消设置黑名单
	* @param[in] accid 好友id
	* @param[in] set_black 取消或设置
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void SetBlack(const std::string& accid, bool set_black, const SetBlackCallback& cb, const std::string& json_extension = "");

	/** 
	* 设置、取消设置静音名单
	* @param[in] accid 好友id
	* @param[in] set_mute 取消或设置
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void SetMute(const std::string& accid, bool set_mute, const SetMuteCallback& cb, const std::string& json_extension = "");

	/** 
	* 获得黑名单/静音名单
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void GetMuteAndBlacklist(const GetMuteAndBlacklistCallback& cb, const std::string& json_extension = "");



	/**
	* 统一注册用户信息变更通知回调函数
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegUserInfoChangedCb(const UserInfoChangedCallback& cb, const std::string& json_extension = "");

	/**
	* 获取本地的指定账号的用户信息
	* @param[in] accids 例 ["litianyi01", "litianyi02", ...]
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void GetUserInfo(const std::list<std::string>& accids, const GetUserInfoCallback& cb, const std::string& json_extension = "");

	/**
	* 在线查询指定账号的用户信息
	* @param[in] accids 例 ["litianyi01", "litianyi02", ...]
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void GetUserInfoOnline(const std::list<std::string>& accids, const GetUserInfoCallback& cb, const std::string& json_extension = "");

	/**
	* 更新用户信息
	* @param[in] info_json 用户信息json
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void UpdateUserInfo(const std::string& info_json, const UpdateUserInfoCallback& cb, const std::string& json_extension = "");
};

}