#pragma once

namespace nim_comp
{
typedef std::function<void(HWND, bool cut)> SelectWndCallback;

/** @class WindowSelectForm
  * @brief 提供枚举并选择窗口的功能
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author gaoqi
  * @date 2016/09/21
  */
class WindowSelectForm : public nim_comp::WindowEx
{
public:
	WindowSelectForm();
	~WindowSelectForm();

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
	* @param[in] cb 回调函数
	* @return void 无返回值
	*/
	void SetSelWndCb(SelectWndCallback cb){ select_wnd_cb_ = cb; }

private:
	/**
	* 刷新窗口列表
	* @return void	无返回值
	*/
	void RefreshWndlist();

public:
	static const LPTSTR kClassName;

private:
	SelectWndCallback select_wnd_cb_;
	ui::ListBox* wnd_list_;
};
}