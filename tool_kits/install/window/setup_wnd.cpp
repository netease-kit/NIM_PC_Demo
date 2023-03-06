#include "setup_wnd.h"
#include "base\util\string_util.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "licence_box.h"
#include "main/main_thread.h"
#include "main/setup_data.h"
#include <shellapi.h>
#include "ui_component\ui_kit\util\windows_manager.h"

const LPCTSTR SetupForm::kClassName = L"SetupForm";
bool SetupForm::destroy_wnd_ = false;
int32_t SetupForm::show_msg_res_ = -1;
uint32_t SetupForm::pre_progress_pos_ = 0;
SetupForm::SetupForm(bool update)
{
	update_yixin_ = update;
	last_setup_path_ = CSetupData::GetDefaultInstallDir();
}

SetupForm::~SetupForm()
{
	destroy_wnd_ = true;
}

LRESULT SetupForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN)
	{
		if(wParam == VK_ESCAPE)
		{
			Close();
			return 1;
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

std::wstring SetupForm::GetSkinFolder()
{
	return L"form";
}

std::wstring SetupForm::GetSkinFile()
{
	return L"setup_form.xml";
}

std::wstring SetupForm::GetWindowClassName() const
{
	return kClassName;
}

UINT SetupForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void SetupForm::OnFinalMessage( HWND hWnd )
{
	show_msg_res_ = 0;
	destroy_wnd_ = true;
	StdClosure cb = std::bind(&SetupForm::OnFinalMessageEx, this, hWnd);
	nbase::ThreadManager::PostTask(ToWeakCallback(cb));
}
void SetupForm::OnFinalMessageEx(HWND hWnd)
{
	MainThread* thread = static_cast<MainThread *>(nbase::FrameworkThread::current());
	if (thread)
	{
		thread->EndSession();
	}
	__super::OnFinalMessage(hWnd);
}

void SetupForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::EventType::kEventAll, std::bind(&SetupForm::Notify, this, std::placeholders::_1));
	box_setup_1_ = (ui::Box*)FindControl(L"setup_1");
	box_setup_2_ = (ui::Box*)FindControl(L"setup_2");
	box_path_ = (ui::Box*)FindControl(L"setup_path");
	box_fast_setup_ = (ui::Box*)FindControl(L"fast_setup");
	check_path_ = (ui::CheckBox*)FindControl(L"btn_path");
	richedit_path_ = (ui::RichEdit*)FindControl(L"edit_path");
	progress_ = (ui::Progress*)FindControl(L"progress");
	progress_pos_ = (ui::Label*)FindControl(L"progress_pos");
	free_space_label_ = (ui::Label*)FindControl(L"local_space");
	path_err_label_ = (ui::Label*)FindControl(L"path_err_tip");
	check_read_ = (ui::CheckBox*)FindControl(L"chkbox_read");
	btn_setup_ = (ui::Button*)FindControl(L"setup");
	richedit_path_->SetText(last_setup_path_);
	if (!btn_setup_->IsEnabled())
	{
		check_path_->Selected(true, true);
	}
	else if (update_yixin_)
	{
		Setup();
	}
}
LRESULT SetupForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (start_setup_ && wParam == 1)
	{
		bHandled = TRUE;
		MsgForm::MsgboxCallback msgbox_cb = [this](MsgBoxRet ret)
		{
			if (!destroy_wnd_ && ret == kMsgBtn1)
			{
				this->Close(200);
			}
		};
		MsgBox()->SetTitle(L"STRING_TIPS")->SetInfor(L"STRING_STOP_SETUP_TIP")->SetIcon(kMsgIconWarn)
			->AddButton(L"STRING_SETUP_STOP", true)->AddButton(L"STRING_SETUP_AG")
			->AsynShow(NULL, ToWeakCallback(msgbox_cb));
	}
	return 0;
}

