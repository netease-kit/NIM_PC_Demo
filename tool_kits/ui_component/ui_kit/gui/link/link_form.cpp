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

ui::UILIB_RESOURCETYPE LinkForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring LinkForm::GetZIPFileName() const
{
	return L"link.zip";
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
	if (nim_ui::UserConfig::GetInstance()->GetIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetIcon());
	}

	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&LinkForm::OnClicked, this, std::placeholders::_1));

	SwitchTo(LINK_TIP);

	unregister_cb.Add(NotifyCenter::GetInstance()->RegNotify(NT_LINK, nbase::Bind(&LinkForm::OnRelink, this, std::placeholders::_1)));
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
		SwitchTo(LINK_TIP);
	}
}

void LinkForm::OnTimeup()
{
	this->Close();
}


void ShowLinkForm()
{
	LinkForm* link_form = (LinkForm*)( WindowsManager::GetInstance()->GetWindow(LinkForm::kClassName, LinkForm::kClassName) );
	if(link_form)
	{
		link_form->ActiveWindow();
	}
	else
	{
		link_form = new LinkForm;
		HWND hwnd = link_form->Create(NULL, LinkForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, WS_EX_TOOLWINDOW);
		if(hwnd == NULL)
			return;
		link_form->ShowWindow();
	}

	POINT pt = GetPopupWindowPos(link_form);
	UiRect rc(pt.x, pt.y, 0, 0);
	link_form->SetPos(rc, SWP_NOSIZE, NULL, true);
}
}