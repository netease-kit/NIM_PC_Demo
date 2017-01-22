#include "bubble_text.h"
#include "module/emoji/emoji_info.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleText::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);
	
	msg_text_ = new Box;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_text_, L"session/text_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_text_, L"session/text_left.xml");
	bubble_box_->Add(msg_text_);

	text_ = (RichEdit*) msg_text_->FindSubControl(L"text");

	ITextServices* text_services = text_->GetTextServices();
	IRichEditOleCallbackEx* richedit_cb = new IRichEditOleCallbackEx( text_services, std::function<void()>() );
	text_services->Release();
	text_->SetOleCallback( richedit_cb );
	text_->SetEventMask(ENM_LINK);
	text_->SetAutoURLDetect(true);
	text_->AttachMenu(nbase::Bind(&MsgBubbleText::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleText::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);

	std::wstring str = nbase::UTF8ToUTF16(msg.content_);
	if (IsNetCallMsg((nim::NIMMessageType)msg.type_, msg.attach_))
	{
		GetNotifyMsg(msg.attach_, msg.sender_accid_, msg.receiver_accid_, str, sid_);
		msg_.content_ = nbase::UTF16ToUTF8(str);
	}
	else if (msg.type_ == nim::kNIMMessageTypeCustom)
	{
		str = GetCustomMsg(msg.sender_accid_, msg.attach_);
		msg_.content_ = nbase::UTF16ToUTF8(str);
	}

	SetMsgText(str);
}

void MsgBubbleText::SetMsgText( const std::wstring &str )
{
	if(str.empty())
	{
		text_->SetFixedWidth(10);
		text_->SetFixedHeight(20);
	}
	else
	{
		InsertTextToEdit(text_, str);
	}
}

ui::CSize MsgBubbleText::EstimateSize(ui::CSize szAvailable)
{
	if (msg_.content_.empty())
		return Box::EstimateSize(szAvailable);

	int width = szAvailable.cx - 200;

	ui::CSize sz = text_->GetNaturalSize(width, 0);
	text_->SetFixedWidth(sz.cx);
	text_->SetFixedHeight(sz.cy);

	return Box::EstimateSize(szAvailable);
}

bool MsgBubbleText::OnMenu( ui::EventArgs* arg )
{
	bool can_recall = !IsNetCallMsg(msg_.type_, msg_.attach_);
	bool can_retweet = msg_.type_ != nim::kNIMMessageTypeNotification;

	PopupMenu(true, can_recall, can_retweet);
	return false;
}

void MsgBubbleText::OnMenuCopy()
{
	long start_pos = 0, end_pos = 0;
	text_->GetSel( start_pos, end_pos );
	if( start_pos == end_pos )
	{
		text_->SetSelAll();
		text_->Copy();
		text_->SetSelNone();
	}
	else
	{
		text_->Copy();
	}
}

}