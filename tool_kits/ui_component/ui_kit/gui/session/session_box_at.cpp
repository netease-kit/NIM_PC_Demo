#include "session_box.h"
#include "session_form.h"
#include "module/session/force_push_manager.h"
#include "gui/session/control/atme_view.h"
#include "gui/session/atlist/at_list_form.h"

using namespace ui;
namespace
{
	const int kMaxRecordLastPeople = 5; //最多记录5个最近发言的人
}

namespace nim_comp
{

void SessionBox::InitAtMeView(std::vector<ForcePushManager::ForcePushInfo> &infos)
{
	for (auto& i : infos)
	{
		std::string sender_name = GetShowName(i.sender_id);
		if (!sender_name.empty())
		{
			AtMeView::AtMeInfo info;
			info.msg_body = nbase::UTF8ToUTF16(i.msg_body);
			info.uuid = nbase::UTF8ToUTF16(i.uuid);
			info.sender_name = nbase::UTF8ToUTF16(sender_name);
			atme_view_->AddMessage(info);
		}
	}
}

void SessionBox::AddAtMessage(const nim::IMMessage &msg)
{
	if (ForcePushManager::GetInstance()->IsAtMeMsg(msg))
	{
		std::string sender_name = GetShowName(msg.sender_accid_);
		if (!sender_name.empty())
		{
			AtMeView::AtMeInfo info;
			info.msg_body = nbase::UTF8ToUTF16(msg.content_);
			info.uuid = nbase::UTF8ToUTF16(msg.client_msg_id_);
			info.sender_name = nbase::UTF8ToUTF16(sender_name);
			atme_view_->AddMessage(info);
		}
	}
}

void SessionBox::ScrollToControl(const Control *control)
{
	if (scroll_weak_flag_.HasUsed())
		return;

	if (msg_list_)
	{
		ui::CSize sz = msg_list_->GetScrollPos();

		StdClosure callback = nbase::Bind([sz, control, this]()
		{
			int top_pos = control->GetPos().top - msg_list_->GetPos().top;
			int bottom_pos = control->GetPos().bottom - msg_list_->GetPos().top;

			if (top_pos < sz.cy)
			{
				// 如果目标控件在上面
				scroll_top_ = true;
				scroll_step_ = abs((sz.cy - top_pos) / 10);
			}
			else if (bottom_pos > sz.cy + msg_list_->GetFixedHeight())
			{
				// 如果目标控件在下面
				scroll_top_ = false;
				scroll_step_ = abs((bottom_pos - sz.cy) / 10);
			}

			if (scroll_step_ < 100)
				scroll_step_ = 100;

			auto playCallback = nbase::Bind(&SessionBox::ScrollToControlCallback, this, control);
			TimerManager::GetInstance()->AddCancelableTimer(scroll_weak_flag_.GetWeakFlag(), playCallback, 50, TimerManager::REPEAT_FOREVER);
		}
		);

		nbase::ThreadManager::PostTask(ToWeakCallback(callback));
	}

}

void SessionBox::ScrollToControlCallback(const Control *control)
{
	assert(scroll_step_ >= 100);

	// 如果没有出现滚动条，那么目标控件就在当前的显示范围内，不需要滚动了
	if (!msg_list_->IsVScrollBarValid())
	{
		scroll_weak_flag_.Cancel();
		atme_view_->ShowLoadingTip(false);
		atme_view_->ShowNextMessage();
		return;
	}

	// VListBox中元素的位置减去VListBox本身的位置后的值是在VListBox中的滚动范围
	int top_pos = control->GetPos().top - msg_list_->GetPos().top;
	int bottom_pos = control->GetPos().bottom - msg_list_->GetPos().top;
	ui::CSize sz = msg_list_->GetScrollPos();

	if (scroll_top_ && top_pos < sz.cy)
	{
		sz.cy -= scroll_step_;
		if (sz.cy < top_pos)
			sz.cy = top_pos;

		msg_list_->SetScrollPosY(sz.cy);
	}
	else if (!scroll_top_ && bottom_pos > sz.cy + msg_list_->GetHeight())
	{
		sz.cy += scroll_step_;
		if (sz.cy > bottom_pos)
			sz.cy = bottom_pos;

		msg_list_->SetScrollPosY(sz.cy);
	}
	else
	{
		scroll_weak_flag_.Cancel();
		atme_view_->ShowLoadingTip(false);
		atme_view_->ShowNextMessage();
	}
}

std::string SessionBox::GetShowName(const std::string& uid)
{
	std::wstring alias = UserService::GetInstance()->GetFriendAlias(uid);
	if (!alias.empty())
		return nbase::UTF16ToUTF8(alias);

	std::string card_name;
	auto i = team_member_info_list_.find(uid);
	if (i != team_member_info_list_.end())
	{
		card_name = i->second.GetNick();
	}
	if (!card_name.empty())
		return card_name;

	std::wstring nick = UserService::GetInstance()->GetUserName(uid, false);
	return nbase::UTF16ToUTF8(nick);
}

bool SessionBox::OnAtMeViewClick(ui::EventArgs* param)
{
	if (param->pSender->GetName() != L"btn_at_me")
		return true;

	if (scroll_weak_flag_.HasUsed())
		return true;

	if (atme_view_->GetAtMeCount() > 0)
	{
		MsgBubbleItem *cell = NULL;
		// 先查找当前窗体内是否有需要的消息
		std::wstring uuid = atme_view_->GetUuid();
		for (int i = msg_list_->GetCount() - 1; i >= 0; --i)
		{
			if (uuid == msg_list_->GetItemAt(i)->GetName())
			{
				cell = static_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
				break;
			}
		}

		if (cell && cell->GetMsgType() == nim::kNIMMessageTypeText)
		{
			ScrollToControl(cell);
		}
	}
	else
	{
		atme_view_->SetVisible(false);
	}

	return true;
}

bool SessionBox::OnCloseAtMeView(ui::EventArgs* param)
{
	if (!atme_view_)
		return false;

	atme_view_->ShowNextMessage();
	atme_view_->SetVisible(false);

	return true;
}

//处理输入框的@消息
bool SessionBox::HandleAtMsg(WPARAM wParam, LPARAM lParam)
{
	if ((input_edit_ == NULL) || !input_edit_->IsFocused())
		return false;

	if (session_type_ != nim::kNIMSessionTypeTeam)
		return false;

	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::UTF8ToUTF16(session_id_));
	if (NULL == at_list_form)
		return false;

