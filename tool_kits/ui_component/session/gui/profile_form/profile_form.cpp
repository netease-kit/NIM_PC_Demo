#include "profile_form.h"
#include "module/session/session_manager.h"
#include "module/service/mute_black_service.h"
#include "head_modify_form.h"

namespace nim_comp
{
const LPCTSTR ProfileForm::kClassName = L"ProfileForm";

ProfileForm * ProfileForm::ShowProfileForm(UTF8String uid)
{
	ProfileForm* form = (ProfileForm*)WindowsManager::GetInstance()->GetWindow(kClassName, kClassName);
	if (form != NULL && form->m_uinfo.account.compare(uid) == 0) //当前已经打开的名片正是希望打开的名片
		; // 直接显示
	else
	{
		if (form != NULL && form->m_uinfo.account.compare(uid) != 0)//当前已经打开的名片不是希望打开的名片
			::DestroyWindow(form->m_hWnd); //关闭重新创建
		form = new ProfileForm();
		form->Create(NULL, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0L);
		form->m_uinfo.account = uid;
		
		// 获取用户信息
		OnGetUserInfoCallback cb = form->ToWeakCallback([form](bool ret, const std::list<UserInfo> &uinfos) {
			if (!ret || uinfos.empty()) return;
			if (uinfos.cbegin()->account == form->m_uinfo.account)
				form->InitUserInfo(*uinfos.cbegin());
		});
		UserService::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, uid), cb);
	}
	if (!::IsWindowVisible(form->m_hWnd))
	{
		::ShowWindow(form->m_hWnd, SW_SHOW);
		form->CenterWindow();
	}
	form->__super::ActiveWindow();
	return form;
}

ProfileForm::ProfileForm()
{
	auto user_info_change_cb = nbase::Bind(&ProfileForm::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));

	auto friend_list_change_cb = nbase::Bind(&ProfileForm::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));

	OnUserPhotoReadyCallback cb = ToWeakCallback([this](const std::string& accid, const std::wstring& photo_path) {
		if (m_uinfo.account == accid) head_image_btn->SetBkImage(photo_path); });
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(cb));
}

ProfileForm::~ProfileForm()
{

}

std::wstring ProfileForm::GetSkinFolder()
{
	return L"profile_form";
}

std::wstring ProfileForm::GetSkinFile()
{
	return L"profile_form.xml";
}

std::wstring ProfileForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ProfileForm::GetWindowId() const
{
	return kClassName;
}

void ProfileForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&ProfileForm::Notify, this, std::placeholders::_1));

	head_image_btn = static_cast<ui::Button*>(FindControl(L"head_image"));
	nickname_label = static_cast<ui::Label*>(FindControl(L"nickname"));
	user_id = static_cast<ui::Label*>(FindControl(L"userid"));
	notify_switch = static_cast<ui::CheckBox*>(FindControl(L"notify_switch"));
	black_switch = static_cast<ui::CheckBox*>(FindControl(L"black_switch"));
	start_chat = static_cast<ui::Button*>(FindControl(L"start_chat"));
	add_or_del = static_cast<ui::TabBox*>(FindControl(L"add_or_del"));
	delete_friend = static_cast<ui::Button*>(FindControl(L"delete_friend"));
	add_friend = static_cast<ui::Button*>(FindControl(L"add_friend"));

	sex_icon = static_cast<ui::CheckBox*>(FindControl(L"sex_icon"));
	btn_modify_info = static_cast<ui::Button*>(FindControl(L"modify_info"));
	btn_cancel_modify = static_cast<ui::Button*>(FindControl(L"cancel_modify"));
	btn_save_modify = static_cast<ui::Button*>(FindControl(L"save_modify"));
	nickname_box = static_cast<ui::HBox*>(FindControl(L"nickname_box"));
	nickname_edit = static_cast<ui::RichEdit*>(FindControl(L"nickname_edit"));
	nickname_error_tip = static_cast<ui::Label*>(FindControl(L"nickname_error_tip"));
	sex_box = static_cast<ui::HBox*>(FindControl(L"sex_box"));
	sex_option = static_cast<ui::Combo*>(FindControl(L"sex_option"));
	birthday_label = static_cast<ui::Label*>(FindControl(L"birthday"));
	birthday_combo_box = static_cast<ui::HBox*>(FindControl(L"birthday_combobox"));
	birth_year_combo = static_cast<ui::Combo*>(birthday_combo_box->FindSubControl(L"year"));
	birth_month_combo = static_cast<ui::Combo*>(birthday_combo_box->FindSubControl(L"month"));
	birth_day_combo = static_cast<ui::Combo*>(birthday_combo_box->FindSubControl(L"day"));
	phone_label = static_cast<ui::Label*>(FindControl(L"phone"));
	phone_edit = static_cast<ui::RichEdit*>(FindControl(L"phone_edit"));
	email_label = static_cast<ui::Label*>(FindControl(L"email"));
	email_edit = static_cast<ui::RichEdit*>(FindControl(L"email_edit"));
	signature_label = static_cast<ui::Label*>(FindControl(L"signature"));
	signature_edit = static_cast<ui::RichEdit*>(FindControl(L"signature_edit"));
}

