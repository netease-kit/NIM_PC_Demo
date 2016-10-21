#include "bubble_file.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "module/msglog/msg_extend_db.h"
#include "module/session/session_manager.h"
#include "shared/tool.h"

using namespace ui;

namespace nim_comp
{
MsgBubbleFile:: ~MsgBubbleFile()
{
	if (my_msg_ && msg_.status_ == nim::kNIMMsgLogStatusSending)
	{
		nim::Talk::StopSendMsg(msg_.client_msg_id_, msg_.type_);
	}
	if (loading_)
	{
		nim::NOS::StopFetchMedia(msg_);
	}
	//if (download_request_id_ != -1)
	//{
	//	nim_http::RemoveRequest(download_request_id_);
	//}
}
void MsgBubbleFile::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);
	loading_ = false;
	download_fail_ = false;
	msg_file_ = new Box;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_file_, L"session/file_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_file_, L"session/file_left.xml");
	bubble_box_->Add(msg_file_);

	file_image_ = static_cast<Button *>(this->FindSubControl(L"file_image"));
	file_info_name_ = (Label*) this->FindSubControl(L"file_info_name");
	file_info_size_ = (Label*) this->FindSubControl(L"file_info_size");

	http_progress_ = static_cast<Progress *>(this->FindSubControl(L"http_progress"));
	http_status_ = static_cast<Label *>(this->FindSubControl(L"http_status"));
	file_saveas_ = static_cast<Button *>(this->FindSubControl(L"file_saveas"));
	//file_save_ = static_cast<Button *>(this->FindSubControl(L"file_save"));
	file_open_ = static_cast<Button *>(this->FindSubControl(L"file_open"));
	file_openducu_ = static_cast<Button *>(this->FindSubControl(L"file_opendocu"));
	file_cancel_ = static_cast<Button *>(this->FindSubControl(L"file_cancel"));
	file_reup_ = static_cast<Button *>(this->FindSubControl(L"file_reup"));
	file_redl_ = static_cast<Button *>(this->FindSubControl(L"file_redl"));
	progress_vertlayout_ = static_cast<VBox *>(this->FindSubControl(L"progress_vertlayout"));

	ui::MutiLanSupport *multi = ui::MutiLanSupport::GetInstance();
	file_saveas_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_SAVEAS").c_str());
	//file_save_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_SAVE").c_str());
	file_open_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_OPEN").c_str());
	file_openducu_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_OPENDOCU").c_str());
	file_cancel_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_CANCEL").c_str());
	file_redl_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_REDL").c_str());
	file_reup_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_REUP").c_str());

	std::wstring image_path = QPath::GetAppPath() + L"res/icons/file_default.png";
	file_image_->SetBkImage(image_path.c_str());

	file_saveas_->AttachClick(nbase::Bind(&MsgBubbleFile::OnEvent, this, std::placeholders::_1));
	file_open_->AttachClick(nbase::Bind(&MsgBubbleFile::OnEvent, this, std::placeholders::_1));
	file_openducu_->AttachClick(nbase::Bind(&MsgBubbleFile::OnEvent, this, std::placeholders::_1));
	file_cancel_->AttachClick(nbase::Bind(&MsgBubbleFile::OnEvent, this, std::placeholders::_1));
	file_redl_->AttachClick(nbase::Bind(&MsgBubbleFile::OnEvent, this, std::placeholders::_1));
	file_reup_->AttachClick(nbase::Bind(&MsgBubbleFile::OnEvent, this, std::placeholders::_1));

	msg_file_->AttachMenu(nbase::Bind(&MsgBubbleFile::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleFile::InitInfo(const nim::IMMessage &msg)
{
	nim::IMFile file_data;
	nim::Talk::ParseFileMessageAttach(msg, file_data);
	file_name_ = file_data.display_name_;
	file_size_ = file_data.size_;
	file_url_ = file_data.url_;

	std::string path, extend;
	MsgExDB::GetInstance()->QueryDataWithMsgId(msg.client_msg_id_, path, extend);
	if (!path.empty())
	{
		local_path_ = path;
	}
	range_start_ = 0;
	download_request_id_ = -1;
	download_cancel_ = false;

	__super::InitInfo(msg);

	if (file_name_.empty())
	{
		return;
	}

	SetFileIcon(file_name_);

	//---------------------------------------
	std::wstring file = nbase::UTF8ToUTF16(file_name_);

	std::wstring exten;
	nbase::FilePathExtension(file, exten);

	int max_char = file.size() - exten.size();

	if (exten.find(L'.') == std::wstring::npos)
		exten.insert(0, L"...");
	else
		exten.insert(0, L"..");

	file_info_name_->SetText(file);

	const int LABEL_MAX_WIDTH = 235;

	ui::CSize sz;
	sz = file_info_name_->EstimateSize(sz);

	while (sz.cx > LABEL_MAX_WIDTH)
	{
		file_info_name_->SetText(file.substr(0, max_char) + exten);
		sz = file_info_name_->EstimateSize(sz);

		max_char -= 1;
	}

	//-----------------------------------
	std::wstring file_size = GetFileSizeStr(file_size_);
	file_info_size_->SetText(file_size);

	if (msg.status_ == nim::kNIMMsgLogStatusSending)
	{
		SetProgressValue(0);
		progress_vertlayout_->SetVisible();
	}
}

void MsgBubbleFile::SetMsgStatus(nim::NIMMsgLogStatus status)
{
	__super::SetMsgStatus(status);
	msg_.status_ = status;

	file_saveas_->SetVisible(false);
	//file_save_->SetVisible(false);
	file_open_->SetVisible(false);
	file_openducu_->SetVisible(false);
	file_cancel_->SetVisible(false);
	file_reup_->SetVisible(false);
	file_redl_->SetVisible(false);

	if (my_msg_)
	{
		if (status == nim::kNIMMsgLogStatusSent
			|| status == nim::kNIMMsgLogStatusReceipt
			|| !file_url_.empty())
		{
			progress_vertlayout_->SetVisible(false);
			http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_FILESTATUS_UPLOADED").c_str());
		}
		else if (msg_.local_res_path_.empty() || !nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg_.local_res_path_), false))
		{
			http_status_->SetText(L"路径错误");
			status_resend_->SetVisible(false);
		}
		else if (status == nim::kNIMMsgLogStatusSending)
		{
			progress_vertlayout_->SetVisible();
			file_cancel_->SetVisible(true);
		}
		else if (status == nim::kNIMMsgLogStatusSendCancel)
		{
			http_status_->SetText(L"取消发送");
			file_reup_->SetVisible(true);
		}
		else if (status == nim::kNIMMsgLogStatusSendFailed
			|| file_url_.empty())
		{
			http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_FILESTATUS_UPLOADERROR").c_str());
			file_reup_->SetVisible(true);
		}
	} 
	else
	{
		if (loading_)
		{
			progress_vertlayout_->SetVisible();
			http_status_->SetText(L"正在下载");
			file_cancel_->SetVisible(true);
		} 
		else if (download_fail_)
		{
			file_redl_->SetVisible();
			file_saveas_->SetVisible();
			//progress_vertlayout_->SetVisible(false);
			if (download_cancel_)
			{
				http_status_->SetText(L"取消下载");
			} 
			else
			{
				http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_FILESTATUS_DOWNLOADEDERROR").c_str());
			}
		}
		else if (!local_path_.empty() && nbase::FilePathIsExist(nbase::UTF8ToUTF16(local_path_), false))
		{
			progress_vertlayout_->SetVisible(false);
			file_open_->SetVisible();
			file_openducu_->SetVisible();
			http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_FILESTATUS_DOWNLOADED").c_str());
		} 
		else
		{
			file_saveas_->SetVisible();
			http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_FILESTATUS_UNDOWNLOADED").c_str());
		}
	}
}

