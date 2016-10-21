#include "merge_item.h"

using namespace ui;
namespace nim_comp
{
MergeItem::MergeItem()
{
	unread_count_ = 0;
}

void MergeItem::InitControl(std::string session_id, bool is_team)
{
	this->AttachMenu(nbase::Bind(&MergeItem::OnItemMenu, this, std::placeholders::_1));
	this->AttachMouseEnter(nbase::Bind(&MergeItem::OnMouseEnter, this, std::placeholders::_1));
	this->AttachMouseLeave(nbase::Bind(&MergeItem::OnMouseLeave, this, std::placeholders::_1));

	session_icon_ = (Button*) this->FindSubControl(L"merge_item_icon");
	session_name_ = (Label*) this->FindSubControl(L"merge_item_name");
	button_close_ = (Button*) this->FindSubControl(L"merge_item_close");
	label_unread_ = (Label*) this->FindSubControl(L"merge_item_unread");

	this->SetUTF8Name(session_id);

	if (is_team)
	{
		SetTitle(TeamService::GetInstance()->GetTeamName(session_id));
		SetIcon(PhotoService::GetInstance()->GetTeamPhoto(session_id, true));
	}
	else
	{
		SetTitle(UserService::GetInstance()->GetUserName(session_id));
		SetIcon(PhotoService::GetInstance()->GetUserPhoto(session_id));
	}
}

void MergeItem::SetTitle(std::wstring name)
{
	this->SetToolTipText(name);
	session_name_->SetText(name);
}

void MergeItem::SetIcon(std::wstring icon)
{
	session_icon_->SetBkImage(icon);
}

void MergeItem::SetUnread(int unread)
{
	unread_count_ = unread;

	if (unread_count_ > 0)
	{
		if (unread_count_ < 100)
			label_unread_->SetText(nbase::StringPrintf(L"%d", unread_count_));
		else
			label_unread_->SetText(L"99+");

		label_unread_->SetVisible(true);
	}
	else
	{
		label_unread_->SetVisible(false);
	}
}

bool MergeItem::OnMouseEnter(ui::EventArgs* msg)
{
	button_close_->SetVisible(true);

	label_unread_->SetVisible(false);
	return true;
}

bool MergeItem::OnMouseLeave(ui::EventArgs* msg)
{
	if (!m_rcItem.IsPointIn(msg->ptMouse))
	{
		button_close_->SetVisible(false);

		if (unread_count_ > 0)
			label_unread_->SetVisible(true);
	}
	
	return false;
}

bool MergeItem::OnItemMenu(ui::EventArgs* arg)
{
	POINT point;
	::GetCursorPos(&point);
	PopupItemMenu(point);
	return true;
}

void MergeItem::PopupItemMenu(POINT point)
{

}

bool MergeItem::MergeItemMenuItemClick(ui::EventArgs* param)
{

	return true;
}

}