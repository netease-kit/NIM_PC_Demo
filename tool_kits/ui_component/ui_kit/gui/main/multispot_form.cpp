#include "resource.h"
#include "multispot_form.h"
#include "module/session/session_util.h"
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
	if (nim_ui::UserConfig::GetInstance()->GetDefaultIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetDefaultIcon());
	}

	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MultispotForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MultispotForm::OnClicked, this, std::placeholders::_1));
	dev_list_ = (ui::ListBox*)FindControl(L"dev_list");
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
	static std::wstring tip_template = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_MOBILEPHONE_CLIENT");
	dev_list_->RemoveAll();
	for (auto& it : map_multispot_infos)
	{
		ui::ListContainerElement * item = new ui::ListContainerElement;

		GlobalManager::FillBoxWithCache(item, L"main/dev_item.xml");

		ui::Label *multi_des = (ui::Label*)item->FindSubControl(L"multi_des");
		
		std::wstring client_des;
		switch (it.second.client_type_)
		{
		case nim::kNIMClientTypeAndroid:
			client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_AOS");
			break;
		case nim::kNIMClientTypeiOS:
			client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_IOS");
			break;
		case nim::kNIMClientTypePCWindows:
		case nim::kNIMClientTypeMacOS:
			client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_PC");
			break;
		case nim::kNIMClientTypeWeb:
			client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_WEB");
			break;
		default:
			client_des = nbase::UTF8ToUTF16(map_multispot_infos.begin()->second.client_os_);
			break;
		}
		std::wstring show_tip = nbase::StringPrintf(tip_template.c_str(), client_des.c_str());
		multi_des->SetText(show_tip);

		Button* kick = (Button*)item->FindSubControl(L"kick");
		kick->SetDataID(nbase::UTF8ToUTF16(map_multispot_infos.begin()->second.device_id_));

		dev_list_->Add(item);
	}
}
}