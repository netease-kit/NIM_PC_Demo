#include "stdafx.h"
#include "chatroom_list.h"

namespace nim_comp {

const LPCTSTR ChatroomList::kClassName = L"ChatroomList";

std::wstring nim_comp::ChatroomList::GetSkinFolder()
{
	return L"chatroom_list";
}

std::wstring nim_comp::ChatroomList::GetSkinFile()
{
	return L"chatroom_list.xml";
}

std::wstring nim_comp::ChatroomList::GetWindowClassName() const
{
	return kClassName;
}

std::wstring nim_comp::ChatroomList::GetWindowId() const
{
	return kClassName;
}

ui::Control* nim_comp::ChatroomList::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefControl")
	{
		return new CefControl;
	}

	return NULL;
}

void nim_comp::ChatroomList::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&ChatroomList::OnClicked, this, std::placeholders::_1));

	// 初始化 Cef Control
	cef_control_ = static_cast<CefControl*>(FindControl(L"cef_control"));
	cef_control_->AttachLoadEnd(nbase::Bind(&ChatroomList::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachAfterCreated(nbase::Bind(&ChatroomList::OnAfterCreated, this, std::placeholders::_1));
	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef_themes/chatroom_list/chatroom_list.html";
	cef_control_->LoadURL(html_path);
}

bool nim_comp::ChatroomList::OnClicked(ui::EventArgs* arg)
{
	return true;
}

void nim_comp::ChatroomList::OnLoadEnd(int httpStatusCode)
{

}

void nim_comp::ChatroomList::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{

}

}