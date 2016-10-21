#pragma once
#include "util/window_ex.h"

/** @class ProxyForm
  * @brief 代理设置窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author wp
  * @date 2014/02/14
  */
class ProxyForm : public nim_comp::WindowEx
{
public:
	ProxyForm();
	virtual ~ProxyForm();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };
	virtual UINT GetClassStyle() const override	{ return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

private:
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool Notify(ui::EventArgs* msg);

	/**
	* 处理代理类型组合框被选择的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnProxyTypeSelected(ui::EventArgs* msg);

	/**
	* 检查输入的代理地址和端口是否合法
	* @return bool true 合法，false 不合法
	*/
	bool CheckProxyLegality();

	/**
	* 把代理类型组合框的选中索引转换为对应的代理类型
	* @param[in] index 选中索引
	* @return nim::NIMProxyTyp 代理类型
	*/
	nim::NIMProxyType ConvertIndexToProxyType(int index);

	/**
	* 把代理类型转换为代理类型组合框的选中索引
	* @param[in] nim::NIMProxyTyp 代理类型
	* @return int 索引
	*/
	int ConvertProxyTypeToIndex(nim::NIMProxyType type);

	/**
	* 根据设置的代理信息，设置nim、nim_http、chatroom模块的代理信息
	* @return void	无返回值
	*/
	void ApplyProxySetting();

private:
	ui::Combo*    proxy_type_comb_;
	ui::RichEdit* addr_ctrl_;
	ui::RichEdit* port_ctrl_;
	ui::RichEdit* user_ctrl_;
	ui::RichEdit* pass_ctrl_;
	ui::Button*	 test_connect_btn_;
	ui::Button*	 confirm_btn_;
	ui::Button*   apply_btn_;
	ui::Control*  gifanim_ctrl_;

public:
	static const LPCTSTR kClassName;
	
	static nim::NIMProxyType	cur_type;
	static std::string			cur_host;
	static short				cur_port;
	static std::string			cur_user;
	static std::string			cur_pass;
};