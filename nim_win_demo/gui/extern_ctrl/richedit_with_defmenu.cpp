#include "richedit_with_defmenu.h"
#include "richedit_with_defmenu_btnimp.h"
const std::wstring RichEditWithDefMenu::kMenuUIConfigPath = L"default_edit_menu.xml";
const std::wstring RichEditWithDefMenu::kMenuBTNName_Cut = L"MID_CUT";//¼ôÇÐ
const std::wstring RichEditWithDefMenu::kMenuBTNName_Copy = L"MID_COPY";//¸´ÖÆ
const std::wstring RichEditWithDefMenu::kMenuBTNName_Paste = L"MID_PASTE";//Õ³Ìù
RichEditWithDefMenu::RichEditWithDefMenu()
{ 
	RegisterMenuButtonHandel();
}
RichEditWithDefMenu::~RichEditWithDefMenu()
{
}
void RichEditWithDefMenu::DoInit()
{
	if (m_bInited)
		return;
	__super::DoInit();	
	AttachMenu(nbase::Bind(&RichEditWithDefMenu::OnMenu,this,std::placeholders::_1));
}
bool RichEditWithDefMenu::OnMenu(ui::EventArgs* event)
{
	if (!IsEnabled() || show_menu_list_.empty())
		return true;
	{
		ShowDefaultMenu(event);
	}
	return true;
}
void RichEditWithDefMenu::ShowDefaultMenu(ui::EventArgs* event)
{	
	auto pMenu = CreateMenuWnd(event->ptMouse);
	if (pMenu != nullptr)
	{
		SetNoSelOnKillFocus(false);
		pMenu->AttachWindowClose(nbase::Bind(&RichEditWithDefMenu::OnMenuWndClose, this, std::placeholders::_1));
		pMenu->Show();
		HideSelection(false, false);
	}	
}
ui::CMenuWnd* RichEditWithDefMenu::CreateMenuWnd(const POINT& mouse_pos)
{
	long start_char, end_char;
	POINT pt = mouse_pos;
	::ClientToScreen(GetWindow()->GetHWND(), &pt);
	GetSel(start_char, end_char);
	int char_pos = CharFromPos(mouse_pos);
	if (char_pos > end_char || char_pos < start_char)
	{
		SetSel(char_pos, char_pos);
		start_char = end_char = char_pos;
	}
	ui::CMenuWnd* pMenu = new ui::CMenuWnd();	
	ui::STRINGorID xml(kMenuUIConfigPath.c_str());
	pMenu->Init(xml, _T("xml"), pt);
	auto menu_list = dynamic_cast<ui::ListBox*>(pMenu->FindControl(L"menu_list"));
	if (menu_list != nullptr)
	{
		for (int index = 0; index < menu_list->GetCount(); index++)
		{
			auto menu_element = dynamic_cast<ui::CMenuElementUI*>(menu_list->GetItemAt(index));
			if (menu_element == nullptr)
				continue;
			auto&& element_name = menu_element->GetName();
			if (IsMenuButtonShow(element_name))
			{
				menu_element->SetVisible(true);
				auto it = menu_button_handel_.find(element_name);
				if (it != menu_button_handel_.end())
					it->second->Reset(menu_element, start_char, end_char);
				else
					menu_element->SetVisible(false);
			}
			else
			{
				menu_element->SetVisible(false);
			}
		}
	}
	return pMenu;
}
bool RichEditWithDefMenu::OnMenuWndClose(ui::EventArgs* event)
{
	SetNoSelOnKillFocus(true);
	return true;
}
void RichEditWithDefMenu::RegisterMenuButtonHandel()
{
	menu_button_handel_.insert(std::make_pair(kMenuBTNName_Copy, std::shared_ptr<MenuBtnHandel>(new MenuBtnHandel_Copy(this))));
	menu_button_handel_.insert(std::make_pair(kMenuBTNName_Cut, std::shared_ptr<MenuBtnHandel>(new MenuBtnHandel_Cut(this))));
	menu_button_handel_.insert(std::make_pair(kMenuBTNName_Paste, std::shared_ptr<MenuBtnHandel>(new MenuBtnHandel_Paste(this))));
}
bool RichEditWithDefMenu::IsMenuButtonShow(const std::wstring& name)
{
	return show_menu_list_.find(name) != show_menu_list_.end();
}
void RichEditWithDefMenu::SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue)
{
	if (pstrName == _T("defmenu")) {
		SetContextMenuUsed(true);	
		if (pstrValue.find(kMenuBTNName_Cut) != std::wstring::npos)
			AddShowMenuButton(kMenuBTNName_Cut);
		if (pstrValue.find(kMenuBTNName_Copy) != std::wstring::npos)
			AddShowMenuButton(kMenuBTNName_Copy);
		if (pstrValue.find(kMenuBTNName_Paste) != std::wstring::npos)
			AddShowMenuButton(kMenuBTNName_Paste);
	}
	else
	{
		__super::SetAttribute(pstrName, pstrValue);
	}
}
void RichEditWithDefMenu::AddShowMenuButton(const std::wstring& name)
{
	if (menu_button_handel_.find(name) != menu_button_handel_.end())
		show_menu_list_[name] = 0;
}