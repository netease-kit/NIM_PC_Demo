#include "unread_item.h"
#include "module/session/session_manager.h"
#include "module/session/session_util.h"
#include "gui/main/team_event_form.h"
#include "gui/team_info/team_notify.h"
#include "tool_kits/shared/ui/ui_menu.h"
#include "callback/team/team_callback.h"
#include "gui/profile_form/profile_form.h"

using namespace ui;
namespace nim_comp
{

UnreadItem::UnreadItem()
{

}

UnreadItem::~UnreadItem()
{

}

void UnreadItem::InitCtrl()
{
	btn_head_ = (Button*) this->FindSubControl(L"btn_head");
	member_name_ = (Label*) this->FindSubControl(L"member_name");
	member_org_ = (Label*) this->FindSubControl(L"member_org");
}

bool UnreadItem::InitInfo(const nim::TeamMemberProperty &json)
{
	member_ = json;
	std::string id = json.GetAccountID();
	btn_head_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(id, false));
	std::string nick = json.GetNick();
	if (nick.empty())
		nick = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(id));
	member_name_->SetUTF8Text(nick);
	SetName(nbase::UTF8ToUTF16(json.GetAccountID()));

	btn_head_->AttachClick(nbase::Bind(&UnreadItem::OnHeadImageClick, this, id, std::placeholders::_1));
	return true;
}


bool UnreadItem::OnHeadImageClick(const std::string& uid, ui::EventArgs*)
{
	ProfileForm::ShowProfileForm(uid);
	return true;
}

bool UnreadItem::OnUserPhotoReady(const std::string & accid, const std::wstring & photo_path)
{
	if (accid == member_.GetAccountID())
	{
		btn_head_->SetBkImage(photo_path);
		return true;
	}
	return false;
}

}