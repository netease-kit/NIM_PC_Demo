/** @file nim_team_helper.h
  * @brief Team 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/14
  */

#ifndef _NIM_SDK_CPP_TEAM_HELPER_H_
#define _NIM_SDK_CPP_TEAM_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_sdk_util.h"
#include "nim_user_helper.h"
#include "nim_sdk_defines.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @brief 群组信息 */
struct TeamInfo
{
public:
	/** 构造函数 */
	TeamInfo(const std::string& team_id, const nim::NIMTeamType type)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyID] = team_id;
		team_info_json_value_[nim::kNIMTeamInfoKeyType] = type;
	}

	/** 构造函数 */
	TeamInfo()
	{

	}

public:
	void operator = (const TeamInfo& new_info)
	{
		Update(new_info);
	}

	void Update(const TeamInfo& new_info)
	{
		if (new_info.ExistValue(kNIMTeamInfoKeyName))
			SetName(new_info.GetName());
		if (new_info.ExistValue(kNIMTeamInfoKeyType))
			SetType(new_info.GetType());
		if (new_info.ExistValue(kNIMTeamInfoKeyCreator))
			SetOwnerID(new_info.GetOwnerID());
		if (new_info.ExistValue(kNIMTeamInfoKeyLevel))
			SetLevel(new_info.GetLevel());
		if (new_info.ExistValue(kNIMTeamInfoKeyProperty))
			SetProperty(new_info.GetProperty());
		if (new_info.ExistValue(kNIMTeamInfoKeyIntro))
			SetIntro(new_info.GetIntro());
		if (new_info.ExistValue(kNIMTeamInfoKeyAnnouncement))
			SetAnnouncement(new_info.GetAnnouncement());
		if (new_info.ExistValue(kNIMTeamInfoKeyJoinMode))
			SetJoinMode(new_info.GetJoinMode());
		//群属性,开发者无需关注 20161011 by Oleg
		//if (new_info.ExistValue(kTeamInfoKeyConfigBits))
		//	SetConfigBits(new_info.GetConfigBits());
		if (new_info.ExistValue(kNIMTeamInfoKeyCustom))
			SetCustom(new_info.GetCustom());
		if (new_info.ExistValue(kNIMTeamInfoKeyIcon))
			SetIcon(new_info.GetIcon());
		if (new_info.ExistValue(kNIMTeamInfoKeyBeInviteMode))
			SetBeInviteMode(new_info.GetBeInviteMode());
		if (new_info.ExistValue(kNIMTeamInfoKeyInviteMode))
			SetInviteMode(new_info.GetInviteMode());
		if (new_info.ExistValue(kNIMTeamInfoKeyUpdateInfoMode))
			SetUpdateInfoMode(new_info.GetUpdateInfoMode());
		if (new_info.ExistValue(kNIMTeamInfoKeyUpdateCustomMode))
			SetUpdateCustomMode(new_info.GetUpdateCustomMode());
		if (new_info.ExistValue(kNIMTeamInfoKeyID))
			SetTeamID(new_info.GetTeamID());
		if (new_info.ExistValue(kNIMTeamInfoKeyValidFlag))
			SetValid(new_info.IsValid());
		if (new_info.ExistValue(kNIMTeamInfoKeyMemberValid))
			SetMemberValid(new_info.IsMemberValid());
		if (new_info.ExistValue(kNIMTeamInfoKeyMemberCount))
			SetMemberCount(new_info.GetMemberCount());
		if (new_info.ExistValue(kNIMTeamInfoKeyListTime))
			SetMemberListTimetag(new_info.GetMemberListTimetag());
		if (new_info.ExistValue(kNIMTeamInfoKeyCreateTime))
			SetCreateTimetag(new_info.GetCreateTimetag());
		if (new_info.ExistValue(kNIMTeamInfoKeyUpdateTime))
			SetUpdateTimetag(new_info.GetUpdateTimetag());
		if (new_info.ExistValue(kNIMTeamInfoKeyServerCustom))
			SetServerCustom(new_info.GetServerCustom());
		if (new_info.ExistValue(kNIMTeamInfoKeyMuteAll))
			SetAllMemberMute(new_info.IsAllMemberMute());
	}

