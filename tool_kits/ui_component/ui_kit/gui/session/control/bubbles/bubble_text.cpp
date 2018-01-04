#include "bubble_text.h"
#include "module/emoji/emoji_info.h"

using namespace ui;

namespace nim_comp
{
	MsgBubbleText::MsgBubbleText() :
		msg_text_(nullptr), text_(nullptr), text_has_emoji_(false)
	{

	}
	MsgBubbleText::~MsgBubbleText()
	{

	}
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
	text_->AttachGetNaturalSize([this](LONG width, LONG height, CSize& sz)->bool{
		if (text_has_emoji_)
			return false;
		ITextServices *text_service = text_->GetTextServices();
		if (text_service == nullptr)
			return false;
		LONG lWidth = width;
		LONG lHeight = height;
		SIZEL szExtent = { -1, -1 };
		std::wstring text(std::move(text_->GetText()));
		std::wstring text_temp(text);
		ui::StringHelper::ReplaceAll(L" ", L"!", text_temp);
		text_->SetText(text_temp);
		text_service->TxGetNaturalSize(
			DVASPECT_CONTENT,
			GetWindow()->GetPaintDC(),
			NULL,
			NULL,
			TXTNS_FITTOCONTENT,
			&szExtent,
			&lWidth,
			&lHeight);
		text_->SetText(text);
		sz.cx = (int)lWidth;
		sz.cy = (int)lHeight;
		return true;		
	});
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
		text_has_emoji_ = InsertTextToEdit(text_, str) > 0;
	}
}

ui::CSize MsgBubbleText::EstimateSize(ui::CSize szAvailable)
{
	if (msg_.content_.empty())
		return Box::EstimateSize(szAvailable);

	int sub_width = 200;
	int width = szAvailable.cx - DpiManager::GetInstance()->ScaleInt(sub_width);

	ui::CSize sz = text_->GetNaturalSize(width, 0);
	text_->SetFixedWidth(sz.cx, true, false);
	text_->SetFixedHeight(sz.cy, false);

	return Box::EstimateSize(szAvailable);
}

bool MsgBubbleText::OnMenu( ui::EventArgs* arg )
{
	bool can_recall = !IsNetCallMsg(msg_.type_, msg_.attach_);
	bool can_retweet = msg_.type_ != nim::kNIMMessageTypeNotification && msg_.type_ != nim::kNIMMessageTypeRobot;
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