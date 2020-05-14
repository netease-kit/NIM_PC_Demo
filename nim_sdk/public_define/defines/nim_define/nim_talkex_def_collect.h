/** @file nim_talkex_def_collect.h
  * @brief 收藏接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2020/04/19
  */
#ifndef NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_COLLECT_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_COLLECT_H_

#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @typedef void (*nim_talkex_collect_add_collect_cb_func)(int code, const char *collect_info, const void *user_data)
  * 添加收藏回调函数定义
  * @param[out] code	执行结果
  * @param[out] collect_info	添加收藏成功后会返回收藏的内容
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_collect_add_collect_cb_func)(int code, const char *collect_info, const void *user_data);

/** @typedef void (*nim_talkex_collect_remove_collects_cb_func)(int code, int count, const void *user_data)
  * 添加收藏回调函数定义
  * @param[out] code	执行结果
  * @param[out] count	删除的记录条数
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_collect_remove_collects_cb_func)(int code, int count, const void *user_data);

/** @typedef void (*nim_talkex_collect_update_collect_ext_cb_func)(int code, const char *collect_info, const void *user_data)
  * 更新收藏ext回调函数定义
  * @param[out] code	执行结果
  * @param[out] collect_info	更新成功后会返回收藏的内容
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_collect_update_collect_ext_cb_func)(int code, const char* collect_info, const void* user_data);

/** @typedef void (*nim_talkex_collect_query_collects_cb_func)(int code, int total_count,const char *collect_info_list, const void *user_data)
  * 分页查询收藏列表回调函数定义
  * @param[out] code	执行结果
  * @param[out] total_count	记录的总条数(非本次查询 到的条数)
  * @param[out] collect_info_list	查询到的收藏列表
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_collect_query_collects_cb_func)(int code, int total_count,const char *collect_info_list, const void *user_data);

/** @name收藏内容Json Keys
  * @{
  */
static const char *kNIMCollectInfoKeyID			= "id";			/**< uint64_t,ID */
static const char * kNIMCollectInfoKeyType= "type";			/**< int,收藏类型,开发者可根据业务自行定义 */
static const char * kNIMCollectInfoKeyData = "data";				/**< string,数据,最大20480 */
static const char * kNIMCollectInfoKeyExt = "ext";				/**< string,扩展字段,最大1024 */
static const char * kNIMCollectInfoKeyUniqueID = "unique_id";	/**< string,去重字段，拥有要同去重字段的收藏将会被覆盖 */
static const char * kNIMCollectInfoKeyCreateTime = "create_time";				/**< uint64_t,创建时间 */
static const char * kNIMCollectInfoKeyUpdateTime = "update_time";			/**< uint64_t,更新时间 */

/** @}*/ //收藏内容Json Keys

/** @name匹配收藏Json Keys
  * @{
  */
static const char* kNIMMatchCollectInfoKeyID = "id";			/**< uint64_t,ID */
static const char* kNIMMatchCollectInfoKeyCreateTime = "create_time";				/**< uint64_t,创建时间 */

/** @}*/ //匹配收藏Json Keys

/** @name分页查询收藏Json Keys
  * @{
  */
static const char* kNIMQueryCollectKeyFromTime = "from_time";			/**< uint64_t,起始时间,默认0  */
static const char* kNIMQueryCollectKeyToTime = "to_time";				/**< uint64_t,结束时间,推荐当前时间+1小时 */
static const char* kNIMQueryCollectKeyExcludeID = "exclude_id";				/**< uint64_t,结束查询的最后一条收藏的id(不包含在查询结果中) 缺省 可填0 */
static const char* kNIMQueryCollectKeyLimit = "limit";				/**< int,本次查询的上限(最多100条) */
static const char* kNIMQueryCollectKeyReverse = "reverse";				/**< bool,反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false） */
static const char* kNIMQueryCollectKeyType = "type";			/**< int 收藏类型，缺省(0)表示所有类型 */
/** @}*/ //分页查询收藏Json Keys

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_COLLECT_H_