public:
	/** 设置群组ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetTeamID(const std::string& id)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyID] = id;
	}

	/** 获取群组ID */
	std::string GetTeamID() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyID].asString();
	}

	/** 设置群组名字 */
	void SetName(const std::string& name)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyName] = name;
	}

	/** 获取群组名字 */
	std::string GetName() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyName].asString();
	}

	/** 设置群组类型 */
	void SetType(nim::NIMTeamType type)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyType] = type;
	}

	/** 获取群组类型 */
	nim::NIMTeamType GetType() const
	{
		return (NIMTeamType)team_info_json_value_[nim::kNIMTeamInfoKeyType].asUInt();
	}

	/** 设置群组拥有者ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetOwnerID(const std::string& id)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyCreator] = id;
	}

	/** 获取群组拥有者ID */
	std::string GetOwnerID() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyCreator].asString();
	}

	/** 设置群组等级,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetLevel(int level)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyLevel] = level;
	}

	/** 获取群组等级 */
	int GetLevel() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyLevel].asUInt();
	}

	/** 设置群组属性 */
	void SetProperty(const std::string& prop)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyProperty] = prop;
	}

	/** 获取群组属性 */
	std::string GetProperty() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyProperty].asString();
	}

	/** 设置群组有效性,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetValid(bool valid)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyValidFlag] = valid ? 1 : 0;
	}

	/** 获取群组有效性 */
	bool IsValid() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyValidFlag].asUInt() == 1;
	}

	/** 设置群组成员数量,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetMemberCount(int count)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyMemberCount] = count;
	}

	/** 获取群组成员数量 */
	int GetMemberCount() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyMemberCount].asUInt();
	}

	/** 设置群组成员档案时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetMemberListTimetag(int64_t timetag)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyListTime] = timetag;
	}

	/** 获取群组成员档案时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	int64_t GetMemberListTimetag() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyListTime].asUInt64();
	}

	/** 设置群组创建时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetCreateTimetag(int64_t timetag)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyCreateTime] = timetag;
	}

	/** 获取群组创建时间戳(毫秒) */
	int64_t GetCreateTimetag() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyCreateTime].asUInt64();
	}

	/** 设置群组更新时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetUpdateTimetag(int64_t timetag)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyUpdateTime] = timetag;
	}

	/** 获取群组更新时间戳(毫秒) */
	int64_t GetUpdateTimetag() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyUpdateTime].asUInt64();
	}

	/** 设置群组成员有效性,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetMemberValid(bool valid)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyMemberValid] = valid ? 1 : 0;
	}

	/** 获取群组成员有效性 */
	bool IsMemberValid() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyMemberValid].asUInt() == 1;
	}

	/** 设置群组简介 */
	void SetIntro(const std::string& intro)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyIntro] = intro;
	}

	/** 获取群组简介 */
	std::string GetIntro() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyIntro].asString();
	}

	/** 设置群组公告 */
	void SetAnnouncement(const std::string& announcement)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyAnnouncement] = announcement;
	}

	/** 获取群组公告 */
	std::string GetAnnouncement() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyAnnouncement].asString();
	}

	/** 设置群组验证模式 */
	void SetJoinMode(nim::NIMTeamJoinMode mode)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyJoinMode] = mode;
	}

	/** 获取群组验证模式 */
	nim::NIMTeamJoinMode GetJoinMode() const
	{
		return (NIMTeamJoinMode)team_info_json_value_[nim::kNIMTeamInfoKeyJoinMode].asUInt();
	}

	/** 群属性,开发者无需关注 20161011 by Oleg */
	//void SetConfigBits(int64_t bit)
	//{
	//	team_info_json_value_[nim::kNIMTeamInfoKeyBits] = bit;
	//}

	/** 获取群属性,开发者无需关注 20161011 by Oleg */
	//int64_t GetConfigBits() const
	//{
	//	return team_info_json_value_[nim::kNIMTeamInfoKeyBits].asUInt64();
	//}

	/** 设置群组客户端扩展内容 */
	void SetCustom(const std::string& custom)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyCustom] = custom;
	}

	/** 获取群组客户端扩展内容 */
	std::string GetCustom() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyCustom].asString();
	}

	/** 设置群组服务器端扩展内容 */
	void SetServerCustom(const std::string& custom)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyServerCustom] = custom;
	}

	/** 获取群组服务器端扩展内容 */
	std::string GetServerCustom() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyServerCustom].asString();
	}

	/** 设置群头像 */
	void SetIcon(const std::string& icon)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyIcon] = icon;
	}

	/** 获取群头像 */
	std::string GetIcon() const 
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyIcon].asString();
	}

	/** 设置被邀请人同意方式 */
	void SetBeInviteMode(NIMTeamBeInviteMode mode)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyBeInviteMode] = mode;
	}

	/** 获得被邀请人同意方式 */
	NIMTeamBeInviteMode GetBeInviteMode() const
	{
		return (NIMTeamBeInviteMode)team_info_json_value_[nim::kNIMTeamInfoKeyBeInviteMode].asUInt();
	}

	/** 设置谁可以邀请他人入群 */
	void SetInviteMode(NIMTeamInviteMode mode)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyInviteMode] = mode;
	}

	/** 获取谁可以邀请他人入群 */
	NIMTeamInviteMode GetInviteMode() const
	{
		return (NIMTeamInviteMode)team_info_json_value_[nim::kNIMTeamInfoKeyInviteMode].asUInt();
	}

	/** 设置谁可以修改群资料 */
	void SetUpdateInfoMode(NIMTeamUpdateInfoMode mode)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyUpdateInfoMode] = mode;
	}

	/** 获取谁可以修改群资料 */
	NIMTeamUpdateInfoMode GetUpdateInfoMode() const
	{
		return (NIMTeamUpdateInfoMode)team_info_json_value_[nim::kNIMTeamInfoKeyUpdateInfoMode].asUInt();
	}

	/** 设置谁可以修改群资料属性 */
	void SetUpdateCustomMode(NIMTeamUpdateCustomMode mode)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyUpdateCustomMode] = mode;
	}

	/** 获取谁可以修改群资料属性 */
	NIMTeamUpdateCustomMode GetUpdateCustomMode() const
	{
		return (NIMTeamUpdateCustomMode)team_info_json_value_[nim::kNIMTeamInfoKeyUpdateCustomMode].asUInt();
	}

	/** 设置全员禁言（除管理员） */
	void SetAllMemberMute(bool mute)
	{
		team_info_json_value_[nim::kNIMTeamInfoKeyMuteAll] = mute ? 1: 0;
	}

	/** 是否全员禁言（除管理员） */
	bool IsAllMemberMute() const
	{
		return team_info_json_value_[nim::kNIMTeamInfoKeyMuteAll].asUInt() == 1;
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

	/** @fn Json::Value ToJsonValue() const
	  * @brief 获取JsonValue格式的数据
	  * @return Json::Value 
      */
	Json::Value ToJsonValue() const
	{
		return team_info_json_value_;
	}

private:
	Json::Value		team_info_json_value_;
};

