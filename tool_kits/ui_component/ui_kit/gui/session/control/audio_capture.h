#pragma once

namespace nim_comp
{
typedef std::function<void(int rescode, const std::string& sid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration)> OnCaptureCompeleteCallback;

/** @class AudioCaptureView
  * @brief 在会话盒子中显示语音录制界面
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/9/12
  */
class AudioCaptureView : public ui::ButtonBox
{
public:
	AudioCaptureView();

	/**
	* 初始化语音录制功能界面
	* @param[in] session_id 语音录制界面所属会话id
	* @param[in] callback 语音录制完毕的回调通知函数
	* @return void 无返回值
	*/
	void InitControl(const std::string& session_id, OnCaptureCompeleteCallback callback);

	/**
	* 显示(隐藏)语音录制界面
	* @param[in] show 显示(隐藏)语音录制界面
	* @return void 无返回值
	*/
	void ShowControl(bool show);
private:
	/**
	* 响应按钮单击消息
	* @param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* param);
	
	/**
	* 开始录制语音的回调
	* @param[in] session_id 触发开始录音功能的会话id
	* @param[in] code 返回码
	* @return void 无返回值
	*/
	void OnStartCaptureCallback(const std::string& session_id, int code);

	/**
	* 录制语音完成的回调
	* @param[in] rescode 操作结果，一切正常200
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @param[in] file_path 语音文件绝对路径
	* @param[in] file_ext 文件扩展名
	* @param[in] file_size 文件大小
	* @param[in] audio_duration 语音时长
	* @return void 无返回值
	*/
	void OnStopCaptureCallback(int rescode, const std::string& sid, const std::string& cid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration);

	/**
	* 取消录制语音的回调
	* @param[in] session_id 触发取消录音功能的会话id
	* @param[in] code 返回码
	* @return void 无返回值
	*/
	void OnCancelCaptureCallback(const std::string& session_id, int code);

	/**
	* 枚举本地录音采集设备的回调
	* @param[in] rescode 操作结果，一切正常200
	* @param[in] device_list 设备列表
	*/
	void OnEnumCaptureDeviceCallback(int rescode, const wchar_t* device_list);

	/**
	* 录制播放时，更新录制进度的回调函数
	* @return void 无返回值
	*/
	void OnTick();

private:
	std::string		session_id_;
	OnCaptureCompeleteCallback compelete_callback_;

	bool			need_cancel_;
	std::string		msg_uuid_;

	ui::Label		*time_text_;
	ui::Label		*tip_text;
	ui::Button		*button_start_;
	ui::Button		*button_stop_;
	ui::Button		*button_cancel_;

	nbase::WeakCallbackFlag timer_;
	int				tick_;
};
}