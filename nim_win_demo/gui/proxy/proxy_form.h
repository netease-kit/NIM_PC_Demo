//
//  proxy_form.h
//
//
//  Created by wp on 2014.02.14.
//  Copyright (c) 2013 Netease. All rights reserved.
//
//  代理设置窗口

#ifndef NIM_WIN_GUI_PROXY_FORM_H_
#define NIM_WIN_GUI_PROXY_FORM_H_

#include "util/window_ex.h"

class ProxyForm : public nim_comp::WindowEx
{
public:
	ProxyForm();
	virtual ~ProxyForm();

	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override
	{
		return kClassName;
	};
	virtual std::wstring GetWindowId() const override
	{
		return kClassName;
	};
	virtual UINT GetClassStyle() const override
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	};
	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* msg);

private:
	bool OnProxyTypeSelected(ui::EventArgs* msg);
	bool CheckProxyLegality();
	nim::NIMProxyType ConvertIndexToProxyType(int index);
	int ConvertProxyTypeToIndex(nim::NIMProxyType type);
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

#endif //YIXIN_WIN_GUI_PROXY_FORM_H_