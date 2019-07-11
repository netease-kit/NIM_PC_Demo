#include "profile_mine.h"
#include "av_kit/module/video/video_manager.h"
#include "base/encrypt/encrypt.h"
#include "base/encrypt/encrypt_impl.h"

namespace nim_comp {

const LPCTSTR ProfileMine::kClassName = L"ProfileMine";

std::wstring nim_comp::ProfileMine::GetSkinFolder()
{
	return L"profile_mine";
}

std::wstring ProfileMine::GetSkinFile()
{
	return L"profile_mine.xml";
}

std::wstring ProfileMine::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ProfileMine::GetWindowId() const
{
	return kClassName;
}

ui::Control* ProfileMine::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefControl")
	{
		return new CefControl;
	}

	return NULL;
}

void ProfileMine::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&ProfileMine::OnClicked, this, std::placeholders::_1));

	// 获取用户信息
	std::string uid = nim_comp::LoginManager::GetInstance()->GetAccount();
	UserService::GetInstance()->GetUserInfo(uid, info_);

	// 注册用户资料改变回调
	auto user_info_change_cb = nbase::Bind(&ProfileMine::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));
	auto misc_uinfo_change_cb = nbase::Bind(&ProfileMine::OnMiscUInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegMiscUInfoChange(misc_uinfo_change_cb));

	// 初始化 Cef Control
	cef_control_ = static_cast<CefControl*>(FindControl(L"cef_control"));
	cef_control_->AttachLoadEnd(nbase::Bind(&ProfileMine::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachAfterCreated(nbase::Bind(&ProfileMine::OnAfterCreated, this, std::placeholders::_1));
	cef_control_->AttachBeforeCLose(nbase::Bind(&ProfileMine::OnBeforeClose, this, std::placeholders::_1));
	cef_control_->AttachBeforeNavigate(nbase::Bind(&ProfileMine::OnBeforeLoadResource, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachBeforeContextMenu(nbase::Bind(&ProfileMine::OnBeforeContextMenu, this, std::placeholders::_1, std::placeholders::_2));
	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef_themes/profile/profile.html";
	cef_control_->LoadURL(html_path);
}

bool ProfileMine::OnClicked(ui::EventArgs* arg)
{
	return true;
}

void ProfileMine::OnLoadEnd(int httpStatusCode)
{
	SetUserInfo();
	SetMultiportPushStatus();
	SetWebRTCStatus();
}

void ProfileMine::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	cef_control_->RegisterCppFunc(L"updateUserInfo", nbase::Bind(&ProfileMine::UpdateUserInfo, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->RegisterCppFunc(L"updateMultiportPushStatus", nbase::Bind(&ProfileMine::UpdateMultiportPushStatus, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->RegisterCppFunc(L"updateWebRTCStatus", nbase::Bind(&ProfileMine::UpdateWebRTCStatus, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->RegisterCppFunc(L"updateHeadImage", nbase::Bind(&ProfileMine::UpdateHeadImage, this, std::placeholders::_1, std::placeholders::_2));
}

void ProfileMine::OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear();
}

CefRequestHandler::ReturnValue ProfileMine::OnBeforeLoadResource(CefRefPtr<CefRequest> request, bool is_redirect)
{
	if (app_sdk::AppSDKInterface::IsSafeUrl(request->GetURL().ToString()))
	{
		CefRequest::HeaderMap headers;
		request->GetHeaderMap(headers);

		std::string appkey = app_sdk::AppSDKInterface::GetAppKey();
		std::string referer = app_sdk::AppSDKInterface::GetReferrer();
		std::string nonce = nim::Tool::GetUuid();
		std::string curtime = nbase::Uint64ToString(nbase::Time::Now().ToTimeT());
		std::string checksum_src = "";
		checksum_src.append(appkey).append(nonce).append(curtime);
		nbase::EncryptInterface_var encrypt_sha1(new nbase::Encrypt_Impl());
		encrypt_sha1->SetMethod(nbase::ENC_SHA1);
		encrypt_sha1->Encrypt(checksum_src);
		const static char HEX_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		std::string checksum;
		for (size_t j = 0; j < checksum_src.length(); j++)
		{
			checksum.append(1, HEX_DIGITS[(checksum_src[j] >> 4) & 0x0f]).append(1, HEX_DIGITS[checksum_src[j] & 0x0f]);
		}

		headers.emplace("AppKey", appkey);
		headers.emplace("Nonce", nonce);
		headers.emplace("CurTime", curtime);
		headers.emplace("CheckSum", checksum);
		request->SetHeaderMap(headers);

		// referer 要单独设置，不允许使用 SetHeaderMap 接口设置 referer 内容见 SetHeaderMap 注释
		request->SetReferrer(referer, REFERRER_POLICY_ALWAYS);

		QLOG_APP(L"ProfileMine::OnBeforeLoadResource short URL = {0}") << request->GetURL().ToString16();
	}

	return RV_CONTINUE;
}

void ProfileMine::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{

}

void ProfileMine::OnMultiportPushConfigChange(int rescode, bool switch_on)
{
	if (rescode == nim::kNIMResSuccess)
	{
		Json::Value values;
		values["multiportPush"] = switch_on;

		cef_control_->CallJSFunction(L"onMultiportPushChanged", nbase::UTF8ToUTF16(values.toStyledString()), nullptr);
	}
}

void ProfileMine::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto info : uinfos)
	{
		if (info_.GetAccId() == info.GetAccId())
		{
			info_.Update(info);
			SetUserInfo();
			break;
		}
	}
}

void ProfileMine::OnMiscUInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto info : uinfos)
	{
		if (info_.GetAccId() == info.GetAccId())
		{
			info_.Update(info);
			SetUserInfo();
			break;
		}
	}
}

void ProfileMine::OnModifyHeaderComplete(const std::string& id, const std::string &url)
{
	// 头像上传成功，开始更新用户信息
	auto update_cb = nbase::Bind(&ProfileMine::UpdateUInfoHeaderCallback, this, std::placeholders::_1);
	UserService::GetInstance()->InvokeUpdateMyPhoto(url, update_cb);
}

void ProfileMine::UpdateUInfoHeaderCallback(int res)
{
	QLOG_ERR(L"updateUInfoHeaderCallback error code: {0}.") << res;
}

void ProfileMine::SetUserInfo()
{
	Json::Value values;
	values["account"] = info_.GetAccId();
	values["nickname"] = info_.GetName();
	values["avatar"] = info_.GetIconUrl();
	values["birthday"] = info_.GetBirth();
	values["signature"] = info_.GetSignature();
	values["email"] = info_.GetEmail();
	values["gender"] = info_.GetGender();
	values["telephone"] = info_.GetMobile();

	std::wstring birth = nbase::UTF8ToUTF16(values.toStyledString());

	cef_control_->CallJSFunction(L"onUserInfoChanged", nbase::UTF8ToUTF16(values.toStyledString()), nullptr);
}

void ProfileMine::SetMultiportPushStatus()
{
	// 获取多端推送开关
	nim::Client::GetMultiportPushConfigAsync(nbase::Bind(&ProfileMine::OnMultiportPushConfigChange, this,
		std::placeholders::_1, std::placeholders::_2));
}

void ProfileMine::SetWebRTCStatus()
{
	// 获取 web rtc 开关状态
	Json::Value values;
	values["opening"] = nim_comp::VideoManager::GetInstance()->GetWebrtc();
	cef_control_->CallJSFunction(L"onWebRTCStatusChanged", nbase::UTF8ToUTF16(values.toStyledString()), nullptr);
}

void ProfileMine::UpdateUserInfo(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(params, values))
	{
		std::string nickname = values.isMember("nickname") ? values["nickname"].asString() : "";
		int gender = values.isMember("gender") ? values["gender"].asInt() : UG_MALE;
		int birth_year = values.isMember("birthYear") ? values["birthYear"].asInt() : 0;
		int birth_month = values.isMember("birthMonth") ? values["birthMonth"].asInt() : 0;
		int birth_day = values.isMember("birthDay") ? values["birthDay"].asInt() : 0;
		std::string telephone = values.isMember("telephone") ? values["telephone"].asString() : "";
		std::string email = values.isMember("email") ? values["email"].asString() : "";
		std::string signature = values.isMember("signature") ? values["signature"].asString() : "";
	
		nim::UserNameCard new_info;
		new_info.SetAccId(info_.GetAccId());

		if (!info_.ExistValue(nim::kUserNameCardKeyName) || nickname != info_.GetName())
		{
			new_info.SetName(nickname);
		}

		if (!info_.ExistValue(nim::kUserNameCardKeyGender) || info_.GetGender() != gender)
		{
			new_info.SetGender(gender);
		}

		if (birth_year != 0 && birth_month != 0 && birth_day != 0)
		{
			std::string new_birthday = nbase::StringPrintf("%04d-%02d-%02d", birth_year, birth_month, birth_day);
			if (!info_.ExistValue(nim::kUserNameCardKeyBirthday) || new_birthday != info_.GetBirth())
			{
				new_info.SetBirth(nbase::StringPrintf("%04d-%02d-%02d", birth_year, birth_month, birth_day));
			}
		}
		else
		{
			if (!info_.ExistValue(nim::kUserNameCardKeyBirthday) || !info_.GetBirth().empty())
			{
				new_info.SetBirth(nbase::StringPrintf("", birth_year, birth_month, birth_day));
			}
		}

		if (!info_.ExistValue(nim::kUserNameCardKeyMobile) && !telephone.empty()
			|| (info_.ExistValue(nim::kUserNameCardKeyMobile) && telephone != info_.GetMobile()))
		{
			new_info.SetMobile(telephone);
		}

		if (!info_.ExistValue(nim::kUserNameCardKeyEmail) && !email.empty()
			|| (info_.ExistValue(nim::kUserNameCardKeyEmail) && email != info_.GetEmail()))
		{
			new_info.SetEmail(email);
		}

		if (!info_.ExistValue(nim::kUserNameCardKeySignature) && !signature.empty()
			|| (info_.ExistValue(nim::kUserNameCardKeySignature) && signature != info_.GetSignature()))
		{
			new_info.SetSignature(signature);
		}

		UserService::GetInstance()->InvokeUpdateMyInfo(new_info, ToWeakCallback([callback](nim::NIMResCode res) {
			if (res == 200)
			{
				if (callback)
					callback(false, R"({ "message": "Update user info successfully." })");
			}
			else
			{
				if (callback)
					callback(true, R"({ "message": "Failed to update user info." })");
			}
		}));
	}
}

void ProfileMine::UpdateMultiportPushStatus(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;

	if (reader.parse(params, values) && values.isMember("multiportPush"))
	{
		bool is_turn_on = values["multiportPush"].asBool();
		nim::Client::SetMultiportPushConfigAsync(is_turn_on, nbase::Bind(&ProfileMine::OnMultiportPushConfigChange, this,
			std::placeholders::_1, std::placeholders::_2));
		if (callback)
			callback(false, R"({ "message": "Set multi port push config successfully." })");
	}
	else
	{
		if (callback)
			callback(true, R"({ "message": "Invalid params." })");
	}
}

void ProfileMine::UpdateWebRTCStatus(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;

	if (reader.parse(params, values) && values.isMember("opening"))
	{
		bool is_opening = values["opening"].asBool();
		nim_comp::VideoManager::GetInstance()->SetWebrtc(is_opening);
		SetWebRTCStatus();
		if (callback)
			callback(false, R"({ "message": "Switch Web RTC status successfully." })");
	}
	else
	{
		if (callback)
			callback(true, R"({ "message": "Invalid params." })");
	}
}

void ProfileMine::UpdateHeadImage(const std::string& params, nim_cef::ReportResultFunction callback)
{
	std::string uid = info_.GetAccId();
	HeadModifyForm* form = (HeadModifyForm*)WindowsManager::GetInstance()->GetWindow(HeadModifyForm::kClassName, nbase::UTF8ToUTF16(uid));
	if (form == NULL)
	{
		form = new HeadModifyForm(uid, L"");
		form->Create(NULL, NULL, WS_OVERLAPPED, 0L);
		form->ShowWindow(true);
		form->CenterWindow();
		if (callback)
			callback(false, R"({ "message": "Show window for modify user head image successfully." })");
	}
	else
	{
		::SetForegroundWindow(form->GetHWND());
		if (callback)
			callback(false, R"({ "message": "Failed to show window for modify user head image." })");
	}

	form->RegCompleteCallback(nbase::Bind(&ProfileMine::OnModifyHeaderComplete, this, std::placeholders::_1, std::placeholders::_2));
}

}
