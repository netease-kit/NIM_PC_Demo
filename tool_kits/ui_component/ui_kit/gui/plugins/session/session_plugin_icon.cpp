#include "stdafx.h"
#include "session_plugin_icon.h"
#include "export\nim_ui_session_list_manager.h"
#include "gui\main\control\custom_button_box.h"
#include "module\session\session_manager.h"
using namespace nim_comp;
SessionPluginIcon::SessionPluginIcon() :
real_option_(nullptr), label_unread_(nullptr), box_unread_(nullptr)
{

}
SessionPluginIcon::~SessionPluginIcon()
{

}
ui::Control* SessionPluginIcon::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CustomButtonBox")
	{
		return new nim_comp::CustomButtonBox;
	}

	return NULL;
}
void SessionPluginIcon::DoInit()
{
	real_option_ = dynamic_cast<ui::OptionBox*>(FindSubControl(L"op_ctrl"));
	label_unread_ = dynamic_cast<ui::Label*>(FindSubControl(L"label_unread"));
	box_unread_ = dynamic_cast<nim_comp::CustomButtonBox*>(FindSubControl(L"box_unread"));	
	box_unread_->AttachClick([this](ui::EventArgs* param){
		nim::Session::SetAllUnreadCountZeroAsync([this](nim::NIMResCode rescode, const nim::SessionData& data, int total_unread_counts){
			if (rescode != nim::kNIMResSuccess)
			{
				QLOG_APP(L"SetAllUnreadCountZeroAsync::OnChangeCallback Error! {0}, uid:{1}, unread_count: {2}") << rescode << data.id_ << total_unread_counts;
				assert(0);
				return;
			}
			if (data.command_ == nim::kNIMSessionCommandUpdate)
			{
				nim_comp::SessionManager::GetInstance()->ResetUnread(data.id_);
			}
		});
		nim::MsgLog::ReadAllAsync(nim::MsgLog::DBFunctionCallback());
		return true;
	});
	AttachSelect([this](ui::EventArgs* param){
		real_option_->Selected(true, false);
		return true;
	});
	AttachUnSelect([this](ui::EventArgs* param){
		real_option_->Selected(false, false);
		return true;
	});
}
ui::OptionBox* SessionPluginIcon::GetPluginIconAction()
{
	return real_option_;
}
void SessionPluginIcon::OnSessionListAttached()
{
	nim_ui::SessionListManager::GetInstance()->RegUnreadCountChange([this](int unread_count){	
		if (unread_count > 0)
		{
			label_unread_->SetText(unread_count > 99 ? nbase::StringPrintf(L"99+") : nbase::StringPrintf(L"%d", unread_count));
			box_unread_->SetVisible(true);
		}
		else
		{
			box_unread_->SetVisible(false);
		}
	});
}