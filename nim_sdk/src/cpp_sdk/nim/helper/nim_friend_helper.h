/** @file nim_friend_helper.h
  * @brief SDK好友辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/20
  */

#ifndef _NIM_SDK_CPP_FRIEND_HELPER_H_
#define _NIM_SDK_CPP_FRIEND_HELPER_H_

#include <string>
#include <list>
#include <assert.h>
#include "include/depend_lib/include/nim_json_util.h"
#include "public_define/nim_util_include.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @enum 云信好友数据标记Key,用以标记对应数据的有效性 */
enum NIM_SDK_CPPWRAPPER_DLL_API FriendProfileKey
{
	kFriendProfileKeyNone = 0,						/**< 无数据  */
	kFriendProfileKeyRelationship = 1,				/**< 主动好友关系  */
	kFriendProfileKeyPassiveRelationship = 1 << 1,	/**< 被动好友关系  */
	kFriendProfileKeySource = 1 << 2,				/**< 好友来源  */
	kFriendProfileKeyAlias = 1 << 3,				/**< 别称  */
	kFriendProfileKeyBits = 1 << 4,					/**< 扩展项,int64  */
	kFriendProfileKeyEx = 1 << 5,					/**< 扩展项,json value  */
	kFriendProfileKeyServerEx = 1 << 6,					/**< 服务端扩展项,string 只读 */
	kFriendProfileKeyAll = (1 << 7) - 1				/**< 有数据  */
};

/** @brief 删除好有拓展选项*/
struct NIM_SDK_CPPWRAPPER_DLL_API DeleteFriendOption
{
	DeleteFriendOption() : delete_alias_(true) {}

public:
	bool delete_alias_;
};

/** @brief 云信好友 */
struct NIM_SDK_CPPWRAPPER_DLL_API FriendProfile
{
	/** 构造函数 */
	FriendProfile() : relationship_(kNIMFriendFlagNotFriend)
		, passive_relationship_(kNIMFriendFlagNotFriend)
		, source_(kNIMFriendSourceDefault)
		, bits_(0)
		, create_timetag_(0)
		, update_timetag_(0)
		, value_available_flag_(0){}

	/** 构造函数，推荐使用 */
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

	/** 设置用户ID */
	void SetAccId(const std::string& accid)
	{
		accid_ = accid;
	}

	/** 获取用户ID */
	std::string GetAccId() const
	{
		return accid_;
	}

	/** 设置主动的好友关系 */
	void SetRelationship(NIMFriendFlag flag)
	{
		relationship_ = flag;
		value_available_flag_ |= kFriendProfileKeyRelationship;
	}

	/** 获取主动的好友关系 */
	NIMFriendFlag GetRelationship() const
	{
		return relationship_;
	}

	/** 设置被动的好友关系 */
	void SetPassiveRelationship(NIMFriendFlag flag)
	{
		passive_relationship_ = flag;
		value_available_flag_ |= kFriendProfileKeyPassiveRelationship;
	}

	/** 获取被动的好友关系 */
	NIMFriendFlag GetPassiveRelationship() const
	{
		return passive_relationship_;
	}

	/** 设置好友来源 */
	void SetSource(NIMFriendSource src)
	{
		source_ = src;
		value_available_flag_ |= kFriendProfileKeySource;
	}

	/** 获取好友来源 */
	NIMFriendSource GetSource() const
	{
		return source_;
	}

	/** 设置别称 */
	void SetAlias(const std::string& alias)
	{
		alias_ = alias;
		value_available_flag_ |= kFriendProfileKeyAlias;
	}

	/** 获取别称 */
	std::string GetAlias() const
	{
		return alias_;
	}

	/** 设置扩展信息(int64)*/
	void SetBits(int64_t bits)
	{
		bits_ = bits;
		value_available_flag_ |= kFriendProfileKeyBits;
	}

	/** 获取扩展信息(int64) */
	int64_t GetBits() const
	{
		return bits_;
	}

	/** 设置扩展信息(json value) */
	void SetEx(const nim_cpp_wrapper_util::Json::Value& ex)
	{
		expand_ = ex;
		value_available_flag_ |= kFriendProfileKeyEx;
	}

	/** 获取扩展信息(json value) */
	nim_cpp_wrapper_util::Json::Value GetEx() const
	{
		return expand_;
	}

	/** 设置扩展信息(string) */
	void SetServerEx(const std::string& srv_ex)
	{
		server_expand_ = srv_ex;
		value_available_flag_ |= kFriendProfileKeyServerEx;
	}

	/** 获取扩展信息(string) */
	std::string GetServerEx() const
	{
		return server_expand_;
	}

	/** 设置档案创建时间(毫秒) */
	void SetCreateTimetag(int64_t timetag)
	{
		create_timetag_ = timetag;
	}

