#include "msg_record.h"
#include "export/nim_ui_user_config.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR MsgRecordForm::kClassName	= L"MsgRecordForm";

MsgRecordForm::MsgRecordForm()
{
	btn_max_restore_ = NULL;

	first_show_msg_ = true;
	last_msg_time_ = 0;
	farst_msg_time_ = 0;
	last_server_id_ = 0;

	has_more_ = true;
	is_loading_ = false;
}

MsgRecordForm::~MsgRecordForm()
{

}

std::wstring MsgRecordForm::GetSkinFolder()
{
	return L"session";
}

std::wstring MsgRecordForm::GetSkinFile()
{
	return L"msg_record.xml";
}

ui::UILIB_RESOURCETYPE MsgRecordForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring MsgRecordForm::GetZIPFileName() const
{
	return L"msg_record.zip";
}

std::wstring MsgRecordForm::GetWindowClassName() const 
{
	return MsgRecordForm::kClassName;
}

std::wstring MsgRecordForm::GetWindowId() const 
{
	return MsgRecordForm::kClassName;
}

UINT MsgRecordForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MsgRecordForm::OnFinalMessage( HWND hWnd )
{
	std::string play_sid = AudioCallback::GetPlaySid();
	std::wstring wsid = nbase::UTF8ToUTF16(play_sid);
	if(GetWindowId() == wsid)
	{
		AudioCallback::SetPlaySid("");
		AudioCallback::SetPlayCid("");

		nim_audio::Audio::StopPlayAudio();
	}
	__super::OnFinalMessage(hWnd);
}

LRESULT MsgRecordForm::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(uMsg == WM_SIZE)
	{
		if(wParam == SIZE_RESTORED) 
			OnWndSizeMax(false);
		else if(wParam == SIZE_MAXIMIZED) 
			OnWndSizeMax(true);
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MsgRecordForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetIcon()>0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetIcon());
	}
	
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MsgRecordForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MsgRecordForm::OnClicked, this, std::placeholders::_1));

	label_title_ = (Label*) FindControl(L"title");
	btn_max_restore_ = (Button*) FindControl(L"btn_max_restore");

	msg_list_ = (ListBox*) FindControl(L"msg_list");
}

bool MsgRecordForm::Notify(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if(param->Type == ui::kEventNotify)
	{
		MsgBubbleItem* item = dynamic_cast<MsgBubbleItem*>( param->pSender );
		assert(item);
		nim::IMMessage md = item->GetMsg();

		if(param->wParam == BET_RELOAD)
		{
			item->SetLoadStatus(RS_LOADING);
			nim::NOS::FetchMedia(md, nbase::Bind(&MsgRecordForm::OnDownloadCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), nim::NOS::ProgressCallback());
		}
	}
	else if(param->Type == ui::kEventScrollChange)
	{
		if(name == L"msg_list")
		{
			bool list_top= (msg_list_->GetScrollPos().cy <= 10);
			if(list_top && has_more_ && !is_loading_)
			{
				is_loading_ = true;

				LoadingTip(true);
				msg_list_->SetScrollPosY(0);

				ShowMore(true);
			}
		}
	}
	return true;
}

bool MsgRecordForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if(name == L"btn_max_restore")
	{
		DWORD style = GetWindowLong(m_hWnd, GWL_STYLE);
		if(style & WS_MAXIMIZE)
			::ShowWindow(m_hWnd, SW_RESTORE);
		else
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
	}
	return true;
}

void MsgRecordForm::OnWndSizeMax( bool max )
{
	if(btn_max_restore_)
	{
		btn_max_restore_->SetClass(max ? L"btn_wnd_restore" : L"btn_wnd_max");
	}
}

void MsgRecordForm::LoadingTip( bool add )
{
	const std::wstring kLoadingTipName = L"LoadingOnlineMsg";

	Control* ctrl = msg_list_->FindSubControl(kLoadingTipName);
	if(ctrl)
		msg_list_->Remove(ctrl);

	if(add)
	{
		ListContainerElement* cell = new ListContainerElement;
		GlobalManager::FillBoxWithCache(cell, L"session/loading_online_msg.xml");
		msg_list_->AddAt(cell, 0);

		cell->SetName(kLoadingTipName);
	}
}
}