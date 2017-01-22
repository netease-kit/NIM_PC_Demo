#include "resource.h"
#include "link_form.h"
#include "callback/login/login_callback.h"
#include "module/login/login_manager.h"
#include "export/nim_ui_user_config.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR LinkForm::kClassName	= L"LinkForm";

LinkForm::LinkForm()
{

}

LinkForm::~LinkForm()
{
	
}

std::wstring LinkForm::GetSkinFolder()
{
	return L"login";
}

std::wstring LinkForm::GetSkinFile()
{
	return L"link.xml";
}

std::wstring LinkForm::GetWindowClassName() const 
{
	return kClassName;
}

std::wstring LinkForm::GetWindowId() const 
{
	return kClassName;
}

UINT LinkForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LinkForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetDefaultIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetDefaultIcon());
	}

	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&LinkForm::OnClicked, this, std::placeholders::_1));

	//默认SDK一直在重连 20170104 litianyi
	SwitchTo(LINK_ING);

	unregister_cb.Add(NotifyCenter::GetInstance()->RegNotify(NT_LINK, nbase::Bind(&LinkForm::OnRelink, this, std::placeholders::_1)));
}

void LinkForm::SetLoginRetryingFlag(nim::NIMResCode res_code, bool retrying)
{
	if (!nim_comp::LoginManager::GetInstance()->IsLinkActive())
	{
		if (retrying && res_code != nim::kNIMLocalResNetworkError)
			SwitchTo(LINK_ING);
		else
		{
			SwitchTo(LINK_TIP);
			Label *tip_label = (Label*)FindControl(L"link_tip_label");
			if (tip_label)
			{
				if (res_code == nim::kNIMLocalResNetworkError)
				{
					tip_label->SetText(L"网络连接断开，请检查本地网络");
				}
				else
				{
					std::wstring tip_string = nbase::StringPrintf(L"网络连接断开(%d)", res_code);
					tip_label->SetText(tip_string);
				}
			}
		}
	}
}

bool LinkForm::OnClicked( ui::EventArgs* msg )
{
	std::wstring name = msg->pSender->GetName();
	if(name == L"btn_relink")
	{
		SwitchTo(LINK_ING);

		LoginCallback::ReLogin();
	}
	return true;
}

void LinkForm::SwitchTo( LinkPage lp )
{
	Control* tip = FindControl(L"link_tip_page");
	tip->SetVisible(lp == LINK_TIP);

	Control* ing = FindControl(L"link_ing_page");
	ing->SetVisible(lp == LINK_ING);

	Control* success = FindControl(L"link_success_page");
	success->SetVisible(lp == LINK_SUCCESS);

	if(lp == LINK_TIP)
	{
		timer_.Cancel();
	}
}

void LinkForm::OnRelink( const Json::Value &json )
{
	bool link = json["link"].asBool();
	if(link)
	{
		SwitchTo(LINK_SUCCESS);
		
		StdClosure cb = nbase::Bind(&LinkForm::OnTimeup, this);
		auto weak_cb = timer_.ToWeakCallback(cb);
		nbase::ThreadManager::PostDelayedTask(weak_cb, nbase::TimeDelta::FromSeconds(2));
	}
	else
	{
		//默认SDK一直在重连 20170104 litianyi
		SwitchTo(LINK_ING);
	}
}

void LinkForm::OnTimeup()
{
	this->Close();
}

void ShowLinkForm(nim::NIMResCode res_code, bool retrying)
{
	LinkForm* link_form = (LinkForm*)( WindowsManager::GetInstance()->GetWindow(LinkForm::kClassName, LinkForm::kClassName) );
	if(link_form)
	{
		link_form->ActiveWindow();
		link_form->SetLoginRetryingFlag(res_code, retrying);
	}
	else
	{
		link_form = new LinkForm;
		HWND hwnd = link_form->Create(NULL, LinkForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, WS_EX_TOOLWINDOW);
		if(hwnd == NULL)
			return;
		link_form->ShowWindow();
		link_form->SetLoginRetryingFlag(res_code, retrying);
	}

	POINT pt = GetPopupWindowPos(link_form);
	UiRect rc(pt.x, pt.y, 0, 0);
	link_form->SetPos(rc, SWP_NOSIZE, NULL, true);
}
}