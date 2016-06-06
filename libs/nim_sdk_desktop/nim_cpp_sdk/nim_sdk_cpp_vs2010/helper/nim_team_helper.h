/** @file nim_team_helper.h
  * @brief Team 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/14
  */

#ifndef _NIM_SDK_CPP_TEAM_HELPER_H_
#define _NIM_SDK_CPP_TEAM_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_sdk_helper.h"
#include "nim_user_helper.h"

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

#include "nim_team_def.h"
#include "nim_msglog_def.h"
#include "nim_res_code_def.h"

/** @brief 群组信息数据标记Key,用以标记对应数据的有效性 */
enum TeamInfoKey
{
	kTeamInfoKeyNone = 0,					/**< 无数据 */	
	kTeamInfoKeyName = 1,					/**< 群组名字 */
	kTeamInfoKeyType = 1 << 1,				/**< 群组类型 */
	kTeamInfoKeyOwnerID = 1 << 2,			/**< 群组拥有者ID */
	kTeamInfoKeyLevel = 1 << 3,				/**< 群组等级 */
	kTeamInfoKeyProperty = 1 << 4,			/**< 群组属性 */
	kTeamInfoKeyIntro = 1 << 5,				/**< 群组简介 */
	kTeamInfoKeyAnnouncement = 1 << 6,		/**< 群组公告 */
	kTeamInfoKeyJoinMode = 1 << 7,			/**< 群组验证类型 */
	kTeamInfoKeyConfigBits = 1 << 8,		/**< 群组配置项 */
	kTeamInfoKeyCustom = 1 << 9,			/**< 群组扩展项 */
	kTeamInfoKeyIcon = 1 << 10,				/**< 群头像 */
	kTeamInfoKeyBeInviteMode = 1 << 11,		/**< 被邀请人同意方式 */
	kTeamInfoKeyInviteMode = 1 << 12,		/**< 谁可以邀请他人入群 */
	kTeamInfoKeyUpdateInfoMode = 1 << 13,	/**< 谁可以修改群资料 */
	kTeamInfoKeyUpdateCustomMode = 1 << 14,	/**< 谁可以更新群自定义属性 */
	kTeamInfoKeyAll = (1 << 15) - 1			/**< 有数据 */
};

/** @brief 群组信息 */
struct TeamInfo
{
public:
	/** 构造函数，推荐使用 */
	TeamInfo(const std::string& team_id) : member_list_timetag_(0)
		, create_timetag_(0)
		, update_timetag_(0)
		, member_count_(0)
		, config_bits_(0)
		, level_(0)
		, valid_(false)
		, type_(kNIMTeamTypeNormal)
		, member_valid_(false)
		, join_mode_(kNIMTeamJoinModeNoAuth) 
		, value_available_flag_(0)
		, be_invite_mode_(kNIMTeamBeInviteModeNeedAgree)
		, invite_mode_(kNIMTeamInviteModeManager)
		, update_info_mode_(kNIMTeamUpdateInfoModeManager)
		, update_custom_mode_(kNIMTeamUpdateCustomModeManager)
	{
		id_ = team_id;
	}

	/** 构造函数 */
	TeamInfo() : member_list_timetag_(0)
			, create_timetag_(0)
			, update_timetag_(0)
			, member_count_(0)
			, config_bits_(0)
			, level_(0)
			, valid_(false)
			, type_(kNIMTeamTypeNormal)
			, member_valid_(false)
			, join_mode_(kNIMTeamJoinModeNoAuth) 
			, value_available_flag_(0) 
			, be_invite_mode_(kNIMTeamBeInviteModeNeedAgree)
			, invite_mode_(kNIMTeamInviteModeManager)
			, update_info_mode_(kNIMTeamUpdateInfoModeManager)
			, update_custom_mode_(kNIMTeamUpdateCustomModeManager){}

public:
	void operator = (const TeamInfo& new_info)
	{
		Update(new_info);
	}