bool SetupForm::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if(msg->Type == ui::EventType::kEventClick) 
	{
		if (name == L"setup")
		{
			SetupCheck();
		} 
		else if (name == L"custom_path")
		{
			SelectPath();
		}
		else if (name == L"btn_readme")
		{
            // nim_comp::WindowsManager::GetInstance()->SingletonShow<LicenceForm>(LicenceForm::kClassName);
			LicenceForm::ShowLicence();
		}
	}
	else if (msg->Type == ui::EventType::kEventSelect || msg->Type == ui::EventType::kEventUnSelect)
	{
		if (name == L"btn_path")
		{
			StartPathBoxAnimationPlayer();
		}
		else if (name == L"chkbox_read")
		{
			CheckSetupPath();
		}
	}
	else if (msg->Type == ui::EventType::kEventTextChange)
	{
		if (name == L"edit_path")
		{
			CheckSetupPath();
		}
	}
	return true;
}
//选择安装路径
void SetupForm::SelectPath()
{
	CFileDialogEx* file_dlg = new CFileDialogEx();
    PTR_VOID(file_dlg);
	file_dlg->SetFileName(richedit_path_->GetText().c_str());
	file_dlg->SetParentWnd(m_hWnd);	
	CFileDialogEx::FileDialogCallback2 callback2 = std::bind(&SetupForm::SelectPathCallBack, this, std::placeholders::_1, std::placeholders::_2);
	// file_dlg->AyncShowOpenDirDlg(callback2);
    // file_dlg->AyncShowOpenFileDlg(callback2);
    file_dlg->AsyncShowOpenDirFileDlg(ToWeakCallback(callback2));
}
void SetupForm::SelectPathCallBack(BOOL ret, std::wstring path)
{
	if (ret)
	{
		richedit_path_->SetText(path);
	}
}
//准备安装
void SetupForm::SetupCheck()
{
	std::wstring path = richedit_path_->GetText();
	if (!path.empty())
	{
		path = CSetupData::CheckDirAndReplace(path);
		std::wstring check_path = path;
		std::wstring last_path = L"\\NIM\\";
		if (path.size() <= last_path.size() || nbase::MakeLowerString(path.substr(path.size() - last_path.size())) != nbase::MakeLowerString(last_path))
		{
			path = CSetupData::CheckDirAndReplace(path + last_path);
		}
		std::wstring pre_path = CSetupData::GetLastInstDir();
		if (!pre_path.empty() && 
			nbase::MakeLowerString(pre_path) != nbase::MakeLowerString(path) && 
			nbase::MakeLowerString(pre_path) != nbase::MakeLowerString(check_path))
		{
			MsgForm::MsgboxCallback msgbox_cb = [this](MsgBoxRet ret)
			{
				if (!destroy_wnd_ && ret == kMsgBtn2)
				{
					this->Setup();
				}
			};
			std::wstring info2 = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_OVERWRITE_2");
			MsgBox()->SetTitle(L"STRING_TIPS")->SetIcon(kMsgIconWarn)
				->SetInfor(L"STRING_OVERWRITE_1", true)->SetPathInfo(pre_path)->SetInfor2(info2)
				->AddButton(L"STRING_NO", true)->AddButton(L"STRING_OK")->AsynShow(m_hWnd, ToWeakCallback(msgbox_cb));
		}
		else
		{
			Setup();
		}
	}
}
void SetupForm::Setup()
{
	CheckSetupPath();
	if (!btn_setup_->IsEnabled())
	{
		return;
	}
	std::wstring path = richedit_path_->GetText();
	if (!path.empty())
	{
		path = CSetupData::CheckDirAndReplace(path);
		std::wstring last_path = L"\\NIM\\";
		if (path.size() <= last_path.size() || nbase::MakeLowerString(path.substr(path.size() - last_path.size())) != nbase::MakeLowerString(last_path))
		{
			path += last_path;
		} 
		QLOG_APP(L"last_setup_path: {0}") << last_path;
		QLOG_APP(L"new_setup_path: {0}") << path;
		CSetupData::SetInstDir(path);
		box_setup_2_->SetVisible(true);
		box_setup_1_->SetVisible(false);
		check_path_->SetVisible(false);
		start_setup_ = true;
		StdClosure cb = std::bind(&SetupForm::DelFile, this);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb));
	}
}

void SetupForm::InstallRedist()
{
	std::wstring inst_dir = CSetupData::GetInstDir();
#ifdef _WIN64
	std::wstring msvcr_file = inst_dir + L"/vc_redist.x64.exe";
#else
	std::wstring msvcr_file = inst_dir + L"/vc_redist.x86.exe";
#endif
	if (!nbase::FilePathIsExist(msvcr_file, false))
	{
#if 0
		StdClosure cb_temp = std::bind(&SetupForm::EndSetupCallback, this, ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INSTAL_RUNTIMELIB_ERROR2"), GetLastError());
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb_temp));
#else
        pre_progress_pos_ = PROGRESS_DELFILE + PROGRESS_UNZIP;
        StdClosure cb_temp = std::bind(&SetupForm::CreateLink, this);
        nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb_temp));
#endif
		return;
	}

	HINSTANCE inst = ::ShellExecute(NULL, L"open", msvcr_file.c_str(), L"/quiet /norestart", NULL, SW_SHOW);
	int result = (int)inst;
	if (result > 32)
	{
		pre_progress_pos_ = PROGRESS_DELFILE + PROGRESS_UNZIP;
		StdClosure cb_temp = std::bind(&SetupForm::CreateLink, this);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb_temp));
	}
	else
	{
		StdClosure cb_temp = std::bind(&SetupForm::EndSetupCallback, this, ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INSTAL_RUNTIMELIB_ERROR"), result);
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb_temp));
	}
}

