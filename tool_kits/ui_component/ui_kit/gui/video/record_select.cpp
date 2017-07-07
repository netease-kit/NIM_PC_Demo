#include "record_select.h"

#include <ShlObj.h>
#include <sys/timeb.h>


namespace nim_comp
{

std::wstring def_path_;
const LPTSTR RecordSelectForm::kClassName = L"RecordSelectForm";

RecordSelectForm::RecordSelectForm()
{
	select_file_cb_ = nullptr;
	if (def_path_.empty())
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		//SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_DESKTOPDIRECTORY, FALSE);
		def_path_ = nbase::StringPrintf(L"%s", szPath);
	}
}

RecordSelectForm::~RecordSelectForm()
{
}

std::wstring RecordSelectForm::GetSkinFolder()
{
	return L"video";
}

std::wstring RecordSelectForm::GetSkinFile()
{
	return L"record_select.xml";
}

std::wstring RecordSelectForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring RecordSelectForm::GetWindowId() const
{
	return kClassName;
}

UINT RecordSelectForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void RecordSelectForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&RecordSelectForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&RecordSelectForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&RecordSelectForm::OnSelected, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&RecordSelectForm::OnSelected, this, std::placeholders::_1));

	path_edit_ = (ui::RichEdit*)FindControl(L"path_edit");
	path_edit_->SetReadOnly(true);
	path_edit_->SetText(def_path_);
	audio_sel_ = (ui::CheckBox*)FindControl(L"audio_sel");
	mp4_sel_ = (ui::CheckBox*)FindControl(L"mp4_sel");
	mp4_sel2_ = (ui::CheckBox*)FindControl(L"mp4_sel2");
}

LRESULT RecordSelectForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool RecordSelectForm::Notify(ui::EventArgs * msg)
{
	return false;
}

bool RecordSelectForm::OnClicked(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"select")
	{
		if (select_file_cb_)
		{
			std::wstring path = path_edit_->GetText();
			if (!path.empty() && nbase::FilePathIsExist(path, true))
			{
				def_path_ = path;
				timeb time_now;
				ftime(&time_now); // 秒数
				std::wstring aac_file_name;
				std::wstring mp4_file_name;
				std::wstring mp42_file_name;
				if (audio_sel_->IsSelected())
				{
					aac_file_name = nbase::StringPrintf(L"%s/%d.aac", path.c_str(), time_now.time);
				}
				if (mp4_sel_->IsSelected())
				{
					mp4_file_name = nbase::StringPrintf(L"%s/%d_self.mp4", path.c_str(), time_now.time);
				}
				if (mp4_sel2_->IsSelected())
				{
					mp42_file_name = nbase::StringPrintf(L"%s/%d_other.mp4", path.c_str(), time_now.time);
				}
				select_file_cb_(true, mp4_file_name, aac_file_name, mp42_file_name);
			}
		}
		Close();
	}
	else if (name == L"close")
	{
		Close();
	}
	else if (name == L"path")
	{
		BROWSEINFO bInfo = { 0 };
		bInfo.hwndOwner = GetHWND();//父窗口  
		bInfo.lpszTitle = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RECORD_SELECT_SCAN_FOLDERS").c_str();
		bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ |
			BIF_UAHINT/*带TIPS提示*/ | BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
		LPITEMIDLIST lpDlist;
		lpDlist = SHBrowseForFolder(&bInfo);
		if (lpDlist != NULL)//单击了确定按钮  
		{
			TCHAR szPathName[MAX_PATH];
			SHGetPathFromIDList(lpDlist, szPathName);
			std::wstring path;
			path = szPathName;
			path_edit_->SetText(path);
		}
	}
	return true;
}

bool RecordSelectForm::OnSelected(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"manage_room")
	{

	}
	else if (name == L"apply_room")
	{

	}

	return false;
}
void RecordSelectForm::SetSelFileCb(bool video_mode, SelectRecordFileCallback cb)
{
	ui::Label*mp4_sel_label = (ui::Label*)FindControl(L"mp4_sel_label");
	mp4_sel_label->SetTextId(video_mode ? L"STRID_RECORD_SELECT_MY_AUDIO_VIDEO" : L"STRID_RECORD_SELECT_MY_AUDIO");
	ui::Label*mp4_sel2_label = (ui::Label*)FindControl(L"mp4_sel2_label");
	mp4_sel2_label->SetTextId(video_mode ? L"STRID_RECORD_SELECT_OTHER_AUDIO_VIDEO" : L"STRID_RECORD_SELECT_OTHER_AUDIO");
	select_file_cb_ = cb;
}
}