std::wstring MsgBubbleFile::GetFileSizeStr(int64_t size)
{
	std::wstring file_size;
	float kb = size / 1024.0f;
	float mb = size / 1024.0f / 1024.0f;
	if (mb > 1.0)
	{
		file_size = nbase::StringPrintf(L"(%.2fMB)", mb);
	}
	else if (kb > 1.0)
	{
		file_size = nbase::StringPrintf(L"(%.2fKB)", kb);
	}
	else
	{
		file_size = nbase::StringPrintf(L"(%dB)", size);
	}
	return file_size;
}


bool MsgBubbleFile::SetFileIcon(const std::string& file_name)
{
	if (file_image_ && file_name.size() > 0)
	{
		std::wstring file_exten;
		std::wstring file_sel = nbase::UTF8ToUTF16(file_name);
		nbase::FilePathExtension(file_sel, file_exten);
		if (file_exten.find(L"/") != -1 || file_exten.find(L"\\") != -1 || file_exten.size() > 5)
		{
			file_exten.clear();
		}
		if (file_exten.size() > 0)
		{
			std::wstring image_path;
			file_exten = file_exten.substr(1);
			file_exten = nbase::MakeLowerString(file_exten);
			if (file_exten == L"doc" || file_exten == L"docx")
			{
				image_path = L"icons/file_doc.png";
			}
			else if (file_exten == L"ppt" || file_exten == L"pptx")
			{
				image_path = L"icons/file_ppt.png";
			}
			else if (file_exten == L"xls" || file_exten == L"xlsx")
			{
				image_path = L"icons/file_excel.png";
			}
			else if (file_exten == L"mp3")
			{
				image_path = L"icons/file_mp3.png";
			}
			else if (file_exten == L"htm" || file_exten == L"html")
			{
				image_path = L"icons/file_html.png";
			}
			else if (file_exten == L"txt" || file_exten == L"text")
			{
				image_path = L"icons/file_txt.png";
			}
			else if (file_exten == L"pdf")
			{
				image_path = L"icons/file_pdf.png";
			}
			else if (file_exten == L"zip")
			{
				image_path = L"icons/file_zip.png";
			}
			else if (file_exten == L"rar")
			{
				image_path = L"icons/file_rar.png";
			}
			else if (file_exten == L"zip")
			{
				image_path = L"icons/file_zip.png";
			}
			if (!image_path.empty())
			{
				image_path = QPath::GetAppPath() + L"res/" + image_path;
				file_image_->SetBkImage(image_path.c_str());
			}
		}
	}
	return true;
}

