#pragma once

/** @class WindowsManager
* @brief 提供一个简易的提示窗体，相当于MessageBox的简化版
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2016/8/24
*/
namespace nim_comp
{
class Toast : public ui::WindowImplBase
{
public:
	/**
	* 在屏幕中间显示一个提示框
	* @param[in] content 提示内容
	* @param[in] duration 持续时间(毫秒),0代表一直显示
	* @param[in] parent 父窗口句柄，如果提供的话toast会显示在父窗口的中心
	* @return void 无返回值
	*/
	static void ShowToast(const std::wstring &content, int duration = 0, HWND parent = NULL);
public:
	Toast(){};
	~Toast(){};

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const /*override*/;
	virtual UINT GetClassStyle() const override;

	virtual void InitWindow() override;
	void SetDuration(int duration);

private:
	bool OnClicked(ui::EventArgs* msg);
	void SetContent(const std::wstring &str);

public:
	static const LPCTSTR kClassName;
private:
	ui::RichEdit	*content_;
	ui::Button		*close_button_;
};
}