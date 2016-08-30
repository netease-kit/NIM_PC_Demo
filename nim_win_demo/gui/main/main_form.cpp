#include "main_form.h"

using namespace ui;

bool MainForm::SearchEditChange(ui::EventArgs* param)
{
	UTF8String search_key = search_edit_->GetUTF8Text();
	bool has_serch_key = !search_key.empty();
	btn_clear_input_->SetVisible(has_serch_key);
	search_result_list_->SetVisible(has_serch_key);
	FindControl(L"no_search_result_tip")->SetVisible(has_serch_key);
	if (has_serch_key)
	{
		nim_ui::ContactsListManager::GetInstance()->FillSearchResultList(search_result_list_, search_key);
		FindControl(L"no_search_result_tip")->SetVisible(search_result_list_->GetCount() == 0);
	}
	return true;
}

bool MainForm::OnClearInputBtnClicked(ui::EventArgs* param)
{
	search_edit_->SetText(L"");
	return true;
}
