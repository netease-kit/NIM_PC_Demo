#include "emoji_form.h"
#include "module/emoji/emoji_info.h"

using namespace ui;
namespace nim_comp
{
const LPCTSTR EmojiForm::kClassName = L"EmojiForm";

EmojiForm::EmojiForm()
{

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

ui::UILIB_RESOURCETYPE EmojiForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring EmojiForm::GetZIPFileName() const
{
	return L"emoji_form.zip";
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

LRESULT EmojiForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	if( cls_cb_ )
	{
		Post2UI( nbase::Bind(cls_cb_) );
	}
	return __super::OnClose(u, w, l, bHandled);
}

LRESULT EmojiForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_KILLFOCUS )
	{
		this->Close();
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void EmojiForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&EmojiForm::OnSelChanged, this, std::placeholders::_1));
	emoj_ = (TileBox*) FindControl(L"tile_emoj");

	std::vector<emoji::Emoticon> vec;
	emoji::GetEmoticon(vec);
	if( vec.empty() )
		return;

	for(std::vector<emoji::Emoticon>::const_iterator i = vec.begin(); i != vec.end(); i++)
	{
		ButtonBox* box = new ButtonBox;
		GlobalManager::FillBoxWithCache(box, L"emoji/emoji_item.xml");
		emoj_->Add(box);

		box->AttachClick(nbase::Bind(&EmojiForm::OnClicked, this, std::placeholders::_1));
		std::wstring tag = i->tag;
		assert(tag.size() > 2);
		box->SetToolTipText( tag.substr(1, tag.size() - 2) );

		Control* c = box->FindSubControl(L"ctrl_emoj");
		c->SetBkImage(i->path);
	}
	//AddSticker(L"ajmd", 48);
	//AddSticker(L"lt", 20);
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

		box->AttachClick(nbase::Bind(&EmojiForm::OnClicked2, this, std::placeholders::_1));

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

void EmojiForm::ShowEmoj(POINT pt, OnSelectEmotion sel, OnSelectEmotion2 sel2, OnEmotionClose cls)
{
	sel_cb_ = sel;
	sel2_cb_ = sel2;
	cls_cb_ = cls;

	HWND hwnd = WindowEx::Create(NULL, L"", WS_POPUP, WS_EX_TOOLWINDOW);
	if(hwnd == NULL)
		return;

	UiRect rc( pt.x, pt.y, 0, 0 );
	this->SetPos( rc, SWP_NOSIZE | SWP_SHOWWINDOW, HWND_TOPMOST );
}

bool EmojiForm::OnClicked( ui::EventArgs* arg )
{
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
bool EmojiForm::OnClicked2(ui::EventArgs* arg)
{
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

	if (sel2_cb_)
	{
		Post2UI(nbase::Bind(sel2_cb_, sticker_name, id));
	}

	this->Close();
	return false;
}
}