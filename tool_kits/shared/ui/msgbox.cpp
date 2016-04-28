#include "stdafx.h"
#include "closure.h"
#include "msgbox.h"


using namespace ui;

void ShowMsgBox(HWND hwnd, const std::wstring &content, MsgboxCallback cb,
	const std::wstring &title, const std::wstring &yes, const std::wstring &no)
{
	MsgBox* msgbox = new MsgBox;
	HWND hWnd = msgbox->Create(hwnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	if(hWnd == NULL)
		return;
	msgbox->SetTitle(title);
	msgbox->SetContent(content);
	msgbox->SetButton(yes, no);
	msgbox->Show(hwnd, cb);
}


const LPCTSTR MsgBox::kClassName = L"MsgBox";

MsgBox::MsgBox()
{
}

MsgBox::~MsgBox()
{
}

std::wstring MsgBox::GetSkinFolder()
{
	return L"msgbox";
}

std::wstring MsgBox::GetSkinFile()
{
	return L"msgbox.xml";
}

ui::UILIB_RESOURCETYPE MsgBox::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring MsgBox::GetZIPFileName() const
{
	return (L"msgbox.zip");
}

std::wstring MsgBox::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MsgBox::GetWindowId() const
{
	return kClassName;
}

UINT MsgBox::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT MsgBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if(uMsg == WM_DESTROY)
	//{
	//	HWND hWndParent = GetWindowOwner(m_hWnd);
	//	if(hWndParent)
	//	{
	//		::EnableWindow(hWndParent, TRUE);
	//		::SetForegroundWindow(hWndParent);
	//		::SetFocus(hWndParent);
	//	}
	//}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MsgBox::OnEsc( BOOL &bHandled )
{
	bHandled = TRUE;
	EndMsgBox(MB_NO);
}
void MsgBox::Close(UINT nRet)
{
	// 提示框关闭之前先Enable父窗口，防止父窗口隐到后面去。
	HWND hWndParent = GetWindowOwner(m_hWnd);
	if (hWndParent)
	{
		::EnableWindow(hWndParent, TRUE);
		::SetFocus(hWndParent);
	}

	__super::Close(nRet);
}

void MsgBox::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MsgBox::OnClicked, this, std::placeholders::_1));

	title_ = (Label*) FindControl(L"title");
	content_ = (RichEdit*) FindControl(L"content");
	btn_yes_ = (Button*) FindControl(L"btn_yes");
	btn_no_ = (Button*) FindControl(L"btn_no");
}

bool MsgBox::OnClicked( ui::EventArgs* msg )
{
	std::wstring name = msg->pSender->GetName();
	if(name == L"btn_yes")
	{
		EndMsgBox(MB_YES);
	}
	else if(name == L"btn_no")
	{
		EndMsgBox(MB_NO);
	}
	else if(name == L"closebtn")
	{
		EndMsgBox(MB_NO);
	}
	return true;
}

void MsgBox::SetTitle( const std::wstring &str )
{
	title_->SetText(str);
}

void MsgBox::SetContent( const std::wstring &str )
{
	content_->SetText(str);

	int width = content_->GetFixedWidth();

	ui::CSize sz = content_->GetNaturalSize(width, 0);
	content_->SetFixedHeight(sz.cy);
}

void MsgBox::SetButton( const std::wstring &yes, const std::wstring &no )
{
	if( !yes.empty() )
	{
		btn_yes_->SetText(yes);
		btn_yes_->SetVisible(true);
		btn_yes_->SetFocus();
	}
	else
	{
		btn_yes_->SetVisible(false);
	}

	if( !no.empty() )
	{
		btn_no_->SetText(no);
		btn_no_->SetVisible(true);
		if( yes.empty() )
			btn_no_->SetFocus();
	}
	else
	{
		btn_no_->SetVisible(false);
	}
}

void MsgBox::Show( HWND hwnd, MsgboxCallback cb )
{
	msgbox_callback_ = cb;

	::EnableWindow(hwnd, FALSE);
	CenterWindow();
	ShowWindow(true);
}

void MsgBox::EndMsgBox( MsgBoxRet ret )
{
	this->Close(0);

	if( msgbox_callback_ )
	{
		Post2UI( nbase::Bind(msgbox_callback_, ret) );
	}
}