	if (wParam == '@')
	{
		RECT rc = input_edit_->GetCaretRect();
		POINT pt = { rc.left, rc.bottom };
		::ClientToScreen(this->GetWindow()->GetHWND(), &pt);

		std::list<std::string> uid_last_five;
		GetLastFiveSender(uid_last_five);
		at_list_form->ShowAllItems(uid_last_five);
		at_list_form->SetShowPos(pt);

		return false;
	}
	else if (wParam == VK_BACK)
	{
		// 获取当前光标之前的所有文本
		long start = 0;
		long end = 0;
		input_edit_->GetSel(start, end);
		std::wstring sel_str = input_edit_->GetTextRange(0, end);
		size_t pos = sel_str.rfind(L"@");

		if (pos == std::wstring::npos || end == 0)
		{
			at_list_form->ShowWindow(false, false);
			return false;
		}

		std::wstring at_str = input_edit_->GetTextRange(pos + 1, end);
		if (!at_str.empty())
		{
			if (at_str.length() == 1)
			{
				// 如果只有一个字符，则删除后就剩下了@，此时显示所有@项
				ui::CPoint pt = input_edit_->PosFromChar(end - 1);
				pt.y = input_edit_->GetCaretRect().bottom;
				::ClientToScreen(this->GetWindow()->GetHWND(), &pt);

				std::list<std::string> uid_last_five;
				GetLastFiveSender(uid_last_five);
				at_list_form->ShowAllItems(uid_last_five);
				at_list_form->SetShowPos(pt);
				return false;
			}

			wchar_t last_char = at_str.at(at_str.length() - 1);
			std::wstring at_name = at_str.substr(0, at_str.length() - 1);

			if (last_char == L' ')
			{
				//如果当前要删的是空格，而且空格前包含了完整的@某人的信息，就删除整条信息，并且不让消息继续传递			
				if (uid_at_someone_.find(nbase::UTF16ToUTF8(at_name)) != uid_at_someone_.end())
				{
					input_edit_->SetSel(pos, end);
					input_edit_->ReplaceSel(L"", true);
					at_list_form->ShowWindow(false, false);
					return true;
				}
			}
			else
			{
				wchar_t last_second_char = at_str.at(at_str.length() - 2);
				if (last_second_char == L' ')
				{
					std::wstring at_name = at_str.substr(0, at_str.length() - 2);
					//如果当前要删的是空格的后一位，而且空格前包含了完整的@某人的信息，就不继续匹配而隐藏@列表，直接返回		
					if (uid_at_someone_.find(nbase::UTF16ToUTF8(at_name)) != uid_at_someone_.end())
					{
						return false;
					}
				}

				ui::CPoint pt = input_edit_->PosFromChar(end - 1);
				pt.y = input_edit_->GetCaretRect().bottom;
				::ClientToScreen(this->GetWindow()->GetHWND(), &pt);
				if (at_list_form->Match(at_name, false))
				{
					at_list_form->SetShowPos(pt);
				}
				else
				{
					at_list_form->ShowWindow(false, false);
				}

			}
		}
		else
		{
			at_list_form->ShowWindow(false, false);
		}

		return false;
	}
	else if (wParam == VK_RETURN)
	{
		return at_list_form->HandleKeyEnter();
	}
	else if (wParam == VK_UP)
	{
		return at_list_form->HandleKeyUp();
	}
	else if (wParam == VK_DOWN)
	{
		return at_list_form->HandleKeyDown();
	}
	else
	{
		// 获取当前光标之前的所有文本
		long start = 0;
		long end = 0;
		input_edit_->GetSel(start, end);
		std::wstring sel_str = input_edit_->GetTextRange(0, end);
		size_t pos = sel_str.rfind(L"@");

		if (pos == std::wstring::npos || end == 0)
		{
			at_list_form->ShowWindow(false, false);
			return false;
		}

		std::wstring at_str = input_edit_->GetTextRange(pos + 1, end);
		if (!at_str.empty())
		{
			RECT rc = input_edit_->GetCaretRect();
			POINT pt = { rc.left, rc.bottom };
			::ClientToScreen(this->GetWindow()->GetHWND(), &pt);

			// 如果匹配的关键字只有一个，则是第一次匹配
			// 由于响应@时ShowAllItems函数隐藏了列表中最近发言的5个人，所以第一次匹配时，match_visible设为false不让Match函数忽略隐藏项
			bool match_visible = at_str.length() > 1;
			if (at_list_form->Match(at_str, match_visible))
			{
				at_list_form->SetShowPos(pt);
			}
			else
			{
				at_list_form->ShowWindow(false, false);
			}
		}
	}

