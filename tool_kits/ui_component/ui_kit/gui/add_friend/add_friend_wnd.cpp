#include "add_friend_wnd.h"
#include "util/windows_manager.h"
#include "shared/ui/msgbox.h"
#include "module/login/login_manager.h"
#include "module/session/session_manager.h"

using namespace ui;
using namespace std;

namespace nim_comp
{
const LPCTSTR AddFriendWindow::kClassName	  = L"AddFriendWindow";


AddFriendWindow::AddFriendWindow() : 
	id_(),
	search_key_(nullptr),  
	apply_words_(nullptr),  
	tablayout_(nullptr),  
	addfriend_or_chat_(nullptr),  
	da_key_(false)
{

}

AddFriendWindow::~AddFriendWindow()
{
}

wstring AddFriendWindow::GetSkinFolder()
{
	return L"add_friend";
}

wstring AddFriendWindow::GetSkinFile()
{
	return L"add_friend_wnd.xml";
}

LRESULT AddFriendWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN) {
		if(wParam == VK_ESCAPE) {
			Close();
			return 1;
		}
		else if (wParam == VK_RETURN) {
			std::wstring page_name = tablayout_->GetItemAt(tablayout_->GetCurSel())->GetName();
			if (page_name == g_ADDFRIEND_SEARCH_PAGE) {
				Search(NULL);
			}
			else if (page_name == g_ADDFRIEND_USERINFO_PAGE) {
				if (addfriend_or_chat_->GetCurSel() == 0)
					AddFriendClick(NULL);
				if (addfriend_or_chat_->GetCurSel() == 1)
					ChatClick(NULL);
				else if (addfriend_or_chat_->GetCurSel() == 2)
					RemoveFromBlack(NULL);
			}
			else if (page_name == g_ADDFRIEND_NOTFOUND_PAGE) {
				PreOrNextClick(NULL, g_ADDFRIEND_SEARCH_PAGE, SearchAndSendApplyEdit); 
			}
			else if (page_name == g_ADDFRIEND_SENDFRIENDAPPLY_PAGE) {
				SendFriendApplyClick(NULL);
			}
			else if (page_name == g_ADDFRIEND_SENDCOMPLETE_PAGE) {
				PreOrNextClick(NULL, L"", NONE); 
			}
			else if (page_name == g_ADDFRIEND_SENDTOOFREQUENT_PAGE) {
				PreOrNextClick(NULL, L"", NONE); 
			}
			else if (page_name == g_ADDFRIEND_ADDSUCCESS_PAGE) {
				PreOrNextClick(NULL, L"", NONE); 
			}
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void AddFriendWindow::InitWindow()
{
	tablayout_ = static_cast<TabBox*>(FindControl(_T("tablayout")));

	//search_page
	VBox* search_page = static_cast<VBox*>(FindControl(g_ADDFRIEND_SEARCH_PAGE));
	Button* confirm = static_cast<Button*>(search_page->FindSubControl(L"confirm"));
	confirm->AttachClick(nbase::Bind(&AddFriendWindow::Search, this, std::placeholders::_1));

	Button* cancel = static_cast<Button*>(search_page->FindSubControl(L"cancel"));
	cancel->AttachClick(nbase::Bind(&AddFriendWindow::Cancel, this, std::placeholders::_1));

	search_key_ = (RichEdit*) search_page->FindSubControl(L"search_key");
	search_key_->AttachSetFocus(nbase::Bind(&AddFriendWindow::OnSearchKeyEditSetFocus, this, std::placeholders::_1));
	search_key_->SetFocus();

	//userinfo_page
	VBox* userinfo_page = static_cast<VBox*>(FindControl(_T("userinfo_page")));
	Button* userinfo_page_pre_step = (Button*)userinfo_page->FindSubControl(L"pre_step");
	userinfo_page_pre_step->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_SEARCH_PAGE, SendApplyEdit)); 
	addfriend_or_chat_ = (TabBox*)userinfo_page->FindSubControl(L"addfriend_or_chat");
	userinfo_page_add_friend_ = (Button*)userinfo_page->FindSubControl(L"add_friend");
	userinfo_page_add_friend_->AttachClick(nbase::Bind(&AddFriendWindow::AddFriendClick, this, std::placeholders::_1));
	Button* userinfo_page_chat = (Button*)userinfo_page->FindSubControl(L"chat");
	userinfo_page_chat->AttachClick(nbase::Bind(&AddFriendWindow::ChatClick, this, std::placeholders::_1)); 
	Button* remove_form_black = (Button*)userinfo_page->FindSubControl(L"remove");
	remove_form_black->AttachClick(nbase::Bind(&AddFriendWindow::RemoveFromBlack, this, std::placeholders::_1));

	//notfound_page
	VBox* notfound_page = static_cast<VBox*>(FindControl(_T("notfound_page")));
	Button* notfound_page_continue_search = (Button*)notfound_page->FindSubControl(L"continue_search");
	notfound_page_continue_search->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_SEARCH_PAGE, SearchAndSendApplyEdit)); 
	Button* notfound_page_ok = (Button*)notfound_page->FindSubControl(L"ok");
	notfound_page_ok->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, L"", NONE)); 

	//send_friend_apply_page
	VBox* send_friend_apply_page = static_cast<VBox*>(FindControl(_T("send_friend_apply_page")));
	Button* send_friend_apply_page_pre_step = (Button*)send_friend_apply_page->FindSubControl(L"pre_step");
	send_friend_apply_page_pre_step->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_USERINFO_PAGE, NONE)); 
	Button* send_friend_apply_page_send = (Button*)send_friend_apply_page->FindSubControl(L"send");
	send_friend_apply_page_send->AttachClick(nbase::Bind(&AddFriendWindow::SendFriendApplyClick, this, std::placeholders::_1)); 
	apply_words_   = static_cast<RichEdit*>(FindControl(_T("apply_words")));

	apply_words_->SetLimitText( 64 ); //readme 21754  以安卓为准

	//send_complete_page
	VBox* send_complete_page = static_cast<VBox*>(FindControl(_T("send_complete_page")));
	Button* send_complete_page_continue_search = (Button*)send_complete_page->FindSubControl(L"continue_search");
	send_complete_page_continue_search->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_SEARCH_PAGE, SearchAndSendApplyEdit)); 
	Button* send_complete_page_ok = (Button*)send_complete_page->FindSubControl(L"ok");
	send_complete_page_ok->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, L"", NONE)); 

	//send_too_frequent_page
	VBox* send_too_frequent_page = static_cast<VBox*>(FindControl(_T("send_too_frequent_page")));
	Button* send_too_frequent_page_continue_search = (Button*)send_too_frequent_page->FindSubControl(L"continue_search");
	send_too_frequent_page_continue_search->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_SEARCH_PAGE, SearchAndSendApplyEdit)); 
	Button* send_too_frequent_page_ok = (Button*)send_too_frequent_page->FindSubControl(L"ok");
	send_too_frequent_page_ok->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, L"", NONE)); 

	//add_success_page
	VBox* add_success_page = static_cast<VBox*>(FindControl(_T("add_success_page")));
	Button* add_success_page_continue_search = (Button*)add_success_page->FindSubControl(L"continue_search");
	add_success_page_continue_search->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_SEARCH_PAGE, SearchAndSendApplyEdit)); 
	Button* add_success_page_ok = (Button*)add_success_page->FindSubControl(L"ok");
	add_success_page_ok->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, L"", NONE)); 

	// net_abnormal_page
	VBox* net_abnormal_page = static_cast<VBox*>(FindControl(L"net_abnormal"));
	Button* net_abnormal_page_continue_search = (Button*)net_abnormal_page->FindSubControl(L"continue_search");
	net_abnormal_page_continue_search->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, g_ADDFRIEND_SEARCH_PAGE, SearchAndSendApplyEdit));
	Button* net_abnormal_page_ok = (Button*)net_abnormal_page->FindSubControl(L"ok");
	net_abnormal_page_ok->AttachClick(nbase::Bind(&AddFriendWindow::PreOrNextClick, this, std::placeholders::_1, L"", NONE));

	OnUserPhotoReadyCallback cb = ToWeakCallback([this](const std::string& accid, const std::wstring& photo_path) {
		if (tablayout_->GetCurSel() == 1 && id_ == accid)
		{
			VBox* userinfo_page = static_cast<VBox*>(FindControl(_T("userinfo_page")));
			Button* headimage = (Button*)userinfo_page->FindSubControl(L"headimage");
			headimage->SetBkImage(photo_path);
		}
	});
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(cb));
}

