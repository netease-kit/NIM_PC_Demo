#include "shared/ui/msgbox.h"
#include "proxy_form.h"
#include "base/network/network_util.h"
//#include "base/network/http.h"
//#include "util/NetStateTestHelper.h"
//#include "LibNEPLoginHelper.h"

using namespace ui;
const LPCTSTR ProxyForm::kClassName = L"ProxyForm";

nim::NIMProxyType ProxyForm::cur_type = nim::kNIMProxyNone;
std::string ProxyForm::cur_host = "";
short ProxyForm::cur_port = 0;
std::string ProxyForm::cur_user = "";
std::string ProxyForm::cur_pass = "";

ProxyForm::ProxyForm()
{
	being_test_connect_ = false;
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
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&ProxyForm::Notify, this, std::placeholders::_1));
	proxy_type_comb_ = (Combo*)FindControl(L"proxytypecomb");
	proxy_type_comb_->AttachSelect(nbase::Bind(&ProxyForm::OnProxyTypeSelected, this, std::placeholders::_1));

	addr_ctrl_ = (RichEdit*)FindControl(L"proxyaddr");
	port_ctrl_ = (RichEdit*)FindControl(L"proxyport");
	user_ctrl_ = (RichEdit*)FindControl(L"account");
	pass_ctrl_ = (RichEdit*)FindControl(L"password");

	port_ctrl_->SetLimitText(5);

	addr_ctrl_->SetSelAllOnFocus(true);
	port_ctrl_->SetSelAllOnFocus(true);
	user_ctrl_->SetSelAllOnFocus(true);
	pass_ctrl_->SetSelAllOnFocus(true);

	test_connect_btn_ = (ui::Button*)FindControl(L"testconnect");
	confirm_btn_ = (ui::Button*)FindControl(L"confirm");
	apply_btn_ = (ui::Button*)FindControl(L"apply");

	gifanim_ctrl_ = (ui::Control*)FindControl(L"gifanim");
	gifanim_ctrl_->SetVisible(false);

	proxy_type_comb_->SelectItem(ConvertProxyTypeToIndex(cur_type));
	OnProxyTypeSelected(NULL);
	addr_ctrl_->SetUTF8Text(cur_host);
	port_ctrl_->SetUTF8Text(cur_port == 0 ? "" : nbase::IntToString(cur_port));
	user_ctrl_->SetUTF8Text(cur_user);
	pass_ctrl_->SetUTF8Text(cur_pass);
}

bool ProxyForm::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if(msg->Type == kEventClick)
	{
		if(name == L"confirm")
		{
			ApplyProxySetting();
			Close();
		}
		else if (name == L"close")
		{
			Close();
		}
		else if (name == L"testconnect")
		{
			if (!CheckProxyLegality())
			{
				return true;
			}
			being_test_connect_ = true;
			test_connect_btn_->SetEnabled(false);
			gifanim_ctrl_->SetVisible(true);
			DetectProxyLegality();
		}
	}
	else if(msg->Type == kEventTextChange)
	{
		if(name == L"proxyaddr" || name == L"proxyport")
		{
			bool enable = CheckProxyLegality();
			test_connect_btn_->SetEnabled(enable);
			confirm_btn_->SetEnabled(enable);
		}
	}
	else if(msg->Type == kEventTab)
	{
		if (name == L"proxyaddr")
			port_ctrl_->SetFocus();
		else if (name == L"proxyport")
			user_ctrl_->SetFocus();
		else if (name == L"account")
			pass_ctrl_->SetFocus();
		else if (name == L"password")
			addr_ctrl_->SetFocus();
	}

	return true;
}

bool ProxyForm::OnProxyTypeSelected(ui::EventArgs * msg)
{
	int sel = proxy_type_comb_->GetCurSel();
	if (sel == 0 || sel == 1)
		confirm_btn_->SetEnabled(sel == 0);

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
		port_ctrl_->SetText(L"1080");
		port_ctrl_->SetEnabled(true);
		user_ctrl_->SetEnabled(false);
		pass_ctrl_->SetEnabled(false);
		confirm_btn_->SetEnabled(CheckProxyLegality());
		break;
	case 4:
		addr_ctrl_->SetEnabled(true);
		port_ctrl_->SetText(L"1080");
		port_ctrl_->SetEnabled(true);
		user_ctrl_->SetEnabled(true);
		pass_ctrl_->SetEnabled(true);
		confirm_btn_->SetEnabled(CheckProxyLegality());
		break;
	default:
		break;
	}

	return true;
}

void ProxyForm::DetectProxyLegality()
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

	nim::Global::DetectProxy(type, host, port, user, pass, nbase::Bind(&ProxyForm::CallbackDetectProxy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void ProxyForm::CallbackDetectProxy(bool connect, nim::NIMProxyDetectStep step, const std::string& json_extention)
{
	if (step == nim::kNIMProxyDetectStepAllComplete)
	{
		being_test_connect_ = false;
		test_connect_btn_->SetEnabled(true);
		gifanim_ctrl_->SetVisible(false);
		SetPanelEnabled(true);
		if (connect)
		{
			ShowMsgBox(m_hWnd, MsgboxCallback(), L"STRID_LOGIN_FORM_DETECT_PROXY_OK");
		}
		else
		{
			ShowMsgBox(m_hWnd, MsgboxCallback(), L"STRID_LOGIN_FORM_DETECT_PROXY_ERROR");
		}
	}
}

void ProxyForm::SetPanelEnabled(bool enable)
{
	addr_ctrl_->SetEnabled(enable);
	port_ctrl_->SetEnabled(enable);
	user_ctrl_->SetEnabled(enable);
	pass_ctrl_->SetEnabled(enable);
	proxy_type_comb_->SetEnabled(enable);
}

bool ProxyForm::CheckProxyLegality()
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

nim::NIMProxyType ProxyForm::ConvertIndexToProxyType(int index)
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
	}
	return nim::kNIMProxyNone;
}

int ProxyForm::ConvertProxyTypeToIndex(nim::NIMProxyType type)
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

void ProxyForm::ApplyProxySetting()
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
		user = user_ctrl_->GetUTF8Text();
		pass = pass_ctrl_->GetUTF8Text();
	}

	nim_http::SetGlobalProxy((NIMProxyType)type, host, (short)port, user, pass);
	nim::Global::SetProxy(type, host, port, user, pass);
	nim_chatroom::ChatRoom::SetProxy((nim_chatroom::NIMChatRoomProxyType)type, host, (short)port, user, pass);

	cur_type = type;
	cur_host = host;
	cur_port = (short)port;
	cur_user = user;
	cur_pass = pass;
}
