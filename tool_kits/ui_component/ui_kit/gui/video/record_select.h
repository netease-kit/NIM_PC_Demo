#pragma once

namespace nim_comp
{
	typedef std::function<void(bool ret, std::wstring, std::wstring, std::wstring)> SelectRecordFileCallback;

/** @class RecordSelectForm
  * @brief 提供选择音视频通话时录制选择的功能
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @author gq
  * @date 2017/02/15
  */
class RecordSelectForm : public nim_comp::WindowEx
{
public:
	RecordSelectForm();
	~RecordSelectForm();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/**
	* 处理所有控件的选择消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnSelected(ui::EventArgs* msg);

	/**
	* 设置选择窗口完毕的回调函数
	* @param[in] video_mode 是否是视频模式
	* @param[in] cb 回调函数
	* @return void 无返回值
	*/
	void SetSelFileCb(bool video_mode, SelectRecordFileCallback cb);

private:

public:
	static const LPTSTR kClassName;

private:
	SelectRecordFileCallback select_file_cb_; 
	ui::RichEdit* path_edit_;
	ui::CheckBox* mp4_sel_;
	ui::CheckBox* mp4_sel2_;
	ui::CheckBox* audio_sel_;
};
}