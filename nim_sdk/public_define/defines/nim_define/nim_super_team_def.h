/** @file nim_super_team_def.h
  * @brief 群组 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2019/05/26
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_SUPERTEAM_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_SUPERTEAM_DEF_H_

#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @typedef void (*nim_super_team_event_cb_func)(int res_code, int notification_id, const char *tid, const char *result, const char *json_extension, const void *user_data)
  * 群组通知的回调函数定义
  * @param[out] res_code		结果代码，一切正常200
  * @param[out] notification_id	通知类型枚举值 ('nim_msglog_def.h' NIMNotificationId)
  * @param[out] tid				群组id
  * @param[out] result			Json string (Keys SEE MORE `nim_msglog_def.h` 『群组通知 Json Keys』)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_event_cb_func)(int res_code, int notification_id, const char *tid, const char *result, const char *json_extension, const void *user_data);

/** @typedef nim_super_team_event_cb_func nim_super_team_opt_cb_func
  * 群组操作的回调函数定义
  */
typedef nim_super_team_event_cb_func nim_super_team_opt_cb_func;

/** @typedef void (*nim_super_team_query_all_my_teams_cb_func)(int team_count, const char *result, const char *json_extension, const void *user_data)
  * 查询我的所有群组的回调函数定义
  * @param[out] team_count		有效群组数量
  * @param[out] result			tids Json string array
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_query_all_my_teams_cb_func)(int team_count, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_super_team_query_all_my_teams_info_cb_func)(int team_count, const char *result, const char *json_extension, const void *user_data)
  * 查询我的所有群组信息的回调函数定义
  * @param[out] team_count		有效群组数量
  * @param[out] result			Json string array (Keys SEE MORE `nim_super_team_def.h` 『群组信息 Json Keys』)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_query_all_my_teams_info_cb_func)(int team_count, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_super_team_query_my_all_member_infos_cb_func)(int team_count, const char *result, const char *json_extension, const void *user_data)
  * 查询我的所有群里我的成员信息的回调函数定义
  * @param[out] team_count		有效群组数量
  * @param[out] result			群成员信息 Json string array
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void(*nim_super_team_query_my_all_member_infos_cb_func)(int team_count, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_super_team_query_team_members_cb_func)(const char *tid, int member_count, bool include_user_info, const char *result, const char *json_extension, const void *user_data)
  * 查询群成员的回调函数定义
  * @param[out] code			返回码 10602 表示该群群成员同步尚未完成
  * @param[out] tid				群组id
  * @param[out] member_count	有效群成员数量
  * @param[out] include_user_info 返回结果里是否包含user_info
  * @param[out] result			群成员id json string array
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_query_team_members_cb_func)(int code,const char *tid, 
	int member_count, 
	bool include_user_info, 
	const char *result, 
	const char *json_extension, 
	const void *user_data);

/** @typedef void (*nim_super_team_query_team_member_cb_func)(const char *tid, const char *user_id, const char *result, const char *json_extension, const void *user_data)
  * 查询(单个)群成员的回调函数定义
  * @param[out] tid				群组id
  * @param[out] user_id			群成员id
  * @param[out] result			群成员信息 json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_query_team_member_cb_func)(const char *tid, 
	const char *user_id, 
	const char *result, 
	const char *json_extension, 
	const void *user_data);

/** @typedef void (*nim_super_team_query_team_info_cb_func)(const char *tid, const char *result, const char *json_extension, const void *user_data)
  * 查询群信息的回调函数定义
  * @param[out] tid				群组id
  * @param[out] result			Json string (Keys SEE MORE `nim_super_team_def.h` 『群组信息 Json Keys』)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_query_team_info_cb_func)(const char *tid, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_super_team_query_mute_list_cb_func)(int res_code, int member_count, const char *tid, const char *result, const char *json_extension, const void *user_data)
  * 获取群禁言成员列表的回调函数定义
  * @param[out] res_code		结果代码，一切正常200
  * @param[out] member_count	禁言列表人数
  * @param[out] tid				群组id
  * @param[out] result			Json string(群组成员信息 json object array)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_super_team_query_mute_list_cb_func)(int res_code, int member_count, const char *tid, const char *result, const char *json_extension, const void *user_data);

/** @name 群组信息 Json Keys
  * @{
  */
