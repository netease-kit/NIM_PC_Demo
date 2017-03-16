#include "team_search.h"
#include "module/emoji/richedit_util.h"
#include "module/session/session_util.h"
#include "callback/team/team_callback.h"
#include "export/nim_ui_photo_manager.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR TeamSearchForm::kClassName = L"TeamSearchForm";

TeamSearchForm::TeamSearchForm()
{

}

TeamSearchForm::~TeamSearchForm()
{

}

std::wstring TeamSearchForm::GetSkinFolder()
{
	return L"team_info";
}

std::wstring TeamSearchForm::GetSkinFile()
{
	return L"team_search.xml";
}

std::wstring TeamSearchForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring TeamSearchForm::GetWindowId() const
{
	return kClassName;
}

UINT TeamSearchForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT TeamSearchForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN && wParam == 'V')
	{
		if(::GetKeyState(VK_CONTROL) < 0)
		{
			if(re_tid_->IsFocused())
			{
				re_tid_->PasteSpecial(CF_TEXT);
				return 1;
			}
		}
	}
	else if (uMsg == WM_KEYDOWN && wParam == VK_RETURN)
	{
		if (tsp_[TSP_SEARCH]->IsVisible())
		{
			if (btn_search_->IsEnabled())
				SendNotify(btn_search_, kEventClick);
			else
				re_tid_->SetFocus();
		}
		else if (tsp_[TSP_APPLY]->IsVisible())
		{
			if (btn_apply_->IsEnabled())
				SendNotify(btn_apply_, kEventClick);
			else
				SendNotify(FindControl(L"btn_return_search"), kEventClick);
		}
		else if (tsp_[TSP_APPLY_OK]->IsVisible())
		{
			SendNotify(FindControl(L"btn_apply_ok"), kEventClick);
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void TeamSearchForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&TeamSearchForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&TeamSearchForm::OnClicked, this, std::placeholders::_1));

	tsp_[TSP_SEARCH] = FindControl(L"page_search");
	tsp_[TSP_APPLY] = FindControl(L"page_apply");
	tsp_[TSP_APPLY_OK] = FindControl(L"page_apply_ok");

	re_tid_ = (RichEdit*) FindControl(L"re_tid");
	re_tid_->SetLimitText(20);

	error_tip_ = (Label*) FindControl(L"error_tip");
	btn_search_ = (Button*) FindControl(L"btn_search");

	team_icon_ = (Button*) FindControl(L"team_icon");
	team_name_ = (Label*) FindControl(L"team_name");
	team_id_ = (Label*) FindControl(L"team_id");
	label_group_ = (Label*) FindControl(L"label_group");
	btn_apply_ = (Button*) FindControl(L"btn_apply");

	re_apply_ = (RichEdit*) FindControl(L"re_apply");

	GotoPage(TSP_SEARCH);

	OnPhotoReadyCallback cb2 = nbase::Bind(&TeamSearchForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(nim_ui::PhotoManager::GetInstance()->RegPhotoReady(cb2));
}

void TeamSearchForm::GotoPage( TeamSearchPage page )
{
	for(int i = 0; i < TSP_COUNT; i++)
	{
		tsp_[i]->SetVisible(false);
	}
	tsp_[page]->SetVisible(true);

	if (page == TSP_SEARCH)
	{
		tid_.clear();
		re_tid_->SetText(L"");
		re_tid_->SetFocus();
		btn_search_->SetEnabled(true);
	}
}

void TeamSearchForm::OnUserPhotoReady(PhotoType type, const std::string& tid, const std::wstring &photo_path)
{
	if (type == kTeam && tid == tid_)
	{
		team_icon_->SetBkImage(photo_path);
	}
}
void TeamSearchForm::ShowTeamInfo(const nim::TeamEvent& team_event)
{
	if (!team_event.team_info_.GetIcon().empty())
		PhotoService::GetInstance()->DownloadTeamIcon(team_event.team_info_);

	team_icon_->SetBkImage( PhotoService::GetInstance()->GetTeamPhoto(team_event.team_id_, false) );

	team_id_->SetUTF8Text(team_event.team_id_);

	std::string name = team_event.team_info_.GetName();
	if (name.empty())
	{
		tname_ = nbase::UTF8ToUTF16(team_event.team_id_);
	}
	else
	{
		tname_ = nbase::UTF8ToUTF16(name);
		team_name_->SetText(tname_);
	}

	nim::NIMTeamType type = team_event.team_info_.GetType();
	if (type == nim::kNIMTeamTypeNormal)
	{
		label_group_->SetVisible(true);
		btn_apply_->SetEnabled(false);
	}
	else
	{
		label_group_->SetVisible(false);
		btn_apply_->SetEnabled(true);
	}
}

bool TeamSearchForm::Notify(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if(arg->Type == kEventTextChange)
	{
		if(name == L"re_tid")
		{
			error_tip_->SetVisible(false);
			btn_search_->SetEnabled(true);
		}
	}
	return false;
}

bool TeamSearchForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"btn_search")
	{
		std::string tid;
		{
			std::wstring str = GetRichText(re_tid_);
			StringHelper::Trim(str);
			if( str.empty() )
			{
				error_tip_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_SEARCH_ID_NON_EMPTY"));
				error_tip_->SetVisible(true);

				btn_search_->SetEnabled(false);
				return false;
			}
			tid = nbase::UTF16ToUTF8(str);
		}
		tid_ = tid;
		btn_search_->SetEnabled(false);

		nim::Team::QueryTeamInfoOnlineAsync(tid_, nbase::Bind(&TeamSearchForm::OnGetTeamInfoCb, this, std::placeholders::_1));
	}
	else if(name == L"btn_return_search")
	{
		label_group_->SetVisible(false);
		btn_apply_->SetEnabled(true);

		GotoPage(TSP_SEARCH);
	}
	else if(name == L"btn_apply")
	{
		nim::Team::ApplyJoinAsync(tid_, "", nbase::Bind(&TeamSearchForm::OnApplyJoinCb, this, std::placeholders::_1));

		GotoPage(TSP_APPLY_OK);
	}
	else if(name == L"btn_apply_ok")
	{
		this->Close();
	}
	return false;
}

void TeamSearchForm::OnGetTeamInfoCb(const nim::TeamEvent& team_event)
{
	QLOG_APP(L"search team: {0}") << team_event.res_code_;
	
	if (team_event.res_code_ == 200)
	{
		ShowTeamInfo(team_event);
		GotoPage(TSP_APPLY);
	}
	else
	{
		error_tip_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_SEARCH_TEAM_NOT_EXISTS"));
		error_tip_->SetVisible(true);
	}
}

void TeamSearchForm::OnApplyJoinCb(const nim::TeamEvent& team_event)
{
	QLOG_APP(L"apply join: {0}") << team_event.res_code_;
	
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	
	switch (team_event.res_code_)
	{
	case nim::kNIMResTeamAlreadyIn:
	{
		re_apply_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_TEAM_SEARCH_ALREADY_IN").c_str(), tname_.c_str()));
	}
	break;
	case nim::kNIMResSuccess:
	{
		nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(TeamCallback::OnTeamEventCallback, team_event));
		re_apply_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_TEAM_SEARCH_AGREE_APPLY").c_str(), tname_.c_str()));
	}
	break;
	case nim::kNIMResTeamApplySuccess:
		re_apply_->SetText(mls->GetStringViaID(L"STRID_TEAM_SEARCH_WAIT_VERIFY"));
		break;
	default:
	{
		re_apply_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_TEAM_SEARCH_REFUSE_APPLY").c_str(), tname_.c_str()));
	}
	break;
	}
}
}