	void Update(const TeamInfo& info)
	{
		if (info.ExistValue(kTeamInfoKeyName))
			SetName(info.GetName());
		if (info.ExistValue(kTeamInfoKeyType))
			SetType(info.GetType());
		if (info.ExistValue(kTeamInfoKeyOwnerID))
			SetOwnerID(info.GetOwnerID());
		if (info.ExistValue(kTeamInfoKeyLevel))
			SetLevel(info.GetLevel());
		if (info.ExistValue(kTeamInfoKeyProperty))
			SetProperty(info.GetProperty());
		if (info.ExistValue(kTeamInfoKeyIntro))
			SetIntro(info.GetIntro());
		if (info.ExistValue(kTeamInfoKeyAnnouncement))
			SetAnnouncement(info.GetAnnouncement());
		if (info.ExistValue(kTeamInfoKeyJoinMode))
			SetJoinMode(info.GetJoinMode());
		if (info.ExistValue(kTeamInfoKeyConfigBits))
			SetConfigBits(info.GetConfigBits());
		if (info.ExistValue(kTeamInfoKeyCustom))
			SetCustom(info.GetCustom());
		if (info.ExistValue(kTeamInfoKeyIcon))
			SetIcon(info.GetIcon());
		if (info.ExistValue(kTeamInfoKeyBeInviteMode))
			SetBeInviteMode(info.GetBeInviteMode());
		if (info.ExistValue(kTeamInfoKeyInviteMode))
			SetInviteMode(info.GetInviteMode());
		if (info.ExistValue(kTeamInfoKeyUpdateInfoMode))
			SetUpdateInfoMode(info.GetUpdateInfoMode());
		if (info.ExistValue(kTeamInfoKeyUpdateCustomMode))
			SetUpdateCustomMode(info.GetUpdateCustomMode());

		SetTeamID(info.GetTeamID());
		SetValid(info.IsValid());
		if (info.GetMemberCount() > -1)
			SetMemberCount(info.GetMemberCount());
		if (info.GetMemberListTimetag() > -1)
			SetMemberListTimetag(info.GetMemberListTimetag());
		if (info.GetCreateTimetag() > -1)
			SetCreateTimetag(info.GetCreateTimetag());
		if (info.GetUpdateTimetag() > -1)
			SetUpdateTimetag(info.GetUpdateTimetag());
		if (!info.GetServerCustom().empty())
			SetServerCustom(info.GetServerCustom());
	}

public:
	/** 设置群组ID */
	void SetTeamID(const std::string& id)
	{
		id_ = id;
	}

	/** 获取群组ID */
	std::string GetTeamID() const
	{
		return id_;
	}

	/** 设置群组名字 */
	void SetName(const std::string& name)
	{
		name_ = name;
		value_available_flag_ |= kTeamInfoKeyName;
	}

	/** 获取群组名字 */
	std::string GetName() const
	{
		return name_;
	}

	/** 设置群组类型 */
	void SetType(nim::NIMTeamType type)
	{
		type_ = type;
		value_available_flag_ |= kTeamInfoKeyType;
	}

	/** 获取群组类型 */
	nim::NIMTeamType GetType() const
	{
		return type_;
	}

	/** 设置群组拥有者ID */
	void SetOwnerID(const std::string& id)
	{
		owner_id_ = id;
		value_available_flag_ |= kTeamInfoKeyOwnerID;
	}

	/** 获取群组拥有者ID */
	std::string GetOwnerID() const
	{
		return owner_id_;
	}

	/** 设置群组等级 */
	void SetLevel(int level)
	{
		level_ = level;
		value_available_flag_ |= kTeamInfoKeyLevel;
	}

	/** 获取群组等级 */
	int GetLevel() const
	{
		return level_;
	}

	/** 设置群组属性 */
	void SetProperty(const std::string& prop)
	{
		property_ = prop;
		value_available_flag_ |= kTeamInfoKeyProperty;
	}

	/** 获取群组属性 */
	std::string GetProperty() const
	{
		return property_;
	}

	/** 设置群组有效性 */
	void SetValid(bool valid)
	{
		valid_ = valid;
	}

	/** 获取群组有效性 */
	bool IsValid() const
	{
		return valid_;
	}

	/** 设置群组成员数量 */
	void SetMemberCount(int count)
	{
		member_count_ = count;
	}

	/** 获取群组成员数量 */
	int GetMemberCount() const
	{
		return member_count_;
	}

	/** 设置群组成员档案时间戳(毫秒) */
	void SetMemberListTimetag(__int64 timetag)
	{
		member_list_timetag_ = timetag;
	}

	/** 获取群组成员档案时间戳(毫秒) */
	__int64 GetMemberListTimetag() const
	{
		return member_list_timetag_;
	}

