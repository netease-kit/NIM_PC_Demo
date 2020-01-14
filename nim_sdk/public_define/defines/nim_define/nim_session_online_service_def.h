/** @file nim_session_online_service_def.h
  * @brief 会话服务接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2019, NetEase Inc. All rights reserved
  * @author cqu227hk
  * @date 2019/10/21
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_SESSION_ONLINE_SERVICE_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_SESSION_ONLINE_SERVICE_DEF_H_

#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif
	/** @typedef void(*nim_session_online_service_query_session_list_cb_func)(int rescode, const char *result, const void *user_data)
  * 查询会话列表的回调函数定义
  * @param[out] rescode		操作结果,成功200
  * @param[out] result		会话列表Json string
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
	typedef void(*nim_session_online_service_query_session_list_cb_func)(int rescode, const char *result, const void *user_data);

	/** @typedef void(*nim_session_online_service_query_session_cb_func)(int rescode, const char *result, const void *user_data)
	* 查询会话的回调函数定义
	* @param[out] rescode		操作结果,成功200
	* @param[out] result		会话Json string
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_session_online_service_query_session_cb_func)(int rescode, const char *result, const void *user_data);

	/** @typedef void(*nim_session_online_service_update_session_cb_func)(int rescode, const void *user_data)
	* 更新会话的回调函数定义
	* @param[out] rescode		操作结果,成功200
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_session_online_service_update_session_cb_func)(int rescode, const void *user_data);

	/** @typedef void(*nim_session_online_service_delete_session_cb_func)(int rescode, const void *user_data)
	* 删除会话的回调函数定义
	* @param[out] rescode		操作结果,成功200
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_session_online_service_delete_session_cb_func)(int rescode, const void *user_data);

	/** @typedef void(*nim_session_online_service_session_changed_cb_func)(const char *session_info, const void *user_data)
	* 会话变更时的回调函数定义
	* @param[out] session_info	变更会话的信息
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_session_online_service_session_changed_cb_func)(const char *session_info, const void *user_data);

/** @name 会话服务 【会话信息】定义 */
static const char *kMINSessionOLSRVInfoType = "info_session_tpye";     /**< int 会话类型 */
static const char *kMINSessionOLSRVInfoID = "info_session_id";				/**< string 会话ID*/
static const char *kMINSessionOLSRVInfoUpdateT = "info_update_time";/**< uint64_t 会话最后更新时间*/
static const char *kMINSessionOLSRVInfoEXT = "info_ext";						/**< string 扩展字段，可自定义*/
static const char *kMINSessionOLSRVInfoLastMSG = "info_last_msg";		/**< string 最后一条消息的相关数据 json格式,*/
/** @}*/ //会话服务 【会话信息】定义参数

/** @name 会话服务 【查询会话列表】参数定义 */
static const char *kMINSessionOLSRVQueryListRESCode = "query_list_res_code";		/**< int 查询结果错误码 200成功 */
static const char *kMINSessionOLSRVQueryListMinTimestamp = "query_list_min_timestamp";		/**< uint64_t 最小时间戳 */
static const char *kMINSessionOLSRVQueryListMaxTimestamp = "query_list_max_timestamp";		/**< uint64_t 最大时间戳，翻页时使用 */
static const char *kMINSessionOLSRVQueryListNeedLastMsg = "query_list_need_last_msg";			/**< bool 结果中是否需要返回最近一条消息，缺省true */
static const char *kMINSessionOLSRVQueryListLimit = "query_list_limit";										/**< int，获取的最多记录数量，最大100，缺省100 */
static const char *kMINSessionOLSRVQueryListHasMore = "query_list_has_more";						/**<bool 结果集是否完整 如果为"false" 可以根据上一请求的 MaxTimestamp 再次发起增时请求*/
static const char *kMINSessionOLSRVQueryListInfos = "query_list_sessions";									/**< array 查询到的会话列表*/
/** @}*/ // 会话服务 【查询会话列表】参数定义

/** @name 会话服务 【查询会话】参数定义 */
static const char *kMINSessionOLSRVQueryType = "query_session_type";		/**< int 会话类型 */
static const char *kMINSessionOLSRVQueryID = "query_session_id";				/**< string 会话ID*/
/** @}*/ // 会话服务 【查询会话】参数定义

/** @name 会话服务 【更新会话】参数定义 */
static const char *kMINSessionOLSRVUpdateType = "update_session_type";		/**< int 会话类型 */
static const char *kMINSessionOLSRVUpdateID = "update_session_id";				/**< string 会话ID*/
static const char *kMINSessionOLSRVUpdateEXT = "update_ext";						/**< string 扩展字段，可自定义*/
/** @}*/ // 会话服务 【更新会话】参数定义

/** @name 会话服务 【删除会话】参数定义 */
static const char *kMINSessionOLSRVDeleteType = "delete_session_type";		/**< int 会话类型 */
static const char *kMINSessionOLSRVDeleteID = "delete_session_id";				/**< string 会话ID*/
/** @}*/ // 会话服务 【删除会话】参数定义

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_SESSION_ONLINE_SERVICE_DEF_H_