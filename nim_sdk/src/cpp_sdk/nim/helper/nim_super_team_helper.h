/** @file nim_super_team_helper.h
  * @brief SuperSuperTeam 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/14
  */

#ifndef _NIM_SDK_CPP_SUPERTEAM_HELPER_H_
#define _NIM_SDK_CPP_SUPERTEAM_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "src/cpp_sdk/nim/helper/nim_user_helper.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @brief 群组信息 */
struct NIM_SDK_CPPWRAPPER_DLL_API SuperTeamInfo
{
public:
	/** 构造函数 */
	SuperTeamInfo(const std::string& team_id)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyID] = team_id;
	}

	/** 构造函数 */
	SuperTeamInfo()
	{

	}

public:
	/** 重载= */
	void operator = (const SuperTeamInfo& new_info)
	{
		Update(new_info);
	}

	/** 更新 */
	void Update(const SuperTeamInfo& new_info)
	{
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyName))
			SetName(new_info.GetName());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyCreator))
			SetOwnerID(new_info.GetOwnerID());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyLevel))
			SetLevel(new_info.GetLevel());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyProperty))
			SetProperty(new_info.GetProperty());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyIntro))
			SetIntro(new_info.GetIntro());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyAnnouncement))
			SetAnnouncement(new_info.GetAnnouncement());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyJoinMode))
			SetJoinMode(new_info.GetJoinMode());
		//群属性,开发者无需关注 20161011 by Oleg
		//if (new_info.ExistValue(kSuperTeamInfoKeyConfigBits))
		//	SetConfigBits(new_info.GetConfigBits());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyCustom))
			SetCustom(new_info.GetCustom());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyIcon))
			SetIcon(new_info.GetIcon());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyBeInviteMode))
			SetBeInviteMode(new_info.GetBeInviteMode());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyInviteMode))
			SetInviteMode(new_info.GetInviteMode());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyUpdateInfoMode))
			SetUpdateInfoMode(new_info.GetUpdateInfoMode());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyUpdateCustomMode))
			SetUpdateCustomMode(new_info.GetUpdateCustomMode());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyID))
			SetSuperTeamID(new_info.GetSuperTeamID());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyValidFlag))
			SetValid(new_info.IsValid());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyMemberValid))
			SetMemberValid(new_info.IsMemberValid());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyMemberCount))
			SetMemberCount(new_info.GetMemberCount());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyListTime))
			SetMemberListTimetag(new_info.GetMemberListTimetag());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyCreateTime))
			SetCreateTimetag(new_info.GetCreateTimetag());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyUpdateTime))
			SetUpdateTimetag(new_info.GetUpdateTimetag());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyServerCustom))
			SetServerCustom(new_info.GetServerCustom());
		if (new_info.ExistValue(kNIMSuperTeamInfoKeyMuteAll) || new_info.ExistValue(kNIMSuperTeamInfoKeyMuteType))
			SetMute(new_info.GetMuteType());
	}

