#ifndef _NIM_SDK_CPP_USER_HELPER_H_
#define _NIM_SDK_CPP_USER_HELPER_H_

#include <string>
#include <list>
#include <assert.h>
#include "json.h"

namespace nim
{
/** @brief SDK用户信息辅助方法
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#include "nim_user_def.h"

enum UserNameCardValueKey
{
	kUserNameCardKeyNone		= 0,
	kUserNameCardKeyName		= 1,
	kUserNameCardKeyIconUrl	= 1 << 1,
	kUserNameCardKeyGender		= 1 << 2,
	kUserNameCardKeyBirthday	= 1 << 3,
	kUserNameCardKeyMobile		= 1 << 4,
	kUserNameCardKeyEmail		= 1 << 5,
	kUserNameCardKeySignature	= 1 << 6,
	kUserNameCardKeyEx			= 1 << 7,
	kUserNameCardKeyAll		= (1 << 8) - 1
};

struct UserNameCard
{
	UserNameCard(const std::string& accid) : value_available_flag_(0), create_timetag_(0), update_timetag_(0), gender_(0)
	{
		accid_ = accid;
	}

	UserNameCard() : value_available_flag_(0), create_timetag_(0), update_timetag_(0), gender_(0) {}

public:
	void SetAccId(const std::string& id)
	{
		accid_ = id;
	}

	std::string GetAccId() const
	{
		return accid_;
	}

	void SetName(const std::string& name)
	{
		nickname_ = name;
		value_available_flag_ |= kUserNameCardKeyName;
	}

	std::string GetName() const
	{
		return nickname_;
	}

	void SetIconUrl(const std::string& url)
	{
		icon_url_ = url;
		value_available_flag_ |= kUserNameCardKeyIconUrl;
	}

	std::string GetIconUrl() const
	{
		return icon_url_;
	}

	void SetSignature(const std::string& sign)
	{
		signature_ = sign;
		value_available_flag_ |= kUserNameCardKeySignature;
	}

	std::string GetSignature() const
	{
		return signature_;
	}

	void SetGender(int gender)
	{
		gender_ = gender;
		value_available_flag_ |= kUserNameCardKeyGender;
	}

	int GetGender() const
	{
		return gender_;
	}

	void SetEmail(const std::string& email)
	{
		email_ = email;
		value_available_flag_ |= kUserNameCardKeyEmail;
	}

	std::string GetEmail() const
	{
		return email_;
	}

	void SetBirth(const std::string& birth)
	{
		birth_ = birth;
		value_available_flag_ |= kUserNameCardKeyBirthday;
	}

	std::string GetBirth() const
	{
		return birth_;
	}

	void SetMobile(const std::string& mobile)
	{
		mobile_ = mobile;
		value_available_flag_ |= kUserNameCardKeyMobile;
	}

	std::string GetMobile() const
	{
		return mobile_;
	}

	void SetExpand(const std::string& expand)
	{
		expand_ = expand;
		value_available_flag_ |= kUserNameCardKeyEx;
	}

	std::string GetExpand() const
	{
		return expand_;
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

	void Update(const UserNameCard& namecard)
	{
		assert(namecard.GetAccId() == accid_);
		if (namecard.GetAccId() != accid_)
			return;

		if (namecard.ExistValue(kUserNameCardKeyName))
			SetName(namecard.GetName());
		if (namecard.ExistValue(kUserNameCardKeyIconUrl))
			SetIconUrl(namecard.GetIconUrl());
		if (namecard.ExistValue(kUserNameCardKeySignature))
			SetSignature(namecard.GetSignature());
		if (namecard.ExistValue(kUserNameCardKeyGender))
			SetGender(namecard.GetGender());
		if (namecard.ExistValue(kUserNameCardKeyEmail))
			SetEmail(namecard.GetEmail());
		if (namecard.ExistValue(kUserNameCardKeyBirthday))
			SetBirth(namecard.GetBirth());
		if (namecard.ExistValue(kUserNameCardKeyMobile))
			SetMobile(namecard.GetMobile());
		if (namecard.ExistValue(kUserNameCardKeyEx))
			SetExpand(namecard.GetExpand());
		if (namecard.GetCreateTimetag() > 0)
			create_timetag_ = namecard.GetCreateTimetag();
		if (namecard.GetUpdateTimetag() > 0)
			update_timetag_ = namecard.GetUpdateTimetag();
	}

	bool ExistValue(UserNameCardValueKey value_key) const
	{
		return (value_available_flag_ & value_key) != 0;
	}

	std::string ToJsonString() const
	{
		Json::Value values;
		values[kNIMNameCardKeyAccid] = GetAccId();
		if (ExistValue(kUserNameCardKeyName))
			values[kNIMNameCardKeyName] = GetName();
		if (ExistValue(kUserNameCardKeyIconUrl))
			values[kNIMNameCardKeyIcon] = GetIconUrl();
		if (ExistValue(kUserNameCardKeySignature))
			values[kNIMNameCardKeySign] = GetSignature();
		if (ExistValue(kUserNameCardKeyGender))
			values[kNIMNameCardKeyGender] = GetGender();
		if (ExistValue(kUserNameCardKeyEmail))
			values[kNIMNameCardKeyEmail] = GetEmail();
		if (ExistValue(kUserNameCardKeyBirthday))
			values[kNIMNameCardKeyBirth] = GetBirth();
		if (ExistValue(kUserNameCardKeyMobile))
			values[kNIMNameCardKeyMobile] = GetMobile();
		if (ExistValue(kUserNameCardKeyEx))
			values[kNIMNameCardKeyEx] = GetExpand();

		return values.toStyledString();
	}

private:
	std::string		accid_;
	std::string		nickname_;
	std::string		icon_url_;
	std::string		signature_;
	int				gender_;
	std::string		email_;
	std::string		birth_;
	std::string		mobile_;
	std::string		expand_;
	__int64			create_timetag_;
	__int64			update_timetag_;

	unsigned int	value_available_flag_;
};

struct BlackListInfo
{
	std::string		accid_;
	bool			set_black_;
	__int64			create_timetag_;
	__int64			update_timetag_;

	BlackListInfo() : set_black_(true), create_timetag_(0), update_timetag_(0){}
};

struct MuteListInfo
{
	std::string		accid_;
	bool			set_mute_;
	__int64			create_timetag_;
	__int64			update_timetag_;

	MuteListInfo() : set_mute_(true), create_timetag_(0), update_timetag_(0) {}
};

struct SpecialRelationshipChangeEvent
{
	NIMUserSpecialRelationshipChangeType type_;
	std::string							 content_;
};

bool ParseNameCards(const std::string& namecards_json, std::list<UserNameCard>& namecards);
bool ParseNameCard(const std::string& namecard_json, UserNameCard& namecard);
void ParseNameCard(const Json::Value& namecard_json, UserNameCard& namecard);

bool ParseSpecialListInfo(const std::string& namecards_json, std::list<BlackListInfo>& black_list, std::list<MuteListInfo>& mute_list);
}

#endif //_NIM_SDK_CPP_USER_HELPER_H_
