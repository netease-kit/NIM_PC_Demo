#include "resource.h"
#include "msglog_manage_form.h"
#include "gui/main/main_form.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "nim_service/module/service/session_service.h"

using namespace ui;

const LPCTSTR MsglogManageForm::kClassName = L"MsglogManageForm";

MsglogManageForm::MsglogManageForm()
{
	db_running_ = false;
	open_file_ = false;
	export_or_import_ = true;
}

MsglogManageForm::~MsglogManageForm()
{

}

std::wstring MsglogManageForm::GetSkinFolder()
{
	return L"msglogmanage";
}

std::wstring MsglogManageForm::GetSkinFile()
{
	return L"manage_form.xml";
}

std::wstring MsglogManageForm::GetWindowClassName() const
{
	return MsglogManageForm::kClassName;
}

std::wstring MsglogManageForm::GetWindowId() const
{
	return MsglogManageForm::kClassName;
}

UINT MsglogManageForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MsglogManageForm::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

LRESULT MsglogManageForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE)
		{
			if (db_running_)
			{
				return 0;
			}
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MsglogManageForm::InitWindow()
{
	SetIcon(IDI_ICON);

	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MsglogManageForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MsglogManageForm::OnClicked, this, std::placeholders::_1));

	tip_text_ = (ui::Label*)FindControl(L"tip_text");
	btn_sel_ = (Button*)FindControl(L"btn_sel");
	btn_run_ = (Button*)FindControl(L"btn_run");
	path_edit_ = (RichEdit*)FindControl(L"path_edit");
	path_box_ = (Box*)FindControl(L"path_box");
	prg_box_ = (Box*)FindControl(L"prg_box");
	progress_ = (Progress*)FindControl(L"progress");
	result_text_ = (ui::Label*)FindControl(L"result_text");
	progress_text_ = (ui::Label*)FindControl(L"progress_text");
}

bool MsglogManageForm::Notify(ui::EventArgs* param)
{
	return true;
}

bool MsglogManageForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_run")
	{
		std::wstring path = path_edit_->GetText();
		if (export_or_import_)
		{
			if (path.empty())
			{
				ShowMsgBox(m_hWnd, MsgboxCallback(), L"STRID_MSGLOG_MANAGE_SELECT_EXPORT_PATH");
				return true;
			}
		}
		else
		{
			if (path.empty())
			{
				ShowMsgBox(m_hWnd, MsgboxCallback(), L"STRID_MSGLOG_MANAGE_SELECT_IMPORT_FILE");
				return true;
			}
		}

		path_box_->SetEnabled(false);
		btn_run_->SetEnabled(false);
		progress_->SetValue(0);
		progress_->SetVisible(!export_or_import_);
		prg_box_->SetVisible();

		if (export_or_import_)
		{
			Export(nbase::UTF16ToUTF8(path));
		} 
		else
		{
			Import(nbase::UTF16ToUTF8(path));
		}
	}
	else if (name == L"btn_sel")
	{
		SelectPath();
	}
	else if (name == L"btn_close")
	{
		if (!db_running_)
		{
			Close();
		}
	}
	return true;
}

bool MsglogManageForm::SetType(bool export_or_import)
{
	if (db_running_ || open_file_)
	{
		return false;
	}
	close_timer_.Cancel();
	export_or_import_ = export_or_import;
	path_box_->SetEnabled(true);
	prg_box_->SetVisible(false);
	progress_text_->SetText(L"");
	MutiLanSupport* multilan = MutiLanSupport::GetInstance();
	if (export_or_import_)
	{
		tip_text_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORT_MSGLOG"));
		btn_run_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORT"));
	}
	else
	{
		tip_text_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORT_MSGLOG"));
		btn_run_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORT"));
	}
	path_edit_->SetText(L"");
	btn_run_->SetEnabled(true);
	return true;
}