public:
	/** 设置群组ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetSuperTeamID(const std::string& id)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyID] = id;
	}

	/** 获取群组ID */
	std::string GetSuperTeamID() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyID].asString();
	}

	/** 设置群组名字 */
	void SetName(const std::string& name)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyName] = name;
	}

	/** 获取群组名字 */
	std::string GetName() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyName].asString();
	}

	/** 设置群组拥有者ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetOwnerID(const std::string& id)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyCreator] = id;
	}

	/** 获取群组拥有者ID */
	std::string GetOwnerID() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyCreator].asString();
	}
	/** 在创建群时，设置群的最大成员数，不可超过应用设定的最大成员数，否则返回414 */
	void SetMemberMaxCount(int count)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyMemberMaxCount] = count;
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyLevel] = count;
	}

	/** 获取群的最大成员数 */
	int GetMemberMaxCount() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyMemberMaxCount].asUInt();
	}
	/** 设置群组属性 */
	void SetProperty(const std::string& prop)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyProperty] = prop;
	}

	/** 获取群组属性 */
	std::string GetProperty() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyProperty].asString();
	}

	/** 设置群组有效性,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetValid(bool valid)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyValidFlag] = valid ? 1 : 0;
	}

	/** 获取群组有效性 */
	bool IsValid() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyValidFlag].asUInt() == 1;
	}

	/** 设置群组成员数量,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetMemberCount(int count)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyMemberCount] = count;
	}

	/** 获取群组成员数量 */
	int GetMemberCount() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyMemberCount].asUInt();
	}

	/** 设置群组成员档案时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetMemberListTimetag(int64_t timetag)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyListTime] = timetag;
	}

	/** 获取群组成员档案时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	int64_t GetMemberListTimetag() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyListTime].asUInt64();
	}

	/** 设置群组创建时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetCreateTimetag(int64_t timetag)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyCreateTime] = timetag;
	}

	/** 获取群组创建时间戳(毫秒) */
	int64_t GetCreateTimetag() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyCreateTime].asUInt64();
	}

	/** 设置群组更新时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetUpdateTimetag(int64_t timetag)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyUpdateTime] = timetag;
	}

	/** 获取群组更新时间戳(毫秒) */
	int64_t GetUpdateTimetag() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyUpdateTime].asUInt64();
	}

	/** 设置群组成员有效性,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetMemberValid(bool valid)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyMemberValid] = valid ? 1 : 0;
	}

	/** 获取群组成员有效性 */
	bool IsMemberValid() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyMemberValid].asUInt() == 1;
	}

	/** 设置群组简介 */
	void SetIntro(const std::string& intro)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyIntro] = intro;
	}

	/** 获取群组简介 */
	std::string GetIntro() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyIntro].asString();
	}

	/** 设置群组公告 */
	void SetAnnouncement(const std::string& announcement)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyAnnouncement] = announcement;
	}

	/** 获取群组公告 */
	std::string GetAnnouncement() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyAnnouncement].asString();
	}

	/** 设置群组验证模式 */
	void SetJoinMode(nim::NIMSuperTeamJoinMode mode)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyJoinMode] = mode;
	}

	/** 获取群组验证模式 */
	nim::NIMSuperTeamJoinMode GetJoinMode() const
	{
		return (NIMSuperTeamJoinMode)team_info_json_value_[nim::kNIMSuperTeamInfoKeyJoinMode].asUInt();
	}

	/** 设置群组客户端扩展内容 */
	void SetCustom(const std::string& custom)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyCustom] = custom;
	}

	/** 获取群组客户端扩展内容 */
	std::string GetCustom() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyCustom].asString();
	}

	/** 设置群组服务器端扩展内容 */
	void SetServerCustom(const std::string& custom)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyServerCustom] = custom;
	}

	/** 获取群组服务器端扩展内容 */
	std::string GetServerCustom() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyServerCustom].asString();
	}

	/** 设置群头像 */
	void SetIcon(const std::string& icon)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyIcon] = icon;
	}

	/** 获取群头像 */
	std::string GetIcon() const 
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyIcon].asString();
	}

	/** 设置邀请入群是否需要验证*/
	void SetBeInviteMode(int be_invite_mode)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyBeInviteMode] = be_invite_mode;
	}

	/** 获取邀请入群是否需要验证标记 */
	int GetBeInviteMode() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyBeInviteMode].asUInt();
	}

	/** 设置谁可以邀请他人入群 */
	void SetInviteMode(NIMSuperTeamInviteMode mode)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyInviteMode] = mode;
	}

	/** 获取谁可以邀请他人入群 */
	NIMSuperTeamInviteMode GetInviteMode() const
	{
		return (NIMSuperTeamInviteMode)team_info_json_value_[nim::kNIMSuperTeamInfoKeyInviteMode].asUInt();
	}

	/** 设置谁可以修改群资料 */
	void SetUpdateInfoMode(NIMSuperTeamUpdateInfoMode mode)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyUpdateInfoMode] = mode;
	}

	/** 获取谁可以修改群资料 */
	NIMSuperTeamUpdateInfoMode GetUpdateInfoMode() const
	{
		return (NIMSuperTeamUpdateInfoMode)team_info_json_value_[nim::kNIMSuperTeamInfoKeyUpdateInfoMode].asUInt();
	}

	/** 设置谁可以修改群资料属性 */
	void SetUpdateCustomMode(NIMSuperTeamUpdateCustomMode mode)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyUpdateCustomMode] = mode;
	}

	/** 获取谁可以修改群资料属性 */
	NIMSuperTeamUpdateCustomMode GetUpdateCustomMode() const
	{
		return (NIMSuperTeamUpdateCustomMode)team_info_json_value_[nim::kNIMSuperTeamInfoKeyUpdateCustomMode].asUInt();
	}
	/** 设置全员禁言（除管理员） */
	void SetMute(NIMSuperTeamMuteType mute_type)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyMuteType] = mute_type;
	}

	/** 获取群禁言状态 */
	NIMSuperTeamMuteType GetMuteType() const
	{
		if (team_info_json_value_.isMember(nim::kNIMSuperTeamInfoKeyMuteAll) && team_info_json_value_[nim::kNIMSuperTeamInfoKeyMuteType].asUInt() == 1)
			return kNIMSuperTeamMuteTypeNomalMute;
		return (NIMSuperTeamMuteType)team_info_json_value_[nim::kNIMSuperTeamInfoKeyMuteType].asUInt();
	}

	/** @fn bool ExistValue(const std::string& nim_team_info_key) const
	  * @brief 群组信息数据标记Key对应的数据是否有效（存在，非初始值状态）
	  * @param[in] nim_team_info_key 群组信息数据标记Key nim_team_def.h
	  * @return bool 有效性 
	  */
	bool ExistValue(const std::string& nim_team_info_key) const
	{
		return team_info_json_value_.isMember(nim_team_info_key);
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string ToJsonString() const
	{
		return GetJsonStringWithNoStyled(team_info_json_value_);
	}

	/** @fn nim_cpp_wrapper_util::Json::Value ToJsonValue() const
	  * @brief 获取JsonValue格式的数据
	  * @return nim_cpp_wrapper_util::Json::Value 
      */
	nim_cpp_wrapper_util::Json::Value ToJsonValue() const
	{
		return team_info_json_value_;
	}
private:
	/** 设置群组等级,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetLevel(int level)
	{
		team_info_json_value_[nim::kNIMSuperTeamInfoKeyLevel] = level;
	}

	/** 获取群组等级 */
	int GetLevel() const
	{
		return team_info_json_value_[nim::kNIMSuperTeamInfoKeyLevel].asUInt();
	}
private:
	nim_cpp_wrapper_util::Json::Value		team_info_json_value_;
};

