#pragma once
#include "util/window_ex.h"
class ProxyTip;
class ProxyTipGlobal;
class ProxyTipSub;
class SubModuleProxySetting : public  nbase::ProxySetting
{
public:
	SubModuleProxySetting() : use_global_setting_(true){}
public:
	bool IsUseGlobalSetting() const { return use_global_setting_;	};
	void SetUseGlobalSetting(bool use){ use_global_setting_ = use; }
private:
	bool use_global_setting_;
};
typedef std::map<std::wstring, std::shared_ptr<nbase::ProxySetting>> ProxySettingList;
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
	virtual void Close(UINT nRet  = IDOK) override;
	virtual void AttachBeforClose(const std::function<void()>& cb) { befor_close_cb_ = cb; };
	static std::shared_ptr<nbase::ProxySetting> GetProxySetting(const std::wstring& name)
	{
		if (proxy_setting_list_.find(name) != proxy_setting_list_.end())
			return proxy_setting_list_[name];
		return nullptr;
	}
private:
	ProxyTipSub* CreateSubModuleProxyTip(const std::function<ProxyTipSub*()>& sub_creator,const std::wstring& name);
	void ApplyProxySetting();
	void ConfirmSetting();
public:
	static const LPCTSTR kClassName;
private:
	ui::Button*	 confirm_btn_;
	ui::Button*	 close_btn_;
	ProxyTipGlobal* global_proxy_;
	ProxyTipSub* proxy_chatroom_;
	ProxyTipSub* proxy_vchat_;
	ProxyTipSub* proxy_rts_;
	ProxyTipSub* proxy_httptool_;
	std::function<void()> befor_close_cb_;
	static ProxySettingList proxy_setting_list_;
};
template<typename TProxySetting>
class ProxySettingHost
{
public:
	ProxySettingHost() : proxy_setting_(nullptr){};
	virtual ~ProxySettingHost(){}
	static std::shared_ptr<TProxySetting> CreateSetting()
	{
		return std::make_shared<TProxySetting>();
	}
	void SetProxySetting(const std::shared_ptr<TProxySetting>& setting){ proxy_setting_ = setting; };
	std::shared_ptr<TProxySetting> GetProxySetting() const { return proxy_setting_; };
protected:
	std::shared_ptr<TProxySetting> proxy_setting_;
};
class ProxyTip : public ui::Box
{
public:
	virtual void DoInit() override;
	void SetProxyName(const std::wstring& name);
	std::wstring GetProxyName() const;
	/**
	* 根据设置的代理信息，设置nim、nim_http、chatroom模块的代理信息
	* @return void	无返回值
	*/
	virtual void ApplyProxySetting() = 0;
	/**
	* 把NIM代理类型转换为对应的BASE代理类型
	* @param[in] type nim代理类型
	* @return nbase::ProxyType BASE代理类型
	*/
	static nbase::ProxyType ConvertNimProxytypeToBaseProxyType(nim::NIMProxyType type);

	/**
	* 把Base代理类型转换为对应的Nim代理类型
	* @param[in] type BASE代理类型
	* @return nim::NIMProxyType Nim代理类型
	*/
	static nim::NIMProxyType ConvertBaseProxytypeToNimProxyType(nbase::ProxyType type);
protected:
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
	* 设置面板可用
	* @param[in] enable 可用
	* @return void	无返回值
	*/
	void SetPanelEnabled(bool enable);	

	virtual std::vector<std::wstring> GetSupportedProxyTypeName() = 0;
protected:
	ui::Combo*    proxy_type_comb_;
	ui::RichEdit* addr_ctrl_;
	ui::RichEdit* port_ctrl_;
	ui::RichEdit* user_ctrl_;
	ui::RichEdit* pass_ctrl_;	
	std::wstring proxy_name_;
};
class ProxyTipGlobal : public ProxyTip, public ProxySettingHost< nbase::ProxySetting>
{
public:
	virtual void DoInit() override;
	virtual void ApplyProxySetting() override;
protected:
	virtual std::vector<std::wstring> GetSupportedProxyTypeName() override;
private:
	/**
	* 检查输入的代理地址和端口是否有效
	* @return void
	*/
	void DetectProxyLegality();
	void CallbackDetectProxy(bool connect, nim::NIMProxyDetectStep step, const std::string& json_extention);
private:
	ui::Button*	 test_connect_btn_;
	ui::Control*  gifanim_ctrl_;
	bool being_test_connect_;
};
class ProxyTipSub : public ProxyTip, public ProxySettingHost< SubModuleProxySetting>
{
public:
	virtual void DoInit() override;
	virtual void ApplyProxySetting() override;
private:
	ui::CheckBox* use_global_;
};
class ProxyTipSub_VChat : public ProxyTipSub
{
protected:
	virtual std::vector<std::wstring> GetSupportedProxyTypeName() override;
};
class ProxyTipSub_Rts : public ProxyTipSub
{
protected:
	virtual std::vector<std::wstring> GetSupportedProxyTypeName() override;
};
class ProxyTipSub_Chatroom : public ProxyTipSub
{
protected:
	virtual std::vector<std::wstring> GetSupportedProxyTypeName() override;
};
class ProxyTipSub_Httptool : public ProxyTipSub
{
protected:
	virtual std::vector<std::wstring> GetSupportedProxyTypeName() override;
};