bool MsgBubbleFile::OnEvent(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"file_saveas")
	{
		SaveAs();
		return false;
	}
	else if (name == L"file_open")
	{
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, nbase::Bind(&shared::tools::SafeOpenUrlEx, local_path_, SW_SHOW));
		return false;
	}
	else if (name == L"file_opendocu")
	{
		std::string file_path;
		shared::FilePathApartDirectory(local_path_, file_path);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, nbase::Bind(&shared::tools::SafeOpenUrlEx, file_path, SW_SHOW));
		return false;
	}
	else if (name == L"file_cancel")
	{
		if (my_msg_)
		{
			nim::Talk::StopSendMsg(msg_.client_msg_id_, msg_.type_);
		}
		else
		{
			download_cancel_ = true;
			//nim_http::RemoveRequest(download_request_id_);
			nim::NOS::StopFetchMedia(msg_);
		}
		return false;
	}
	else if (name == L"file_reup")
	{
		if (msg_.local_res_path_.empty() || !nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg_.local_res_path_), false))
		{
			SetMsgStatus(nim::kNIMMsgLogStatusSendFailed);
		} 
		else
		{
			nim::Talk::FileUpPrgCallback* cb_pointer = new nim::Talk::FileUpPrgCallback(GetFileUpPrgCallback());
			nim::Talk::SendMsg(msg_.ToJsonString(true), msg_.client_msg_id_, cb_pointer);
			SetMsgStatus(nim::kNIMMsgLogStatusSending);
		}
		return false;
	}
	else if (name == L"file_redl")
	{
		if (local_path_.empty())
		{
			SaveAs();
		} 
		else
		{
			//SaveFile(TRUE, nbase::UTF8ToUTF16(local_path_));
			StartDownload();
		}
		return false;
	}
	return true;
}

void MsgBubbleFile::FileUpPrgCallback(int64_t uploaded_size, int64_t file_size)
{
	if (file_size > 0)
	{
		int prog_value = (int)(uploaded_size * 100 / file_size);
		SetProgressValue(prog_value);
	}
}

void MsgBubbleFile::SetProgressValue(int prog_value)
{
	http_progress_->SetValue(prog_value);
	std::wstring http_status;
	nbase::StringPrintf(http_status, L"%d%%", prog_value);
	http_status_->SetText(http_status.c_str());
}

