#include "atme_view.h"
#include "module/session/force_push_manager.h"

using namespace ui;
namespace nim_comp
{
void AtMeView::InitControl()
{
	GlobalManager::FillBoxWithCache(this, L"session/at_me_view.xml");

	content_ = static_cast<Label*>(FindSubControl(L"msg_body"));
	sender_name_ = static_cast<Label*>(FindSubControl(L"sender_name"));
}

void AtMeView::AddMessage(const AtMeInfo &at_me_info)
{
	if (at_me_info.uuid.empty() || at_me_info.sender_name.empty() || at_me_info.msg_body.empty())
		return;

	at_me_info_.push_back(at_me_info);
	UpdateView();
}

void AtMeView::UpdateView()
{
	if (at_me_info_.size() == 0)
	{
		SetVisible(false);
		return;
	}
			
	AtMeInfo item = at_me_info_[at_me_info_.size() - 1];
	
	std::wstring show_text = item.sender_name;
	show_text.append(L" : ");

	sender_name_->SetText(show_text);
	content_->SetText(item.msg_body);

	SetVisible(true);
}

void AtMeView::ShowNextMessage()
{
	if (at_me_info_.size() > 0)
	{
		at_me_info_.pop_back();
	}

	UpdateView();
}

std::wstring AtMeView::GetUuid()
{
	if (at_me_info_.size() == 0)
		return L"";

	AtMeInfo item = at_me_info_[at_me_info_.size() - 1];
	return item.uuid;
}

void AtMeView::ShowLoadingTip(bool show)
{
	if (show)
	{
		content_->SetText(L"消息加载中···");
		sender_name_->SetVisible(false);
		this->SetEnabled(false);
	}
	else
	{
		sender_name_->SetVisible(true);
		this->SetEnabled(true);
	}
}

UINT AtMeView::GetAtMeCount()
{
	return at_me_info_.size();
}
}