#include "rts_replay.h"
#include "shared/modal_wnd/file_dialog_ex.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR RtsReplay::kClassName = L"RtsReplay";

RtsReplay::RtsReplay()
{
	show_type_ = ShowOpAll;
}

RtsReplay::~RtsReplay()
{
}

ui::UILIB_RESOURCETYPE RtsReplay::GetResourceType() const
{
	return ui::UILIB_FILE; 
}

std::wstring RtsReplay::GetZIPFileName() const
{
	return (L"rts.zip");
}

std::wstring RtsReplay::GetSkinFolder()
{
	return L"rts";
}

std::wstring RtsReplay::GetSkinFile()
{
	return L"rts_replay.xml";
}

std::wstring RtsReplay::GetWindowId() const
{
	return kClassName;
}
ui::Control* RtsReplay::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BoardBox"))
	{
		return new BoardControl();
	}
	return NULL;
}

LRESULT RtsReplay::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void RtsReplay::InitWindow()
{
	std::wstring title_text = L"白板回放";
	SetTaskbarTitle(title_text);
	Label* title = (Label*)FindControl(L"title");
	title->SetText(title_text);

	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&RtsReplay::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&RtsReplay::OnClicked, this, std::placeholders::_1));

	board_ = (BoardControl*)FindControl(L"board");
	mine_slider_ = (Slider*)FindControl(L"mine_prg");
	other_slider_ = (Slider*)FindControl(L"other_prg");
	mine_size_label_ = (Label*)FindControl(L"mine_size");
	other_size_label_ = (Label*)FindControl(L"other_size");
	mine_pos_label_ = (Label*)FindControl(L"mine_pos");
	other_pos_label_ = (Label*)FindControl(L"other_pos");

	ReShowInfo(false);
	ReShowInfo(true);
}

void RtsReplay::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

bool RtsReplay::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (msg->Type == ui::kEventValueChange)
	{
		if (name == L"mine_prg")
		{
			ReShowInfo(true);
		}
		else if (name == L"other_prg")
		{
			ReShowInfo(false);
		}
	}
	return true;
}

bool RtsReplay::OnClicked(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();		
	if(name == L"btn_showtype")
	{
		switch (show_type_)
		{
		case ShowOpAll:
			show_type_ = ShowOpMine;
			((Button*)(arg->pSender))->SetText(L"显示自己");
			break;
		case ShowOpMine:
			show_type_ = ShowOpOther;
			((Button*)(arg->pSender))->SetText(L"显示对方");
			break;
		case ShowOpOther:
			show_type_ = ShowOpAll;
			((Button*)(arg->pSender))->SetText(L"显示全部");
			break;
		}
		board_->SetShowType(show_type_);
	}
	else if (name == L"btn_mine")
	{
		OnBtnFile(true);
	}
	else if (name == L"btn_other")
	{
		OnBtnFile(false);
	}
	else if (name == L"btn_mine_play")
	{
		auto_play_mine_weakflag_.Cancel();
		board_->ReleaseAllDrawUnits(ShowOpMine);
		AutoPlay(true, 0);
	}
	else if (name == L"btn_other_play")
	{
		auto_play_other_weakflag_.Cancel();
		board_->ReleaseAllDrawUnits(ShowOpOther);
		AutoPlay(false, 0);
	}
	else if (name == L"btn_mine_stop")
	{
		auto_play_mine_weakflag_.Cancel();
	}
	else if (name == L"btn_other_stop")
	{
		auto_play_other_weakflag_.Cancel();
	}
	return true;
}