/** @brief 群组成员信息 */
struct TeamMemberProperty
{
public:
	/** 构造函数 */
	TeamMemberProperty(const std::string& team_id, const std::string& accid, const nim::NIMTeamUserType type)
	{
		member_info_json_value_[kNIMTeamUserKeyID] = team_id;
		member_info_json_value_[kNIMTeamUserKeyAccID] = accid;
		member_info_json_value_[kNIMTeamUserKeyType] = type;
	}

	/** 构造函数 */
	TeamMemberProperty()
	{

	}

public:
	/** 设置群组ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetTeamID(const std::string& id)
	{
		member_info_json_value_[kNIMTeamUserKeyID] = id;
	}

	/** 获取群组ID */
	std::string GetTeamID() const
	{
		return member_info_json_value_[kNIMTeamUserKeyID].asString();
	}

	/** 设置群成员ID,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetAccountID(const std::string& id)
	{
		member_info_json_value_[kNIMTeamUserKeyAccID] = id;
	}

	/** 获取群成员ID */
	std::string GetAccountID() const
	{
		return member_info_json_value_[kNIMTeamUserKeyAccID].asString();
	}

	/** 设置群成员类型 */
	void SetUserType(nim::NIMTeamUserType type)
	{
		member_info_json_value_[kNIMTeamUserKeyType] = type;
	}

	/** 获取群成员类型 */
	nim::NIMTeamUserType GetUserType() const
	{
		return (nim::NIMTeamUserType)member_info_json_value_[kNIMTeamUserKeyType].asUInt();
	}

	/** 设置群成员昵称 */
	void SetNick(const std::string& nick)
	{
		member_info_json_value_[kNIMTeamUserKeyNick] = nick;
	}

	/** 获取群成员昵称 */
	std::string GetNick() const
	{
		return member_info_json_value_[kNIMTeamUserKeyNick].asString();
	}

	/** 设置群成员配置项 */
	void SetBits(int64_t bit)
	{
		member_info_json_value_[kNIMTeamUserKeyBits] = bit;
	}

	/** 获取群成员配置项 */
	int64_t GetBits() const
	{
		return member_info_json_value_[kNIMTeamUserKeyBits].asUInt64();
	}

	/** 设置群成员有效性,通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetValid(bool valid)
	{
		member_info_json_value_[kNIMTeamUserKeyValidFlag] = valid ? 1 : 0;
	}

	/** 获取群成员有效性 */
	bool IsValid() const
	{
		return member_info_json_value_[kNIMTeamUserKeyValidFlag].asUInt() == 1;
	}

	/** 设置群成员创建时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetCreateTimetag(int64_t timetag)
	{
		member_info_json_value_[kNIMTeamUserKeyCreateTime] = timetag;
	}

	/** 获取群成员创建时间戳(毫秒) */
	int64_t GetCreateTimetag() const
	{
		return member_info_json_value_[kNIMTeamUserKeyCreateTime].asUInt64();
	}