	/** 设置群组创建时间戳(毫秒) */
	void SetCreateTimetag(__int64 timetag)
	{
		create_timetag_ = timetag;
	}

	/** 获取群组创建时间戳(毫秒) */
	__int64 GetCreateTimetag() const
	{
		return create_timetag_;
	}

	/** 设置群组更新时间戳(毫秒) */
	void SetUpdateTimetag(__int64 timetag)
	{
		update_timetag_ = timetag;
	}

	/** 获取群组更新时间戳(毫秒) */
	__int64 GetUpdateTimetag() const
	{
		return update_timetag_;
	}

	/** 设置群组成员有效性 */
	void SetMemberValid(bool valid)
	{
		member_valid_ = valid;
	}

	/** 获取群组成员有效性 */
	bool IsMemberValid() const
	{
		return member_valid_;
	}

	/** 设置群组简介 */
	void SetIntro(const std::string& intro)
	{
		intro_ = intro;
		value_available_flag_ |= kTeamInfoKeyIntro;
	}

	/** 获取群组简介 */
	std::string GetIntro() const
	{
		return intro_;
	}

	/** 设置群组公告 */
	void SetAnnouncement(const std::string& announcement)
	{
		announcement_ = announcement;
		value_available_flag_ |= kTeamInfoKeyAnnouncement;
	}

	/** 获取群组公告 */
	std::string GetAnnouncement() const
	{
		return announcement_;
	}

	/** 设置群组验证模式 */
	void SetJoinMode(nim::NIMTeamJoinMode mode)
	{
		join_mode_ = mode;
		value_available_flag_ |= kTeamInfoKeyJoinMode;
	}

	/** 获取群组验证模式 */
	nim::NIMTeamJoinMode GetJoinMode() const
	{
		return join_mode_;
	}

	/** 设置群组配置项 */
	void SetConfigBits(__int64 bit)
	{
		config_bits_ = bit;
		value_available_flag_ |= kTeamInfoKeyConfigBits;
	}

	/** 获取群组配置项 */
	__int64 GetConfigBits() const
	{
		return config_bits_;
	}

	/** 设置群组客户端扩展内容 */
	void SetCustom(const std::string& custom)
	{
		custom_ = custom;
		value_available_flag_ |= kTeamInfoKeyCustom;
	}

	/** 获取群组客户端扩展内容 */
	std::string GetCustom() const
	{
		return custom_;
	}

	/** 设置群组服务器端扩展内容 */
	void SetServerCustom(const std::string& custom)
	{
		server_custom_ = custom;
	}

	/** 获取群组服务器端扩展内容 */
	std::string GetServerCustom() const
	{
		return server_custom_;
	}

	/** 设置群头像 */
	void SetIcon(const std::string& icon)
	{
		icon_ = icon;
		value_available_flag_ |= kTeamInfoKeyIcon;
	}

	/** 获取群头像 */
	std::string GetIcon() const 
	{
		return icon_;
	}

	/** 设置被邀请人同意方式 */
	void SetBeInviteMode(NIMTeamBeInviteMode mode)
	{
		be_invite_mode_ = mode;
		value_available_flag_ |= kTeamInfoKeyBeInviteMode;
	}

	/** 获得被邀请人同意方式 */
	NIMTeamBeInviteMode GetBeInviteMode() const
	{
		return be_invite_mode_;
	}

	/** 设置谁可以邀请他人入群 */
	void SetInviteMode(NIMTeamInviteMode mode)
	{
		invite_mode_ = mode;
		value_available_flag_ |= kTeamInfoKeyInviteMode;
	}

	/** 获取谁可以邀请他人入群 */
	NIMTeamInviteMode GetInviteMode() const
	{
		return invite_mode_;
	}

	/** 设置谁可以修改群资料 */
	void SetUpdateInfoMode(NIMTeamUpdateInfoMode mode)
	{
		update_info_mode_ = mode;
		value_available_flag_ |= kTeamInfoKeyUpdateInfoMode;
	}

	/** 获取谁可以修改群资料 */
	NIMTeamUpdateInfoMode GetUpdateInfoMode() const
	{
		return update_info_mode_;
	}

	/** 设置谁可以修改群资料属性 */
	void SetUpdateCustomMode(NIMTeamUpdateCustomMode mode)
	{
		update_custom_mode_ = mode;
		value_available_flag_ |= kTeamInfoKeyUpdateCustomMode;
	}