void MsglogManageForm::SelectPath()
{
	open_file_ = true;
	std::wstring file_type = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_FORMAT");
	std::wstring text = nbase::StringPrintf(L"%s(*.db)", file_type.c_str());
	std::wstring file_exten = L".db";
	std::wstring file_name;
	//nbase::FilePathExtension(file_name, file_exten);
	// 要保存的文件名
	CFileDialogEx* file_dlg = new CFileDialogEx();
	std::map<LPCTSTR, LPCTSTR> filters;
	filters[text.c_str()] = L"*.db";
	file_dlg->SetFilter(filters);
	file_dlg->SetFileName(file_name.c_str());
	file_dlg->SetDefExt(file_exten.c_str());
	file_dlg->SetParentWnd(GetHWND());
	// 弹出非模态对话框
	CFileDialogEx::FileDialogCallback2 callback2 = nbase::Bind(&MsglogManageForm::OnSelectPathCallback, this, std::placeholders::_1, std::placeholders::_2);
	if (export_or_import_)
	{
		file_dlg->AyncShowSaveFileDlg(callback2);
	} 
	else
	{
		file_dlg->AyncShowOpenFileDlg(callback2);
	}
}

void MsglogManageForm::OnSelectPathCallback(BOOL ret, std::wstring file_path)
{
	open_file_ = false;
	if (ret)
	{
		path_edit_->SetText(file_path);
	}
}

void MsglogManageForm::Export(const std::string& path)
{
	SetDbStatus(true);
	result_text_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORTING"));
	nim::MsgLog::ExportDbAsync(path, nbase::Bind(&MsglogManageForm::OnExportCompeleteCallback, this, std::placeholders::_1));
}

void MsglogManageForm::OnExportCompeleteCallback(nim::NIMResCode res_code)
{
	SetDbStatus(false);
	if (res_code == nim::kNIMResSuccess)
	{
		result_text_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORT_COMPLETE"));
		DelayClose();
	}
	else
	{
		path_box_->SetEnabled(true);
		result_text_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORT_FAIL"));
		btn_run_->SetEnabled(true);
	}
}

void MsglogManageForm::Import(const std::string& path)
{
	SetDbStatus(true);
	result_text_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORTING"));
	progress_text_->SetText(L"0%");
	nim::MsgLog::ImportDbAsync(path, nbase::Bind(&MsglogManageForm::OnImportCompeleteCallback, this, std::placeholders::_1), 
		nbase::Bind(&MsglogManageForm::OnImportProgressCallback, this, std::placeholders::_1, std::placeholders::_2));
}

void MsglogManageForm::OnImportProgressCallback(int64_t imported_count, int64_t total_count)
{
	int prg_pos = imported_count * 100.0 / total_count;
	progress_->SetValue(prg_pos);
	std::wstring pos_text = nbase::StringPrintf(L"%d%%", prg_pos);
	progress_text_->SetText(pos_text);
}

void MsglogManageForm::OnImportCompeleteCallback(nim::NIMResCode res_code)
{
	SetDbStatus(false);
	if (res_code == nim::kNIMResSuccess)
	{
		OnImportProgressCallback(1, 1);
		result_text_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORT_COMPLETE"));
		nim_comp::SessionService::GetInstance()->InvokeLoadSessionList();

		DelayClose();
	}
	else
	{
		path_box_->SetEnabled(true);
		result_text_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORT_FAIL"));
		btn_run_->SetEnabled(true);
	}
}

void MsglogManageForm::DelayClose()
{
	close_timer_.Cancel();
	StdClosure timer_cb = nbase::Bind(&MsglogManageForm::Close, this, IDOK);
	timer_cb = close_timer_.ToWeakCallback(timer_cb);
	nbase::ThreadManager::PostDelayedTask(timer_cb, nbase::TimeDelta::FromSeconds(3));
}

void MsglogManageForm::SetDbStatus(bool running)
{
	ui::Control* close = FindControl(L"btn_close");
	close->SetEnabled(!running);
	db_running_ = running;
}
