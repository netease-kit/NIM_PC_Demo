#pragma once

namespace nim_comp
{
/** @class AudioCallback
  * @brief 语音消息事件回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class AudioCallback
{
public:
	/**
	* 设置当前正在播放的语音消息所在的会话id
	* @param[in] sid 会话id
	* @return void 无返回值
	*/
	static void SetPlaySid(const std::string &sid);

	/**
	* 返回当前正在播放的语音消息所在的会话id
	* @return std::string 返回会话id
	*/
	static std::string GetPlaySid();

	/**
	* 设置当前正在播放的语音消息的消息id
	* @param[in] cid 消息id
	* @return void 无返回值
	*/
	static void SetPlayCid(const std::string &cid);

	/**
	* 返回当前正在播放的语音消息的消息id
	* @return std::string 返回消息id
	*/
	static std::string GetPlayCid();
public:
	/**
	* 开始播放语音消息的回调（播放语音消息需要和sdk交互，并非一定播放成功）
	* @param[in] code 返回码
	* @param[in] file_path 语音消息文件路径
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @return void 无返回值
	*/
	static void OnPlayAudioCallback(int code, const char* file_path, const char* sid, const char* cid);

	/**
	* 停止播放语音消息的回调
	* @param[in] code 返回码
	* @param[in] file_path 语音消息文件路径
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @return void 无返回值
	*/
	static void OnStopAudioCallback(int code, const char* file_path, const char* sid, const char* cid);
private:
	//当前正在播放的会话窗口id及语音消息id
	static std::string play_sid_;
	static std::string play_cid_;
};
}