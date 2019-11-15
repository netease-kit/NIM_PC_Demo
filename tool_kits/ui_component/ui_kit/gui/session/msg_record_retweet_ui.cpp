#include "msg_record_retweet.h"
#include "export/nim_ui_user_config.h"
#include "module/audio/audio_manager.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR MsgRecordRetweetForm::kClassName	= L"MsgRecordRetweetForm";

MsgRecordRetweetForm::MsgRecordRetweetForm()
{
	btn_max_restore_ = NULL;	
}

MsgRecordRetweetForm::~MsgRecordRetweetForm()
{

}

std::wstring MsgRecordRetweetForm::GetSkinFolder()
{
	return L"session";
}

std::wstring MsgRecordRetweetForm::GetSkinFile()
{
	return L"msg_record_retweet.xml";
}

std::wstring MsgRecordRetweetForm::GetWindowClassName() const 
{
	return MsgRecordRetweetForm::kClassName;
}

std::wstring MsgRecordRetweetForm::GetWindowId() const 
{
	return MsgRecordRetweetForm::kClassName;
}

UINT MsgRecordRetweetForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MsgRecordRetweetForm::OnFinalMessage( HWND hWnd )
{
	AudioManager::GetInstance()->StopPlayAudio(nbase::UTF16ToUTF8(GetWindowId()));
	__super::OnFinalMessage(hWnd);
}

LRESULT MsgRecordRetweetForm::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void MsgRecordRetweetForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetDefaultIcon()>0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetDefaultIcon());
	}
	
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MsgRecordRetweetForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MsgRecordRetweetForm::OnClicked, this, std::placeholders::_1));

	label_title_ = (Label*) FindControl(L"title");
	btn_max_restore_ = (Button*) FindControl(L"btn_max_restore");

	msg_list_ = (ListBox*) FindControl(L"msg_list");
	progress_box_ = dynamic_cast<ui::Box*>(FindControl(L"progress_box"));
	progress_ = dynamic_cast<ui::Label*>(FindControl(L"progress"));
}

bool MsgRecordRetweetForm::Notify(ui::EventArgs* param)
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
			nim::NOS::FetchMedia(md, nbase::Bind(&MsgRecordRetweetForm::OnDownloadCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), nim::NOS::ProgressCallback());
		}
	}
	return true;
}

bool MsgRecordRetweetForm::OnClicked(ui::EventArgs* param)
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

void MsgRecordRetweetForm::OnWndSizeMax( bool max )
{
	if(btn_max_restore_)
	{
		btn_max_restore_->SetClass(max ? L"btn_wnd_restore" : L"btn_wnd_max");
	}
}
}