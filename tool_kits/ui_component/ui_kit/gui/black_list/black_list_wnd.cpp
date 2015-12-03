#include "black_list_wnd.h"

namespace nim_comp
{
const LPCTSTR BlackListWindow::kClassName = L"AddFriendWindow";

BlackListWindow::BlackListWindow()
{
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

void BlackListWindow::OnSetBlackCallback(const std::string& id, bool black)
{
	switch (black)
	{
	case true:
	{	
		OnGetUserInfoCallback cb = ToWeakCallback([this](const std::list<nim::UserNameCard> uinfos) {
			if (uinfos.empty()) return;
			AddBlackListMember(*uinfos.cbegin());
		});
		UserService::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, id), cb);
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

void BlackListWindow::RefreshBlackList()
{
	m_black_list->RemoveAll();

	const std::set<std::string> black_list = MuteBlackService::GetInstance()->GetBlackList();
	std::list<std::string> account_list(black_list.cbegin(), black_list.cend());

	OnGetUserInfoCallback cb = ToWeakCallback([this](const std::list<nim::UserNameCard> uinfos) {
		for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
			AddBlackListMember(*iter);
	});
	UserService::GetInstance()->GetUserInfoWithEffort(account_list, cb);
}

void BlackListWindow::RestoreUserInfo(const nim::UserNameCard &info)
{
	ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(info.GetAccId()));
	if (black_item == NULL)
		return;
	ui::Control* head_image = black_item->FindSubControl(L"head_image");
	ui::Label* contact = static_cast<ui::Label*>(black_item->FindSubControl(L"contact"));
	head_image->SetBkImage(UserService::GetInstance()->GetUserPhoto(info.GetAccId()));
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
	RestoreUserInfo(info);
	ui::Button* remove = static_cast<ui::Button*>(black_item->FindSubControl(L"confirm"));
	remove->SetUTF8DataID(info.GetAccId()); // 把用户帐号保存到按钮的m_sUserData中
	remove->AttachClick(nbase::Bind(&BlackListWindow::OnRemoveBtnClicked, this, std::placeholders::_1));
}

bool BlackListWindow::OnRemoveBtnClicked(ui::EventArgs *args)
{
	MuteBlackService::GetInstance()->InvokeSetBlack(args->pSender->GetUTF8DataID(), false);
	return true;
}
}