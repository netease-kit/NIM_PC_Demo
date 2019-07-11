#include "chatroom_form.h"
#include "gui/session/control/atme_view.h"
#include "gui/session/atlist/at_list_form.h"

using namespace ui;
using namespace nim_comp;

namespace nim_chatroom
{
//处理输入框的@消息
bool ChatroomForm::HandleAtMsg(WPARAM wParam, LPARAM lParam)
{
	if ((input_edit_ == NULL) || !input_edit_->IsFocused())
		return false;

	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::Int64ToString16(room_id_));
	if (NULL == at_list_form)
		return false;

	if (wParam == '@')
	{
		RECT rc = input_edit_->GetCaretRect();
		POINT pt = { rc.left, rc.bottom };
		::ClientToScreen(this->GetHWND(), &pt);
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
				::ClientToScreen(this->GetHWND(), &pt);
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
				::ClientToScreen(this->GetHWND(), &pt);
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
			::ClientToScreen(this->GetHWND(), &pt);

			// 如果匹配的关键字只有一个，则是第一次匹配
			// 由于响应@时ShowMemberItems函数隐藏了列表中最近发言的5个人，所以第一次匹配时，match_visible设为false不让Match函数忽略隐藏项
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

bool ChatroomForm::HandleAtMouseWheel(WPARAM wParam, LPARAM lParam)
{
	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::Int64ToString16(room_id_));
	if (at_list_form == NULL)
		return false;

	if (IsWindowVisible(at_list_form->GetHWND()) && at_list_form->GetHWND() != ::GetFocus())
	{
		int zDelta = (int)(short)HIWORD(wParam);
		return at_list_form->HandleMouseWheel(zDelta > 0);
	}

	return false;
}

void ChatroomForm::HideAtListForm()
{
	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::Int64ToString16(room_id_));
	if (at_list_form == NULL)
		return;

	if (IsWindowVisible(at_list_form->GetHWND()) && at_list_form->GetHWND() != ::GetFocus())
	{
		at_list_form->ShowWindow(false, false);
	}
}
} // namespace nim_comp
