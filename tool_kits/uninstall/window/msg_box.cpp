#include "msg_box.h"

using namespace ui;

MsgForm* MsgBox()
{
	MsgForm* mf = new MsgForm;
	return mf;
}

MsgForm* MsgBox( std::wstring titleID, std::wstring inforID )
{
	MsgForm* mf = new MsgForm;
	return mf->SetTitle(titleID)->SetInfor(inforID);
}

// MsgForm

const LPCTSTR MsgForm::kClassName		= L"MsgForm";
const LPCTSTR MsgForm::kCheckBoxName	= L"check_box";

MsgForm::MsgForm()
{
	icon_		= kMsgIconQuestion;
	buttons_	= 0;
	has_check_	= false;
	is_use_infor_id_ = true;
}

MsgForm::~MsgForm()
{
	
}

MsgForm* MsgForm::SetTitle( std::wstring titleID )
{
	title_ = titleID;
	return this;
}

MsgForm* MsgForm::SetIcon( MsgBoxIconType type )
{
	icon_ = type;
	return this;
}

MsgForm* MsgForm::SetInfor( std::wstring inforID, bool is_id )
{
	infor_ = inforID;
	is_use_infor_id_ = is_id;
	return this;
}

MsgForm* MsgForm::SetCheckBox( std::wstring checkID, bool checked )
{
	has_check_ = true;
	check_ = checkID;

	is_checked_ = checked;
	return this;
}

MsgForm* MsgForm::AddButton( std::wstring strID, bool gray )
{
	button_[buttons_].text_ = strID;
	button_[buttons_].gray_ = gray;
	buttons_ += 1;
	return this;
}

int MsgForm::AsynShow(HWND parent_hwnd, const MsgboxCallback& msgbox_callback)
{
	msgbox_callback_ = msgbox_callback;

	__super::Create(parent_hwnd, L"MsgBox", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, 0);
	::EnableWindow(parent_hwnd, FALSE);
	CenterWindow();
	ShowWindow(true);

	return 1;
}

int MsgForm::AsynShow( HWND parent_hwnd, const MsgboxCallback2& msgbox_callback )
{
	msgbox_callback2_ = msgbox_callback;

	__super::Create(parent_hwnd, L"MsgBox", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, 0);
	::EnableWindow(parent_hwnd, FALSE);
	CenterWindow();
	ShowWindow(true);

	return 1;
}

MsgBoxRet MsgForm::ShowModal( HWND hwnd )
{
	__super::Create(hwnd, L"MsgBox", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, 0);
	CenterWindow();
	ActiveWindow();
	MsgBoxRet ret = (MsgBoxRet)__super::ShowModal();

	return ret;
}

