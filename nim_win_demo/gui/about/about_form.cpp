#include "about_form.h"


using namespace ui;

const LPCTSTR AboutForm::kClassName = L"AboutForm";

AboutForm::AboutForm()
{

}

AboutForm::~AboutForm()
{

}

std::wstring AboutForm::GetSkinFolder()
{
	return L"about";
}

std::wstring AboutForm::GetSkinFile()
{
	return L"about.xml";
}

ui::UILIB_RESOURCETYPE AboutForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring AboutForm::GetZIPFileName() const
{
	return L"about.zip";
}

std::wstring AboutForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring AboutForm::GetWindowId() const
{
	return kClassName;
}

UINT AboutForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void AboutForm::InitWindow()
{
	SetTaskbarTitle(L"关于云信");
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&AboutForm::OnClicked, this, std::placeholders::_1));

	Label* version = (Label*) FindControl(L"version");
	{
		int ver = 0;
		std::wstring vf;
		nim_ui::UserConfig::GetInstance()->GetAppLocalVersion(ver, vf);
		version->SetText(nbase::StringPrintf(L"版本: %s.%d", vf.c_str(), ver));
	}
}

bool AboutForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	name;
	return false;
}