static const char *kNIMSuperTeamInfoKeyID			= "tid";			/**< string,群id,通常情况下由SDK维护,开发者作为只读无需设置 */	
static const char *kNIMSuperTeamInfoKeyName			= "name";			/**< string,群名称 */
static const char *kNIMSuperTeamInfoKeyType			= "type";			/**< int,群类型(NIMTeamType) */
static const char *kNIMSuperTeamInfoKeyCreator		= "creator";		/**< string 群拥有者ID,通常情况下由SDK维护,开发者作为只读无需设置*/
static const char *kNIMSuperTeamInfoKeyLevel			= "level";			/**< int,■■已废弃■■ 群等级,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyMemberMaxCount = "member_max_count";			/**< int,在创建群时，设置群的最大成员数，不可超过应用设定的最大成员数，否则返回414 */
static const char *kNIMSuperTeamInfoKeyProperty		= "prop";			/**< string,群性质,长度限制：6000字符 */
static const char *kNIMSuperTeamInfoKeyValidFlag		= "valid";			/**< int,群有效性标记位,有效1,无效0,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyMemberCount	= "member_count";	/**< int,群成员数量,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyListTime		= "list_timetag";	/**< long,群列表时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyCreateTime	= "create_timetag";	/**< long,群创建时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyUpdateTime	= "update_timetag";	/**< long,群信息上次更新时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyMemberValid	= "member_valid";	/**< int,群有效性标记位,客户端用,有效1,无效0,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamInfoKeyIntro			= "intro";			/**< string,群介绍,长度限制：255字符 */
static const char *kNIMSuperTeamInfoKeyAnnouncement	= "announcement";	/**< string,群公告,长度限制：5000字符 */
static const char *kNIMSuperTeamInfoKeyJoinMode		= "join_mode";		/**< int,入群模式(NIMSuperTeamJoinMode),默认为kNIMSuperTeamJoinModeNoAuth(0),不需要验证 */
static const char *kNIMSuperTeamInfoKeyBits			= "bits";			/**< long, 群属性,开发者无需关注 20161011 by Oleg*/
static const char *kNIMSuperTeamInfoKeyCustom		= "custom";			/**< string, 第三方扩展字段（仅负责存储和透传） */
static const char *kNIMSuperTeamInfoKeyServerCustom	= "server_custom";	/**< string, 第三方服务器扩展字段（该配置项只能通过服务器接口设置，对客户端只读） */
static const char *kNIMSuperTeamInfoKeyIcon			= "icon";			/**< string, 群头像,长度限制：1024字符 */
static const char *kNIMSuperTeamInfoKeyBeInviteMode	= "be_invite_mode"; /**< int, 被邀请人同意方式，属性本身只有群主管理员可以修改,默认kNIMTeamBeInviteModeNeedAgree(0)/kNIMTeamBeInviteModeNotNeedAgree(1) */
static const char *kNIMSuperTeamInfoKeyInviteMode	= "invite_mode";	/**< int, 谁可以邀请他人入群，属性本身只有群主管理员可以修改,默认kNIMTeamInviteModeManager(0)/kNIMTeamInviteModeEveryone(1) */
static const char *kNIMSuperTeamInfoKeyUpdateInfoMode= "update_info_mode";/**< int, 谁可以修改群资料，属性本身只有群主管理员可以修改,默认kNIMTeamUpdateInfoModeManager(0)/kNIMTeamUpdateInfoModeEveryone(1) */
static const char *kNIMSuperTeamInfoKeyUpdateCustomMode	= "update_custom_mode";/**< int, 谁可以更新群自定义属性，属性本身只有群主管理员可以修改,默认kNIMTeamUpdateCustomModeManager(0)/kNIMTeamUpdateCustomModeEveryone(1) */
/** @}*/ //群组信息Json Keys

