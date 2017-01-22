#pragma once

namespace nim_comp
{
/** @class LinkForm
  * @brief 断线重连提示窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class LinkForm : public WindowEx
{
	enum LinkPage
	{
		LINK_TIP,
		LINK_ING,
		LINK_SUCCESS
	};
public:
	LinkForm();
	~LinkForm();
	
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
	* 设置登录结果。
	* @param[in] res_code SDK状态码
	* @param[in] retrying SDK是否在重连
	* @return void	无返回值
	*/
	void SetLoginRetryingFlag(nim::NIMResCode res_code, bool retrying);
private:
	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnClicked( ui::EventArgs* msg );

	/**
	* 切换到指定的页面
	* @param[in] lp 页面类型
	* @return void	无返回值
	*/
	void SwitchTo(LinkPage lp);

	/**
	* 处理断线重连事件
	* @param[in] json 断线重连消息json串
	* @return void	无返回值
	*/
	void OnRelink(const Json::Value &json);

	/**
	* 重连成功后超时自动关闭窗口的回调函数
	* @return void	无返回值
	*/
	void OnTimeup();

public:
	static const LPCTSTR kClassName;
private:
	nbase::WeakCallbackFlag timer_;
	AutoUnregister unregister_cb;
};

/**
* 显示断线重连窗口
* @param[in] res_code SDK状态码
* @param[in] retrying SDK是否在重连
* @return void	无返回值
*/
void ShowLinkForm(nim::NIMResCode res_code, bool retrying);
}