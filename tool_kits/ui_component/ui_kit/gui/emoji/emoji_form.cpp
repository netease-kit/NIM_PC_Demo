#include "emoji_form.h"
#include "module/emoji/emoji_info.h"

using namespace ui;
namespace nim_comp
{
const LPCTSTR EmojiForm::kClassName = L"EmojiForm";

EmojiForm::EmojiForm()
{
	only_emoj_ = false;
	is_closing_ = false;
}

EmojiForm::~EmojiForm()
{

}

std::wstring EmojiForm::GetSkinFolder()
{
	return L"emoji";
}

std::wstring EmojiForm::GetSkinFile()
{
	return L"emoji_form.xml";
}

std::wstring EmojiForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring EmojiForm::GetWindowId() const
{
	return kClassName;
}

UINT EmojiForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void EmojiForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&EmojiForm::OnSelChanged, this, std::placeholders::_1));
	emoj_ = (TileBox*) FindControl(L"tile_emoj");

	if (only_emoj_)
	{
		FindControl(L"sticker_vector_container")->SetVisible(false);
	}

	std::vector<emoji::Emoticon> vec;
	emoji::GetEmoticon(vec);
	if( vec.empty() )
		return;

	for(std::vector<emoji::Emoticon>::const_iterator i = vec.begin(); i != vec.end(); i++)
	{
		ButtonBox* box = new ButtonBox;
		GlobalManager::FillBoxWithCache(box, L"emoji/emoji_item.xml");
		emoj_->Add(box);

		box->SetKeyboardEnabled(false);
		box->AttachClick(nbase::Bind(&EmojiForm::OnEmojiClicked, this, std::placeholders::_1));
		std::wstring tag = i->tag;
		assert(tag.size() > 2);
		box->SetToolTipText( tag.substr(1, tag.size() - 2) );

		Control* c = box->FindSubControl(L"ctrl_emoj");
		c->SetBkImage(i->path);
	}
	//AddSticker(L"ajmd", 48);
	//AddSticker(L"lt", 20);
}

LRESULT EmojiForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KILLFOCUS)
	{
		this->Close();
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT EmojiForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	if (close_cb_)
	{
		Post2UI(nbase::Bind(close_cb_));
	}
	return __super::OnClose(u, w, l, bHandled);
}

void EmojiForm::Close(UINT nRet /*= IDOK*/)
{
	is_closing_ = true;
	__super::Close(nRet);
}

void EmojiForm::ShowEmoj(POINT pt, OnSelectEmotion sel, OnSelectSticker sel_sticker, OnEmotionClose close_cb, bool only_emoj)
{
	sel_cb_ = sel;
	sel_sticker_cb_ = sel_sticker;
	close_cb_ = close_cb;
	only_emoj_ = only_emoj;

	HWND hwnd = WindowEx::Create(NULL, L"", WS_POPUP, WS_EX_TOOLWINDOW);
	if (hwnd == NULL)
		return;

	UiRect rc(pt.x, pt.y, 0, 0);
	this->SetPos(rc, SWP_NOSIZE | SWP_SHOWWINDOW, HWND_TOPMOST);
}

void EmojiForm::AddSticker(std::wstring name, int num)
{
	std::wstring sticker_id = nbase::StringPrintf(L"sticker_%s", name.c_str());
	ui::TileBox* sticker = (TileBox*)FindControl(sticker_id);
	for (int i = 1; i <= num; i++)
	{
		ButtonBox* box = new ButtonBox;
		GlobalManager::FillBoxWithCache(box, L"emoji/sticker.xml");
		sticker->Add(box);
		sticker->SetDataID(name);

		box->SetKeyboardEnabled(false);
		box->AttachClick(nbase::Bind(&EmojiForm::OnStickerClicked, this, std::placeholders::_1));

		Control* c = box->FindSubControl(L"sticker");
		std::wstring sticker_name = nbase::StringPrintf(L"%s%.3d", name.c_str(), i);

		box->SetDataID(sticker_name);
		std::wstring dir = QPath::GetAppPath() + L"res\\stickers\\" + name + L"\\";
		std::wstring path = dir + sticker_name + L".png";
		c->SetBkImage(path);
	}
}

bool EmojiForm::OnSelChanged(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	std::wstring dataid = param->pSender->GetDataID();
	int pos = name.find(L"sticker_op_");
	if (pos != -1 && !dataid.empty())
	{
		int num = 0;
		nbase::StringToInt(dataid, &num);
		if (num > 0)
		{
			AddSticker(name.substr(11), num);
			param->pSender->SetDataID(L"");
		}
	}
	return true;
}

bool EmojiForm::OnEmojiClicked( ui::EventArgs* arg )
{
	if (is_closing_)
		return false;

	std::wstring tip = arg->pSender->GetToolTipText();
	if( tip.empty() )
	{
		this->Close();
		return false;
	}

	if( sel_cb_ )
	{
		std::wstring face_tag = L"[" + tip + L"]";
		Post2UI( nbase::Bind(sel_cb_, face_tag) );
	}

	this->Close();
	return false;
}

bool EmojiForm::OnStickerClicked(ui::EventArgs* arg)
{
	if (is_closing_)
		return false;

	std::wstring id = arg->pSender->GetDataID();
	if (id.empty())
	{
		this->Close();
		return false;
	}
	std::wstring sticker_name = arg->pSender->GetParent()->GetDataID();
	if (sticker_name.empty())
	{
		this->Close();
		return false;
	}

	if (sel_sticker_cb_)
	{
		Post2UI(nbase::Bind(sel_sticker_cb_, sticker_name, id));
	}

	this->Close();
	return false;
}
}