	/** 获取档案创建时间(毫秒) */
	int64_t GetCreateTimetag() const
	{
		return create_timetag_;
	}

	/** 设置档案更新时间(毫秒) */
	void SetUpdateTimetag(int64_t timetag)
	{
		update_timetag_ = timetag;
	}

	/** 获取档案更新时间(毫秒) */
	int64_t GetUpdateTimetag() const
	{
		return update_timetag_;
	}

	/** @fn bool ExistValue(FriendProfileKey value_key) const
	  * @brief 好友信息数据标记Key对应的数据是否有效（存在，非初始值状态）
	  * @param[in] value_key 好友信息数据标记Key
	  * @return bool 有效性 
	  */
	bool ExistValue(FriendProfileKey key) const
	{
		return (value_available_flag_ & key) != 0;
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string		ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value friend_profile_json;
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
			friend_profile_json[kNIMFriendKeyEx] = GetJsonStringWithNoStyled(expand_);
		if (create_timetag_ > 0)
			friend_profile_json[kNIMFriendKeyCreateTime] = create_timetag_;
		if (update_timetag_ > 0)
			friend_profile_json[kNIMFriendKeyUpdateTime] = update_timetag_;
		return GetJsonStringWithNoStyled(friend_profile_json);
	}

	/** @fn void Update(const FriendProfile& profile)
	  * @brief 更新好友数据
	  * @param[in] profile 好友数据
	  * @return void 
	  */
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
	std::string		accid_;						/**< 用户账号 */
	NIMFriendFlag	relationship_;				/**< 主动的好友关系 */
	NIMFriendFlag	passive_relationship_;		/**< 被动的好友关系 */
	NIMFriendSource source_;					/**< 好友来源 */
	std::string		alias_;						/**< 好友别名 */
	int64_t			bits_;						/**< 扩展数据 */
	nim_cpp_wrapper_util::Json::Value		expand_;					/**< 扩展数据 */
	unsigned int	value_available_flag_;		/**< 好友数据有效性，结合好友Key使用 */
	std::string	 	server_expand_;	/**< 扩展数据 */
private:
	int64_t			create_timetag_;			/**< 好友创建时间戳（毫秒） */
	int64_t			update_timetag_;			/**< 好友更新时间戳（毫秒） */

};

/** @brief 云信好友变更事件 */
struct NIM_SDK_CPPWRAPPER_DLL_API FriendChangeEvent
{
	NIMFriendChangeType	type_;			/**< 事件类型 */
	std::string			content_;		/**< 事件内容，根据事件类型通过提供的ParsexxxEvent接口(nim_cpp_friend.h)解析该内容 */
};

/** @brief 云信好友变更事件（请求添加） */
struct NIM_SDK_CPPWRAPPER_DLL_API FriendAddEvent
{
	std::string		accid_;				/**< 用户ID */
	NIMVerifyType	add_type_;			/**< 验证类型 */
	std::string		msg_;				/**< 附言 */
};

/** @brief 云信好友变更事件（删除） */
struct NIM_SDK_CPPWRAPPER_DLL_API FriendDelEvent
{
	std::string		accid_;				/**< 用户ID */
};

/** @brief 云信好友变更事件（更新） */
struct NIM_SDK_CPPWRAPPER_DLL_API FriendProfileUpdateEvent
{
	FriendProfile	profile_;			/**< 用户信息 */
};

/** @brief 云信好友变更事件（多端同步） */
struct NIM_SDK_CPPWRAPPER_DLL_API FriendProfileSyncEvent
{
	std::list<FriendProfile> profiles_;	/**< 用户信息列表 */
};

/** @fn bool ParseFriendsProfile(const std::string& friends_profile_json, std::list<FriendProfile>& profiles)
  * @brief 解析（多）好友信息
  * @param[in] friends_profile_json 好友信息(Json Value数据字符串)
  * @param[out] profiles 好友信息
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseFriendsProfile(const std::string& friends_profile_json, std::list<FriendProfile>& profiles);

/** @fn bool ParseFriendProfile(const std::string& friend_profile_json, FriendProfile& profile)
  * @brief 解析（单个）好友信息
  * @param[in] friend_profile_json 好友信息(Json Value字符串)
  * @param[out] profile 好友信息
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseFriendProfile(const std::string& friend_profile_json, FriendProfile& profile);

/** @fn void ParseFriendProfile(const nim_cpp_wrapper_util::Json::Value& friend_profile_json, FriendProfile& profile)
  * @brief 解析（单个）好友信息
  * @param[in] friend_profile_json 好友信息(Json Value数据)
  * @param[out] profile 好友信息
  * @return void
  */
NIM_SDK_CPPWRAPPER_DLL_API void ParseFriendProfile(const nim_cpp_wrapper_util::Json::Value& friend_profile_json, FriendProfile& profile);
}

#endif //_NIM_SDK_CPP_FRIEND_HELPER_H_