/** @enum NIMSuperTeamBitsConfigMask 群组成员信息Bits属性kNIMSuperTeamUserKeyBits的配置定义 */
enum NIMSuperTeamBitsConfigMask
{
	kNIMSuperTeamBitsConfigMaskMuteNotify	= 1,	/**< 关闭消息提醒0：开启提醒, 1：关闭提醒, */
	kNIMSuperTeamBitsConfigMaskOnlyAdmin		= 2,	/**< 只接收管理员消息0：开启提醒, 1：关闭提醒 */
};

/** @name 群组成员信息 Json Keys
  * @{
  */
static const char *kNIMSuperTeamUserKeyID			= "tid";			/**< string,群id,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamUserKeyAccID			= "accid";			/**< string,群成员id,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamUserKeyType			= "type";			/**< int,群成员类型(NIMSuperTeamUserType),默认kNIMSuperTeamUserTypeNomal(0) */
static const char *kNIMSuperTeamUserKeyNick			= "nick";			/**< string,群成员昵称 */
static const char *kNIMSuperTeamUserKeyBits			= "bits";			/**< long,群成员属性,位操作(NIMSuperTeamBitsConfigMask) */
static const char *kNIMSuperTeamUserKeyValidFlag		= "valid";			/**< int,群成员有效性标记位,有效1,无效0,通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamUserKeyCreateTime	= "create_timetag";	/**< long,入群时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamUserKeyUpdateTime	= "update_timetag";	/**< long,群成员信息上次更新时间戳(毫秒),通常情况下由SDK维护,开发者作为只读无需设置 */
static const char *kNIMSuperTeamUserKeyCustom		= "custom";			/**< string,群成员自定义扩展字段,必须为可以解析为json的非格式化的字符串 */
static const char *kNIMSuperTeamUserKeyMute			= "mute";			/**< int,是否被禁言,0-非禁言(默认),1-禁言 */
/** @}*/ //群组成员信息Json Keys

/** @enum NIMSuperTeamUserType 群成员类型 */
enum NIMSuperTeamUserType
{
	kNIMSuperTeamUserTypeNomal		= 0,		/**< 普通成员 */
	kNIMSuperTeamUserTypeCreator		= 1,		/**< 创建者 */
	kNIMSuperTeamUserTypeManager		= 2,		/**< 管理员 */
	kNIMSuperTeamUserTypeApply		= 3,		/**< 申请加入用户 */
	kNIMSuperTeamUserTypeLocalWaitAccept = 100,	/**< 本地记录等待正在入群的用户 */
};

/** @enum NIMSuperTeamJoinMode 群允许加入类型 */
enum NIMSuperTeamJoinMode 
{
	kNIMSuperTeamJoinModeNoAuth		= 0,		/**< 不用验证 */
	kNIMSuperTeamJoinModeNeedAuth	= 1,		/**< 需要验证 */
	kNIMSuperTeamJoinModeRejectAll	= 2,		/**< 拒绝所有人入群 */
};

/** @name 搜索本地群组相关信息扩展参数
  * @{
  */
static const char *kNIMSuperTeamSearchKeyIncludeInvalid = "include_invalid";			/**< bool,是否包括无效群(成员)，不在群里（离开或被踢）或者群已经解散 */
static const char *kNIMSuperTeamSearchKeyAnchorJoinTime = "anchor_join_time";		/**< int64, 查询锚点：入群时间，根据锚点分批查询 */
static const char *kNIMSuperTeamSearchKeyAnchorAccid = "anchor_accid";			/**< string, 查询锚点：用户id，根据锚点分批查询，不填可能不精确 */
static const char *kNIMSuperTeamSearchKeyMaxCount = "max_count";			/**< int, 分批查询最大数量 */
/** @}*/ //搜索本地群组相关信息扩展参数


#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_TEAM_DEF_H_