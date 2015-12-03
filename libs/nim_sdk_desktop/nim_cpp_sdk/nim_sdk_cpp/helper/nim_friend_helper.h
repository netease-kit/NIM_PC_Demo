#ifndef _NIM_SDK_CPP_FRIEND_HELPER_H_
#define _NIM_SDK_CPP_FRIEND_HELPER_H_

#include <string>
#include <list>
#include <assert.h>
#include "json.h"

namespace nim
{
/** @brief SDKºÃÓÑ¸¨Öú·½·¨
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#include "nim_friend_def.h"

enum FriendProfileKey
{
	kFriendProfileKeyNone = 0,
	kFriendProfileKeyRelationship = 1,
	kFriendProfileKeyPassiveRelationship = 1 << 1,
	kFriendProfileKeySource = 1 << 2,
	kFriendProfileKeyAlias = 1 << 3,
	kFriendProfileKeyBits = 1 << 4,
	kFriendProfileKeyEx = 1 << 5,
	kFriendProfileKeyAll = (1 << 6) - 1
};

struct FriendProfile
{
	FriendProfile() : relationship_(kNIMFriendFlagNotFriend)
		, passive_relationship_(kNIMFriendFlagNotFriend)
		, source_(kNIMFriendSourceDefault)
		, bits_(0)
		, create_timetag_(0)
		, update_timetag_(0)
		, value_available_flag_(0){}

	FriendProfile(const std::string& accid) : relationship_(kNIMFriendFlagNotFriend)
		, passive_relationship_(kNIMFriendFlagNotFriend)
		, source_(kNIMFriendSourceDefault)
		, bits_(0)
		, create_timetag_(0)
		, update_timetag_(0)
		, value_available_flag_(0)
	{
		accid_ = accid;
	}

	void SetAccId(const std::string& accid)
	{
		accid_ = accid;
	}

	std::string GetAccId() const
	{
		return accid_;
	}

	void SetRelationship(NIMFriendFlag flag)
	{
		relationship_ = flag;
		value_available_flag_ |= kFriendProfileKeyRelationship;
	}

	NIMFriendFlag GetRelationship() const
	{
		return relationship_;
	}

	void SetPassiveRelationship(NIMFriendFlag flag)
	{
		passive_relationship_ = flag;
		value_available_flag_ |= kFriendProfileKeyPassiveRelationship;
	}

	NIMFriendFlag GetPassiveRelationship() const
	{
		return passive_relationship_;
	}

	void SetSource(NIMFriendSource src)
	{
		source_ = src;
		value_available_flag_ |= kFriendProfileKeySource;
	}

	NIMFriendSource GetSource() const
	{
		return source_;
	}

	void SetAlias(const std::string& alias)
	{
		alias_ = alias;
		value_available_flag_ |= kFriendProfileKeyAlias;
	}

	std::string GetAlias() const
	{
		return alias_;
	}

	void SetBits(__int64 bits)
	{
		bits_ = bits;
		value_available_flag_ |= kFriendProfileKeyBits;
	}

	__int64 GetBits() const
	{
		return bits_;
	}

	void SetEx(const std::string& ex)
	{
		expand_ = ex;
		value_available_flag_ |= kFriendProfileKeyEx;
	}

	std::string GetEx() const
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

	bool ExistValue(FriendProfileKey key) const
	{
		return (value_available_flag_ & key) != 0;
	}

	std::string		ToJsonString() const
	{
		Json::Value friend_profile_json;
		friend_profile_json[kNIMFriendKeyAccid] = accid_;
		if (ExistValue(kFriendProfileKeyRelationship))
			friend_profile_json[kNIMFriendKeyFlag] = relationship_;
		if (ExistValue(kFriendProfileKeyPassiveRelationship))
			friend_profile_json[kNIMFriendKeyBeFlag] = passive_relationship_;
		if (ExistValue(kFriendProfileKeySource))
			friend_profile_json[kNIMFriendKeySource] = source_;
		if (ExistValue(kFriendProfileKeyAlias))
			friend_profile_json[kNIMFriendKeyAlias] = alias_;
		if (ExistValue(kFriendProfileKeyBits))
			friend_profile_json[kNIMFriendKeyBits] = bits_;
		if (ExistValue(kFriendProfileKeyEx))
			friend_profile_json[kNIMFriendKeyEx] = expand_;
		if (create_timetag_ > 0)
			friend_profile_json[kNIMFriendKeyCreateTime] = create_timetag_;
		if (update_timetag_ > 0)
			friend_profile_json[kNIMFriendKeyUpdateTime] = update_timetag_;
		return friend_profile_json.toStyledString();
	}

	void Update(const FriendProfile& profile)
	{
		assert(profile.accid_ == accid_);
		if (profile.accid_ != accid_)
			return;

		if (profile.ExistValue(kFriendProfileKeyRelationship))
			relationship_ = profile.relationship_;
		if (profile.ExistValue(kFriendProfileKeyPassiveRelationship))
			passive_relationship_ = profile.passive_relationship_;
		if (profile.ExistValue(kFriendProfileKeySource))
			source_ = profile.source_;
		if (profile.ExistValue(kFriendProfileKeyAlias))
			alias_ = profile.alias_;
		if (profile.ExistValue(kFriendProfileKeyBits))
			bits_ = profile.bits_;
		if (profile.ExistValue(kFriendProfileKeyEx))
			expand_ = profile.expand_;
		if (profile.create_timetag_ > 0)
			create_timetag_ = profile.create_timetag_;
		if (profile.update_timetag_ > 0)
			update_timetag_ = profile.update_timetag_;
	}

private:
	std::string		accid_;
	NIMFriendFlag	relationship_;
	NIMFriendFlag	passive_relationship_;
	NIMFriendSource source_;
	std::string		alias_;
	__int64			bits_;
	std::string		expand_;
	unsigned int	value_available_flag_;

private:
	__int64			create_timetag_;
	__int64			update_timetag_;

};

struct FriendChangeEvent
{
	NIMFriendChangeType	type_;
	std::string			content_;
};

struct FriendAddEvent
{
	std::string		accid_;
	NIMVerifyType	add_type_;
	std::string		msg_;
};

struct FriendDelEvent
{
	std::string		accid_;
};

struct FriendProfileUpdateEvent
{
	FriendProfile	profile_;
};

struct FriendProfileSyncEvent
{
	std::list<FriendProfile> profiles_;
};

bool ParseFriendsProfile(const std::string& friends_profile_json, std::list<FriendProfile>& profiles);
bool ParseFriendProfile(const std::string& friend_profile_json, FriendProfile& profile);
void ParseFriendProfile(const Json::Value& friend_profile_json, FriendProfile& profile);
}

#endif //_NIM_SDK_CPP_FRIEND_HELPER_H_