	return false;
}

bool SessionBox::HandleAtMouseWheel(WPARAM wParam, LPARAM lParam)
{
	if (session_type_ != nim::kNIMSessionTypeTeam)
		return false;

	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::UTF8ToUTF16(session_id_));
	if (at_list_form == NULL)
		return false;

	if (IsWindowVisible(at_list_form->GetHWND()) && at_list_form->GetHWND() != ::GetFocus())
	{
		int zDelta = (int)(short)HIWORD(wParam);
		return at_list_form->HandleMouseWheel(zDelta > 0);
	}

	return false;
}

void SessionBox::HideAtListForm()
{
	if (session_type_ != nim::kNIMSessionTypeTeam)
		return;

	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::UTF8ToUTF16(session_id_));
	if (at_list_form == NULL)
		return;

	if (IsWindowVisible(at_list_form->GetHWND()) && at_list_form->GetHWND() != ::GetFocus())
	{
		at_list_form->ShowWindow(false, false);
	}
}

void SessionBox::OnSelectAtItemCallback(const std::string& uid)
{
	if (uid.empty())
		return;

	if (!LoginManager::GetInstance()->IsEqual(uid))
	{
		//找到当前光标之前的最近一个@的位置
		long start = 0;
		long end = 0;
		input_edit_->GetSel(start, end);
		std::wstring sel_str = input_edit_->GetTextRange(0, end);
		size_t pos = sel_str.rfind(L"@");

		if (pos == std::wstring::npos || end == 0)
			return;
		
		std::string show_name;
		auto i = team_member_info_list_.find(uid);
		if (i != team_member_info_list_.end())
		{
			show_name = i->second.GetNick();
		}
		if (show_name.empty())
		{
			show_name = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(uid, false));
		}

		uid_at_someone_[show_name] = uid;

		std::wstring show_text = nbase::UTF8ToUTF16(show_name);
		show_text.append(L" ");

		input_edit_->SetSel(pos + 1, end);
		input_edit_->ReplaceSel(show_text, false);
	}
}

void SessionBox::GetLastFiveSender(std::list<std::string> &uid_last_five)
{
	// 更新最近发消息的成员的链表
	if (session_type_ != nim::kNIMSessionTypeTeam)
		return;

	if (msg_list_)
	{
		for (int i = msg_list_->GetCount(); i >= 0; i--)
		{
			MsgBubbleItem* cell = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
			if (cell)
			{
				std::string sender_id = cell->GetSenderId();
				if (sender_id.empty() || LoginManager::GetInstance()->IsEqual(sender_id))
					continue;;

				auto it = std::find(uid_last_five.begin(), uid_last_five.end(), sender_id);
				if (it == uid_last_five.end())
				{
					uid_last_five.push_back(sender_id);
					if (uid_last_five.size() >= kMaxRecordLastPeople)
						break;
				}
			}
		}
	}
}

} // namespace nim_comp