	/** 设置群成员更新时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
	void SetUpdateTimetag(int64_t timetag)
	{
		member_info_json_value_[kNIMTeamUserKeyUpdateTime] = timetag;
	}

	/** 获取群成员更新时间戳(毫秒) */
	int64_t GetUpdateTimetag() const
	{
		return member_info_json_value_[kNIMTeamUserKeyUpdateTime].asUInt64();
	}

	/** 设置禁言 */
	void SetMute(bool mute)
	{
		member_info_json_value_[kNIMTeamUserKeyMute] = mute ? 1 : 0;
	}

	/** 是否禁言 */
	bool IsMute() const
	{
		return member_info_json_value_[kNIMTeamUserKeyMute].asUInt() == 1;
	}

	/** 设置扩展字段 */
	void SetCustom(const std::string& custom)
	{
		member_info_json_value_[kNIMTeamUserKeyCustom] = custom;
	}

	/** 获取扩展字段 */
	std::string GetCustom() const
	{
		return member_info_json_value_[kNIMTeamUserKeyCustom].asString();
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

	/** @fn Json::Value ToJsonValue() const
	  * @brief 获取member info json value
	  * @return Json::Value 
      */
	Json::Value ToJsonValue() const
	{
		return member_info_json_value_;
	}

private:
	Json::Value		member_info_json_value_;
};

/** @brief 群组事件通知 */
struct TeamEvent
{
	NIMResCode res_code_;					/**< 错误码 */
	NIMNotificationId notification_id_;		/**< 通知类型ID */
	std::string team_id_;					/**< 群组ID */
	std::list<std::string> ids_;			/**< 通知可能涉及到的群成员ID */
	std::list<std::string> invalid_ids_;	/**< 通知可能涉及到的失效的群成员ID，比如邀请入群的成员的群数量超限导致当次邀请失败 */
	std::list<UserNameCard> namecards_;		/**< 通知可能涉及到的群成员的用户名片 */
	TeamInfo	team_info_;					/**< 通知可能涉及到的群信息 */
	TeamMemberProperty member_property_;	/**< 群成员属性 */
	bool	opt_;							/**< 操作*/
	std::string attach_;					/**< 扩展字段,目前仅kick和invite事件可选*/
};

/** @fn void ParseTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, TeamEvent& team_event)
  * @brief 解析群组事件通知
  * @param[in] rescode 错误码
  * @param[in] team_id 群组ID
  * @param[in] notification_id 通知类型ID
  * @param[in] team_event_json 通知内容（Json Value数据）
  * @param[out] team_event 群组事件通知
  * @return void 
  */
void ParseTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, TeamEvent& team_event);

/** @fn bool ParseTeamInfoJson(const Json::Value& team_info_json, TeamInfo& team_info)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据）
  * @param[out] team_info 群组信息
  * @return void 
  */
void ParseTeamInfoJson(const Json::Value& team_info_json, TeamInfo& team_info);

/** @fn bool ParseTeamInfoJson(const std::string& team_info_json, TeamInfo& team_info)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据字符串）
  * @param[out] team_info 群组信息
  * @return bool 解析成功或失败 
  */
bool ParseTeamInfoJson(const std::string& team_info_json, TeamInfo& team_info);

/** @fn bool ParseTeamInfosJson(const std::string& team_infos_json, std::list<TeamInfo>& team_infos)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据字符串）
  * @param[out] team_infos 群组信息
  * @return bool 解析成功或失败 
  */
bool ParseTeamInfosJson(const std::string& team_infos_json, std::list<TeamInfo>& team_infos);

/** @fn void ParseTeamMemberPropertyJson(const Json::Value& team_member_prop_json, TeamMemberProperty& team_member_property)
  * @brief 解析群成员信息
  * @param[in] team_member_prop_json 群成员信息（Json Value数据）
  * @param[out] team_member_property 群成员信息
  * @return void 
  */
void ParseTeamMemberPropertyJson(const Json::Value& team_member_prop_json, TeamMemberProperty& team_member_property);

/** @fn bool ParseTeamMemberPropertyJson(const std::string& team_member_prop_json, TeamMemberProperty& team_member_property)
  * @brief 解析群成员信息
  * @param[in] team_member_prop_json 群成员信息（Json Value数据字符串）
  * @param[out] team_member_property 群成员信息
  * @return bool 解析成功或失败 
  */
bool ParseTeamMemberPropertyJson(const std::string& team_member_prop_json, TeamMemberProperty& team_member_property);

/** @fn bool ParseTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<TeamMemberProperty>& team_member_propertys)
  * @brief 解析群成员信息
  * @param[in] team_member_props_json 群成员信息（Json Value数据字符串）
  * @param[out] team_member_propertys 群成员信息
  * @return bool 解析成功或失败 
  */
bool ParseTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<TeamMemberProperty>& team_member_propertys);
}//namespace nim

#endif //_NIM_SDK_CPP_TEAM_HELPER_H_