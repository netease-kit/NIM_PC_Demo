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

	text_->AttachMenu(nbase::Bind(&MsgBubbleText::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleText::InitInfo(const MsgData &msg)
{
	__super::InitInfo(msg);

	std::wstring str = nbase::UTF8ToUTF16(msg.msg_body);
	if (IsNetCallMsg((nim::NIMMessageType)msg.msg_type, msg.msg_attach))
	{
		GetNotifyMsg(msg.msg_attach, msg.from_account, msg.to_account, str);
		msg_.msg_body = nbase::UTF16ToUTF8(str);
	}
	else if (msg.msg_type == nim::kNIMMessageTypeCustom)
	{
		str = GetCustomMsg(msg.msg_attach);
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
		emoji::InsertToEdit(text_, str);
	}
}

ui::CSize MsgBubbleText::EstimateSize(ui::CSize szAvailable)
{
	if (msg_.msg_body.empty())
		return Box::EstimateSize(szAvailable);

	int width = szAvailable.cx - 200;

	ui::CSize sz = text_->GetNaturalSize(width, 0);
	text_->SetFixedWidth(sz.cx);
	text_->SetFixedHeight(sz.cy);

	return Box::EstimateSize(szAvailable);
}

bool MsgBubbleText::OnMenu( ui::EventArgs* arg )
{
	PopupMenu(true);
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