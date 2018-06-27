#include "main_form_menu.h"
#include "shared\ui\ui_menu.h"
#include "module\config\config_helper.h"
#include "gui/about/about_form.h"
#include "gui/msglogmanage/msglog_manage_form.h"
#include "util\user.h"
using namespace ui;
void MainFormMenu::OnPopupMainMenu(POINT point)
{
	//创建菜单窗口
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"main_menu.xml");
	pMenu->Init(xml, _T("xml"), point);
	//注册回调
	CMenuElementUI* look_log = (CMenuElementUI*)pMenu->FindControl(L"look_log");
	look_log->AttachSelect(nbase::Bind(&MainFormMenu::LookLogMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* file_trans = (CMenuElementUI*)pMenu->FindControl(L"file_helper");
	file_trans->AttachSelect(nbase::Bind(&MainFormMenu::FileTransMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* address = (CMenuElementUI*)pMenu->FindControl(L"address");
	address->AttachSelect(nbase::Bind(&MainFormMenu::AddressMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* export_msglog = (CMenuElementUI*)pMenu->FindControl(L"export_msglog");
	export_msglog->AttachSelect(nbase::Bind(&MainFormMenu::ExportMsglogMenuItemClick, this, std::placeholders::_1));
	CMenuElementUI* import_msglog = (CMenuElementUI*)pMenu->FindControl(L"import_msglog");
	import_msglog->AttachSelect(nbase::Bind(&MainFormMenu::ImportMsglogMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* clear_chat_record = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record");
	clear_chat_record->AttachSelect(nbase::Bind(&MainFormMenu::ClearChatRecordMenuItemClick, this, true, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_ex = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_ex");
	clear_chat_record_ex->AttachSelect(nbase::Bind(&MainFormMenu::ClearChatRecordMenuItemClick, this, false, std::placeholders::_1));

	CMenuElementUI* clear_chat_record_p2p = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_p2p");
	clear_chat_record_p2p->AttachSelect(nbase::Bind(&MainFormMenu::ClearChatRecordBySessionTypeMenuItemClick, this, true, nim::kNIMSessionTypeP2P, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_p2p_ex = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_p2p_ex");
	clear_chat_record_p2p_ex->AttachSelect(nbase::Bind(&MainFormMenu::ClearChatRecordBySessionTypeMenuItemClick, this, false, nim::kNIMSessionTypeP2P, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_team = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_team");
	clear_chat_record_team->AttachSelect(nbase::Bind(&MainFormMenu::ClearChatRecordBySessionTypeMenuItemClick, this, true, nim::kNIMSessionTypeTeam, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_team_ex = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_team_ex");
	clear_chat_record_team_ex->AttachSelect(nbase::Bind(&MainFormMenu::ClearChatRecordBySessionTypeMenuItemClick, this, false, nim::kNIMSessionTypeTeam, std::placeholders::_1));

	CMenuElementUI* vchat_setting = (CMenuElementUI*)pMenu->FindControl(L"vchat_setting");
	vchat_setting->AttachSelect(nbase::Bind(&MainFormMenu::VChatSettingMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* rts_replay = (CMenuElementUI*)pMenu->FindControl(L"rts_replay");
	rts_replay->AttachSelect(nbase::Bind(&MainFormMenu::RtsReplayMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* adapt_dpi = (CMenuElementUI*)pMenu->FindControl(L"adapt_dpi");
	adapt_dpi->AttachSelect(nbase::Bind(&MainFormMenu::AdaptDpiMenuItemClick, this, std::placeholders::_1));
	CheckBox* check_dpi = (CheckBox*)adapt_dpi->FindSubControl(L"check_dpi");
	check_dpi->Selected(ConfigHelper::GetInstance()->IsAdaptDpi());

	CMenuElementUI* language = (CMenuElementUI*)pMenu->FindControl(L"language");
	language->AttachMouseEnter(nbase::Bind(&MainFormMenu::ShowLanguageList, this, std::placeholders::_1));
	language->AttachMouseLeave(nbase::Bind(&MainFormMenu::CloseLanguageList, this, std::placeholders::_1, true));
	pMenu->AttachWindowClose(nbase::Bind(&MainFormMenu::CloseLanguageList, this, std::placeholders::_1, false));

	CMenuElementUI* about = (CMenuElementUI*)pMenu->FindControl(L"about");
	about->AttachSelect(nbase::Bind(&MainFormMenu::AboutMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* logoff = (CMenuElementUI*)pMenu->FindControl(L"logoff");
	logoff->AttachSelect(nbase::Bind(&MainFormMenu::LogoffMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* quit = (CMenuElementUI*)pMenu->FindControl(L"quit");
	quit->AttachSelect(nbase::Bind(&MainFormMenu::QuitMenuItemClick, this, std::placeholders::_1));
	//显示
	pMenu->Show();
}
static void LookLogClick(HWND m_hWnd)
{
	//TODO：暂时显示用户数据所在的目录，方便收集用户反馈！
	std::wstring dir = nim_ui::UserConfig::GetInstance()->GetUserDataPath();
	std::wstring tip = nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MENU_CURRENT_USER_DIR").c_str(), dir.c_str());
	ShowMsgBox(m_hWnd, MsgboxCallback(), tip, false, L"STRING_TIPS", true, L"STRID_MAINWINDOW_MENU_GOT_IT", true);
	HINSTANCE inst = ::ShellExecute(NULL, L"open", dir.c_str(), NULL, NULL, SW_SHOW);
	int ret = (int)inst;
	if (ret > 32)
	{
		QLOG_APP(L"open user data path: {0}") << dir.c_str();
		return;
	}
	else
	{
		QLOG_ERR(L"failed to open user data path: {0}") << dir.c_str();
		return;
	}
}

bool MainFormMenu::LookLogMenuItemClick(ui::EventArgs* param)
{
	nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(&LookLogClick, host_window_->GetHWND()));
	return false;
}

bool MainFormMenu::FileTransMenuItemClick(ui::EventArgs* param)
{
	nim_ui::SessionManager::GetInstance()->OpenSessionBox(nim_ui::LoginManager::GetInstance()->GetAccount(), nim::kNIMSessionTypeP2P);
	return true;
}

bool MainFormMenu::AddressMenuItemClick(ui::EventArgs* param)
{
	if (!nim_ui::LoginManager::GetInstance()->IsLinkActive())
		nim_ui::WindowsManager::GetInstance()->ShowLinkForm();
	else
		nim_ui::ContactsListManager::GetInstance()->InvokeGetAllUserInfo();
	return true;
}
bool MainFormMenu::ExportMsglogMenuItemClick(ui::EventArgs* param)
{
	MsglogManageForm *form = nim_ui::WindowsManager::SingletonShow<MsglogManageForm>(MsglogManageForm::kClassName);
	form->SetType(true);

	return true;
}
bool MainFormMenu::ImportMsglogMenuItemClick(ui::EventArgs* param)
{
	MsglogManageForm *form = nim_ui::WindowsManager::SingletonShow<MsglogManageForm>(MsglogManageForm::kClassName);
	form->SetType(false);

	return true;
}

bool MainFormMenu::ClearChatRecordMenuItemClick(bool del_session, ui::EventArgs* param)
{
	nim::MsgLog::DeleteAllAsync(del_session, nim::MsgLog::DeleteAllCallback());
	return true;
}

bool MainFormMenu::ClearChatRecordBySessionTypeMenuItemClick(bool del_session, nim::NIMSessionType type, ui::EventArgs* param)
{
	nim::MsgLog::DeleteBySessionTypeAsync(del_session, type, nim::MsgLog::DeleteBySessionTypeCallback());
	return true;
}

bool MainFormMenu::VChatSettingMenuItemClick(ui::EventArgs* param)
{
	nim_ui::WindowsManager::GetInstance()->ShowVideoSettingForm();
	return true;
}

bool MainFormMenu::RtsReplayMenuItemClick(ui::EventArgs* param)
{
	nim_ui::WindowsManager::SingletonShow<nim_comp::RtsReplay>(nim_comp::RtsReplay::kClassName);
	return true;
}

bool MainFormMenu::AdaptDpiMenuItemClick(ui::EventArgs* param)
{
	CMenuElementUI* menu_item = (CMenuElementUI*)(param->pSender);
	CheckBox* check_dpi = (CheckBox*)menu_item->FindSubControl(L"check_dpi");
	ConfigHelper::GetInstance()->SetAdaptDpi(!check_dpi->IsSelected());
	return true;
}

bool MainFormMenu::ShowLanguageList(ui::EventArgs* param)
{
	std::wstring menu_name = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MENU_LANGUAGE_LIST");
	HWND hWnd = ::FindWindow(L"MenuWnd", menu_name.c_str());
	if (hWnd) //语言列表已经打开
	{
		::ShowWindow(hWnd, SW_SHOWNOACTIVATE);
		return true;
	}

	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"language_list.xml");
	ui::UiRect menu_pos = param->pSender->GetWindow()->GetPos(true);
	ui::UiRect elem_pos = param->pSender->GetPos(true);
	ui::CPoint popup_pt(menu_pos.left + elem_pos.right + 2, menu_pos.top + elem_pos.top);
	pMenu->Init(xml, _T("xml"), popup_pt, CMenuWnd::RIGHT_BOTTOM, true);
	::SetWindowText(pMenu->GetHWND(), menu_name.c_str());
	//注册回调
	std::string current_language = ConfigHelper::GetInstance()->GetLanguage();
	Box* language_list = (Box*)((Box*)pMenu->GetRoot())->GetItemAt(0);
	if (language_list)
		for (int i = 0; i < language_list->GetCount(); i++)
		{
			CMenuElementUI* language_item = dynamic_cast<CMenuElementUI*>(language_list->GetItemAt(i));
			if (language_item)
			{
				if (current_language == language_item->GetUTF8Name())
					language_item->Selected(true, false);
				language_item->AttachSelect(nbase::Bind(&MainFormMenu::OnSelectLanguage, this, std::placeholders::_1));
			}
		}

	//显示
	pMenu->Show();
	return true;
}

bool MainFormMenu::CloseLanguageList(ui::EventArgs* param, bool check_mouse)
{
	std::wstring menu_name = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MENU_LANGUAGE_LIST");
	HWND hWnd = ::FindWindow(L"MenuWnd", menu_name.c_str());
	if (!hWnd)
		return true;

	if (check_mouse)
	{
		RECT menu_rect;
		::GetWindowRect(hWnd, &menu_rect);
		POINT mouse_pt;
		::GetCursorPos(&mouse_pt);
		if (::PtInRect(&menu_rect, mouse_pt))
			return true; //鼠标在语言列表上，就不关闭
	}

	::DestroyWindow(hWnd);

	return true;
}

bool MainFormMenu::OnSelectLanguage(ui::EventArgs* param)
{
	std::string current_language = ConfigHelper::GetInstance()->GetLanguage();
	std::string selected_language = param->pSender->GetUTF8Name();
	if (current_language == selected_language)
		return true;

	MsgboxCallback cb = nbase::Bind(&MainFormMenu::OnSelectLanguageCallback, this, std::placeholders::_1, selected_language);
	ShowMsgBox(host_window_->GetHWND(), cb, L"STRID_MAINWINDOW_CHANGE_LANGUAGE_TIP", true, L"STRING_TIPS", true, L"STRING_OK", true, L"STRING_NO", true);

	return true;
}

void MainFormMenu::OnSelectLanguageCallback(MsgBoxRet ret, const std::string& language)
{
	if (ret == MB_YES)
	{
		ConfigHelper::GetInstance()->SetLanguage(language);
		LogoffMenuItemClick(NULL);
		QLOG_APP(L"Selected language: {0}") << nbase::UTF8ToUTF16(language);
	}
}

bool MainFormMenu::AboutMenuItemClick(ui::EventArgs* param)
{
	nim_ui::WindowsManager::SingletonShow<AboutForm>(AboutForm::kClassName);
	return false;
}

bool MainFormMenu::LogoffMenuItemClick(ui::EventArgs* param)
{
	QCommand::Set(kCmdRestart, L"true");
	std::wstring wacc = nbase::UTF8ToUTF16(nim_ui::LoginManager::GetInstance()->GetAccount());
	QCommand::Set(kCmdAccount, wacc);
	auto task = [](){
		nim_ui::LoginManager::GetInstance()->DoLogout(false, nim::kNIMLogoutChangeAccout);
	};
	nbase::ThreadManager::PostTask(task);
	return true;
}

bool MainFormMenu::QuitMenuItemClick(ui::EventArgs* param)
{
	auto task = [](){
		nim_ui::LoginManager::GetInstance()->DoLogout(false);
	};
	nbase::ThreadManager::PostTask(task);
	return true;
}