	/** 获取谁可以修改群资料属性 */
	NIMTeamUpdateCustomMode GetUpdateCustomMode() const
	{
		return update_custom_mode_;
	}

	/** @fn bool ExistValue(TeamInfoKey value_key) const
	  * @brief 群组信息数据标记Key对应的数据是否有效（存在，非初始值状态）
	  * @param[in] value_key 群组信息数据标记Key
	  * @return bool 有效性 
	  */
	bool ExistValue(TeamInfoKey value_key) const
	{
		return (value_available_flag_ & value_key) != 0;
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string ToJsonString() const
	{
		Json::Value json;
		json[nim::kNIMTeamInfoKeyID] = id_;
		if (ExistValue(nim::kTeamInfoKeyName))
			json[nim::kNIMTeamInfoKeyName] = name_;
		if (ExistValue(nim::kTeamInfoKeyType))
			json[nim::kNIMTeamInfoKeyType] = (unsigned int)type_;
		if (ExistValue(nim::kTeamInfoKeyOwnerID))
			json[nim::kNIMTeamInfoKeyCreator] = owner_id_;
		if (ExistValue(nim::kTeamInfoKeyLevel))
			json[nim::kNIMTeamInfoKeyLevel] = level_;
		if (ExistValue(nim::kTeamInfoKeyProperty))
			json[nim::kNIMTeamInfoKeyProperty] = property_;
		json[nim::kNIMTeamInfoKeyValidFlag] = valid_ ? 1 : 0;
		if (member_count_ > 0)
			json[nim::kNIMTeamInfoKeyMemberCount] = member_count_;
		if (member_list_timetag_ > 0)
			json[nim::kNIMTeamInfoKeyListTime] = member_list_timetag_;
		if (create_timetag_ > 0)
			json[nim::kNIMTeamInfoKeyCreateTime] = create_timetag_;
		if (update_timetag_ > 0)
			json[nim::kNIMTeamInfoKeyUpdateTime] = update_timetag_;
		json[nim::kNIMTeamInfoKeyMemberValid] = member_valid_ ? 1 : 0;
		if (ExistValue(nim::kTeamInfoKeyIntro))
			json[nim::kNIMTeamInfoKeyIntro] = intro_;
		if (ExistValue(nim::kTeamInfoKeyAnnouncement))
			json[nim::kNIMTeamInfoKeyAnnouncement] = announcement_;
		if (ExistValue(nim::kTeamInfoKeyJoinMode))
			json[nim::kNIMTeamInfoKeyJoinMode] = (unsigned int)join_mode_;
		if (ExistValue(nim::kTeamInfoKeyConfigBits))
			json[nim::kNIMTeamInfoKeyBits] = config_bits_;
		if (ExistValue(nim::kTeamInfoKeyCustom))
			json[nim::kNIMTeamInfoKeyCustom] = custom_;
		json[nim::kNIMTeamInfoKeyServerCustom] = server_custom_;
		if (ExistValue(nim::kTeamInfoKeyIcon))
			json[nim::kNIMTeamInfoKeyIcon] = icon_;
		if (ExistValue(nim::kTeamInfoKeyBeInviteMode))
			json[nim::kNIMTeamInfoKeyBeInviteMode] = (unsigned int)be_invite_mode_;
		if (ExistValue(nim::kTeamInfoKeyInviteMode))
			json[nim::kNIMTeamInfoKeyInviteMode] = (unsigned int)invite_mode_;
		if (ExistValue(nim::kTeamInfoKeyUpdateInfoMode))
			json[nim::kNIMTeamInfoKeyUpdateInfoMode] = (unsigned int)update_info_mode_;
		if (ExistValue(nim::kTeamInfoKeyUpdateCustomMode))
			json[nim::kNIMTeamInfoKeyUpdateCustomMode] = (unsigned int)update_custom_mode_;

		return GetJsonStringWithNoStyled(json);
	}

private:
	std::string		id_;
	bool			valid_;
	int				member_count_;
	__int64			member_list_timetag_;
	__int64			create_timetag_;
	__int64			update_timetag_;
	std::string		server_custom_;

private:
	std::string		name_;
	nim::NIMTeamType	type_;
	std::string		owner_id_;
	int				level_;
	std::string		property_;
	bool			member_valid_;
	std::string		intro_;
	std::string		announcement_;
	nim::NIMTeamJoinMode join_mode_;
	__int64			config_bits_;
	std::string		custom_;

private:
	std::string		icon_;
	nim::NIMTeamBeInviteMode be_invite_mode_;
	nim::NIMTeamInviteMode invite_mode_;
	nim::NIMTeamUpdateInfoMode update_info_mode_;
	nim::NIMTeamUpdateCustomMode update_custom_mode_;

	unsigned int	value_available_flag_;
};

/** @brief 群成员信息数据标记Key,用以标记对应数据的有效性 */
enum TeamMemberValueKey
{
	kTeamMemberPropertyKeyNone = 0,				/**< 无数据 */
	kTeamMemberPropertyKeyUserType = 1,			/**< 群成员类型 */
	kTeamMemberPropertyKeyNickName = 1 << 1,	/**< 群成员昵称 */
	kTeamMemberPropertyKeyBits = 1 << 2,		/**< 群成员配置项 */
	kTeamMemberPropertyKeyValid = 1 << 3,		/**< 群成员有效性 */
	kTeamMemberPropertyKeyCustom = 1 << 4,		/**< 群成员自定义扩展字段 */
	kTeamMemberPropertyKeyMute = 1 << 5,		/**< 群成员是否禁言 */
	kTeamMemberPropertyKeyAll = (1 << 6) - 1	/**< 有数据 */
};

/** @brief 群组成员信息 */
struct TeamMemberProperty
{
public:
	/** 构造函数，推荐使用 */
	TeamMemberProperty(const std::string& team_id, const std::string& accid) : type_(kNIMTeamUserTypeNomal)
		, valid_(false)
		, bits_(0)
		, create_timetag_(0)
		, update_timetag_(0)
		, value_available_flag_(0)
		, mute_(false)
	{
		team_id_ = team_id;
		account_id_ = accid;
	}

