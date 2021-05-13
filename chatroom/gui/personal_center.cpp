#include "stdafx.h"
#include "personal_center.h"

const LPTSTR PersonalCenterForm::kClassName = L"PersonalCenterForm";

PersonalCenterForm::PersonalCenterForm()
{
}

PersonalCenterForm::~PersonalCenterForm()
{
}

std::wstring PersonalCenterForm::GetSkinFolder()
{
	return L"chatroom";
}

std::wstring PersonalCenterForm::GetSkinFile()
{
	return L"personal_center.xml";
}

ui::UILIB_RESOURCETYPE PersonalCenterForm::GetResourceType() const
{
	return ui::UILIB_RESOURCETYPE::UILIB_FILE;
}

std::wstring PersonalCenterForm::GetZIPFileName() const
{
	return L"";
}

std::wstring PersonalCenterForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring PersonalCenterForm::GetWindowId() const
{
	return kClassName;
}

UINT PersonalCenterForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void PersonalCenterForm::InitWindow()
{
	InitHeader();
//	InitRoomManageList();

	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&PersonalCenterForm::OnSelected, this, std::placeholders::_1));
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserInfoChange(nbase::Bind(&PersonalCenterForm::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(nim_ui::PhotoManager::GetInstance()->RegPhotoReady(nbase::Bind(&PersonalCenterForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

	((ui::OptionBox*)FindControl(L"apply_room"))->Selected(true, true);
}

LRESULT PersonalCenterForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool PersonalCenterForm::Notify(ui::EventArgs * msg)
{
	return false;
}

bool PersonalCenterForm::OnClicked(ui::EventArgs * msg)
{
	return false;
}

bool PersonalCenterForm::OnSelected(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	ui::TabBox* tab_page = (ui::TabBox*)FindControl(L"page_tab");
	if (name == L"manage_room")
		tab_page->SelectItem(0);
	else if (name == L"apply_room")
		tab_page->SelectItem(1);

	return false;
}

void PersonalCenterForm::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	{
		if (nim_ui::LoginManager::GetInstance()->IsEqual(iter->GetAccId()))
		{
			InitHeader();
			break;
		}
	}
}

void PersonalCenterForm::OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path)
{
	if (type == kUser && nim_ui::LoginManager::GetInstance()->GetAccount() == account)
		FindControl(L"header_image")->SetBkImage(photo_path);
}

void PersonalCenterForm::OnApplyButtonClicked(ui::EventArgs * msg)
{

}

void PersonalCenterForm::InitHeader()
{
	std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
	FindControl(L"header_image")->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(my_id));
	((ui::Label*)FindControl(L"name"))->SetText(nim_ui::UserManager::GetInstance()->GetUserName(my_id, false));
}

void PersonalCenterForm::InitRoomManageList()
{
	for (int i = 0; i < 2; i++)
	{
		ui::HBox* item = (ui::HBox*)ui::GlobalManager::CreateBox(L"chatroom/room_manage_item.xml");
		ui::VBox* room_manage_list = (ui::VBox*)FindControl(L"room_manage_list");
		room_manage_list->AddAt(item, room_manage_list->GetCount() - 1);
	}
}
