#ifndef _NIM_SDK_CPP_TOOL_HELPER_H_
#define _NIM_SDK_CPP_TOOL_HELPER_H_

#include <functional>
#include <string>
#include "assert.h"
#include "wtypes.h"

namespace nim
{
/** @brief SDK工具类辅助方法以及定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/24
  */

#include "nim_res_code_def.h"

struct AudioInfo
{
	std::string mime_type_;
	std::string samplerate_;
	std::string url_;
	unsigned __int64 duration_;

	AudioInfo() : duration_(0) {}
};

}

#endif //_NIM_SDK_CPP_TOOL_HELPER_H_