	/** 构造函数 */
	TeamMemberProperty() : type_(kNIMTeamUserTypeNomal)
		, valid_(false)
		, bits_(0)
		, create_timetag_(0)
		, update_timetag_(0)
		, value_available_flag_(0) 
		, mute_(false){}

public:
	/** 设置群组ID */
	void SetTeamID(const std::string& id)
	{
		team_id_ = id;
	}

	/** 获取群组ID */
	std::string GetTeamID() const
	{
		return team_id_;
	}

	/** 设置群成员ID */
	void SetAccountID(const std::string& id)
	{
		account_id_ = id;
	}

	/** 获取群成员ID */
	std::string GetAccountID() const
	{
		return account_id_;
	}

	/** 设置群成员类型 */
	void SetUserType(nim::NIMTeamUserType type)
	{
		type_ = type;
		value_available_flag_ |= kTeamMemberPropertyKeyUserType;
	}

	/** 获取群成员类型 */
	nim::NIMTeamUserType GetUserType() const
	{
		return type_;
	}

	/** 设置群成员昵称 */
	void SetNick(const std::string& nick)
	{
		nick_ = nick;
		value_available_flag_ |= kTeamMemberPropertyKeyNickName;
	}

	/** 获取群成员昵称 */
	std::string GetNick() const
	{
		return nick_;
	}

	/** 设置群成员配置项 */
	void SetBits(__int64 bit)
	{
		bits_ = bit;
		value_available_flag_ |= kTeamMemberPropertyKeyBits;
	}

	/** 获取群成员配置项 */
	__int64 GetBits() const
	{
		return bits_;
	}

	/** 设置群成员有效性 */
	void SetValid(bool valid)
	{
		valid_ = valid;
		value_available_flag_ |= kTeamMemberPropertyKeyValid;
	}

	/** 获取群成员有效性 */
	bool IsValid() const
	{
		return valid_;
	}

	/** 设置群成员创建时间戳(毫秒) */
	void SetCreateTimetag(__int64 timetag)
	{
		create_timetag_ = timetag;
	}

	/** 获取群成员创建时间戳(毫秒) */
	__int64 GetCreateTimetag() const
	{
		return create_timetag_;
	}

	/** 设置群成员更新时间戳(毫秒) */
	void SetUpdateTimetag(__int64 timetag)
	{
		update_timetag_ = timetag;
	}

	/** 获取群成员更新时间戳(毫秒) */
	__int64 GetUpdateTimetag() const
	{
		return update_timetag_;
	}