/** @brief 群组成员信息 */
struct NIM_SDK_CPPWRAPPER_DLL_API SuperTeamMemberProperty
{
public:
	/** 构造函数 */
	SuperTeamMemberProperty(const std::string& team_id, const std::string& accid, const nim::NIMSuperTeamUserType type)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyID] = team_id;
		member_info_json_value_[kNIMSuperTeamUserKeyAccID] = accid;
		member_info_json_value_[kNIMSuperTeamUserKeyType] = type;
	}

	/** 构造函数 */
	SuperTeamMemberProperty()
	{

	}

public:
	/** 设置群组ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetSuperTeamID(const std::string& id)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyID] = id;
	}

	/** 获取群组ID */
	std::string GetSuperTeamID() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyID].asString();
	}

	/** 设置群成员ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetAccountID(const std::string& id)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyAccID] = id;
	}

	/** 获取群成员ID */
	std::string GetAccountID() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyAccID].asString();
	}

	/** 设置群成员类型 */
	void SetUserType(nim::NIMSuperTeamUserType type)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyType] = type;
	}

	/** 获取群成员类型 */
	nim::NIMSuperTeamUserType GetUserType() const
	{
		return (nim::NIMSuperTeamUserType)member_info_json_value_[kNIMSuperTeamUserKeyType].asUInt();
	}

	/** 设置群成员昵称 */
	void SetNick(const std::string& nick)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyNick] = nick;
	}

	/** 获取群成员昵称 */
	std::string GetNick() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyNick].asString();
	}

	/** 设置群成员配置项 */
	void SetBits(int64_t bit)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyBits] = bit;
	}

	/** 获取群成员配置项 */
	int64_t GetBits() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyBits].asUInt64();
	}

	/** 设置群成员有效性,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetValid(bool valid)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyValidFlag] = valid ? 1 : 0;
	}

	/** 获取群成员有效性 */
	bool IsValid() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyValidFlag].asUInt() == 1;
	}

	/** 设置群成员创建时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetCreateTimetag(int64_t timetag)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyCreateTime] = timetag;
	}

	/** 获取群成员创建时间戳(毫秒) */
	int64_t GetCreateTimetag() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyCreateTime].asUInt64();
	}

	/** 设置群成员更新时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetUpdateTimetag(int64_t timetag)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyUpdateTime] = timetag;
	}

	/** 获取群成员更新时间戳(毫秒) */
	int64_t GetUpdateTimetag() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyUpdateTime].asUInt64();
	}

	/** 设置禁言 */
	void SetMute(bool mute)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyMute] = mute ? 1 : 0;
	}

	/** 是否禁言 */
	bool IsMute() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyMute].asUInt() == 1;
	}

	/** 设置扩展字段 */
	void SetCustom(const std::string& custom)
	{
		member_info_json_value_[kNIMSuperTeamUserKeyCustom] = custom;
	}

	/** 获取扩展字段 */
	std::string GetCustom() const
	{
		return member_info_json_value_[kNIMSuperTeamUserKeyCustom].asString();
	}

	/** @fn bool ExistValue(const std::string& nim_team_user_key) const
	  * @brief 群成员信息信息数据标记Key对应的数据是否有效（存在，非初始值状态）
	  * @param[in] nim_team_user_key 群成员信息数据标记Key nim_team_def.h
	  * @return bool 有效性 
	  */
	bool ExistValue(const std::string& nim_team_user_key) const
	{
		return member_info_json_value_.isMember(nim_team_user_key);
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string ToJsonString() const
	{
		return GetJsonStringWithNoStyled(member_info_json_value_);
	}

	/** @fn nim_cpp_wrapper_util::Json::Value ToJsonValue() const
	  * @brief 获取member info json value
	  * @return nim_cpp_wrapper_util::Json::Value 
      */
	nim_cpp_wrapper_util::Json::Value ToJsonValue() const
	{
		return member_info_json_value_;
	}

private:
	nim_cpp_wrapper_util::Json::Value		member_info_json_value_;
};

