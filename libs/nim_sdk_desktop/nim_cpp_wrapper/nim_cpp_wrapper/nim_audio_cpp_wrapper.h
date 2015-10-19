#pragma once


#include <string>
#include <list>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_audio_def.h"

/** @class Audio
  * @brief NIM 提供的语音录制和播放接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/10
  */
class Audio
{

public:
	/** 
	* NIM SDK 初始化语音模块
	* @param[in] user_data_parent_path 用户目录
	* @return bool true 调用成功，false 调用失败
	*/
	static bool Init(const std::string& res_audio_path);

	/**
	* NIM SDK 卸载语音模块（只有在主程序关闭时才有必要调用此接口）
	* @return bool true 调用成功，false 调用失败
	*/
	static void Cleanup();

	/** 
	* NIM SDK 播放,通过回调获取开始播放状态
	* @param[in] file_path 播放文件绝对路径
	* @param[in] call_id 用以定位资源的一级ID，可选
	* @param[in] res_id 用以定位资源的二级ID，可选
	* @return bool true 调用成功，false 调用失败
	*/
	static bool PlayAudio(const char* call_id, const char* res_id, const char* file_path);

	/** 
	* NIM SDK 停止播放,通过回调获取停止播放状态
	* @return bool true 调用成功，false 调用失败
	*/
	static bool StopPlayAudio();

	/** 
	* NIM SDK 注册播放开始事件回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegStartPlayCb(nim_rescode_id_cb cb);

	/** 
	* NIM SDK 注册播放结束事件回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegStopPlayCb(nim_rescode_id_cb cb);


private:
	template <typename F>
	static F Function(const char* function_name)
	{
		F f = (F) ::GetProcAddress(instance_audio_, function_name);
		assert(f);
		return f;
	}

private:
	static HINSTANCE instance_audio_;
};

}