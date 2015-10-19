#include "broad_form.h"
#include "module/emoji/richedit_util.h"
#include "module/session/session_manager.h"
#include "callback/team/team_callback.h"
#include "gui/session/control/session_util.h"


using namespace ui;
namespace nim_comp
{
const LPCTSTR BroadForm::kClassName = L"BroadForm";

BroadForm::BroadForm()
{

}

BroadForm::~BroadForm()
{

}

std::wstring BroadForm::GetSkinFolder()
{
	return L"broad";
}

std::wstring BroadForm::GetSkinFile()
{
	return L"broad.xml";
}

ui::UILIB_RESOURCETYPE BroadForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring BroadForm::GetZIPFileName() const
{
	return L"broad.zip";
}

std::wstring BroadForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring BroadForm::GetWindowId() const
{
	return kClassName;
}

UINT BroadForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT BroadForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN && wParam == 'V')
	{
		if(::GetKeyState(VK_CONTROL) < 0)
		{
			if(re_title_->IsFocused())
			{
				re_title_->PasteSpecial(CF_TEXT);
				return 1;
			}
			else if(re_content_->IsFocused())
			{
				re_content_->PasteSpecial(CF_TEXT);
				return 1;
			}
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void BroadForm::InitWindow()
{
	SetTaskbarTitle(L"发布新公告");
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&BroadForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&BroadForm::OnClicked, this, std::placeholders::_1));

	re_title_ = (RichEdit*) FindControl(L"re_title");
	error_1_ = (Label*) FindControl(L"error_1");

	re_content_ = (RichEdit*) FindControl(L"re_content");
	error_2_ = (Label*) FindControl(L"error_2");

	re_title_->SetLimitText(20);
	re_content_->SetLimitText(300);

	btn_commit_ = (Button*) FindControl(L"commit");
}

void BroadForm::SetTid( const std::string &tid )
{
	tid_ = tid;
}

bool BroadForm::Notify(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if(arg->Type == kEventTextChange)
	{
		if(name == L"re_title")
			error_1_->SetVisible(false);
		else if(name == L"re_content")
			error_2_->SetVisible(false);
	}
	return false;
}

bool BroadForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"commit")
	{
		std::string title;
		{
			std::wstring str = GetRichText(re_title_);
			StringHelper::Trim(str);
			if( str.empty() )
			{
				error_1_->SetText(L"标题不可为空");
				error_1_->SetVisible(true);
				return false;
			}
			title = nbase::UTF16ToUTF8(str);
		}
		
		std::string content;
		{
			std::wstring str = GetRichText(re_content_);
			StringHelper::Trim(str);
			if( str.empty() )
			{
				error_2_->SetText(L"公告内容不可为空");
				error_2_->SetVisible(true);
				return false;
			}
			content = nbase::UTF16ToUTF8(str);
		}

		btn_commit_->SetEnabled(false);

		Json::Value broad;
		broad["title"] = title;
		broad["content"] = content;
		broad["creator"] = LoginManager::GetInstance()->GetAccount();
		broad["time"] = nbase::Time::Now().ToTimeT();

		Json::Value broads;
		broads.append(broad);

		Json::Value param;
		param[nim::kNIMTeamInfoKeyAnnouncement] = broads.toStyledString();

		nim::Team::UpdateTeamInfoAsync(tid_, param.toStyledString(), nbase::Bind(&BroadForm::OnUpdateBroadCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
	return false;
}

void BroadForm::OnUpdateBroadCb( int rescode, int notification_id, const std::string& tid, const std::string& result )
{
	QLOG_APP(L"update broad: code={0} notify_id={1} tid={2} result={3}") <<rescode <<notification_id <<tid <<result;
	
	if (rescode == 200)
	{
		SessionForm* session = SessionManager::GetInstance()->Find(tid_);
		if (session)
			session->InvokeGetTeamInfo();

		this->Close();
	}
	else
	{
		btn_commit_->SetEnabled(true);

		ShowMsgBox(m_hWnd, L"发布公告失败", MsgboxCallback(), L"提示", L"确定", L"");
	}
}
}