bool AddFriendWindow::Search(ui::EventArgs* param)
{
	UTF8String key = search_key_->GetUTF8Text();
	nbase::StringTrim(key);
	if (key.empty())
		return false;

	OnGetUserInfoCallback cb = ToWeakCallback([this](const std::list<nim::UserNameCard> uinfos) {
		assert(nbase::MessageLoop::current()->ToUIMessageLoop());
			if (!uinfos.empty())
				InitUserProfile(*uinfos.cbegin());
			else
				PreOrNextClick(NULL, g_ADDFRIEND_NOTFOUND_PAGE, AddFriendWindow::NONE);

		SetFocus(nullptr);
	});
	UserService::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, key), cb);

	return true;
}

bool AddFriendWindow::AddFriendClick(ui::EventArgs* param)
{
	if (!FindControl(L"add_friend")->IsEnabled()) {
		return true;
	}

	nim::Friend::Request(id_, nim::kNIMVerifyTypeAdd, "", ToWeakCallback([this](int res_code) {
		if (res_code == 200)
		{
			PreOrNextClick(NULL, g_ADDFRIEND_ADDSUCCESS_PAGE, AddFriendWindow::NONE);
		}
		else if(res_code == 408 || res_code == 415){
			PreOrNextClick(NULL, g_ADDFRIEND_NET_ABNORMAL_PAGE, AddFriendWindow::NONE);
		}
		else{

		}
	}));
	return true;
}

