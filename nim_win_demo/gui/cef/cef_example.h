#pragma once
#include "gui/cef/cef_tip.h"
#include "gui/cef/cef_chatroom_list.h"

class CefExample : public ui::Box
{
public:
	CefExample();
	~CefExample();

	virtual void DoInit() override;

private:
	bool OnClicked(ui::EventArgs* arg);

private:
	CefTip*				cef_basic_example_;
	CefChatroomList*	cef_chatroom_list_;

	ui::Button*			btn_basic_example_;
	ui::Button*			btn_chatroom_list_;

	ui::Box*			cef_example_box_;
};