#include "black_list_wnd.h"

namespace nim_comp
{
const LPCTSTR BlackListWindow::kClassName = L"AddFriendWindow";

BlackListWindow::BlackListWindow()
{
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&BlackListWindow::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&BlackListWindow::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetBlackCallback(nbase::Bind(&BlackListWindow::OnSetBlackCallback, this, std::placeholders::_1, std::placeholders::_2)));
}

BlackListWindow::~BlackListWindow()
{
}

std::wstring BlackListWindow::GetSkinFolder()
{
	return L"black_list";
}

std::wstring BlackListWindow::GetSkinFile()
{
	return L"black_list_wnd.xml";
}

void BlackListWindow::InitWindow()
{
	m_black_list = static_cast<ui::ListBox*>(FindControl(L"black_list"));
	RefreshBlackList();
}

void BlackListWindow::RefreshBlackList()
{
	m_black_list->RemoveAll();

	const std::set<std::string> black_list = MuteBlackService::GetInstance()->GetBlackList();
	std::list<std::string> account_list(black_list.cbegin(), black_list.cend());

	std::list<nim::UserNameCard> uinfos;
	UserService::GetInstance()->GetUserInfos(account_list, uinfos);
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
		AddBlackListMember(*iter);
}

bool BlackListWindow::OnRemoveBtnClicked(ui::EventArgs *args)
{
	MuteBlackService::GetInstance()->InvokeSetBlack(args->pSender->GetUTF8DataID(), false);
	return true;
}

void BlackListWindow::ResetUserInfo(const nim::UserNameCard &info)
{
	ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(info.GetAccId()));
	if (black_item == NULL)
		return;
	ui::Control* head_image = black_item->FindSubControl(L"head_image");
	ui::Label* contact = static_cast<ui::Label*>(black_item->FindSubControl(L"contact"));
	head_image->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(info.GetAccId()));
	contact->SetText(UserService::GetInstance()->GetUserName(info.GetAccId()));
}

void BlackListWindow::AddBlackListMember(const nim::UserNameCard & info)
{
	ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(info.GetAccId()));
	if (black_item != NULL)
		return;
	black_item = dynamic_cast<ui::ListContainerElement*>(ui::GlobalManager::CreateBoxWithCache(L"black_list/black_list_item.xml"));
	black_item->SetUTF8Name(info.GetAccId());
	m_black_list->Add(black_item);
	ResetUserInfo(info);
	ui::Button* remove = static_cast<ui::Button*>(black_item->FindSubControl(L"confirm"));
	remove->SetUTF8DataID(info.GetAccId()); // 把用户帐号保存到按钮的m_sUserData中
	remove->AttachClick(nbase::Bind(&BlackListWindow::OnRemoveBtnClicked, this, std::placeholders::_1));
}

void BlackListWindow::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	for (const auto &info : uinfos)
		ResetUserInfo(info);
}

void BlackListWindow::OnUserPhotoReady(PhotoType type, const std::string & account, const std::wstring & photo_path)
{
	if (type == kUser)
	{
		ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(account));
		if (black_item)
			black_item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
}

void BlackListWindow::OnSetBlackCallback(const std::string& id, bool black)
{
	switch (black)
	{
	case true:
	{
		nim::UserNameCard info;
		UserService::GetInstance()->GetUserInfo(id, info);
		AddBlackListMember(info);
		break;
	}
	default:
	{
		ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(id));
		if (black_item != NULL)
			m_black_list->Remove(black_item);
		break;
	}
	}
}

}