bool AddFriendWindow::ChatClick(ui::EventArgs* param)
{
	SessionManager::GetInstance()->OpenSessionForm(id_, nim::NIMSessionType::kNIMSessionTypeP2P);
	Close();
	return true;
}

bool AddFriendWindow::RemoveFromBlack(ui::EventArgs* args)
{
	nim::User::SetBlackCallback cb = ToWeakCallback([this](int res_code, const std::string& accid, bool opt){
		if (res_code == 200)
		{
			if (UserService::GetInstance()->GetUserType(id_) == nim::kNIMFriendFlagNormal)
				addfriend_or_chat_->SelectItem(1);
			else
				addfriend_or_chat_->SelectItem(0);
		}
		else if (res_code == 408 || res_code == 415)
			PreOrNextClick(NULL, g_ADDFRIEND_NET_ABNORMAL_PAGE, AddFriendWindow::NONE);
	});
	nim::User::SetBlack(id_, false, cb);
	return true;
}

bool AddFriendWindow::SendFriendApplyClick(ui::EventArgs* param)
{
	SetFocus(nullptr);
	return true;
}

void AddFriendWindow::InitEdit()
{
	search_key_->SetText(L"");
	apply_words_->SetText(L"");
}

bool AddFriendWindow::OnSearchKeyEditSetFocus(void* param)
{
	if (!da_key_)
	{
		da_key_ = true;
	}
	return true;
}

bool AddFriendWindow::PreOrNextClick(ui::EventArgs* param, const std::wstring& page_name, INIT_TYPE init_edit)
{
	if (page_name.empty())
	{
		Close();
	}
	else
	{
		if (init_edit == SearchAndSendApplyEdit)
		{
			InitEdit();
		}
		else if (init_edit == SendApplyEdit)
		{
			apply_words_->SetText(L"");
		}
		tablayout_->SelectItem(page_name);

		if (page_name == g_ADDFRIEND_SEARCH_PAGE) {
			search_key_->SetFocus();
		}
		else if (page_name == g_ADDFRIEND_SENDFRIENDAPPLY_PAGE) {
			apply_words_->SetFocus();
		}
	}

	return true;
}

void AddFriendWindow::StartSearch(const std::wstring& search_key)
{
	search_key_->SetText(search_key);
	Search(nullptr);
}

void AddFriendWindow::InitUserProfile(const nim::UserNameCard& uinfo)
{
	id_ = uinfo.GetAccId();

	if (LoginManager::GetInstance()->IsEqual(id_))
	{
		userinfo_page_add_friend_->SetEnabled(false);
	}
	else
		userinfo_page_add_friend_->SetEnabled(true);

	tablayout_->SelectItem(g_ADDFRIEND_USERINFO_PAGE);

	VBox* userinfo_page = static_cast<VBox*>(FindControl(_T("userinfo_page")));
	Button* headimage = (Button*)userinfo_page->FindSubControl(L"headimage");
	headimage->SetBkImage(UserService::GetInstance()->GetUserPhoto(uinfo.GetAccId()));

	Label* nick_name = (Label*)userinfo_page->FindSubControl(L"nick_name");
	nick_name->SetText(UserService::GetInstance()->GetUserName(id_));

	Label* id = (Label*)userinfo_page->FindSubControl(L"id");
	id->SetUTF8Text(id_);

	nim::NIMFriendFlag user_type = UserService::GetInstance()->GetUserType(id_);
	if (MuteBlackService::GetInstance()->IsInBlackList(id_))
		addfriend_or_chat_->SelectItem(2);
	else if (user_type == nim::kNIMFriendFlagNotFriend)
		addfriend_or_chat_->SelectItem(0);
	else if (user_type == nim::kNIMFriendFlagNormal)
		addfriend_or_chat_->SelectItem(1);
}

bool AddFriendWindow::Cancel(ui::EventArgs* param)
{
	Close();

	return true;
}

bool AddFriendWindow::SearchTipWrong(ui::EventArgs* param)
{
	return true;
}

}