int RtsReplay::OnParseData(std::string data, std::list<DrawOpInfo>& info_lists)
{
	int count = 0;
	info_lists.clear();
	int pos = data.find(';');
	while (pos != -1)
	{
		ui::DrawOpInfo info;
		std::string cur_data = data.substr(0, pos);
		sscanf(cur_data.c_str(), "%d:%f,%f", &info.draw_op_type_, &info.x_, &info.y_);

		switch (info.draw_op_type_)
		{
		case DrawOpStart:
		case DrawOpUndo:
		case DrawOpMove:
		case DrawOpEnd:
		case DrawOpClear:
			info_lists.push_back(info);
			count++;
			break;
		default:
			break;
		}

		data = data.substr(pos + 1);
		pos = data.find(';');
	}
	return count;
}
void RtsReplay::OnBtnFile(bool mine)
{
	std::wstring file_type = L"文件格式(*.*)";
	LPCTSTR filter = L"*.*";
	std::map<LPCTSTR, LPCTSTR> filters;
	filters[file_type.c_str()] = filter;

	CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&RtsReplay::OnFileSelected, this, mine, std::placeholders::_1, std::placeholders::_2);

	CFileDialogEx* file_dlg = new CFileDialogEx();
	file_dlg->SetFilter(filters);
	file_dlg->SetParentWnd(m_hWnd);
	file_dlg->AyncShowOpenFileDlg(cb);
}
void RtsReplay::OnFileSelected(bool mine, BOOL ret, std::wstring file_path)
{
	if (ret)
	{
		std::string file_data;
		if (nbase::ReadFileToString(file_path, file_data))
		{
			if (mine)
			{
				auto_play_mine_weakflag_.Cancel();
				int count = OnParseData(file_data, info_lists_mine_);
				mine_slider_->SetMaxValue(count);
				mine_slider_->SetValue(count);
				std::wstring text_size = nbase::StringPrintf(L"本人数据节点%d", count);
				mine_size_label_->SetText(text_size);
				ReShowInfo(true);
			} 
			else
			{
				auto_play_other_weakflag_.Cancel();
				int count = OnParseData(file_data, info_lists_other_);
				other_slider_->SetMaxValue(count);
				other_slider_->SetValue(count);
				std::wstring text_size = nbase::StringPrintf(L"对方数据节点%d", count);
				other_size_label_->SetText(text_size);
				ReShowInfo(false);
			}
		}
	}
}

void RtsReplay::ReShowInfo(bool mine)
{
	if (mine)
	{
		auto_play_mine_weakflag_.Cancel();
		int v = mine_slider_->GetValue();
		int count = mine_slider_->GetMaxValue();
		std::wstring text_pos = nbase::StringPrintf(L"%d%%", count > 0 ? v * 100 / count : 0);
		mine_pos_label_->SetText(text_pos);
		board_->ReShowMine(info_lists_mine_, v);
	}
	else
	{
		auto_play_other_weakflag_.Cancel();
		int v = other_slider_->GetValue();
		int count = other_slider_->GetMaxValue();
		std::wstring text_pos = nbase::StringPrintf(L"%d%%", count > 0 ? v * 100 / count : 0);
		other_pos_label_->SetText(text_pos);
		board_->ReShowOther(info_lists_other_, v);
	}
}
void RtsReplay::AutoPlay(bool mine, int pos)
{
	DrawOpInfo info;
	bool find = false;
	if (mine)
	{
		int count = mine_slider_->GetMaxValue();
		mine_slider_->SetValue(pos);
		std::wstring text_pos = nbase::StringPrintf(L"%d%%", count > 0 ? pos * 100 / count : 0);
		mine_pos_label_->SetText(text_pos);
		if (count <= pos)
		{
			return;
		}
		int num = 0;
		for (auto& it : info_lists_mine_)
		{
			if (num == pos)
			{
				find = true;
				info = it;
				break;
			}
			num++;
		}
	}
	else
	{
		int count = other_slider_->GetMaxValue();
		other_slider_->SetValue(pos);
		std::wstring text_pos = nbase::StringPrintf(L"%d%%", count > 0 ? pos * 100 / count : 0);
		other_pos_label_->SetText(text_pos);
		if (count <= pos)
		{
			return;
		}
		int num = 0;
		for (auto& it : info_lists_other_)
		{
			if (num == pos)
			{
				find = true;
				info = it;
				break;
			}
			num++;
		}
	}
	if (find)
	{
		board_->PlayDrawInfo(info, mine);
		StdClosure clouse = nbase::Bind(&RtsReplay::AutoPlay, this, mine, pos + 1);
		if (mine)
		{
			clouse = auto_play_mine_weakflag_.ToWeakCallback(clouse);
		} 
		else
		{
			clouse = auto_play_other_weakflag_.ToWeakCallback(clouse);
		}
		int time = 20;
		if (info.draw_op_type_ == DrawOpUndo)
		{
			time = 100;
		}
		nbase::ThreadManager::PostDelayedTask(clouse, nbase::TimeDelta::FromMilliseconds(time));
	}
}

}