#ifndef _NIM_SDK_CPP_TEAM_HELPER_H_
#define _NIM_SDK_CPP_TEAM_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_sdk_helper.h"

namespace nim
{
/** @brief Team 辅助方法和数据结构定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/14
  */

#include "nim_team_def.h"
#include "nim_msglog_def.h"
#include "nim_res_code_def.h"

enum TeamInfoKey
{
	kTeamInfoKeyNone = 0,
	kTeamInfoKeyName = 1,
	kTeamInfoKeyType = 1 << 1,
	kTeamInfoKeyOwnerID = 1 << 2,
	kTeamInfoKeyLevel = 1 << 3,
	kTeamInfoKeyProperty = 1 << 4,
	kTeamInfoKeyIntro = 1 << 5,
	kTeamInfoKeyAnnouncement = 1 << 6,
	kTeamInfoKeyJoinMode = 1 << 7,
	kTeamInfoKeyConfigBits = 1 << 8,
	kTeamInfoKeyCustom = 1 << 9,
	kTeamInfoKeyAll = (1 << 10) - 1
};

struct TeamInfo
{
public:
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
	{
		id_ = team_id;
	}

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
			, value_available_flag_(0) {}

public:
	void SetTeamID(const std::string& id)
	{
		id_ = id;
	}

	std::string GetTeamID() const
	{
		return id_;
	}

	void SetName(const std::string& name)
	{
		name_ = name;
		value_available_flag_ |= kTeamInfoKeyName;
	}

	std::string GetName() const
	{
		return name_;
	}

	void SetType(nim::NIMTeamType type)
	{
		type_ = type;
		value_available_flag_ |= kTeamInfoKeyType;
	}

	nim::NIMTeamType GetType() const
	{
		return type_;
	}

	void SetOwnerID(const std::string& id)
	{
		owner_id_ = id;
		value_available_flag_ |= kTeamInfoKeyOwnerID;
	}

	std::string GetOwnerID() const
	{
		return owner_id_;
	}

	void SetLevel(int level)
	{
		level_ = level;
		value_available_flag_ |= kTeamInfoKeyLevel;
	}

	int GetLevel() const
	{
		return level_;
	}

	void SetProperty(const std::string& prop)
	{
		property_ = prop;
		value_available_flag_ |= kTeamInfoKeyProperty;
	}

	std::string GetProperty() const
	{
		return property_;
	}

	void SetValid(bool valid)
	{
		valid_ = valid;
	}

	bool IsValid() const
	{
		return valid_;
	}

	void SetMemberCount(int count)
	{
		member_count_ = count;
	}

	int GetMemberCount() const
	{
		return member_count_;
	}

	void SetMemberListTimetag(__int64 timetag)
	{
		member_list_timetag_ = timetag;
	}

	__int64 GetMemberListTimetag() const
	{
		return member_list_timetag_;
	}

	void SetCreateTimetag(__int64 timetag)
	{
		create_timetag_ = timetag;
	}

	__int64 GetCreateTimetag() const
	{
		return create_timetag_;
	}

	void SetUpdateTimetag(__int64 timetag)
	{
		update_timetag_ = timetag;
	}

	__int64 GetUpdateTimetag() const
	{
		return update_timetag_;
	}

	void SetMemberValid(bool valid)
	{
		member_valid_ = valid;
	}

	bool IsMemberValid() const
	{
		return member_valid_;
	}

	void SetIntro(const std::string& intro)
	{
		intro_ = intro;
		value_available_flag_ |= kTeamInfoKeyIntro;
	}

	std::string GetIntro() const
	{
		return intro_;
	}

	void SetAnnouncement(const std::string& announcement)
	{
		announcement_ = announcement;
		value_available_flag_ |= kTeamInfoKeyAnnouncement;
	}

	std::string GetAnnouncement() const
	{
		return announcement_;
	}

	void SetJoinMode(nim::NIMTeamJoinMode mode)
	{
		join_mode_ = mode;
		value_available_flag_ |= kTeamInfoKeyJoinMode;
	}

	nim::NIMTeamJoinMode GetJoinMode() const
	{
		return join_mode_;
	}

	void SetConfigBits(__int64 bit)
	{
		config_bits_ = bit;
		value_available_flag_ |= kTeamInfoKeyConfigBits;
	}

	__int64 GetConfigBits() const
	{
		return config_bits_;
	}

