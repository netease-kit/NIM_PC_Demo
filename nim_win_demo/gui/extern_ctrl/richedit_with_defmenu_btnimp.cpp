#include "richedit_with_defmenu_btnimp.h"
#include "richedit_with_defmenu.h"
MenuBtnHandel::MenuBtnHandel(RichEditWithDefMenu* edit, const std::wstring& name) :
edit_(edit),
menu_name_(name),
sel_start_(0),
sel_end_(0){};
MenuBtnHandel::~MenuBtnHandel()
{
};
void MenuBtnHandel::Reset(ui::CMenuElementUI* menu_element, unsigned sel_start, unsigned sel_end)
{
	sel_start_ = sel_start;
	sel_end_ = sel_end;
	DoReset(menu_element);
}
std::wstring MenuBtnHandel::GetMenuName() const
{
	return menu_name_;
};


MenuBtnHandel_Copy::MenuBtnHandel_Copy(RichEditWithDefMenu* edit) : 
MenuBtnHandel(edit, L"MID_COPY")
{
}
void MenuBtnHandel_Copy::DoReset(ui::CMenuElementUI* menu_element)
{
	if (sel_end_ <= sel_start_)
		menu_element->SetEnabled(false);
	menu_element->AttachSelect(ToWeakCallback([this](ui::EventArgs*){
		edit_->Copy();
		return true;
	}));
}
MenuBtnHandel_Cut::MenuBtnHandel_Cut(RichEditWithDefMenu* edit) : 
MenuBtnHandel(edit, L"MID_CUT")
{
}
void MenuBtnHandel_Cut::DoReset(ui::CMenuElementUI* menu_element)
{
	if (sel_end_ <= sel_start_)
		menu_element->SetEnabled(false);
	menu_element->AttachSelect(ToWeakCallback([this](ui::EventArgs*){
		edit_->Cut();
		edit_->SetSel(sel_start_, sel_start_);
		return true;
	}));
}

MenuBtnHandel_Paste::MenuBtnHandel_Paste(RichEditWithDefMenu* edit) : 
MenuBtnHandel(edit, L"MID_PASTE")
{
}
void MenuBtnHandel_Paste::DoReset(ui::CMenuElementUI* menu_element)
{
	menu_element->SetEnabled(edit_->CanPaste() == TRUE);
	menu_element->AttachSelect(ToWeakCallback([this](ui::EventArgs*){
		edit_->PasteSpecial(CF_TEXT);
		return true;
	}));
}