/** @brief 群组事件通知 */
struct NIM_SDK_CPPWRAPPER_DLL_API SuperTeamEvent
{
	NIMResCode res_code_;					/**< 错误码 */
	NIMNotificationId notification_id_;		/**< 通知类型ID */
	std::string team_id_;					/**< 群组ID */
	std::list<std::string> ids_;			/**< 通知可能涉及到的群成员ID */
	std::list<std::string> invalid_ids_;	/**< 通知可能涉及到的失效的群成员ID，比如邀请入群的成员的群数量超限导致当次邀请失败 */
	std::list<UserNameCard> namecards_;		/**< 通知可能涉及到的群成员的用户名片 */
	SuperTeamInfo	team_info_;					/**< 通知可能涉及到的群信息 */
	SuperTeamMemberProperty member_property_;	/**< 群成员属性 */
	bool	opt_;							/**< 操作*/
	std::string attach_;					/**< 扩展字段,目前仅kick和invite事件可选*/
	nim_cpp_wrapper_util::Json::Value src_data_;					/**< 未解析过的原信息，目前仅支持群消息未读数相关事件*/
};

/** @fn void ParseSuperTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, SuperTeamEvent& team_event)
  * @brief 解析群组事件通知
  * @param[in] rescode 错误码
  * @param[in] team_id 群组ID
  * @param[in] notification_id 通知类型ID
  * @param[in] team_event_json 通知内容（Json Value数据）
  * @param[out] team_event 群组事件通知
  * @return void 
  */
NIM_SDK_CPPWRAPPER_DLL_API void ParseSuperTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, SuperTeamEvent& team_event);

/** @fn bool ParseSuperTeamInfoJson(const nim_cpp_wrapper_util::Json::Value& team_info_json, SuperTeamInfo& team_info)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据）
  * @param[out] team_info 群组信息
  * @return void 
  */
NIM_SDK_CPPWRAPPER_DLL_API void ParseSuperTeamInfoJson(const nim_cpp_wrapper_util::Json::Value& team_info_json, SuperTeamInfo& team_info);

/** @fn bool ParseSuperTeamInfoJson(const std::string& team_info_json, SuperTeamInfo& team_info)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据字符串）
  * @param[out] team_info 群组信息
  * @return bool 解析成功或失败 
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseSuperTeamInfoJson(const std::string& team_info_json, SuperTeamInfo& team_info);

/** @fn bool ParseSuperTeamInfosJson(const std::string& team_infos_json, std::list<SuperTeamInfo>& team_infos)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据字符串）
  * @param[out] team_infos 群组信息
  * @return bool 解析成功或失败 
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseSuperTeamInfosJson(const std::string& team_infos_json, std::list<SuperTeamInfo>& team_infos);

/** @fn void ParseSuperTeamMemberPropertyJson(const nim_cpp_wrapper_util::Json::Value& team_member_prop_json, SuperTeamMemberProperty& team_member_property)
  * @brief 解析群成员信息
  * @param[in] team_member_prop_json 群成员信息（Json Value数据）
  * @param[out] team_member_property 群成员信息
  * @return void 
  */
NIM_SDK_CPPWRAPPER_DLL_API void ParseSuperTeamMemberPropertyJson(const nim_cpp_wrapper_util::Json::Value& team_member_prop_json, SuperTeamMemberProperty& team_member_property);

/** @fn bool ParseSuperTeamMemberPropertyJson(const std::string& team_member_prop_json, SuperTeamMemberProperty& team_member_property)
  * @brief 解析群成员信息
  * @param[in] team_member_prop_json 群成员信息（Json Value数据字符串）
  * @param[out] team_member_property 群成员信息
  * @return bool 解析成功或失败 
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseSuperTeamMemberPropertyJson(const std::string& team_member_prop_json, SuperTeamMemberProperty& team_member_property);

/** @fn bool ParseSuperTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<SuperTeamMemberProperty>& team_member_propertys)
  * @brief 解析群成员信息
  * @param[in] team_member_props_json 群成员信息（Json Value数据字符串）
  * @param[out] team_member_propertys 群成员信息
  * @return bool 解析成功或失败 
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseSuperTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<SuperTeamMemberProperty>& team_member_propertys);
}//namespace nim

#endif //_NIM_SDK_CPP_SUPERTEAM_HELPER_H_