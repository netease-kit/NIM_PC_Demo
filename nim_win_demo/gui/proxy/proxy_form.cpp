#include "stdafx.h"
#include "shared/ui/msgbox.h"
#include "proxy_form.h"
#include "base/network/network_util.h"

//#include "base/network/http.h"
//#include "util/NetStateTestHelper.h"
//#include "LibNEPLoginHelper.h"

using namespace ui;
const LPCTSTR ProxyForm::kClassName = L"ProxyForm";
ProxySettingList ProxyForm::proxy_setting_list_;


ProxyForm::ProxyForm():
	befor_close_cb_(nullptr)
{
	
}

ProxyForm::~ProxyForm()
{

}

std::wstring ProxyForm::GetSkinFolder()
{
	return L"proxy";
}

std::wstring ProxyForm::GetSkinFile()
{
	return L"proxy_form.xml";
}

void ProxyForm::InitWindow()
{
	std::wstring proxy_name;
	auto global_proxy = dynamic_cast<ui::Box*>(FindControl(L"global_proxy"));
	if (global_proxy != nullptr)
	{
		proxy_name = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_PROXY_GLOBAL_SETTIN");
		global_proxy_ = new ProxyTipGlobal;
		if(ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::Simplified_Chinese)
			ui::GlobalManager::FillBoxWithCache(global_proxy_, L"proxy/proxy_tip_global.xml");
		if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
			ui::GlobalManager::FillBoxWithCache(global_proxy_, L"proxy/proxy_tip_global_en.xml");
		global_proxy_->SetProxyName(proxy_name);
		if (proxy_setting_list_.find(proxy_name) == proxy_setting_list_.end())
			proxy_setting_list_[proxy_name] = ProxyTipGlobal::CreateSetting();
		global_proxy_->SetProxySetting(proxy_setting_list_[proxy_name]);
		global_proxy->Add(global_proxy_);
	}
	auto proxys = dynamic_cast<ui::ListBox*>(FindControl(L"proxys"));
	if (proxys != nullptr)
	{
		MutiLanSupport* multilan = MutiLanSupport::GetInstance();
		proxy_chatroom_ = CreateSubModuleProxyTip([](){return (new ProxyTipSub_Chatroom()); }, 
			multilan->GetStringViaID(L"STRID_PROXY_CHATROOM_SETTIN"));
		proxy_vchat_ = CreateSubModuleProxyTip([](){return (new ProxyTipSub_VChat()); }, 
			multilan->GetStringViaID(L"STRID_PROXY_VCHAT_SETTIN"));
		proxy_rts_ = CreateSubModuleProxyTip([](){return (new ProxyTipSub_Rts()); }, 
			multilan->GetStringViaID(L"STRID_PROXY_RTS_SETTIN"));
		proxy_httptool_ = CreateSubModuleProxyTip([](){return (new ProxyTipSub_Httptool); }, 
			multilan->GetStringViaID(L"STRID_PROXY_HTTPTOOL_SETTIN"));
		proxys->Add(proxy_chatroom_);
		proxys->Add(proxy_vchat_);
		proxys->Add(proxy_rts_);
		proxys->Add(proxy_httptool_);
	}
	confirm_btn_ = (ui::Button*)FindControl(L"confirm");
	close_btn_ = dynamic_cast<ui::Button*>(FindControl(L"close"));
	confirm_btn_->AttachClick([this](ui::EventArgs*){
		ApplyProxySetting();
		ConfirmSetting();
		Close(0);
		return true;
	});
	close_btn_->AttachClick([this](ui::EventArgs*){
		Close(0);
		return true;
	});
}
void ProxyForm::ApplyProxySetting()
{
	global_proxy_->ApplyProxySetting();
	proxy_chatroom_->ApplyProxySetting();
	proxy_vchat_->ApplyProxySetting();
	proxy_rts_->ApplyProxySetting();
	proxy_httptool_->ApplyProxySetting();	
}
void ProxyForm::Close(UINT nRet/* = IDOK*/)
{	
	if (befor_close_cb_ != nullptr)
	{
		befor_close_cb_();
	}
	nim_comp::WindowEx::Close(nRet);
}
void ProxyForm::ConfirmSetting()
{
	auto global_proxy_setting = global_proxy_->GetProxySetting();
	nim::Global::SetProxy(ProxyTip::ConvertBaseProxytypeToNimProxyType(global_proxy_setting->GetType()), \
		global_proxy_setting->GetHost(), global_proxy_setting->GetPort(), global_proxy_setting->GetUserName(), global_proxy_setting->GetPassowrd());
	//聊天室
	auto chatroom_proxy_setting = proxy_chatroom_->GetProxySetting();
	if (chatroom_proxy_setting->IsUseGlobalSetting())
		nim_chatroom::ChatRoom::SetProxy((nim_chatroom::NIMChatRoomProxyType)(ProxyTip::ConvertBaseProxytypeToNimProxyType(global_proxy_setting->GetType())),\
		global_proxy_setting->GetHost(), global_proxy_setting->GetPort(), global_proxy_setting->GetUserName(), global_proxy_setting->GetPassowrd());
	else
		nim_chatroom::ChatRoom::SetProxy((nim_chatroom::NIMChatRoomProxyType)(ProxyTip::ConvertBaseProxytypeToNimProxyType(chatroom_proxy_setting->GetType())), \
		chatroom_proxy_setting->GetHost(), chatroom_proxy_setting->GetPort(), chatroom_proxy_setting->GetUserName(), chatroom_proxy_setting->GetPassowrd());
	//httptool
	auto httptool_proxy_setting = proxy_httptool_->GetProxySetting();
	if (httptool_proxy_setting->IsUseGlobalSetting())
		nim_http::SetGlobalProxy((NIMProxyType)(ProxyTip::ConvertBaseProxytypeToNimProxyType(global_proxy_setting->GetType())), \
		global_proxy_setting->GetHost(), global_proxy_setting->GetPort(), global_proxy_setting->GetUserName(), global_proxy_setting->GetPassowrd());
	else
		nim_http::SetGlobalProxy((NIMProxyType)(ProxyTip::ConvertBaseProxytypeToNimProxyType(httptool_proxy_setting->GetType())), \
		httptool_proxy_setting->GetHost(), httptool_proxy_setting->GetPort(), httptool_proxy_setting->GetUserName(), httptool_proxy_setting->GetPassowrd());	

	//VChat
	auto vchat_proxy_setting = proxy_vchat_->GetProxySetting();
	if (vchat_proxy_setting->IsUseGlobalSetting())
		nim::VChat::SetProxy(ProxyTip::ConvertBaseProxytypeToNimProxyType(global_proxy_setting->GetType()), \
		global_proxy_setting->GetHost(), global_proxy_setting->GetPort(), global_proxy_setting->GetUserName(), global_proxy_setting->GetPassowrd());
	else
		nim::VChat::SetProxy(ProxyTip::ConvertBaseProxytypeToNimProxyType(vchat_proxy_setting->GetType()), \
		vchat_proxy_setting->GetHost(), vchat_proxy_setting->GetPort(), vchat_proxy_setting->GetUserName(), vchat_proxy_setting->GetPassowrd());

	//rts
	auto rts_proxy_setting = proxy_rts_->GetProxySetting();
	if (rts_proxy_setting->IsUseGlobalSetting())
		nim::Rts::SetProxy(ProxyTip::ConvertBaseProxytypeToNimProxyType(global_proxy_setting->GetType()), \
		global_proxy_setting->GetHost(), global_proxy_setting->GetPort(), global_proxy_setting->GetUserName(), global_proxy_setting->GetPassowrd());
	else
		nim::Rts::SetProxy(ProxyTip::ConvertBaseProxytypeToNimProxyType(rts_proxy_setting->GetType()), \
		rts_proxy_setting->GetHost(), rts_proxy_setting->GetPort(), rts_proxy_setting->GetUserName(), rts_proxy_setting->GetPassowrd());
}
ProxyTipSub* ProxyForm::CreateSubModuleProxyTip(const std::function<ProxyTipSub*()>& sub_creator, const std::wstring& name)
{
	auto proxy = sub_creator();
	ui::GlobalManager::FillBoxWithCache(proxy, L"proxy/proxy_tip.xml");
	proxy->SetProxyName(name);
	if (proxy_setting_list_.find(name) == proxy_setting_list_.end())
		proxy_setting_list_[name] = ProxyTipSub::CreateSetting();
	proxy->SetProxySetting(std::dynamic_pointer_cast<SubModuleProxySetting>(proxy_setting_list_[name]));
	return proxy;
}
void ProxyTip::SetProxyName(const std::wstring& name)
{
	proxy_name_ = name;
	auto name_tip = dynamic_cast<ui::Label*>(FindSubControl(L"proxy_name"));
	if (name_tip != nullptr)
		name_tip->SetText(proxy_name_);
}
std::wstring ProxyTip::GetProxyName() const
{
	return proxy_name_;
}
bool ProxyTip::OnProxyTypeSelected(ui::EventArgs * msg)
{
	int sel = proxy_type_comb_->GetCurSel();

	switch (sel)
	{
	case 0:
	case 1:
		addr_ctrl_->SetText(L"");
		addr_ctrl_->SetEnabled(false);
		port_ctrl_->SetText(L"");
		port_ctrl_->SetEnabled(false);
		user_ctrl_->SetText(L"");
		user_ctrl_->SetEnabled(false);
		pass_ctrl_->SetText(L"");
		pass_ctrl_->SetEnabled(false);
		break;
	case 2:
	case 3:
		addr_ctrl_->SetEnabled(true);
        if (port_ctrl_->GetText().empty())
            port_ctrl_->SetText(L"1080");
		port_ctrl_->SetEnabled(true);
		user_ctrl_->SetEnabled(false);
		pass_ctrl_->SetEnabled(false);		
		break;
	case 4:
		addr_ctrl_->SetEnabled(true);
        if (port_ctrl_->GetText().empty())
            port_ctrl_->SetText(L"1080");
		port_ctrl_->SetEnabled(true);
		user_ctrl_->SetEnabled(true);
		pass_ctrl_->SetEnabled(true);
		break;
	case 5:
		addr_ctrl_->SetEnabled(true);
		port_ctrl_->SetEnabled(true);
		user_ctrl_->SetEnabled(true);
		pass_ctrl_->SetEnabled(true);
		break;
	default:
		break;
	}

	return true;
}
void ProxyTip::SetPanelEnabled(bool enable)
{
	addr_ctrl_->SetEnabled(enable);
	port_ctrl_->SetEnabled(enable);
	user_ctrl_->SetEnabled(enable);
	pass_ctrl_->SetEnabled(enable);
	proxy_type_comb_->SetEnabled(enable);
}