	/** @fn bool ExistValue(TeamMemberValueKey value_key) const
	  * @brief 群成员信息信息数据标记Key对应的数据是否有效（存在，非初始值状态）
	  * @param[in] value_key 群成员信息数据标记Key
	  * @return bool 有效性 
	  */
	bool ExistValue(TeamMemberValueKey value_key) const
	{
		return (value_available_flag_ & value_key) != 0;
	}

	/** 设置禁言 */
	void SetMute(bool mute)
	{
		mute_ = mute;
		value_available_flag_ |= kTeamMemberPropertyKeyMute;
	}

	/** 是否禁言 */
	bool IsMute() const
	{
		return mute_;
	}

	/** 设置扩展字段 */
	void SetCustom(const std::string& custom)
	{
		custom_ = custom;
		value_available_flag_ |= kTeamMemberPropertyKeyCustom;
	}

	/** 获取扩展字段 */
	std::string GetCustom() const
	{
		return custom_;
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string ToJsonString() const
	{
		Json::Value json;
		json[nim::kNIMTeamUserKeyID] = team_id_;
		json[nim::kNIMTeamUserKeyAccID] = account_id_;
		if (ExistValue(nim::kTeamMemberPropertyKeyUserType))
			json[nim::kNIMTeamUserKeyType] = (unsigned int)type_;
		if (ExistValue(nim::kTeamMemberPropertyKeyNickName))
			json[nim::kNIMTeamUserKeyNick] = nick_;
		if (ExistValue(nim::kTeamMemberPropertyKeyBits))
			json[nim::kNIMTeamUserKeyBits] = bits_;
		if (ExistValue(nim::kTeamMemberPropertyKeyValid))
			json[nim::kNIMTeamUserKeyValidFlag] = valid_ ? 1 : 0;
		if (create_timetag_ > 0)
			json[nim::kNIMTeamUserKeyCreateTime] = create_timetag_;
		if (update_timetag_ > 0)
			json[nim::kNIMTeamUserKeyUpdateTime] = update_timetag_;
		if (ExistValue(nim::kTeamMemberPropertyKeyCustom))
			json[nim::kNIMTeamUserKeyCustom] = custom_;
		if (ExistValue(nim::kTeamMemberPropertyKeyMute))
			json[nim::kNIMTeamUserKeyMute] = mute_ ? 1 : 0;

		return GetJsonStringWithNoStyled(json);
	}

private:
	bool			valid_;
	__int64			create_timetag_;
	__int64			update_timetag_;

private:
	std::string		team_id_;
	std::string		account_id_;
	nim::NIMTeamUserType type_;
	std::string		nick_;
	__int64			bits_;

private:
	bool			mute_;
	std::string		custom_;

	unsigned int	value_available_flag_;
};

/** @brief 群组事件通知 */
struct TeamEvent
{
	NIMResCode res_code_;					/**< 错误码 */
	NIMNotificationId notification_id_;		/**< 通知类型ID */
	std::string team_id_;					/**< 群组ID */
	std::list<std::string> ids_;			/**< 通知可能涉及到的群成员ID */
	std::list<UserNameCard> namecards_;		/**< 通知可能涉及到的群成员的用户名片 */
	TeamInfo	team_info_;					/**< 通知可能涉及到的群信息 */
	TeamMemberProperty member_property_;	/**< 群成员属性 */
	bool	opt_;							/**< 操作*/
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

/** @fn ParseTeamInfoJson(const Json::Value& team_info_json, TeamInfo& team_info)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据）
  * @param[out] team_info 群组信息
  * @return void 
  */
void ParseTeamInfoJson(const Json::Value& team_info_json, TeamInfo& team_info);

/** @fn ParseTeamInfoJson(const std::string& team_info_json, TeamInfo& team_info)
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据字符串）
  * @param[out] team_info 群组信息
  * @return bool 解析成功或失败 
  */
bool ParseTeamInfoJson(const std::string& team_info_json, TeamInfo& team_info);

/** @fn const std::string& team_infos_json, bool include_invalid_team, std::list<TeamInfo>& team_infos
  * @brief 解析群组信息
  * @param[in] team_info_json 群组信息（Json Value数据字符串）
  * @param[in] include_invalid_team 是否包含无效群组（已退群）
  * @param[out] team_infos 群组信息
  * @return bool 解析成功或失败 
  */
bool ParseTeamInfosJson(const std::string& team_infos_json, bool include_invalid_team, std::list<TeamInfo>& team_infos);

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