void ProfileForm::InitUserInfo(const UserInfo &info)
{
	m_uinfo = info;
	InitLabels();

	if (m_uinfo.account == LoginManager::GetInstance()->GetAccount())
	{
		btn_modify_info->SetVisible(true); // 显示“编辑”按钮
		head_image_btn->SetMouseEnabled(true); // 可点击头像进行更换
		nickname_edit->SetLimitText(10);
		phone_edit->SetLimitText(13);
		email_edit->SetLimitText(30);
		signature_edit->SetLimitText(30);
		birth_year_combo->AttachSelect(nbase::Bind(&ProfileForm::OnBirthdayComboSelect, this, std::placeholders::_1));
		birth_month_combo->AttachSelect(nbase::Bind(&ProfileForm::OnBirthdayComboSelect, this, std::placeholders::_1));
		FindControl(L"only_other")->SetVisible(false); // 当名片是自己的时候，下面两块隐藏
		btn_modify_info->AttachClick(nbase::Bind(&ProfileForm::OnModifyOrCancelBtnClicked, this, std::placeholders::_1, true));
		btn_cancel_modify->AttachClick(nbase::Bind(&ProfileForm::OnModifyOrCancelBtnClicked, this, std::placeholders::_1, false));
		btn_save_modify->AttachClick(nbase::Bind(&ProfileForm::OnSaveInfoBtnClicked, this, std::placeholders::_1));
		head_image_btn->AttachClick(nbase::Bind(&ProfileForm::OnHeadImageClicked, this, std::placeholders::_1));
	}
	else
	{
		CheckInMuteBlack();
		notify_switch->AttachSelect(nbase::Bind(&ProfileForm::OnNotifySwitchSelected, this, std::placeholders::_1));
		notify_switch->AttachUnSelect(nbase::Bind(&ProfileForm::OnNotifySwitchUnSelected, this, std::placeholders::_1));
		black_switch->AttachSelect(nbase::Bind(&ProfileForm::OnBlackSwitchSelected, this, std::placeholders::_1));
		black_switch->AttachUnSelect(nbase::Bind(&ProfileForm::OnBlackSwitchUnSelected, this, std::placeholders::_1));
		start_chat->AttachClick(nbase::Bind(&ProfileForm::OnStartChatBtnClicked, this, std::placeholders::_1));
		add_or_del->SelectItem(UserService::GetInstance()->GetUserType(m_uinfo.account) == UT_FRIEND ? 0 : 1);
		delete_friend->AttachClick(nbase::Bind(&ProfileForm::OnDeleteFriendBtnClicked, this, std::placeholders::_1));
		add_friend->AttachClick(nbase::Bind(&ProfileForm::OnAddFriendBtnClicked, this, std::placeholders::_1));

		unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetMuteCallback(nbase::Bind(&ProfileForm::OnNotifyChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetBlackCallback(nbase::Bind(&ProfileForm::OnBlackChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));
	}
}

void ProfileForm::CheckInMuteBlack()
{
	auto service = MuteBlackService::GetInstance();
	auto mute_list = service->GetMuteList();
	auto black_list = service->GetBlackList();
	notify_switch->Selected(!service->IsInMuteList(m_uinfo.account));
	if (service->IsInBlackList(m_uinfo.account))
	{
		black_switch->Selected(true);
		notify_switch->SetEnabled(false);
	}
	else
		black_switch->Selected(false);
}

bool ProfileForm::Notify(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();

	if (msg->Type == ui::kEventTab)
	{
		if (btn_modify_info->IsVisible()) // 当前不是编辑页面
			return false;

		if (name == L"nickname_edit")
		{
			phone_edit->SetFocus();
		}
		else if (name == L"phone_edit")
		{
			email_edit->SetFocus();
		}
		else if (name == L"email_edit")
		{
			signature_edit->SetFocus();
		}
		else if (name == L"signature_edit")
		{
			nickname_edit->SetFocus();
		}
	}
	return true;
}

bool ProfileForm::OnNotifySwitchSelected(ui::EventArgs* args)
{
	MuteBlackService::GetInstance()->InvokeSetMute(m_uinfo.account, false);
	return true;
}

bool ProfileForm::OnNotifySwitchUnSelected(ui::EventArgs* args)
{
	MuteBlackService::GetInstance()->InvokeSetMute(m_uinfo.account, true);
	return true;
}

bool ProfileForm::OnBlackSwitchSelected(ui::EventArgs* args)
{
	MuteBlackService::GetInstance()->InvokeSetBlack(m_uinfo.account, true);
	return true;
}

bool ProfileForm::OnBlackSwitchUnSelected(ui::EventArgs* args)
{
	MuteBlackService::GetInstance()->InvokeSetBlack(m_uinfo.account, false);
	return true;
}

void ProfileForm::OnNotifyChangeCallback(std::string id, bool mute)
{
	if (id == m_uinfo.account)
		notify_switch->Selected(!mute);
}

void ProfileForm::OnBlackChangeCallback(std::string id, bool black)
{
	if (id == m_uinfo.account)
	{
		if (black)
		{
			black_switch->Selected(true);
			notify_switch->SetEnabled(false);
		}
		else
		{
			black_switch->Selected(false);
			notify_switch->SetEnabled(true);
		}
	}
}

bool ProfileForm::OnStartChatBtnClicked(ui::EventArgs* args)
{
	SessionManager::GetInstance()->OpenSessionForm(m_uinfo.account, nim::kNIMSessionTypeP2P);
	return true;
}

bool ProfileForm::OnDeleteFriendBtnClicked(ui::EventArgs* args)
{
	MsgboxCallback cb = nbase::Bind(&ProfileForm::OnDeleteFriendMsgBox, this, std::placeholders::_1);
	ShowMsgBox(m_hWnd, L"删除好友后，将同时解除双方的好友关系", cb, L"提示", L"确定", L"取消");
	return true;
}

void ProfileForm::OnDeleteFriendMsgBox(MsgBoxRet ret)
{
	if (ret == MB_YES)
	{
		nim::Friend::Delete(m_uinfo.account, ToWeakCallback([this](int res_code) {
			if (res_code == 200)
				Close();
		}));
	}
}

bool ProfileForm::OnAddFriendBtnClicked(ui::EventArgs* args)
{
	nim::Friend::Request(m_uinfo.account, nim::kNIMVerifyTypeAdd, "", ToWeakCallback([this](int res_code) {
		if (res_code == 200)
			Close();
	}));

	return true;
}

bool ProfileForm::OnHeadImageClicked(ui::EventArgs * args)
{
	HeadModifyForm* form = (HeadModifyForm*)WindowsManager::GetInstance()->GetWindow(HeadModifyForm::kClassName, HeadModifyForm::kClassName);
	if (form == NULL)
	{
		form = new HeadModifyForm(m_uinfo.account);
		form->Create(NULL, NULL, WS_OVERLAPPED, 0L);
		form->ShowWindow(true);
		form->CenterWindow();
	}
	else
	{
		::SetForegroundWindow(form->GetHWND());
	}

	return true;
}

bool ProfileForm::OnModifyOrCancelBtnClicked(ui::EventArgs* args, bool to_modify)
{
	btn_modify_info->SetVisible(!to_modify);
	btn_save_modify->SetVisible(to_modify);
	btn_cancel_modify->SetVisible(to_modify);
	nickname_box->SetVisible(to_modify);
	sex_box->SetVisible(to_modify);
	birthday_label->SetVisible(!to_modify);
	birthday_combo_box->SetVisible(to_modify);
	phone_label->SetVisible(!to_modify);
	phone_edit->SetVisible(to_modify);
	email_label->SetVisible(!to_modify);
	email_edit->SetVisible(to_modify);
	signature_label->SetVisible(!to_modify);
	signature_edit->SetVisible(to_modify);
	if (to_modify)
	{
		InitBirthdayCombo(); //初始化生日下拉框，在下面设置初始值
		InitEdits();
		nickname_error_tip->SetVisible(false);
	}
	return true;
}

bool ProfileForm::OnSaveInfoBtnClicked(ui::EventArgs * args)
{
	std::wstring nick = nickname_edit->GetText();
	size_t pos = 0;
	while (pos != nick.length() && (nick[pos] == ' ' || nick[pos] == '\t' || nick[pos] == '\n'))
		pos++;
	if (pos == nick.length())
	{
		nickname_error_tip->SetText(L"昵称不能为空");
		nickname_error_tip->SetVisible(true);
		return false;
	}
	else
		nickname_error_tip->SetVisible(false);

	UserInfo new_info;
	new_info.account = m_uinfo.account;

	if ((m_uinfo.field_avail_flag & kUInfoFlagName) == 0 || nbase::UTF16ToUTF8(nick) != m_uinfo.name)
	{
		new_info.field_avail_flag |= kUInfoFlagName;
		new_info.name = nbase::UTF16ToUTF8(nick);
	}

	switch (sex_option->GetCurSel())
	{
	case 0:
		if ((m_uinfo.field_avail_flag & kUInfoFlagGender) == 0 || m_uinfo.gender != UG_MALE)
		{
			new_info.field_avail_flag |= kUInfoFlagGender;
			new_info.gender = UG_MALE;
		}
		break;
	case 1:
		if ((m_uinfo.field_avail_flag & kUInfoFlagGender) == 0 || m_uinfo.gender != UG_FEMALE)
		{
			new_info.field_avail_flag |= kUInfoFlagGender;
			new_info.gender = UG_FEMALE;
		}
		break;
	default:
		break;
	}

	if (birth_year_combo->GetCurSel() != -1 && birth_month_combo->GetCurSel() != -1 && birth_day_combo->GetCurSel() != -1)
	{
		int this_year = nbase::Time::Now().ToTimeStruct(true).year();
		int birth_year = this_year - birth_year_combo->GetCurSel() - 1;
		int birth_month = birth_month_combo->GetCurSel() + 1;
		int birth_day = birth_day_combo->GetCurSel() + 1;
		std::string new_birthday = nbase::StringPrintf("%04d-%02d-%02d", birth_year, birth_month, birth_day);
		if ((m_uinfo.field_avail_flag & kUInfoFlagBirthday) == 0 || new_birthday != m_uinfo.birthday)
		{
			new_info.field_avail_flag |= kUInfoFlagBirthday;
			new_info.birthday = nbase::StringPrintf("%04d-%02d-%02d", birth_year, birth_month, birth_day);
		}
	}

	if ((m_uinfo.field_avail_flag & kUInfoFlagPhone) == 0 && !phone_edit->GetUTF8Text().empty()
		|| ((m_uinfo.field_avail_flag & kUInfoFlagPhone) != 0 && phone_edit->GetUTF8Text() != m_uinfo.phone))
	{
		new_info.field_avail_flag |= kUInfoFlagPhone;
		new_info.phone = nbase::UTF16ToUTF8(phone_edit->GetText());
	}
	if ((m_uinfo.field_avail_flag & kUInfoFlagEmail) == 0 && !email_edit->GetUTF8Text().empty()
		|| ((m_uinfo.field_avail_flag & kUInfoFlagEmail) != 0 && email_edit->GetUTF8Text() != m_uinfo.email))
	{
		new_info.field_avail_flag |= kUInfoFlagEmail;
		new_info.email = nbase::UTF16ToUTF8(email_edit->GetText());
	}
	if ((m_uinfo.field_avail_flag & kUInfoFlagSignature) == 0 && !signature_edit->GetUTF8Text().empty()
		|| (m_uinfo.field_avail_flag & kUInfoFlagSignature) != 0 && signature_edit->GetUTF8Text() != m_uinfo.signature)
	{
		new_info.field_avail_flag |= kUInfoFlagSignature;
		new_info.signature = nbase::UTF16ToUTF8(signature_edit->GetText());
	}
	
	if(new_info.field_avail_flag != kUInfoFlagNone)
		UserService::GetInstance()->InvokeUpdateUserInfo(new_info, nullptr);
	else
		OnModifyOrCancelBtnClicked(nullptr, false);

	return true;
}

void ProfileForm::InitBirthdayCombo()
{
	birth_year_combo->RemoveAll(); 
	birth_month_combo->RemoveAll();
	birth_day_combo->RemoveAll();

	auto create_elem = [](int i) 
	{
		ui::ListContainerElement* elem = new ui::ListContainerElement;
		elem->SetClass(L"listitem");
		elem->SetFixedHeight(30);
		elem->SetTextPadding(ui::UiRect(10, 1, 1, 1));
		elem->SetText(nbase::IntToString16(i));
		elem->SetFont(2);
		elem->SetStateTextColor(ui::kControlStateNormal, L"profile_account");
		return elem;
	};

	int this_year = nbase::Time::Now().ToTimeStruct(true).year();
	for (int i = this_year - 1; i >= 1900; i--)
		birth_year_combo->Add(create_elem(i));
	for (int i = 1; i <= 12; i++)
		birth_month_combo->Add(create_elem(i));
	for (int i = 1; i <= 31; i++)
		birth_day_combo->Add(create_elem(i));
}

bool ProfileForm::OnBirthdayComboSelect(ui::EventArgs * args)
{
	ui::ListContainerElement* cur_year = (ui::ListContainerElement*)birth_year_combo->GetItemAt(birth_year_combo->GetCurSel());
	ui::ListContainerElement* cur_month = (ui::ListContainerElement*)birth_month_combo->GetItemAt(birth_month_combo->GetCurSel());
	if (cur_year == NULL || cur_month == NULL)
		return false;

	int year = std::stoi(cur_year->GetText());
	int month = std::stoi(cur_month->GetText());

	std::set<int> big_months({1, 3, 5, 7, 8, 10, 12});
	std::set<int> small_months({ 4, 6, 9, 11});
	if (big_months.find(month) != big_months.cend()) //大月
	{
		for (int i = 28; i < 31; i++)
		{
			auto item = birth_day_combo->GetItemAt(i);
			if(item) item->SetVisible(true);
		}
	}
	else if (small_months.find(month) != small_months.cend()) //小月
	{
		for (int i = 28; i < 30; i++)
		{
			auto item = birth_day_combo->GetItemAt(i);
			if (item) item->SetVisible(true);
		}
		auto item = birth_day_combo->GetItemAt(30);
		if (item) 
			item->SetVisible(false);

		if (birth_day_combo->GetCurSel() > 29)
			birth_day_combo->SelectItem(29);
	}
	else //二月
	{
		for (int i = 29; i < 31; i++)
		{
			auto item = birth_day_combo->GetItemAt(i);
			if (item) item->SetVisible(false);
		}
		
		auto item = birth_day_combo->GetItemAt(28);
		if (item)
		{
			bool bissextile = (year % 4 == 0 && year % 100 != 0 || year % 400 == 0);
			birth_day_combo->GetItemAt(28)->SetVisible(bissextile);

			if (birth_day_combo->GetCurSel() > 27 + bissextile)
				birth_day_combo->SelectItem(27 + bissextile);
		}
	}

	return true;
}

void ProfileForm::OnFriendListChange(UserChangeType change_type, const UserInfo & info)
{
	if (info.account == m_uinfo.account)
	{
		if (change_type == kChangeTypeAdd)
			add_or_del->SelectItem(0);
		else if (change_type == kChangeTypeDelete)
			add_or_del->SelectItem(1);
	}
}

void ProfileForm::OnUserInfoChange(const std::list<UserInfo>& uinfos)
{
	for (auto info : uinfos)
	{
		if (m_uinfo.account == info.account)
		{
			m_uinfo.Update(info);
			InitLabels();
			OnModifyOrCancelBtnClicked(nullptr, false);
			break;
		}
	}
}

void ProfileForm::InitLabels()
{
	SetTaskbarTitle(nbase::UTF8ToUTF16(m_uinfo.name) + L"的名片"); // 任务栏标题
	head_image_btn->SetBkImage(UserService::GetInstance()->GetUserPhoto(m_uinfo.account)); //头像
	if ((m_uinfo.field_avail_flag & kUInfoFlagName) != 0)
		nickname_label->SetText(nbase::UTF8ToUTF16(m_uinfo.name)); // 头像右边的昵称

	if ((m_uinfo.field_avail_flag & kUInfoFlagGender) != 0)
	{
		switch (m_uinfo.gender) // 昵称右边的性别图标
		{
		case UG_MALE:
			sex_icon->Selected(false);
			sex_icon->SetVisible(true);
			break;
		case UG_FEMALE:
			sex_icon->Selected(true);
			sex_icon->SetVisible(true);
			break;
		case UG_UNKNOWN:
		default:
			sex_icon->SetVisible(false);
		}
	}

	user_id->SetText(L"账号：" + nbase::UTF8ToUTF16(m_uinfo.account));//账号
	if ((m_uinfo.field_avail_flag & kUInfoFlagBirthday) != 0)
		birthday_label->SetText(nbase::UTF8ToUTF16(m_uinfo.birthday)); //生日
	if ((m_uinfo.field_avail_flag & kUInfoFlagPhone) != 0)
		phone_label->SetText(nbase::UTF8ToUTF16(m_uinfo.phone)); //手机
	if ((m_uinfo.field_avail_flag & kUInfoFlagEmail) != 0)
		email_label->SetText(nbase::UTF8ToUTF16(m_uinfo.email)); //邮箱
	if ((m_uinfo.field_avail_flag & kUInfoFlagSignature) != 0)
		signature_label->SetText(nbase::UTF8ToUTF16(m_uinfo.signature)); //签名
}

void ProfileForm::InitEdits()
{
	if ((m_uinfo.field_avail_flag & kUInfoFlagName) != 0)
		nickname_edit->SetText(nbase::UTF8ToUTF16(m_uinfo.name)); //昵称
	if (sex_icon->IsVisible() && (m_uinfo.field_avail_flag & kUInfoFlagGender) != 0)//性别
		sex_option->SelectItem(sex_icon->IsSelected());

	if ((m_uinfo.field_avail_flag & kUInfoFlagBirthday) != 0)
	{
		int birth[3] = { 0,0,0 }; //生日
		size_t pos = std::string::npos;
		int count = 0;
		do
		{
			size_t temp = pos + 1;
			pos = m_uinfo.birthday.find_first_of('-', temp);
			std::string num_str = m_uinfo.birthday.substr(temp, pos - temp);
			if (!num_str.empty())
				birth[count] = std::stoi(num_str);
			count++;
		} while (pos != std::string::npos && count < 3);

		int this_year = nbase::Time::Now().ToTimeStruct(true).year();
		if (birth[0] >= 1900 && birth[0] < this_year && birth_year_combo->GetItemAt(this_year - birth[0] - 1) != NULL)
			birth_year_combo->SelectItem(this_year - birth[0] - 1);
		if (birth[1] >= 1 && birth[1] <= 12 && birth_month_combo->GetItemAt(birth[1] - 1) != NULL)
			birth_month_combo->SelectItem(birth[1] - 1);
		if (birth[2] >= 1 && birth[2] <= 31 && birth_day_combo->GetItemAt(birth[2] - 1) != NULL)
			birth_day_combo->SelectItem(birth[2] - 1);

		OnBirthdayComboSelect(NULL);
	}

	if ((m_uinfo.field_avail_flag & kUInfoFlagPhone) != 0)
		phone_edit->SetText(nbase::UTF8ToUTF16(m_uinfo.phone));//手机
	if ((m_uinfo.field_avail_flag & kUInfoFlagEmail) != 0)
		email_edit->SetText(nbase::UTF8ToUTF16(m_uinfo.email));//邮箱
	if ((m_uinfo.field_avail_flag & kUInfoFlagSignature) != 0)
		signature_edit->SetText(nbase::UTF8ToUTF16(m_uinfo.signature));//签名
}

}