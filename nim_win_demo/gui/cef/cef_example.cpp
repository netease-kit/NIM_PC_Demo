#include "stdafx.h"
#include "cef_example.h"


CefExample::CefExample()
{

}

CefExample::~CefExample()
{
	
}

void CefExample::DoInit()
{
	AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefExample::OnClicked, this, std::placeholders::_1));

	btn_basic_example_ = dynamic_cast<ui::Button*>(FindSubControl(L"btn_basic_example"));
	btn_chatroom_list_ = dynamic_cast<ui::Button*>(FindSubControl(L"btn_chatroom_list"));
	cef_example_box_ = dynamic_cast<ui::Box*>(FindSubControl(L"cef_example_box"));

	cef_basic_example_ = new CefTip;
	ui::GlobalManager::FillBoxWithCache(cef_basic_example_, L"ceftest/cef_test_tip.xml", &CefTip::CreateControl);

	cef_chatroom_list_ = new CefChatroomList;
	ui::GlobalManager::FillBoxWithCache(cef_chatroom_list_, L"ceftest/cef_chatroom_list.xml", &CefChatroomList::CreateControl);

	cef_chatroom_list_->SetVisible(false);

	cef_example_box_->Add(cef_basic_example_);
	cef_example_box_->Add(cef_chatroom_list_);
}

bool CefExample::OnClicked(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();

	if (name == L"btn_basic_example")
	{
		btn_basic_example_->SetBkColor(L"white");
		btn_basic_example_->SetStateTextColor(ui::kControlStateNormal, L"darkcolor");

		btn_chatroom_list_->SetBkColor(L"bk_main_wnd_title");
		btn_chatroom_list_->SetStateTextColor(ui::kControlStateNormal, L"white");

		cef_basic_example_->SetVisible(true);
		cef_chatroom_list_->SetVisible(false);
	}
	else if (name == L"btn_chatroom_list")
	{
		btn_chatroom_list_->SetBkColor(L"white");
		btn_chatroom_list_->SetStateTextColor(ui::kControlStateNormal, L"darkcolor");

		btn_basic_example_->SetBkColor(L"bk_main_wnd_title");
		btn_basic_example_->SetStateTextColor(ui::kControlStateNormal, L"white");

		cef_chatroom_list_->SetVisible(true);
		cef_basic_example_->SetVisible(false);
	}

	return true;
}
