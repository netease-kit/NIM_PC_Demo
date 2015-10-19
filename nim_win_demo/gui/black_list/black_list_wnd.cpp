#include "black_list_wnd.h"

const LPCTSTR BlackListWindow::kClassName = L"AddFriendWindow";

BlackListWindow::BlackListWindow()
{
	unregister_cb.Add(nim_ui::MuteBlackManager::GetInstance()->RegSyncSetBlackCallback(nbase::Bind(&BlackListWindow::OnSetBlackCallback, this, std::placeholders::_1, std::placeholders::_2)));
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
		OnGetUserInfoCallback cb = ToWeakCallback([this](bool ret, const std::list<UserInfo> uinfos) {
			if (!ret || uinfos.empty()) return;
			AddBlackListMember(*uinfos.cbegin());
		});
		nim_ui::UserManager::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, id), cb);
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

	const std::set<std::string> black_list = nim_ui::MuteBlackManager::GetInstance()->GetBlackList();
	std::list<std::string> account_list(black_list.cbegin(), black_list.cend());

	OnGetUserInfoCallback cb = ToWeakCallback([this](bool ret, const std::list<UserInfo> uinfos) {
		if (!ret) return;
		for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
			AddBlackListMember(*iter);
	});
	nim_ui::UserManager::GetInstance()->GetUserInfoWithEffort(account_list, cb);
}

void BlackListWindow::RestoreUserInfo(const UserInfo &info)
{
	ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(info.account));
	if (black_item == NULL)
		return;
	ui::Control* head_image = black_item->FindSubControl(L"head_image");
	ui::Label* contact = static_cast<ui::Label*>(black_item->FindSubControl(L"contact"));
	head_image->SetBkImage(nim_ui::UserManager::GetInstance()->GetUserPhoto(info.account));
	contact->SetUTF8Text(info.name);
}

void BlackListWindow::AddBlackListMember(const UserInfo & info)
{
	ui::ListContainerElement* black_item = (ui::ListContainerElement*)m_black_list->FindSubControl(nbase::UTF8ToUTF16(info.account));
	if (black_item != NULL)
		return;
	black_item = dynamic_cast<ui::ListContainerElement*>(ui::GlobalManager::CreateBoxWithCache(L"black_list/black_list_item.xml"));
	black_item->SetUTF8Name(info.account);
	m_black_list->Add(black_item);
	RestoreUserInfo(info);
	ui::Button* remove = static_cast<ui::Button*>(black_item->FindSubControl(L"confirm"));
	remove->SetUTF8DataID(info.account); // 把用户账号保存到按钮的m_sUserData中
	remove->AttachClick(nbase::Bind(&BlackListWindow::OnRemoveBtnClicked, this, std::placeholders::_1));
}

bool BlackListWindow::OnRemoveBtnClicked(ui::EventArgs *args)
{
	nim_ui::MuteBlackManager::GetInstance()->InvokeSetBlack(args->pSender->GetUTF8DataID(), false);
	return true;
}