void MsgBubbleFile::SaveAs()
{
	std::wstring file_type = L"文件格式";
	std::wstring text = nbase::StringPrintf(L"%s(*.*)", file_type.c_str());
	std::wstring file_exten;
	std::wstring file_name = nbase::UTF8ToUTF16(file_name_);
	nbase::FilePathExtension(file_name, file_exten);
	// 要保存的文件名
	CFileDialogEx* file_dlg = new CFileDialogEx();
	std::map<LPCTSTR, LPCTSTR> filters;
	filters[text.c_str()] = L"*.*";
	file_dlg->SetFilter(filters);
	file_dlg->SetFileName(file_name.c_str());
	file_dlg->SetDefExt(file_exten.c_str());
	file_dlg->SetParentWnd(GetWindow()->GetHWND());
	// 弹出非模态对话框
	CFileDialogEx::FileDialogCallback2 callback2 = nbase::Bind(&MsgBubbleFile::SaveFile, this, std::placeholders::_1, std::placeholders::_2);
	file_dlg->AyncShowSaveFileDlg(callback2);
}

void MsgBubbleFile::SaveFile(BOOL ret, std::wstring file_path)
{
	if (ret)
	{
		ASSERT(!file_url_.empty());
		local_path_ = nbase::UTF16ToUTF8(file_path);
		range_start_ = 0;
		StartDownload();
	}
}

void MsgBubbleFile::StartDownload()
{
	if (!file_url_.empty())
	{
		loading_ = true;
		download_fail_ = false;
		download_request_id_ = -1;
		nim::NOS::DownloadMediaCallback cb1 = nbase::Bind(&MsgBubbleFile::DownloadResourceCallback1, this, \
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		nim::NOS::ProgressCallback cb2 = nbase::Bind(&MsgBubbleFile::DownloadResourceProgressCallback1, this, \
			std::placeholders::_1, std::placeholders::_2);

		//自定义保存路径
// 		std::wstring dir = nim::Tool::GetUserAppdataDir(LoginManager::GetInstance()->GetAccount());
// 		dir += L"temp\\";
// 		bool ret = nbase::CreateDirectoryW(dir);
// 		nim::IMMessage msg = msg_;
// 		std::wstring fileName;
// 		ret = nbase::FilePathApartFileName(nbase::UTF8ToUTF16(msg.local_res_path_), fileName);
// 		if (fileName.empty())
// 		{
// 			nim::IMFile file;
// 			nim::Talk::ParseFileMessageAttach(msg, file);
// 			fileName = nbase::UTF8ToUTF16(file.md5_);
// 		}
// 		msg.local_res_path_ = nbase::UTF16ToUTF8(dir + fileName);
		nim::NOS::FetchMedia(msg_, cb1, cb2);

		SetMsgStatus(nim::kNIMMsgLogStatusNone);
	}
}

void MsgBubbleFile::DownloadResourceCallback1(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id)
{
	local_path_temp_ = file_path;
	DownloadResourceCallback(res_code == nim::kNIMResSuccess, res_code);
}

void MsgBubbleFile::DownloadResourceProgressCallback1(__int64 downloaded_size, __int64 file_size)
{
	DownloadResourceProgressCallback(0.0, 0.0, (double)downloaded_size, (double)file_size);
}

void MsgBubbleFile::DownloadResourceCallback(bool is_ok, int response_code)
{
	download_fail_ = !is_ok || download_cancel_;
	loading_ = false;
	if (!download_fail_)
	{
		//文件转移
		BOOL bRet = ::MoveFileExW(nbase::UTF8ToUTF16(local_path_temp_).c_str(), nbase::UTF8ToUTF16(local_path_).c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		if (!bRet)
		{
			QLOG_ERR(L"MoveFileExW failed src={0} dest={1} err={2}") << local_path_temp_ << local_path_ << GetLastError();
			bRet = nbase::CopyFile(nbase::UTF8ToUTF16(local_path_temp_).c_str(), nbase::UTF8ToUTF16(local_path_).c_str());
			if (!bRet)
			{
				download_fail_ = true;
				QLOG_ERR(L"CopyFile failed");
			}
		}
		if (!download_fail_)
		{
			MsgExDB::GetInstance()->InsertData(msg_.client_msg_id_, local_path_, "");
		}
	}
	SetMsgStatus(nim::kNIMMsgLogStatusNone);
	download_cancel_ = false;
}

void MsgBubbleFile::DownloadResourceProgressCallback(double current_upload, double total_upload, double current_download, double total_downnload)
{
	if (total_downnload > 0)
	{
		range_start_ = (__int64)current_download;
		int prog_value = (int)(current_download * 100 / total_downnload);
		SetProgressValue(prog_value);
	}
}

bool MsgBubbleFile::OnMenu(ui::EventArgs* arg)
{
	PopupMenu(false, true);
	return false;
}

}