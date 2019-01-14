#include "bubble_transferfile.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "module/db/user_db.h"
#include "module/session/session_manager.h"
#include "shared/tool.h"

using namespace ui;

namespace nim_comp
{
MsgBubbleTransferFile:: ~MsgBubbleTransferFile()
{
	if (my_msg_ && msg_.status_ == nim::kNIMMsgLogStatusSending)
	{
		nim::Talk::StopSendMsg(msg_.client_msg_id_, msg_.type_);
	}
}
void MsgBubbleTransferFile::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);
	transfer_file_session_state_ = TransferFileSessionState_NULL;
	msg_file_ = new Box;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_file_, L"session/transfer_file_left.xml");
	else
		GlobalManager::FillBoxWithCache(msg_file_, L"session/transfer_file_right.xml");
	bubble_box_->Add(msg_file_);

	file_image_ = static_cast<Button *>(this->FindSubControl(L"file_image"));
	file_info_name_ = (Label*) this->FindSubControl(L"file_info_name");
	file_info_size_ = (Label*) this->FindSubControl(L"file_info_size");

	http_progress_ = static_cast<Progress *>(this->FindSubControl(L"http_progress"));
	http_status_ = static_cast<Label *>(this->FindSubControl(L"http_status"));

	file_receive_ = static_cast<Button *>(this->FindSubControl(L"file_receive"));			// 接收
	file_saveas_ = static_cast<Button *>(this->FindSubControl(L"file_saveas"));				// 另存为
	file_reject_ = static_cast<Button *>(this->FindSubControl(L"file_reject"));				// 拒绝
	file_cancel_ = static_cast<Button *>(this->FindSubControl(L"file_cancel"));				// 取消接收
	file_open_ = static_cast<Button *>(this->FindSubControl(L"file_open"));					// 
	file_openducu_ = static_cast<Button *>(this->FindSubControl(L"file_opendocu"));

	progress_vertlayout_ = static_cast<VBox *>(this->FindSubControl(L"progress_vertlayout"));

	ui::MutiLanSupport *multi = ui::MutiLanSupport::GetInstance();
	file_receive_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_RECEIVE").c_str());
	file_saveas_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_SAVEAS").c_str());
	file_reject_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_REJECT").c_str());
	file_cancel_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_CANCEL").c_str());
	file_open_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_OPEN").c_str());
	file_openducu_->SetText(multi->GetStringViaID(L"STRID_SESSION_FILEBUBBLE_OPENDOCU").c_str());

	std::wstring image_path = QPath::GetAppPath() + L"res/icons/file_default.png";
	file_image_->SetBkImage(image_path.c_str());

	file_saveas_->AttachClick(nbase::Bind(&MsgBubbleTransferFile::OnEvent, this, std::placeholders::_1));
	file_open_->AttachClick(nbase::Bind(&MsgBubbleTransferFile::OnEvent, this, std::placeholders::_1));
	file_openducu_->AttachClick(nbase::Bind(&MsgBubbleTransferFile::OnEvent, this, std::placeholders::_1));
	file_reject_->AttachClick(nbase::Bind(&MsgBubbleTransferFile::OnEvent, this, std::placeholders::_1));
	file_cancel_->AttachClick(nbase::Bind(&MsgBubbleTransferFile::OnEvent, this, std::placeholders::_1));

	msg_file_->AttachMenu(nbase::Bind(&MsgBubbleTransferFile::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleTransferFile::InitInfo(const nim::IMMessage &msg)
{
	nim::IMFile file_data;

	Json::Value values;
	Json::Reader reader;
	if (reader.parse(msg.attach_, values))
	{
		Json::Value json_file_info = values["params"]["file_info"];
		file_name_ = json_file_info["file_name"].asString();
		file_size_ = json_file_info["file_size"].asInt();
		file_path_ = local_path_ = json_file_info["file_path"].asString();
		transfer_file_session_id_ = values["session_id"].asString();
	}

	__super::InitInfo(msg);

	// 取用户目录
	std::string path, extend;
	UserDB::GetInstance()->QueryDataWithMsgId(msg.client_msg_id_, path, extend);
	if (!path.empty() || !extend.empty())
	{
		local_path_ = path;
		int state = TransferFileSessionState_NULL;
		nbase::StringToInt(extend, &state);

		// 除了失败、拒绝和成功，其他都显示已取消
		switch (state)
		{
		case TransferFileSessionState_Failed:
		case TransferFileSessionState_Rejected:
		case TransferFileSessionState_ReceiverRejected:
		case TransferFileSessionState_Succeeded:
			SetBubbleStatus(static_cast<TransferFileSessionState>(state), false);
			break;
		case TransferFileSessionState_Consulting:
		case TransferFileSessionState_ReceiverCancel:
		case TransferFileSessionState_SenderCancel:
		case TransferFileSessionState_Transferring:
		case TransferFileSessionState_Wait:
			SetBubbleStatus(static_cast<TransferFileSessionState>(TransferFileSessionState_ReceiverCancel), false);
			break;
		}
	}
	else
	{
		SetBubbleStatus(TransferFileSessionState_Wait);
	}

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
}

void MsgBubbleTransferFile::SetBubbleStatus(TransferFileSessionState status, bool write_db/* = true*/)
{
	if (IsTransferFileSessionFinalState(transfer_file_session_state_) ||
		transfer_file_session_state_ == status)
		return;
	transfer_file_session_state_ = status;

	file_saveas_->SetVisible(false);
	file_reject_->SetVisible(false);
	file_cancel_->SetVisible(false);

	file_open_->SetVisible(false);
	file_openducu_->SetVisible(false);
	if (IsTransferFileSessionFinalState(transfer_file_session_state_))
		progress_vertlayout_->SetVisible(false);
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();

	switch (transfer_file_session_state_)
	{
	case TransferFileSessionState_Wait:
		file_saveas_->SetVisible(!my_msg_);
		file_reject_->SetVisible(!my_msg_);
		file_cancel_->SetVisible(my_msg_);
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_WAIT").c_str());
		break;
	case TransferFileSessionState_Consulting:
		file_cancel_->SetVisible(true);
		progress_vertlayout_->SetVisible(true);
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_CONSULTING").c_str());
		break;
	case TransferFileSessionState_Transferring:
		file_cancel_->SetVisible(true);
		progress_vertlayout_->SetVisible(true);
		break;
	case TransferFileSessionState_Succeeded:
		file_open_->SetVisible(true);
		file_openducu_->SetVisible(true);
		progress_vertlayout_->SetVisible(false);
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_DOWNLOADED").c_str());
		break;
	case TransferFileSessionState_Failed:
		progress_vertlayout_->SetVisible(false);
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_FAILED").c_str());
		break;
	case TransferFileSessionState_CMDTimeout:
		progress_vertlayout_->SetVisible(false);
		http_status_->SetText(L"对方无应答");
		break;
	case TransferFileSessionState_ReceiverRejected:
	case TransferFileSessionState_Rejected:
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_RECEIVERREJECTED").c_str());
		break;
	case TransferFileSessionState_ReceiverCancel:
		progress_vertlayout_->SetVisible(false);
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_CANCEL").c_str());
		break;
	case TransferFileSessionState_SenderCancel:
		progress_vertlayout_->SetVisible(false);
		http_status_->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_TRANSFERFILEBUBBLE_CANCEL").c_str());
		break;
	case TransferFileSessionState_NULL:
	default:
		break;
	}

	if (write_db)
	{
		// 如果修改的状态不是初始的等待状态或者未知状态，那么将状态写入数据库保存到本地
		// 确保下次加载历史消息可以获取到正确的状态
		UserDB::GetInstance()->InsertData(msg_.client_msg_id_, local_path_, nbase::IntToString(transfer_file_session_state_));
	}
}

void MsgBubbleTransferFile::SetMsgStatus(nim::NIMMsgLogStatus status)
{
	if (status == nim::kNIMMsgLogStatusSendFailed)
	{
		return;
	}

	__super::SetMsgStatus(status);
}

std::wstring MsgBubbleTransferFile::GetFileSizeStr(int64_t size)
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


bool MsgBubbleTransferFile::SetFileIcon(const std::string& file_name)
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

bool MsgBubbleTransferFile::OnEvent(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"file_receive")
	{
		return false;
	}
	else if (name == L"file_saveas")
	{
		SaveAs();
		return false;
	}
	else if (name == L"file_reject")
	{
		nim_p2p::NimP2PDvelopKit::GetInstance()->RejectReceiveFile(const_cast<TransferFileSessionID>(transfer_file_session_id_.c_str()));
	}
	else if (name == L"file_cancel")
	{
		if (my_msg_)
		{
			nim_p2p::NimP2PDvelopKit::GetInstance()->CancelTransferFile(const_cast<TransferFileSessionID>(transfer_file_session_id_.c_str()));
		}
		else
		{
			nim_p2p::NimP2PDvelopKit::GetInstance()->CancelReceiveFile(const_cast<TransferFileSessionID>(transfer_file_session_id_.c_str()));
		}
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
	return true;
}

void MsgBubbleTransferFile::SetProgressValue(int prog_value)
{
	if (IsTransferFileSessionFinalState(transfer_file_session_state_))
		return;
	http_progress_->SetValue(prog_value);
	std::wstring http_status;
	nbase::StringPrintf(http_status, L"%d%%", prog_value);
	http_status_->SetText(http_status.c_str());
}

void MsgBubbleTransferFile::SaveAs()
{
	std::wstring file_type = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_FILE_FORMAT");
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
	CFileDialogEx::FileDialogCallback2 callback2 = nbase::Bind(&MsgBubbleTransferFile::SaveFile, this, std::placeholders::_1, std::placeholders::_2);
	file_dlg->AyncShowSaveFileDlg(callback2);
}

void MsgBubbleTransferFile::SaveFile(BOOL ret, std::wstring file_path)
{
	if (ret)
	{
		local_path_ = nbase::UTF16ToUTF8(file_path);
		StartDownload();
	}
}

void MsgBubbleTransferFile::StartDownload()
{
	if (!local_path_.empty())
	{
		nim_p2p::NimP2PDvelopKit::GetInstance()->ReceiveFile(const_cast<TransferFileSessionID>(transfer_file_session_id_.c_str()), local_path_);

		SetBubbleStatus(TransferFileSessionState_Transferring);
	}
}

bool MsgBubbleTransferFile::IsTransferFileSessionFinalState(TransferFileSessionState state)
{
	switch (state)
	{	
	case TransferFileSessionState_Succeeded:
	case TransferFileSessionState_Failed:
	case TransferFileSessionState_CMDTimeout:
	case TransferFileSessionState_ReceiverRejected:
	case TransferFileSessionState_Rejected:
	case TransferFileSessionState_ReceiverCancel:
	case TransferFileSessionState_SenderCancel:
		return true;
	default:
		return false;
	}
}

bool MsgBubbleTransferFile::OnMenu(ui::EventArgs* arg)
{
	// 不需要给传送文件加任何右键菜单
	// PopupMenu(false, true);
	return false;
}

void MsgBubbleTransferFile::OnDownloadFileProgressCallback(int total, int transferred)
{
	if (total > 0)
	{
		int prog_value = (int)(transferred * 100.0 / total);
		SetProgressValue(prog_value);
	}
}

}