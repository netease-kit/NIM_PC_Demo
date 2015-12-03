#include "resource.h"
#include "multispot_form.h"
#include "gui/session/control/session_util.h"
#include "export/nim_ui_user_config.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR MultispotForm::kClassName	= L"MultispotForm";

const int kSysmsgCount = 20;

MultispotForm::MultispotForm()
{
}

MultispotForm::~MultispotForm()
{
	
}

std::wstring MultispotForm::GetSkinFolder()
{
	return L"main";
}

std::wstring MultispotForm::GetSkinFile()
{
	return L"multispot.xml";
}

ui::UILIB_RESOURCETYPE MultispotForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring MultispotForm::GetZIPFileName() const
{
	return L"multispot.zip";
}

std::wstring MultispotForm::GetWindowClassName() const 
{
	return kClassName;
}

std::wstring MultispotForm::GetWindowId() const 
{
	return kClassName;
}

UINT MultispotForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MultispotForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetIcon());
	}

	SetTaskbarTitle(L"多端登录管理");
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MultispotForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MultispotForm::OnClicked, this, std::placeholders::_1));
	//dev_list_ = (ui::ListBox*)FindControl(L"dev_list");
}

bool MultispotForm::Notify( ui::EventArgs* msg )
{
	return true;
}

bool MultispotForm::OnClicked( ui::EventArgs* msg )
{
	std::wstring name = msg->pSender->GetName();
	if(name == L"kick")
	{
		std::wstring device_id = msg->pSender->GetDataID();
		std::list<std::string> dev_ids;
		dev_ids.push_back(nbase::UTF16ToUTF8(device_id));
		nim::Client::KickOtherClient(dev_ids);
	}
	return true;
}
void MultispotForm::OnMultispotChange(const std::map<nim::NIMClientType, nim::OtherClientPres>& map_multispot_infos)
{
	if (map_multispot_infos.size() > 0)
	{
		Button* kick = (Button*)FindControl(L"kick");
		kick->SetDataID(nbase::UTF8ToUTF16(map_multispot_infos.begin()->second.device_id_));
	}
	//dev_list_->RemoveAll();
	//for (auto& it : map_multispot_infos)
	//{
	//	ui::ListContainerElement * item = new ui::ListContainerElement;

	//	GlobalManager::FillBoxWithCache(item, L"main/dev_item.xml");

	//	Label* name = (Label*)item->FindSubControl(L"name");
	//	name->SetUTF8Text(it.second.client_os_);
	//	Label* time = (Label*)item->FindSubControl(L"time");
	//	time->SetText(GetMessageTime(it.second.login_time_, false));

	//	Button* kick = (Button*)item->FindSubControl(L"kick");
	//	kick->SetDataID(nbase::UTF8ToUTF16(it.second.device_id_));

	//	dev_list_->Add(item);
	//}
}
}