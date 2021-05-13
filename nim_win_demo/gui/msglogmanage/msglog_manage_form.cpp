#include "stdafx.h"
#include "resource.h"
#include "msglog_manage_form.h"
#include "gui/main/main_form.h"
#include "module/msglog_manager/msglog_callback.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "nim_service/module/service/session_service.h"
#include "shared/unzip.h"
#include "base/encrypt/encrypt.h"
#include "base/encrypt/encrypt_impl.h"
#include <fstream>

using namespace ui;

const LPCTSTR MsglogManageForm::kClassName = L"MsglogManageForm";

MsglogManageForm::MsglogManageForm()
{
	db_running_ = false;
	open_file_ = false;
	export_or_import_ = nim::LogsBackupRemoteOperate_Export;
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
	btn_cancel_ = (Button*)FindControl(L"btn_cancel");
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
		if (export_or_import_ == nim::LogsBackupRemoteOperate_Export)
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
		progress_->SetVisible(export_or_import_ == nim::LogsBackupRemoteOperate_Import);
		prg_box_->SetVisible();

		if (export_or_import_ == nim::LogsBackupRemoteOperate_Export)
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
	else if (name == L"btn_cancel")
	{
		btn_cancel_->SetEnabled(false);

		if (export_or_import_ == nim::LogsBackupRemoteOperate_Export)
			nim::MsgLog::CancelExportBackupToRemote();
		else
			nim::MsgLog::CancelImportBackupFromRemote();

		DelayClose();
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

bool MsglogManageForm::SetType(nim::LogsBackupRemoteOperate option, MigrateMsglogTarget target)
{
	if (db_running_ || open_file_)
	{
		ShowMsgBox(m_hWnd, MsgboxCallback(),
			MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_PROCING_TRY_AGAIN"), 
			false);
		return false;
	}
	
	MutiLanSupport* multilan = MutiLanSupport::GetInstance();
	export_or_import_ = option;

	if (target == kLocal)
	{
		close_timer_.Cancel();
		path_box_->SetEnabled(true);
		prg_box_->SetVisible(false);
		progress_text_->SetText(L"");
		path_edit_->SetText(L"");
		btn_run_->SetEnabled(true);

		if (export_or_import_ == nim::LogsBackupRemoteOperate_Export)
		{
			tip_text_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORT_MSGLOG"));
			btn_run_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_EXPORT"));
		}
		else if (export_or_import_ == nim::LogsBackupRemoteOperate_Import)
		{
			tip_text_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORT_MSGLOG"));
			btn_run_->SetText(multilan->GetStringViaID(L"STRID_MSGLOG_MANAGE_IMPORT"));
		}
	}
	else if (target == kRemote)
	{
		SetDbStatus(true);
		if (export_or_import_ == nim::LogsBackupRemoteOperate_Export)
			ExportToRemote();
		else
			ImportFromRemote();

		path_box_->SetVisible(false);
		prg_box_->SetVisible(true);
		btn_run_->SetVisible(false);
		result_text_->SetText(export_or_import_ == nim::LogsBackupRemoteOperate_Export ? 
			MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORTING")
			: 
			MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORTING"));
		progress_text_->SetText(L"0%");
	}
	
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
	if (export_or_import_ == nim::LogsBackupRemoteOperate_Export)
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

bool MsglogManageForm::ExportToRemote()
{
	nim::LogsBackupExportInfo export_info;

	export_info.SetEncryptKey(nim::Tool::GetMd5(nim::Tool::GetUuid()));
	export_info.ExportBackupToRemoteLogFiter = MsglogCallback::OnMigrateLogFilter;
	export_info.ExportBackupToRemoteEncryptCallback = MsglogCallback::OnMigrateMsglogEncryptCallback;
	export_info.ExportBackupToRemotePackageCallback = MsglogCallback::OnMigrateMsglogPackageCallback;
	export_info.LogsBackupProgressCallback = MsglogCallback::OnMigrateMsglogProgressCallback;
	export_info.LogsBackupCompleteCallback = MsglogCallback::OnMigrateMsglogCompletedCallback;
	
	return nim::MsgLog::ExportBackupToRemote(export_info);
}

bool MsglogManageForm::ImportFromRemote()
{
	nim::LogsBackupImportInfo import_info;

	import_info.ImportBackupFromRemoteDecryptCallback = MsglogCallback::OnMigrateMsglogDecryptCallback;
	import_info.ImportBackupFromRemoteUnPackageCallback = MsglogCallback::OnMigrateMsglogUnPackageCallback;
	import_info.LogsBackupProgressCallback = MsglogCallback::OnMigrateMsglogProgressCallback;
	import_info.LogsBackupCompleteCallback = MsglogCallback::OnMigrateMsglogCompletedCallback;
	return nim::MsgLog::ImportBackupFromRemote(import_info);
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

void MsglogManageForm::OnMigrateMsglogProgressCallbackUI(nim::LogsBackupRemoteOperate operate, float progress)
{
	//QLOG_APP(L"ExportToRemoteProgressCallback running..., progress = {0}") << progress;
	Post2UI(ToWeakCallback([this, progress]() {
		progress_->SetValue(progress * 100);
		auto progress_text = nbase::StringPrintf(L"%.02f%%", ((progress * 100)));
		progress_text_->SetText(progress_text);
		btn_cancel_->SetVisible(true);
	}));
}

void MsglogManageForm::OnMigrateMsglogCompletedCallbackUI(nim::LogsBackupRemoteOperate operate, nim::LogsBackupRemoteState state)
{
	//QLOG_APP(L"ExportToRemoteCompletedCallback running..., state = {0}") << state;
	Post2UI(ToWeakCallback([this, state]() {
		if (state >= nim::LogsBackupRemoteState_FinalState_Begin)
		{
			std::wstring show_text;
			switch (state)
			{			
			case nim::LogsBackupRemoteState_UserCanceled:
				show_text = export_or_import_ == nim::LogsBackupRemoteOperate_Export ? 
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_CANCELED")
					: 
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_CANCELED");
				break;
			case nim::LogsBackupRemoteState_IMP_NoBackup://没有备份文件
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_ERR_NO_BACKUP");
				break;
			case nim::LogsBackupRemoteState_IMP_SyncFromSrvError://查询备份失败一般是网络错误
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_ERR_SYNC_BACKUP");
				break;
			case nim::LogsBackupRemoteState_IMP_DownloadBackupFailed://下载备份文件出错
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_ERR_DOWNLOAD_BACKUP");
				break;
			case nim::LogsBackupRemoteState_IMP_RAWError://解密/解压出来的源文件格式错误
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_ERR_DECRYPT_UNZIP");
				break;
			case nim::LogsBackupRemoteState_IMP_ParseRAWError://解析源文件格式错误
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_ERR_PARSERAW");
				break;
			case nim::LogsBackupRemoteState_IMP_LocalDBFailed://导入本地DB出错	
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_ERR_DB");
				break;
			case nim::LogsBackupRemoteState_EXP_LocalDBFailed://打开本地DB失败
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_ERR_DB");
				break;
			case nim::LogsBackupRemoteState_EXP_RAWError://导出到源文件失败
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_ERR_RAW");
				break;
			case nim::LogsBackupRemoteState_EXP_UploadBackupFailed://上传备份文件出错
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_ERR_UPLOAD");
				break;
			case nim::LogsBackupRemoteState_EXP_SyncToSrvError://同步到服务器出错一般是网络错误
				show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_ERR_SYNCSRV");
				break;
			case nim::LogsBackupRemoteState_SDKError:
				show_text = export_or_import_ == nim::LogsBackupRemoteOperate_Export ? 
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_FAIL_TIP")
					: 
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_FAIL_TIP");
				break;		
			case nim::LogsBackupRemoteState_Done:
				show_text = export_or_import_ == nim::LogsBackupRemoteOperate_Export ? 
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_SUCCESS_TIP")
					:
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_SUCCESS_TIP");
				break;
			case nim::LogsBackupRemoteState_Done_NoLogs:
				show_text = export_or_import_ == nim::LogsBackupRemoteOperate_Export ? 
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_EXPORT_NOLOGS_TIP")
					:
					MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSGLOGMANAGEFORM_IMPORT_NOLOGS_TIP");
				break;
			default:
				break;
			}
			progress_->SetValue(100);
			result_text_->SetText(show_text);
			progress_text_->SetVisible(false);
			progress_text_->SetText(L"100%");
			btn_cancel_->SetVisible(false);
			SetDbStatus(false);
			// DelayClose();
		}
	}));
}
