#include "atme_view.h"

using namespace ui;
namespace nim_comp
{
void AtMeView::InitControl()
{
	GlobalManager::FillBoxWithCache(this, L"session/at_me_view.xml");

	content_ = static_cast<Label*>(FindSubControl(L"msg_body"));
	sender_name_ = static_cast<Label*>(FindSubControl(L"sender_name"));
}

void AtMeView::AddMessage(const std::string &at_me_json)
{
	assert(!at_me_json.empty());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(at_me_json, root))
		return;

	if (root.size() == 0)
		return;

	for (UINT i = 0; i < root.size(); ++i)
	{
		AtMeInfo item;
		Json::Value value = root[i];

		item.msg_body = nbase::UTF8ToUTF16(value["msgbody"].asString());
		item.sender_name = nbase::UTF8ToUTF16(value["sender"].asString());
		item.uuid = nbase::UTF8ToUTF16(value["uuid"].asString());

		if (item.uuid.empty() || item.sender_name.empty() || item.msg_body.empty())
			continue;;

		at_me_info_.push_back(item);
	}

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