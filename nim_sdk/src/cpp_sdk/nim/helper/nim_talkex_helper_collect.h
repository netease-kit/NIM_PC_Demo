/** @file nim_talkex_helper_collect.h
  * @brief Talkex 收藏辅助方法和数据结构定义
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#ifndef _NIM_SDK_CPP_TALKEX_HELPER_COLLECT_H_
#define _NIM_SDK_CPP_TALKEX_HELPER_COLLECT_H_

#include <string>
#include <list>
#include <functional>
#include "include/depend_lib/include/nim_json_util.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
	/** @brief 收藏内容属性 */
	struct  NIM_SDK_CPPWRAPPER_DLL_API CollectInfo
	{
		uint64_t id;/**< id */
		int64_t type;/**< int 类型,开发者可根据业务自定义其含意 */
		std::string data;/**<数据,string,最大20480 */
		std::string ext;/**< 扩展字段,string,最大1024*/
		std::string unique_id;/**< 去重唯一ID */
		uint64_t create_time;/**<创建时间 */
		uint64_t update_time;/**< 更新时间*/
		static std::string ToJsonString(const CollectInfo& info);
		static void FromJsonString(const std::string& json_string, CollectInfo& info);
	};

	/** @brief 收藏列表 */
	struct NIM_SDK_CPPWRAPPER_DLL_API CollectInfoList
	{
		std::list<CollectInfo> list;
		static void FromJsonString(const std::string& json_string, CollectInfoList& list);
	};

	/** @brief 收藏匹配参数 */
	struct NIM_SDK_CPPWRAPPER_DLL_API MatchCollectParm
	{
		MatchCollectParm(uint64_t collet_create_time, uint64_t collet_id) :create_time(collet_create_time), id(collet_id) {}
		uint64_t create_time;/**<创建时间 */
		uint64_t id;/**< id */
		static std::string ToJsonString(const MatchCollectParm& param);
	};

	/** @brief 收藏内容参数 */
	struct NIM_SDK_CPPWRAPPER_DLL_API RemoveCollectsParm
	{		
		void Add(const CollectInfo& info)
		{
			list.emplace_back(info.create_time,info.id);
		}
		void Add(uint64_t id, uint64_t create_time)
		{
			list.emplace_back(create_time, id);
		}
		int Size() const
		{
			return (int)list.size();
		}
		static std::string ToJsonString(const RemoveCollectsParm& info);
	private:
		std::list< MatchCollectParm> list;
	};
	
	/** @brief 分页查询收藏列表参数 */
	struct NIM_SDK_CPPWRAPPER_DLL_API QueryCollectsParm
	{
		uint64_t from_time;/**<起始时间,默认0 */
		uint64_t to_time;/**<结束时间,推荐当前时间+1小时 */ 
		uint64_t exclude_id;/**<结束查询的最后一条收藏的id(不包含在查询结果中)缺省 可填0 */
		int limit; /**< 本次查询的上限(最多100条) */
		bool reverse;/**<反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false） */
		int type;/**< 收藏类型，缺省表示所有类型 */
		static std::string ToJsonString(const QueryCollectsParm& info);
	};
} //namespace nim

#endif //_NIM_SDK_CPP_TALKEX_HELPER_COLLECT_H_