/** @file nim_session_online_service.h
  * @brief 会话服务online 接口头文件
  * @copyright (c) 2015-2019, NetEase Inc. All rights reserved
  * @author cqu227hk
  * @date 2019/10/21
  */
#ifndef NIM_SDK_DLL_API_NIM_SESSION_ONLINE_SERVICE_H_
#define NIM_SDK_DLL_API_NIM_SESSION_ONLINE_SERVICE_H_

#include "public_define/defines/nim_sdk_dll.h"
#include "public_define/defines/nim_define/nim_session_online_service_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
	/** @fn void nim_session_online_service_query_session_list(uint64_t min_time,uint64_t max_time,bool need_last_msg,int limit,nim_session_online_service_query_session_list_cb_func cb, const void *user_data);
	  * 会话服务 查询会话列表
	  * @param[in] min_time 最小时间戳,未知可以填0,表示到最早时间
	  * @param[in] max_time 最大时间戳,未知可以填0,表示从当前时间
	  * @param[in] need_last_msg 是否需要lastMsg，默认true
	  * @param[in] limit 结果集limit，最大100，缺省值100
	  * @param[in] cb 结果回调  可查看nim_session_online_service_query_session_list_cb_func定义
	  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	  * @return void 无返回值
	  */
	NIM_SDK_DLL_API void nim_session_online_service_query_session_list(uint64_t min_time,uint64_t max_time,bool need_last_msg,int limit,nim_session_online_service_query_session_list_cb_func cb, const void *user_data);

	/** @fn void nim_session_online_service_query_session(int to_type,char* session_id, nim_session_online_service_query_session_cb_func cb, const void *user_data)
	  * 会话服务 查询会话
	  * @param[in] to_type 会话类型
	  * @param[in] session_id 会话ID
	  * @param[in] cb 结果回调  可查看nim_session_online_service_query_session_cb_func定义
	  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	  * @return void 无返回值
	  */
	NIM_SDK_DLL_API void nim_session_online_service_query_session(int to_type,char* session_id, nim_session_online_service_query_session_cb_func cb, const void *user_data);

	/** @fn void nim_session_online_service_update_session(int to_type, char* session_id, char* ext, nim_session_online_service_update_session_cb_func cb, const void *user_data)
	  * 会话服务 更新会话
	  * @param[in] to_type 会话类型
	  * @param[in] session_id 会话ID
	  * @param[in] ext 扩展字段
	  * @param[in] cb 结果回调  可查看nim_session_online_service_update_session_cb_func定义
	  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	  * @return void 无返回值
	  */
	NIM_SDK_DLL_API void nim_session_online_service_update_session(int to_type, char* session_id, char* ext, nim_session_online_service_update_session_cb_func cb, const void *user_data);

	/** @fn void nim_session_online_service_delete_session(char* session_list, nim_session_online_service_delete_session_cb_func cb, const void *user_data)
	  * 会话服务 删除会话
	  * @param[in] session_list 要删除的会话列表 json array
	  * @param[in] cb 结果回调  可查看nim_session_online_service_delete_session_cb_func定义
	  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	  * @return void 无返回值
	  */
	NIM_SDK_DLL_API void nim_session_online_service_delete_session(char* session_list, nim_session_online_service_delete_session_cb_func cb, const void *user_data);

	/** @fn void nim_session_online_service_reg_session_changed(nim_session_online_service_session_changed_cb_func cb, const void *user_data)
	  * 会话服务 注册会话变更回调
	  * @param[in] cb 结果回调  可查看nim_session_online_service_session_changed_cb_func定义
	  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	  * @return void 无返回值
	  */
	NIM_SDK_DLL_API void nim_session_online_service_reg_session_changed(nim_session_online_service_session_changed_cb_func cb, const void *user_data);

	
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //NIM_SDK_DLL_API_NIM_DATA_SYNC_H_
