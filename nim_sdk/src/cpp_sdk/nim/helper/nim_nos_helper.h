/** @file nim_nos_helper.h
  * @brief NOS数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2016/12/16
  */

#ifndef _NIM_SDK_CPP_NOS_HELPER_H_
#define _NIM_SDK_CPP_NOS_HELPER_H_

#include <string>
#include <list>
#include <map>
#include <functional>
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

	/** @brief 初始化nos参数 */
	struct NIM_SDK_CPPWRAPPER_DLL_API InitNosConfigParam
	{
		/** 构造函数 */
		InitNosConfigParam() {}
		/** @fn void AddTag(const std::string& tag, uint64_t survival_time)
		* @param[in] tag 场景标签名称
		* @param[in] survival_time 该场景下资源生命周期 最小取值 InitNosConfigParam::kMINSURVIVALTIME
		* @return void
		*/
		void AddTag(const std::string& tag, int32_t survival_time)
		{
			tag_list_[tag] = ((survival_time == 0) ? (0) : (survival_time > kMINSURVIVALTIME ? survival_time : kMINSURVIVALTIME));
		}
		/** @fn void RemoveTag(const std::string& tag)
		* @param[in] tag 场景标签名称
		* @return void
		*/
		void RemoveTag(const std::string& tag)
		{
			tag_list_.erase(tag);
		}
		/** @fn std::map<std::string, uint64_t> GetTagList() const
		* @return std::map<std::string, uint64_t> 场景标签信息列表 
		*/
		std::map<std::string, int32_t> GetTagList() const
		{
			return tag_list_;
		}
		/** @fn void SetGetExtension(const std::string& json_extension)
		* @param[in] json_extension 扩展数据
		* @return void
		*/
		void SetGetExtension(const std::string& json_extension)
		{
			json_extension_ = json_extension;
		}
		/** @fn std::string GetExtension() const
		* @return string  扩展数据 
		*/
		std::string GetExtension() const
		{
			return json_extension_;
		}
	private:
		std::map<std::string, int32_t> tag_list_;/**< std::map<std::string, uint64_t> 场景标签信息列表 */
		std::string json_extension_; /**< Json Value 扩展数据 */
		static const int32_t kMINSURVIVALTIME;/**< int32_t 资源生命周期 最小取值 */
	};
	/** @brief 初始化结果 */
	struct NIM_SDK_CPPWRAPPER_DLL_API InitNosResult
	{
		NIMNosInitConfigResultType result_; /**< enum 初始化结果 */
		std::list<std::string> success_req_tags_; /**< list 初始化成功的tag列表 */
		std::map<std::string, int> failure_req_tags_; /**< map 初始化失败的tag列表 */
		std::list<std::string> ignore_req_tags_; /**< list 不需要重新初始化tag列表 */
		void FromJsonString(const std::string& json_data);
	};

/** @brief 上传完成的结果 */
struct NIM_SDK_CPPWRAPPER_DLL_API UploadMediaResult
{
	std::string url_;				/**< 上传地址 */
	std::string res_id_;			/**< 资源id */
	std::string call_id_;			/**< 请求者id */

	/** 构造函数 */
	UploadMediaResult() {}
};

/** @brief 下载完成的结果 */
struct NIM_SDK_CPPWRAPPER_DLL_API DownloadMediaResult
{
	std::string file_path_;			/**< 本地绝对路径 */
	std::string call_id_;			/**< 请求者id */
	std::string res_id_;			/**< 资源id */

	/** 构造函数 */
	DownloadMediaResult() {}
};

/** @brief 传输过程中的数据 */
struct NIM_SDK_CPPWRAPPER_DLL_API ProgressData
{
	std::string res_id_;			/**< 资源id */

	/** 构造函数 */
	ProgressData() {}
};

/** @fn bool ParseUploadResult(const std::string& url, const std::string& json, UploadMediaResult& res)
  * @brief 解析NOS上传结果
  * @param[in] url 上传结果地址
  * @param[in] json Json Value扩展数据
  * @param[out] res 结果结构体
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseUploadResult(const std::string& url, const std::string& json, UploadMediaResult& res);

/** @fn bool ParseDownloadResult(const std::string& file_path, const std::string& call_id, const std::string& res_id, DownloadMediaResult& res)
  * @brief 解析NOS上传过程数据
  * @param[in] file_path 下载文件地址
  * @param[in] call_id 如果下载的是消息中的资源，则为消息所属的会话id，否则为空
  * @param[int] res_id 如果下载的是消息中的资源，则为消息id，否则为空
  * @param[in] json Json Value扩展数据
  * @param[out] res 数据结构体
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseDownloadResult(const std::string& file_path, const std::string& call_id, const std::string& res_id, DownloadMediaResult& res);

/** @fn bool ParseProgressData(const std::string& json, ProgressData& res)
  * @brief 解析NOS上传过程数据
  * @param[in] json Json Value扩展数据
  * @param[out] res 数据结构体
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseProgressData(const std::string& json, ProgressData& res);

} //namespace nim

#endif //_NIM_SDK_CPP_NOS_HELPER_H_