bool ProxyTip::CheckProxyLegality()
{
	int index = proxy_type_comb_->GetCurSel();
	if (index == 0)
		return true;

	/*std::regex pattern("(25[0-5]|2[0-4]\d|((1\d{2})|([1-9]?\d))\.){3}(25[0-5]|2[0-4]\d|((1\d{2})|([1-9]?\d)))");
	return std::regex_match(account,pattern);*/

	std::wstring addr = addr_ctrl_->GetText();
	std::wstring port = port_ctrl_->GetText();
	StringHelper::Trim(addr);
	StringHelper::Trim(port);

	return !addr.empty() && !port.empty();
}

nim::NIMProxyType ProxyTip::ConvertIndexToProxyType(int index)
{
	switch (index)
	{
	case 0:
		return nim::kNIMProxyNone;
	case 1:
		return nim::kNIMProxyHttp11;
	case 2:
		return nim::kNIMProxySocks4;
	case 3:
		return nim::kNIMProxySocks4a;
	case 4:
		return nim::kNIMProxySocks5;
	case 5:
		return nim::kNIMProxyNrtc;
	}
	return nim::kNIMProxyNone;
}
nbase::ProxyType  ProxyTip::ConvertNimProxytypeToBaseProxyType(nim::NIMProxyType type)
{
	switch (type)
	{
	case nim::kNIMProxyNone:
		return nbase::ProxyType::kProxyTypeNone;
		break;
	case nim::kNIMProxyHttp11:
		return nbase::ProxyType::kProxyTypeHttp11;
		break;
	case nim::kNIMProxySocks4:
		return nbase::ProxyType::kProxyTypeSocks4;
		break;
	case nim::kNIMProxySocks4a:
		return nbase::ProxyType::kProxyTypeSocks4A;
		break;
	case nim::kNIMProxySocks5:
		return nbase::ProxyType::kProxyTypeSocks5;
		break;
	case nim::kNIMProxyNrtc:
		return nbase::ProxyType::kProxyTypeNrtc;
		break;
	default:
		return nbase::ProxyType::kProxyTypeNone;
		break;
	}
}
nim::NIMProxyType ProxyTip::ConvertBaseProxytypeToNimProxyType(nbase::ProxyType type)
{
	switch (type)
	{
	case nbase::ProxyType::kProxyTypeNone:
		return nim::kNIMProxyNone;
		break;
	case nbase::ProxyType::kProxyTypeHttp11:
		return nim::kNIMProxyHttp11;
		break;
	case nbase::ProxyType::kProxyTypeSocks4:
		return nim::kNIMProxySocks4;
		break;
	case nbase::ProxyType::kProxyTypeSocks4A:
		return nim::kNIMProxySocks4a;
		break;
	case nbase::ProxyType::kProxyTypeSocks5:
		return nim::kNIMProxySocks5;
		break;
	case nbase::ProxyType::kProxyTypeNrtc:
		return nim::kNIMProxyNrtc;
		break;
	default:
		return nim::kNIMProxyNone;
		break;
	}
}
int ProxyTip::ConvertProxyTypeToIndex(nim::NIMProxyType type)
{
	switch (type)
	{
	case nim::kNIMProxyNone:
		return 0;
	case nim::kNIMProxyHttp11:
		return 1;
	case nim::kNIMProxySocks4:
		return 2;
	case nim::kNIMProxySocks4a:
		return 3;
	case nim::kNIMProxySocks5:
		return 4;
	}
	return 0;
}
void ProxyTip::DoInit()
{
	proxy_type_comb_ = (Combo*)FindSubControl(L"proxytypecomb");
	proxy_type_comb_->AttachSelect(nbase::Bind(&ProxyTip::OnProxyTypeSelected, this, std::placeholders::_1));

	addr_ctrl_ = (RichEdit*)FindSubControl(L"proxyaddr");
	port_ctrl_ = (RichEdit*)FindSubControl(L"proxyport");
	user_ctrl_ = (RichEdit*)FindSubControl(L"account");
	pass_ctrl_ = (RichEdit*)FindSubControl(L"password");

	port_ctrl_->SetLimitText(5);

	addr_ctrl_->SetSelAllOnFocus(true);
	port_ctrl_->SetSelAllOnFocus(true);
	user_ctrl_->SetSelAllOnFocus(true);
	pass_ctrl_->SetSelAllOnFocus(true);	
	auto&& supported_proxy_type = GetSupportedProxyTypeName();	
	for (auto it : supported_proxy_type)
	{
		auto item = FindSubControl(it);
		item = proxy_type_comb_->GetListBox()->FindSubControl(it);
		if (item != nullptr)
			item->SetVisible(true);
	}
}
void ProxyTipGlobal::DoInit()
{
	being_test_connect_ = false;
	__super::DoInit();
	test_connect_btn_ = (ui::Button*)FindSubControl(L"testconnect");
	test_connect_btn_->AttachClick([this](ui::EventArgs* param){
		if (!CheckProxyLegality())
		{
			return true;
		}
		being_test_connect_ = true;
		test_connect_btn_->SetEnabled(false);
		gifanim_ctrl_->SetVisible(true);
		DetectProxyLegality();
		return true;
	});
	gifanim_ctrl_ = (ui::Control*)FindSubControl(L"gifanim");
	gifanim_ctrl_->SetVisible(false);
	proxy_type_comb_->SelectItem(ConvertProxyTypeToIndex(ConvertBaseProxytypeToNimProxyType(proxy_setting_->GetType())));
	OnProxyTypeSelected(NULL);
	addr_ctrl_->SetUTF8Text(proxy_setting_->GetHost());
	port_ctrl_->SetUTF8Text(proxy_setting_->GetPort() == 0 ? "" : nbase::IntToString(proxy_setting_->GetPort()));
	user_ctrl_->SetUTF8Text(proxy_setting_->GetUserName());
	pass_ctrl_->SetUTF8Text(proxy_setting_->GetPassowrd());
}
void ProxyTipGlobal::ApplyProxySetting()
{
	nim::NIMProxyType type = ConvertIndexToProxyType(proxy_type_comb_->GetCurSel());
	std::string host;
	int port = 0;
	std::string user;
	std::string pass;
	if (type != nim::kNIMProxyNone)
	{
		PTR_VOID(!addr_ctrl_->GetUTF8Text().empty() && !port_ctrl_->GetText().empty())
			host = addr_ctrl_->GetUTF8Text();
		nbase::StringToInt(port_ctrl_->GetUTF8Text(), &port);
		user = user_ctrl_->IsEnabled() ?  user_ctrl_->GetUTF8Text() : "";
		pass = pass_ctrl_->IsEnabled() ?  pass_ctrl_->GetUTF8Text() : "";
	}	
	proxy_setting_->SetType(ConvertNimProxytypeToBaseProxyType(type));
	proxy_setting_->SetHost(host);
	proxy_setting_->SetPort(port);
	proxy_setting_->SetUserName(user);
	proxy_setting_->SetPassowrd(pass);
}
void ProxyTipGlobal::DetectProxyLegality()
{
	SetPanelEnabled(false);

	nim::NIMProxyType type = ConvertIndexToProxyType(proxy_type_comb_->GetCurSel());
	std::string host;
	int port = 0;
	std::string user;
	std::string pass;

	if (type != nim::kNIMProxyNone)
	{
		PTR_VOID(!addr_ctrl_->GetUTF8Text().empty() && !port_ctrl_->GetText().empty())
			host = addr_ctrl_->GetUTF8Text();
		nbase::StringToInt(port_ctrl_->GetUTF8Text(), &port);
		user = user_ctrl_->GetUTF8Text();
		pass = pass_ctrl_->GetUTF8Text();
	}

	nim::Global::DetectProxy(type, host, port, user, pass, nbase::Bind(&ProxyTipGlobal::CallbackDetectProxy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
void ProxyTipGlobal::CallbackDetectProxy(bool connect, nim::NIMProxyDetectStep step, const std::string& json_extention)
{
	if (step == nim::kNIMProxyDetectStepAllComplete)
	{
		being_test_connect_ = false;
		test_connect_btn_->SetEnabled(true);
		gifanim_ctrl_->SetVisible(false);
		SetPanelEnabled(true);
		if (connect)
		{
			ShowMsgBox(GetWindow()->GetHWND(), MsgboxCallback(), L"STRID_LOGIN_FORM_DETECT_PROXY_OK");
		}
		else
		{
			ShowMsgBox(GetWindow()->GetHWND(), MsgboxCallback(), L"STRID_LOGIN_FORM_DETECT_PROXY_ERROR");
		}
		OnProxyTypeSelected(NULL);
	}
}
void ProxyTipSub::DoInit()
{
	__super::DoInit();
	use_global_ = dynamic_cast<ui::CheckBox*>(FindSubControl(L"use_global"));
	if (use_global_ != nullptr)
	{
		use_global_->AttachUnSelect([this](ui::EventArgs* param){
			FindSubControl(L"settingpanel")->SetEnabled(true);
			OnProxyTypeSelected(nullptr);
			proxy_setting_->SetUseGlobalSetting(false);
			return true;
		});
		use_global_->AttachSelect([this](ui::EventArgs* param){
			FindSubControl(L"settingpanel")->SetEnabled(false);
			proxy_setting_->SetUseGlobalSetting(true);
			return true;
		});
	}
	proxy_type_comb_->SelectItem(ConvertProxyTypeToIndex(ConvertBaseProxytypeToNimProxyType(proxy_setting_->GetType())));
	OnProxyTypeSelected(NULL);
	addr_ctrl_->SetUTF8Text(proxy_setting_->GetHost());
	port_ctrl_->SetUTF8Text(proxy_setting_->GetPort() == 0 ? "" : nbase::IntToString(proxy_setting_->GetPort()));
	user_ctrl_->SetUTF8Text(proxy_setting_->GetUserName());
	pass_ctrl_->SetUTF8Text(proxy_setting_->GetPassowrd());
	Post2UI(ToWeakCallback([this](){		
		use_global_->Selected(proxy_setting_->IsUseGlobalSetting(),false);
		FindSubControl(L"settingpanel")->SetEnabled(!proxy_setting_->IsUseGlobalSetting());
	}));	
}
void ProxyTipSub::ApplyProxySetting()
{
	nim::NIMProxyType type = ConvertIndexToProxyType(proxy_type_comb_->GetCurSel());
	std::string host;
	int port = 0;
	std::string user;
	std::string pass;
	if (type != nim::kNIMProxyNone)
	{
		PTR_VOID(!addr_ctrl_->GetUTF8Text().empty() && !port_ctrl_->GetText().empty())
			host = addr_ctrl_->GetUTF8Text();
		nbase::StringToInt(port_ctrl_->GetUTF8Text(), &port);
		user = user_ctrl_->IsEnabled() ? user_ctrl_->GetUTF8Text() : "";
		pass = pass_ctrl_->IsEnabled() ? pass_ctrl_->GetUTF8Text() : "";
	}
	proxy_setting_->SetUseGlobalSetting(use_global_->IsSelected());
	proxy_setting_->SetType(ConvertNimProxytypeToBaseProxyType(type));
	proxy_setting_->SetHost(host);
	proxy_setting_->SetPort(port);
	proxy_setting_->SetUserName(user);
	proxy_setting_->SetPassowrd(pass);
}
std::vector<std::wstring> ProxyTipGlobal::GetSupportedProxyTypeName()
{
	std::vector<std::wstring> ret = { L"noproxy", L"socks4proxy", L"socks4aproxy", L"socks5proxy" };
	return ret;
}
std::vector<std::wstring> ProxyTipSub_VChat::GetSupportedProxyTypeName()
{
	std::vector<std::wstring> ret = { L"noproxy", L"socks5proxy", L"privateproxy" };
	return ret;
}
std::vector<std::wstring> ProxyTipSub_Rts::GetSupportedProxyTypeName()
{
	std::vector<std::wstring> ret = { L"noproxy", L"socks5proxy" };
	return ret;
}
std::vector<std::wstring> ProxyTipSub_Chatroom::GetSupportedProxyTypeName()
{
	std::vector<std::wstring> ret = { L"noproxy", L"socks4proxy", L"socks4aproxy", L"socks5proxy" };
	return ret;
}
std::vector<std::wstring> ProxyTipSub_Httptool::GetSupportedProxyTypeName()
{
	std::vector<std::wstring> ret = { L"noproxy", L"socks4proxy", L"socks4aproxy", L"socks5proxy" };
	return ret;
}