/** @file nim_tool_helper.h
  * @brief SDK工具类辅助方法以及定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/24
  */

#ifndef _NIM_SDK_CPP_TOOL_HELPER_H_
#define _NIM_SDK_CPP_TOOL_HELPER_H_

#include <functional>
#include <string>
#include "assert.h"
#include "wtypes.h"

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

#include "nim_res_code_def.h"

/** @brief 语音信息，用于语音转文字 */
struct AudioInfo
{
	std::string mime_type_;				/**< 语音类型 */
	std::string samplerate_;			/**< 采样率 */
	std::string url_;					/**< 上传云端后得到的下载地址 */
	unsigned __int64 duration_;			/**< 语音时长 */

	/** 构造函数 */
	AudioInfo() : duration_(0) {}
};

}

#endif //_NIM_SDK_CPP_TOOL_HELPER_H_