/** @file nim_super_team.h
  * @brief 超大群组 接口头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2019/05/26
  */
#ifndef NIM_SDK_DLL_API_NIM_SUPERTEAM_H_
#define NIM_SDK_DLL_API_NIM_SUPERTEAM_H_

#include "public_define/defines/nim_sdk_dll.h"
#include "public_define/defines/nim_define/nim_super_team_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_super_team_reg_team_event_cb(const char *json_extension, nim_super_team_event_cb_func cb, const void *user_data)
  * (全局回调)统一注册接收群通知回调函数（收到的群通知通过此接口广播）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		群通知的回调函数, nim_super_team_event_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_super_team_reg_team_event_cb(const char *json_extension, nim_super_team_event_cb_func cb, const void *user_data);

/** @fn void nim_super_team_invite_async(const char *tid, const char *jsonlist_uids, const char *json_extension, nim_super_team_opt_cb_func cb, const void *user_data)
  * 邀请
  * @param[in] tid		群组id
  * @param[in] jsonlist_uids		uids string array
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		邀请的回调函数, nim_super_team_opt_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:无超大群功能
  *				404:目标不存在
  *				801:群人数超限
  *				802:没有权限
  *				803:群不存在
  *				804:用户不在群里面
  *				813:部分成功
  */
NIM_SDK_DLL_API void nim_super_team_invite_async(const char *tid, 
	const char *jsonlist_uids, 
	const char *json_extension, 
	nim_super_team_opt_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_kick_async(const char *tid, const char *jsonlist_uids, const char *json_extension, nim_super_team_opt_cb_func cb, const void *user_data)
  * 踢人
  * @param[in] tid		群组id
  * @param[in] jsonlist_uids		uids string array
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		踢人的回调函数, nim_super_team_opt_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:无超大群功能
  *				414:踢人数量超限
  *				802:群人数超限、不能踢自己
  *				803:群不存在
  *				804:用户不在群里面
  */
NIM_SDK_DLL_API void nim_super_team_kick_async(const char *tid, 
	const char *jsonlist_uids, 
	const char *json_extension, 
	nim_super_team_opt_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_leave_async(const char *tid, const char *json_extension, nim_super_team_opt_cb_func cb, const void *user_data)
  * 离开群
  * @param[in] tid		群组id
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		离开群的回调函数, nim_super_team_opt_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:无超大群功能
  *				802:没有群权限、群主不能退群
  *				803:群不存在
  *				804:用户不在群里
  */
NIM_SDK_DLL_API void nim_super_team_leave_async(const char *tid, 
	const char *json_extension, 
	nim_super_team_opt_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_update_team_info_async(const char *tid, const char *json_info, const char *json_extension, nim_super_team_opt_cb_func cb, const void *user_data)
  * 更新群信息
  * @param[in] tid		群组id
  * @param[in] json_info Json string array (Keys SEE MORE `nim_super_team_def.h` 『群组信息 Json Keys』， 目前只支持更新群名称，群id必需填)
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		更新群信息的回调函数, nim_super_team_opt_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:无超大群功能
  *				802:没有权限
  *				803:群不存在
  *				804:用户不在群里
  */
NIM_SDK_DLL_API void nim_super_team_update_team_info_async(const char *tid, 
	const char *json_info, 
	const char *json_extension, 
	nim_super_team_opt_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_update_my_property_async(const char *info, const char *json_extension, nim_super_team_opt_cb_func cb, const void *user_data)
  * 更新自己的群属性
  * @param[in] info		json string 群组成员信息,包含tid和需要修改的字段（目前支持修改kNIMTeamUserKeyNick、kNIMTeamUserKeyBits、kNIMTeamUserKeyCustom三个字段） (Keys SEE MORE `nim_super_team_def.h` 『群组属性 Json Keys』
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		更新自己的群属性的回调函数, nim_super_team_opt_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:无超大群功能
  *				414:参数错误
  *				802:没有权限
  *				803:群不存在
  *				804:不在群里
  */
NIM_SDK_DLL_API void nim_super_team_update_my_property_async(const char *info,
	const char *json_extension, 
	nim_super_team_opt_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_query_all_my_teams_async(const char *json_extension, nim_super_team_query_all_my_teams_cb_func cb, const void *user_data)
  * 查询本地所有群（有效群-群有效AND我在群里-）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		查询所有群的回调函数, nim_super_team_query_all_my_teams_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_super_team_query_all_my_teams_async(const char *json_extension, 
	nim_super_team_query_all_my_teams_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_query_all_my_teams_info_async(const char *json_extension, nim_super_team_query_all_my_teams_info_cb_func cb, const void *user_data)
  * 查询本地所有群信息（默认查询有效群-群有效AND我在群里-）
  * @param[in] json_extension 扩展参数，可以条件获取缓存中有的无效群(我已经不在里面的群OR解散的群)，SDK2.7.0以上支持，例如 "{"include_invalid" : true}"，参数可查询nim_super_team_def.h 搜索本地群组相关信息扩展参数
  * @param[in] cb		查询所有群信息的回调函数, nim_super_team_query_all_my_teams_info_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_super_team_query_all_my_teams_info_async(const char *json_extension, 
	nim_super_team_query_all_my_teams_info_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_query_my_all_member_infos_async(const char *json_extension, nim_super_team_query_my_all_member_infos_cb_func cb, const void *user_data)
  * 查询本地所有群里我的成员信息（使用场景：获取了所有群列表后，需要查询自己在每个群里自己的成员信息，使用成员信息里的bits字段，可以判断当某个群发来消息后，是否做消息通知）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		 查询我的所有群里我的成员信息的回调函数, nim_super_team_query_my_all_member_infos_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_super_team_query_my_all_member_infos_async(const char *json_extension,
	nim_super_team_query_my_all_member_infos_cb_func cb,
	const void *user_data);

/** @fn void nim_super_team_query_team_members_async(const char *tid, bool include_user_info, const char *json_extension, nim_super_team_query_team_members_cb_func cb, const void *user_data)
  * 查询群成员（默认查询有效群成员-在群里-）
  * @param[in] tid		群组id
  * @param[in] include_user_info 返回结果中是否包含成员信息(关系到如何解析返回的json)
  * @param[in] json_extension 扩展参数，如果include_user_info为true，可以条件获取缓存中有的无效群成员（不在群里了），SDK2.8.0以上支持，例如 "{"include_invalid" : true}"，参数可查询nim_super_team_def.h 搜索本地群组相关信息扩展参数
  * @param[in] cb		查询群成员的回调函数, nim_super_team_query_team_members_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码 200:成功
  *				403:无超大群功能
  *				406:没有变化
  *				802:没有权限
  */
NIM_SDK_DLL_API void nim_super_team_query_team_members_async(const char *tid, 
	bool include_user_info, 
	const char *json_extension, 
	nim_super_team_query_team_members_cb_func cb, 
	const void *user_data);

/** @fn void nim_super_team_query_team_member_async(const char *tid, const char *user_id, const char *json_extension, nim_super_team_query_team_member_cb_func cb, const void *user_data)
  * 查询(单个)群成员信息(如果缓存中有数据，支持有效或无效成员)
  * @param[in] tid		群组id
  * @param[in] user_id  群成员id
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		查询群成员的回调函数, nim_super_team_query_team_member_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_super_team_query_team_member_async(const char *tid, 
	const char *user_id, 
	const char *json_extension, 
	nim_super_team_query_team_member_cb_func cb, 
	const void *user_data);

/** @fn char *nim_super_team_query_team_member_block(const char *tid, const char *user_id)
  * 查询(单个)群成员信息(如果缓存中有数据，支持有效或无效成员)(同步接口，堵塞NIM内部线程，谨慎使用)
  * @param[in] tid		群组id
  * @param[in] user_id  群成员id
  * @return char * 返回群成员信息Json char,需要上层调用nim_global.h提供的内存释放接口释放。
  */
NIM_SDK_DLL_API char *nim_super_team_query_team_member_block(const char *tid, const char *user_id);

/** @fn void nim_super_team_query_team_info_async(const char *tid, const char *json_extension, nim_super_team_query_team_info_cb_func cb, const void *user_data)
  * 本地查询群信息(如果缓存中有数据，支持有效或无效群)
  * @param[in] tid		群组id
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		查询群信息的回调函数, nim_super_team_query_team_info_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_super_team_query_team_info_async(const char *tid, 
	const char *json_extension, 
	nim_super_team_query_team_info_cb_func cb, 
	const void *user_data);

/** @fn char *nim_super_team_query_team_info_block(const char *tid)
  * 本地查询群信息(如果缓存中有数据，支持有效或无效群)(同步接口，堵塞NIM内部线程，谨慎使用)
  * @param[in] tid		群组id
  * @return char * 返回群信息Json char, 需要上层调用nim_global.h提供的内存释放接口释放。
  */
NIM_SDK_DLL_API char *nim_super_team_query_team_info_block(const char *tid);

/** @fn void nim_super_team_query_team_info_online_async(const char *tid, const char *json_extension, nim_super_team_opt_cb_func cb, const void *user_data)
  * 在线查询群信息（从服务器获取）
  * @param[in] tid		群组id
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		获取群信息的回调函数, nim_super_team_opt_cb_func回调函数定义见nim_super_team_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:无超大群功能
  *				802:没有群权限
  *				803:群不存在
  */
NIM_SDK_DLL_API void nim_super_team_query_team_info_online_async(const char *tid, 
	const char *json_extension, 
	nim_super_team_opt_cb_func cb, 
	const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_SUPERTEAM_H_