LRESULT MsgForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN)
	{
		if(wParam == VK_ESCAPE)
		{
			OnClicked(kMsgBtnClose);
		}
	}
	else if (uMsg == WM_DESTROY)
	{
		HWND hWndParent = GetWindowOwner(m_hWnd);
		::EnableWindow(hWndParent, TRUE);
		::SetFocus(hWndParent);
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MsgForm::AdjustContent()
{
	ASSERT(icon_ >= kMsgIconQuestion && icon_ <= kMsgIconOK);

	LPCTSTR icon_file[] = { L"question.png", L"infor.png", L"warning.png", L"error.png", L"ok.png" };

	Control* icon = (Control*) FindControl(L"icon");
	icon->SetBkImage( icon_file[icon_] );
	
	//提示信息
	{
		RichEdit* edit = (RichEdit*) FindControl(L"information");
		edit->SetText(infor_.c_str());

		int  w  = edit->GetFixedWidth();
		ui::CSize sz = edit->GetNaturalSize(w, 0);

		edit->SetFixedHeight(sz.cy);

		if (!infor2_.empty())
		{
			edit = (RichEdit*)FindControl(L"information2");
			edit->SetText(infor2_.c_str());
			w = edit->GetFixedWidth();
			sz = edit->GetNaturalSize(w, 0);
			edit->SetFixedHeight(sz.cy);
			edit->SetVisible();
		}
		if (!path_info_.empty())
		{
			Label* label = (Label*)FindControl(L"path");
			label->SetText(path_info_.c_str());
			label->SetVisible();
		}
	}
	//复选框
	{
		CheckBox* ckb = (CheckBox*) FindControl(L"checkbox");
		if(has_check_)
		{
			ckb->SetText(check_);
			ckb->Selected(is_checked_, false);
			ckb->SetVisible(true);
		}
		else
		{
			ckb->SetVisible(false);
		}
	}
}

void MsgForm::AdjustButtons()
{
	LPCTSTR btn[3]   = { L"button_1", L"button_2", L"button_3" };

	for(int i = 0; i < buttons_; i++)
	{
		Button* bt = (Button*) FindControl(btn[i]);
		bt->SetText(button_[i].text_.c_str());

		if(button_[i].gray_)
			bt->SetClass(L"btn_global_white_80x30");
	}

	for(int i = buttons_; i < 3; i++)
	{
		Button* bt = (Button*) FindControl(btn[i]);
		bt->SetVisible(false);
	}
}

std::wstring MsgForm::GetSkinFolder()
{
	return L"form";
}

std::wstring MsgForm::GetSkinFile()
{
	return L"msgbox.xml";
}

std::wstring MsgForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MsgForm::GetWindowId() const
{
	return kClassName;
}

UINT MsgForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MsgForm::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
}

void MsgForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::EventType::kEventAll, std::bind(&MsgForm::Notify, this, std::placeholders::_1));
	ui::MutiLanSupport* lang = ui::MutiLanSupport::GetInstance();
	if(lang)
	{
		title_ = lang->GetStringViaID(title_);
		if(is_use_infor_id_)
			infor_ = lang->GetStringViaID(infor_);
		check_ = lang->GetStringViaID(check_);

		for(int i = 0; i < buttons_; i++)
			button_[i].text_ = lang->GetStringViaID(button_[i].text_);
	}

	::SetWindowText(m_hWnd, title_.c_str());

	Label* title = (Label*) FindControl(L"title");
	title->SetText(title_.c_str());

	//tabkey_manager_.SetPaintManager(this);

	AdjustContent();
	AdjustButtons();
}

bool MsgForm::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if(msg->Type == EventType::kEventClick) 
	{
		if(name == L"closebtn")
			OnClicked(kMsgBtnClose);
		else if(name == L"button_1")
			OnClicked(kMsgBtn1);
		else if(name == L"button_2")
			OnClicked(kMsgBtn2);
		else if(name == L"button_3")
			OnClicked(kMsgBtn3);
	}
	else if(msg->Type == EventType::kEventReturn)
	{

	}
	//else if(msg->Type==EventType::TAB)
	//{
	//	tabkey_manager_.SetNextFocus();
	//}
	//else if(msg->Type==EventType::SETFOCUS)
	//{
	//	tabkey_manager_.SetCurrentFocus(msg->pSender);
	//}

	return true;
}

void MsgForm::OnClicked( MsgBoxRet arg_ret )
{
	int ret = 0;
	ret |= arg_ret;
	
	CheckBox* ckb = (CheckBox*) FindControl(L"checkbox");
	if(ckb != NULL)
	{
		if( ckb->IsSelected() )
			ret |= kMsgCheck;
	}

	this->Close(ret);

	if (msgbox_callback_)
	{
		StdClosure closure = std::bind(msgbox_callback_, (MsgBoxRet) ret);
		nbase::ThreadManager::PostTask(kThreadUI, closure);
	}

	if (msgbox_callback2_)
	{
		StdClosure closure = std::bind(msgbox_callback2_, ret);
		nbase::ThreadManager::PostTask(kThreadUI, closure);
	}
}
void MsgForm::ActiveWindow()
{
	if (::IsWindow(m_hWnd))
	{
		if (::IsIconic(m_hWnd))
			::ShowWindow(m_hWnd, SW_RESTORE);
		else
			::SetForegroundWindow(m_hWnd);
	}
}