void SetupForm::EndSetupCallback(std::wstring tip, uint32_t res)
{
	start_setup_ = false;
	if (!destroy_wnd_)
	{
		if (res == 0)
		{
			CSetupData::StartRun(update_yixin_ ? L"/afterupdate=1" : L"");
			Close(200);
		}
		else if (tip.empty())
		{
			Close(200);
		}
		else
		{
			MsgForm::MsgboxCallback msgbox_cb = [this](MsgBoxRet ret)
			{
				if (!destroy_wnd_)
				{
					this->Close(200);
				}
			};
			MsgBox()->SetTitle(L"STRING_TIPS")->SetInfor(tip, false)->SetIcon(kMsgIconWarn)
				->AddButton(L"STRING_OK")->AsynShow(m_hWnd, ToWeakCallback(msgbox_cb));
		}
	}
}
void SetupForm::ShowUnzipMsgUI(std::wstring tip, std::wstring path, std::wstring tip2)
{
	MsgForm::MsgboxCallback msgbox_cb = std::bind(&SetupForm::ShowMsgCallBack, this, std::placeholders::_1);
	MsgBox()->SetTitle(L"STRING_TIPS")->SetInfor(tip, false)->SetPathInfo(path)->SetInfor2(tip2)->SetIcon(kMsgIconWarn)
		->AddButton(L"STRING_STOP", true)->AddButton(L"STRING_RETRY")->AddButton(L"STRING_IGNORE")
		->AsynShow(m_hWnd, ToWeakCallback(msgbox_cb));
}
void SetupForm::ShowMsgCallBack(MsgBoxRet res)
{
	show_msg_res_ = res;
}
void SetupForm::ShowProgress(uint32_t pos)
{
	progress_->SetValue(pos);
	std::wstring pos_text = nbase::StringPrintf(L"%d%%", pos);
	progress_pos_->SetText(pos_text);
}
//获取剩余空间
double SetupForm::GetUserFreeSpace(std::wstring path)
{
	double ret = 0;
	nbase::StringReplaceAll(L"\\", L"/", path);
	if (path.size() >= 3 && path.substr(1,2) == L":/")
	{
		std::wstring root_path = path.substr(0, 3);
		DWORD dwSectorsPerCluster = 0;//每簇中扇区数  
		DWORD dwBytesPerSector = 0;//每扇区中字节数  
		DWORD dwFreeClusters = 0;//剩余簇数  
		DWORD dwTotalClusters = 0;//总簇数  
		if (GetDiskFreeSpace(root_path.c_str(), &dwSectorsPerCluster, &dwBytesPerSector,
			&dwFreeClusters, &dwTotalClusters))
		{
			double dd = dwSectorsPerCluster*dwBytesPerSector / (1024.*1024.);
			ret = dwFreeClusters*dd;//该磁盘剩余容量总大小  
		}
	}
	return ret;
}
//检查路径
void SetupForm::CheckSetupPath()
{
	std::wstring path = richedit_path_->GetText();
	path = CSetupData::CheckDirAndReplace(path);
	double dd = GetUserFreeSpace(path);
	std::wstring free_space_tip;
	if (dd > 1024.)
	{
		free_space_tip = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FREE_SPACE_G").c_str(), dd / 1024.);
	}
	else
	{
		free_space_tip = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FREE_SPACE_M").c_str(), dd);
	}
	free_space_label_->SetText(free_space_tip);
	bool path_ok = true;
	if (path.find(L':') == -1 || path.size() < 3 || !nbase::FilePathIsExist(path.substr(0, 3), true))
	{
		path_ok = false;
	}
	else
	{
		std::wstring path_other = path.substr(3);
		if (path_other.find(L':') != -1 ||
			path_other.find(L'*') != -1 ||
			path_other.find(L'?') != -1 ||
			path_other.find(L'"') != -1 ||
			path_other.find(L'<') != -1 ||
			path_other.find(L'>') != -1 ||
			path_other.find(L'|') != -1)
		{
			path_ok = false;
		}
	}
	if (!path_ok)
	{
		path_err_label_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INSTAL_PATH_ERROR"));
		path_err_label_->SetVisible();
		btn_setup_->SetEnabled(false);
		return;
	}
	if (dd < 30)
	{
		path_err_label_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_DISK_SPACE_ERROR"));
		path_err_label_->SetVisible();
		btn_setup_->SetEnabled(false);
		return;
	}
	path_err_label_->SetVisible(false);
	btn_setup_->SetEnabled(check_read_->IsSelected());
}
//开始动画
void SetupForm::StartPathBoxAnimationPlayer()
{
	//head_box_->SetMargin(ui::UiRect(0, HEAD_MARGIN_TOP_START, 0, 0));
	pathbox_timer_weakfalg_.Cancel();
	StdClosure cb = std::bind(&SetupForm::PathBoxAnimationPlayer, this);
	// pathbox_timer_weakfalg_.PostRepeatedTaskWeakly(cb, nbase::TimeDelta::FromMilliseconds(16));
    nbase::ThreadManager::PostRepeatedTask(pathbox_timer_weakfalg_.ToWeakCallback(cb),
        nbase::TimeDelta::FromMilliseconds(16));
}
//动画中
void SetupForm::PathBoxAnimationPlayer()
{
	int32_t height = box_path_->GetFixedHeight();
	if (check_path_->IsSelected())
	{
		height += 20;
		if (height > 130)
		{
			height = 130;
			pathbox_timer_weakfalg_.Cancel();
		}
	}
	else
	{
		height -= 20;
		if (height < 0)
		{
			height = 0;
			pathbox_timer_weakfalg_.Cancel();
		}
	}
	box_path_->SetFixedHeight(height);
}