	void SetCustom(const std::string& custom)
	{
		custom_ = custom;
		value_available_flag_ |= kTeamInfoKeyCustom;
	}

	std::string GetCustom() const
	{
		return custom_;
	}

	void SetServerCustom(const std::string& custom)
	{
		server_custom_ = custom;
	}

	std::string GetServerCustom() const
	{
		return server_custom_;
	}

	bool ExistValue(TeamInfoKey value_key) const
	{
		return (value_available_flag_ & value_key) != 0;
	}

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

		return json.toStyledString();
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

	unsigned int	value_available_flag_;
};

enum TeamMemberValueKey
{
	kTeamMemberPropertyKeyNone = 0,
	kTeamMemberPropertyKeyUserType = 1,
	kTeamMemberPropertyKeyNickName = 1 << 1,
	kTeamMemberPropertyKeyBits = 1 << 2,
	kTeamMemberPropertyKeyValid = 1 << 3,
	kTeamMemberPropertyKeyAll = (1 << 4) - 1
};

struct TeamMemberProperty
{
public:
	TeamMemberProperty(const std::string& team_id, const std::string& accid) : type_(kNIMTeamUserTypeNomal), valid_(false), bits_(0), create_timetag_(0), update_timetag_(0), value_available_flag_(0)
	{
		team_id_ = team_id;
		account_id_ = accid;
	}

	TeamMemberProperty() : type_(kNIMTeamUserTypeNomal), valid_(false), bits_(0), create_timetag_(0), update_timetag_(0), value_available_flag_(0) {}

public:
	void SetTeamID(const std::string& id)
	{
		team_id_ = id;
	}

	std::string GetTeamID() const
	{
		return team_id_;
	}

	void SetAccountID(const std::string& id)
	{
		account_id_ = id;
	}

	std::string GetAccountID() const
	{
		return account_id_;
	}

	void SetUserType(nim::NIMTeamUserType type)
	{
		type_ = type;
		value_available_flag_ |= kTeamMemberPropertyKeyUserType;
	}

	nim::NIMTeamUserType GetUserType() const
	{
		return type_;
	}

	void SetNick(const std::string& nick)
	{
		nick_ = nick;
		value_available_flag_ |= kTeamMemberPropertyKeyNickName;
	}

	std::string GetNick() const
	{
		return nick_;
	}

	void SetBits(__int64 bit)
	{
		bits_ = bit;
		value_available_flag_ |= kTeamMemberPropertyKeyBits;
	}

	__int64 GetBits() const
	{
		return bits_;
	}

	void SetValid_(bool valid)
	{
		valid_ = valid;
		value_available_flag_ |= kTeamMemberPropertyKeyValid;
	}

	bool IsValid() const
	{
		return valid_;
	}

	void SetCreateTimetag(__int64 timetag)
	{
		create_timetag_ = timetag;
	}

	__int64 GetCreateTimetag() const
	{
		return create_timetag_;
	}

	void SetUpdateTimetag(__int64 timetag)
	{
		update_timetag_ = timetag;
	}

	__int64 GetUpdateTimetag() const
	{
		return update_timetag_;
	}

	bool ExistValue(TeamMemberValueKey value_key) const
	{
		return (value_available_flag_ & value_key) != 0;
	}

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

		return json.toStyledString();
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

	unsigned int	value_available_flag_;
};

struct TeamEvent
{
	NIMResCode res_code_;
	NIMNotificationId notification_id_;
	std::string team_id_;
	std::list<std::string> ids_;
	TeamInfo	team_info_;
};

void ParseTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, TeamEvent& team_event);

void ParseTeamInfoJson(const Json::Value& team_info_json, TeamInfo& team_info);
bool ParseTeamInfoJson(const std::string& team_info_json, TeamInfo& team_info);
bool ParseTeamInfosJson(const std::string& team_infos_json, bool include_invalid_team, std::list<TeamInfo>& team_infos);

void ParseTeamMemberPropertyJson(const Json::Value& team_member_prop_json, TeamMemberProperty& team_member_property);
bool ParseTeamMemberPropertyJson(const std::string& team_member_prop_json, TeamMemberProperty& team_member_property);
bool ParseTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<TeamMemberProperty>& team_member_propertys);
}//namespace nim

#endif //_NIM_SDK_CPP_TEAM_HELPER_H_