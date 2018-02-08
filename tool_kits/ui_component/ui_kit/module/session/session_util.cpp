#include "session_util.h"
#include "shared/zoom_image.h"
#include "util/user.h"
#include "module/emoji/richedit_util.h"
#include "module/session/session_manager.h"

namespace nim_comp
{
SessionType GetSessionType(const nim::IMMessage &msg)
{
	if (msg.session_type_ == nim::kNIMSessionTypeP2P)
		return kSessionDouble;
	else if (msg.session_type_ == nim::kNIMSessionTypeTeam)
		return kSessionTeam;
	else
	{
		assert(0);
		return kSessionDouble;
	}
}

std::string GetSessionId(const nim::IMMessage &msg)
{
	if (msg.session_type_ == nim::kNIMSessionTypeP2P)
	{
		if (LoginManager::GetInstance()->IsEqual(msg.sender_accid_))
			return msg.receiver_accid_;
		else
			return msg.sender_accid_;
	}
	else
		return msg.receiver_accid_;
}

bool IsBubbleRight(const nim::IMMessage &msg)
{
	if (LoginManager::GetInstance()->IsEqual(msg.sender_accid_))
	{
		if (msg.type_ == nim::kNIMMessageTypeRobot)
		{
			nim::IMBotRobot robot;
			nim::Talk::ParseBotRobotMessageAttach(msg, robot);
			return !robot.out_msg_;
		}
		if (msg.receiver_accid_ == msg.sender_accid_)//给自己的其他端的消息
		{
			//nim::kNIMClientTypePCWindows
			if (msg.readonly_sender_client_type_ != nim::kNIMClientTypePCWindows && msg.readonly_sender_client_type_ != 0)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

std::wstring GetRichText(ui::RichEdit* re)
{
	std::wstring wstr;
	ITextServices* service = re->GetTextServices();
	Re_GetText(service, wstr);
	service->Release();

	return wstr;
}

bool IsSameWeekDay(nbase::Time::TimeStruct msg_time,nbase::Time::TimeStruct now_time,
	const time_t msg_time_t,const time_t now_time_t)
{
	int now_time_day_of_week = now_time.day_of_week() == 0 ? 7 : now_time.day_of_week();
	int msg_time_day_of_week = msg_time.day_of_week() == 0 ? 7 : msg_time.day_of_week();
	int delta_dat_in_week = now_time_day_of_week - msg_time_day_of_week;
	if (delta_dat_in_week > 0)
	{
		nbase::Time::TimeStruct msg_time_ts;
		msg_time_ts.year_ = msg_time.year();
		msg_time_ts.month_ = msg_time.month();
		msg_time_ts.day_of_week_ = msg_time.day_of_week();
		msg_time_ts.day_of_month_ = msg_time.day_of_month();

		nbase::Time::TimeStruct now_time_ts;
		now_time_ts.year_ = now_time.year();
		now_time_ts.month_ = now_time.month();
		now_time_ts.day_of_week_ = now_time.day_of_week();
		now_time_ts.day_of_month_ = now_time.day_of_month();

		nbase::Time msg_time_ex(true,msg_time_ts);
		nbase::Time now_time_ex(true,now_time_ts);

		nbase::TimeDelta delta = now_time_ex - msg_time_ex; //不能计算精确到second时间差，而应该精确到day的时间差;
		int delta_day = delta.ToDays();
		if (delta_day >= 0 && delta_day <= 7)
		{
			return true;
		}
	}
	return false;
}

std::wstring GetWeekOfDate(int iWeek)
{
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	static const std::wstring weekday[8] = { L"",
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_MONDAY"),
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_TUESDAY"),
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_WEDNESDAY"),
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_THURSDAY"),
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_FRIDAY"),
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_SATURDAY"),
		mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_SUNDAY")
	};
	
	assert(iWeek <= 7 && iWeek >= 1);
	return weekday[iWeek];
}

std::wstring GetMessageTime( const long long t, bool abbreviate )
{
	assert(t > 0);
	if(t <= 0)
		return L"";

	std::wstring show_time;
	std::wstring show_data;

	bool bToday = false;
	time_t msg_time_t(t/1000);
	time_t now_time_t = time(0);
	nbase::Time msg_time_time = nbase::Time::FromTimeT(msg_time_t);
	nbase::Time now_time_time = nbase::Time::FromTimeT(now_time_t);
	nbase::Time yesterday_time_time = nbase::Time::FromTimeT(now_time_t - 3600*24);
	nbase::Time preyesterday_time_time = nbase::Time::FromTimeT(now_time_t - 3600*24*2);

	nbase::Time::TimeStruct msg_time_tm = msg_time_time.ToTimeStruct(true);
	nbase::Time::TimeStruct now_time_tm = now_time_time.ToTimeStruct(true);
	nbase::Time::TimeStruct yesterday_time_tm = yesterday_time_time.ToTimeStruct(true);
	nbase::Time::TimeStruct preyesterday_time_tm = preyesterday_time_time.ToTimeStruct(true);

	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	if (now_time_tm.year() == msg_time_tm.year() 
		&& now_time_tm.month() == msg_time_tm.month() 
		&& now_time_tm.day_of_month() == msg_time_tm.day_of_month())//today
	{
		bToday = true;
		show_data = mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_TODAY");
	}
	else if (yesterday_time_tm.year() == msg_time_tm.year() 
		&& yesterday_time_tm.month() == msg_time_tm.month() 
		&& yesterday_time_tm.day_of_month() == msg_time_tm.day_of_month())//yesterday
	{
		show_data = mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_YESTERDAY");
	}
	//前天的英文太长，且不是十分必要
	//else if (preyesterday_time_tm.year() == msg_time_tm.year() 
	//	&& preyesterday_time_tm.month() == msg_time_tm.month() 
	//	&& preyesterday_time_tm.day_of_month() == msg_time_tm.day_of_month())//preyesterday
	//{
	//	show_data = mls->GetStringViaID(L"STRID_SESSION_ITEM_DAY_DAY_BEFORE");
	//}
	else if (IsSameWeekDay(msg_time_tm,now_time_tm,msg_time_t,now_time_t))//week
	{
		int now_time_day_of_week = msg_time_tm.day_of_week() == 0 ? 7 : msg_time_tm.day_of_week();
		show_data = GetWeekOfDate(now_time_day_of_week);
	}
	else //mm-dd or yyyy-mm-dd
	{
		if (now_time_tm.year() == msg_time_tm.year())
		{
			show_data = nbase::StringPrintf(L"%02d-%02d", msg_time_tm.month(),msg_time_tm.day_of_month());
		}
		else
		{
			show_data = nbase::StringPrintf(L"%04d-%02d-%02d", msg_time_tm.year(),msg_time_tm.month(),msg_time_tm.day_of_month());
		}
	}

	show_time = nbase::StringPrintf(L"%02d:%02d", msg_time_tm.hour(), msg_time_tm.minute());

	if (abbreviate)
	{
		if (bToday)
		{
			return show_time;
		}
		else
		{
			return show_data;
		}
	}
	else
	{
		return show_data + L" " + show_time;
	}
}

bool CheckIfShowTime(const long long old_timestamp, const long long new_timestamp)
{
	static const int kStampStep = 5 * 60 * 1000; //ms
	if (old_timestamp + kStampStep <= new_timestamp)
	{
		return true;
	}
	return false;
}

std::wstring CheckMsgContent(const std::wstring &content)
{
	std::wstring str = content;
	nbase::StringReplaceAll(L"\r\n", L"", str);
	nbase::StringReplaceAll(L"\r", L"", str);
	nbase::StringReplaceAll(L"\n", L"", str);
	nbase::StringReplaceAll(L"\t", L"", str);
	nbase::StringReplaceAll(L"\v", L"", str);
	nbase::StringReplaceAll(L"\f", L"", str);
	nbase::StringReplaceAll(L" ", L"", str);

	return str;
}

std::string GetFileMD5( const std::wstring &file )
{
	std::string utf8 = nbase::UTF16ToUTF8(file);
	return nim::Tool::GetFileMd5(utf8);
}

void GenerateUploadImage( const std::wstring &src, const std::wstring &dest )
{
	std::wstring mime_type = ZoomImage::GetMimeType(src);
	if( mime_type.empty() )
		return;

	if( mime_type == kImageGIF )
	{
		nbase::CopyFile(src, dest);
	}
	else
	{
		std::wstring str = src;

		ZoomImage zoom;
		zoom.SetImagePath(dest);
		zoom.SetAutoZoom(true, 1280, 1280);
		if( zoom.Zoom(str, mime_type == kImagePNG ? kImagePNG : kImageJPEG) )
			return;
		else if( zoom.ConvertImageFormat(str, mime_type == kImagePNG ? kImagePNG : kImageJPEG) )
			return;
		else
		{
			assert(0);
			nbase::CopyFile(src, dest);
		}
	}
}

bool InsertFaceToEdit(ui::RichEdit* edit, const std::wstring &file_name, const std::wstring &tag)
{
	bool ret = false;
	std::wstring file = QPath::GetAppPath();
	file.append(L"res\\emoji\\" + file_name);
	if (nbase::FilePathIsExist(file, false))
	{
		ITextServices* service = edit->GetTextServices();
		ret = Re_InsertFace(service, file, tag);
		if (!ret)
		{
			QLOG_ERR(L"insert emoj {0} {1} fail") << tag << file;
		}
		service->Release();
	}
	else
	{
		QLOG_ERR(L"emoj {0} {1} miss") << tag << file;		
	}
	return ret;
}

//处理可能内嵌的情况，"[[[大笑]" 有表情返回true
bool _FindEmoji(ui::RichEdit* edit, const std::wstring &str)
{
	const wchar_t kCp = L'[';
	bool ret = false;
	std::wstring txt, emo, file;

	size_t p = str.rfind(kCp);
	assert(p != str.npos);
	if (p == 0)
		emo = str;
	else
	{
		txt = str.substr(0, p);
		emo = str.substr(p);
	}

	edit->ReplaceSel(txt, false);
	if (emoji::GetEmojiFileByTag(emo, file))
	{
		ret = InsertFaceToEdit(edit, file, emo);
	}		
	else
	{
		edit->ReplaceSel(emo, false);
		ret = false;
	}		
	return ret;
}

//表情格式 "[大笑]"
int InsertTextToEdit(ui::RichEdit* edit, const std::wstring &str)
{
	const wchar_t kCp = L'[', kCq = L']';
	int ret = 0;
	size_t p1 = 0, p2 = 0, q = 0, len = str.size();
	std::wstring emo, file, txt;
	while (p1 < len)
	{
		//查找"["
		p2 = str.find(kCp, p1);
		if (p2 == str.npos)
		{
			txt = str.substr(p1);
			edit->ReplaceSel(txt, false);
			break;
		}
		else
		{
			if (p2 > p1)
			{
				txt = str.substr(p1, p2 - p1);
				edit->ReplaceSel(txt, false);
			}
			//查找"]"
			q = str.find(kCq, p2);
			if (q == str.npos)
			{
				txt = str.substr(p2);
				edit->ReplaceSel(txt, false);
				break;
			}
			else
			{
				emo = str.substr(p2, q - p2 + 1);
				ret += _FindEmoji(edit, emo) ? 1 : 0;

				p1 = q + 1;
			}
		}
	}
	return ret;
}

void InsertImageToEdit(ui::RichEdit* edit, const std::wstring& image_src, bool loading)
{
	if (edit == NULL)
	{
		ASSERT(0);
		return;
	}

	bool ret = false;
	ITextServices *text_service = edit->GetTextServices();
	long start_char = 0, end_char = 0;
	if (text_service != NULL)
	{
		Re_GetSel(text_service, start_char, end_char);
		Re_SetSel(text_service, end_char, end_char);
		ret = Re_InsertImage(text_service, InsertCustomItemErrorCallback(), true, image_src, L"", loading);

		text_service->Release();
	}

	edit->SetFocus();
}

void InsertFileToEdit(ui::RichEdit* edit, const std::wstring& file_path)
{
	if (edit == NULL || file_path.empty())
	{
		ASSERT(0);
		return;
	}

	ITextServices *text_service = edit->GetTextServices();
	if (text_service != NULL)
	{
		long start_char = 0, end_char = 0;
		Re_GetSel(text_service, start_char, end_char);
		Re_SetSel(text_service, end_char, end_char);
		if (Re_InsertFile(text_service, InsertCustomItemErrorCallback(), file_path))
		{
			//Re_ReplaceSel(text_service, L"\r\n", true);
		}
	}

	if (text_service != NULL)
		text_service->Release();

	edit->SetFocus();
}

void OpenMap(const std::string& title, const std::string& content, const std::string& point)
{
	static const wchar_t kMapExampPath[] = L"res/web/map_marker.html";
	static const wchar_t kMapUserPath[] = L"map_marker.html";

	std::string title_info = title;
	std::string content_info = content;
	if (content_info.empty())
	{
		std::string value = title;
		int left_pos = value.find(' ');
		if(left_pos > 0)
		{
			title_info = value.substr (0, left_pos);
			content_info = value.substr (left_pos+1, value.size()-left_pos-1);
		}
	}
	std::wstring map_path;

	std::wstring app = QPath::GetAppPath();
	std::wstring path_examp = nbase::StringPrintf(L"%s%s", app.c_str(), kMapExampPath);

	std::string file_data;
	if( nbase::ReadFileToString(path_examp, file_data) )
	{
		std::string file_data2;
		nbase::StringPrintf(file_data2, file_data.c_str(),title_info.c_str(), content_info.c_str(), point.c_str());

		std::wstring user_data_path = GetUserDataPath();;
		std::wstring path_user = nbase::StringPrintf(L"%s%s", user_data_path.c_str(), kMapUserPath);
		if( nbase::WriteFile(path_user, file_data2) > 0)
		{
			map_path = path_user;
		}
	}
	if( !map_path.empty() )
	{
		Post2GlobalMisc( nbase::Bind(&shared::tools::SafeOpenUrl, map_path, SW_SHOW) );
	}
}

bool IsResourceExist(const nim::IMMessage &msg)
{
	std::wstring wpath = nbase::UTF8ToUTF16(msg.local_res_path_);

	if (wpath.empty() || !nbase::FilePathIsExist(wpath, false))
	{
		std::string path = nim::Talk::GetAttachmentPathFromMsg(msg);
		wpath = nbase::UTF8ToUTF16(path);
		return nbase::FilePathIsExist(wpath, false);
	}
	else
	{
		return true;
	}
}

void CustomSysMessageToIMMessage(const nim::SysMessage &sys_msg, nim::IMMessage &msg)
{
	//无法对等
	//msg.attach_ = sys_msg.apns_text_;

	//这个方法只能再要显示在ui前调用
	msg.receiver_accid_ = sys_msg.receiver_accid_;
	msg.sender_accid_ = sys_msg.sender_accid_;
	msg.timetag_ = sys_msg.timetag_;
	msg.content_ = sys_msg.content_;
	msg.attach_ = sys_msg.attach_;
	msg.readonly_server_id_ = sys_msg.id_;
	msg.msg_setting_.server_history_saved_ = sys_msg.msg_setting_.need_offline_;
	msg.rescode_ = sys_msg.rescode_;
	msg.feature_ = sys_msg.feature_;
	msg.session_type_ = sys_msg.type_ == nim::kNIMSysMsgTypeCustomP2PMsg ? nim::kNIMSessionTypeP2P : nim::kNIMSessionTypeTeam;
}

bool StringToJson(const std::string &str, Json::Value &json)
{
	Json::Reader reader;
	return reader.parse(str, json);
}

bool IsNoticeMsg(const nim::IMMessage& msg)
{
	if (msg.type_ == nim::kNIMMessageTypeNotification || msg.type_ == nim::kNIMMessageTypeTips)
		return true;

	if (msg.type_ == nim::kNIMMessageTypeCustom)
	{
		Json::Value json;
		if (StringToJson(msg.attach_, json) && json.isObject())
		{
			int sub_type = json["type"].asInt();
			if (sub_type == CustomMsgType_Rts)
				return true;
		}
	}

	return false;
}

void GetNotifyMsg(const std::string& msg_attach, const std::string& from_account, const std::string& to_account, std::wstring &show_text, const std::string& session_id)
{
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	show_text.clear();

	Json::Value json;
	if (StringToJson(msg_attach, json) && json.isObject())
	{
		nim::NIMNotificationId id = (nim::NIMNotificationId)json[nim::kNIMNotificationKeyId].asInt();

		//test
		if (json[nim::kNIMNotificationKeyData].isMember("attach"))
		{
			if (id == nim::kNIMNotificationIdTeamKick)
			{
				QLOG_APP(L"GetNotifyMsg Kick : {0}") << json[nim::kNIMNotificationKeyData]["attach"].asString();
			}
			else if (id == nim::kNIMNotificationIdTeamInvite)
			{
				QLOG_APP(L"GetNotifyMsg invite : {0}") << json[nim::kNIMNotificationKeyData]["attach"].asString();
			}
		}

		std::wstring team_type = mls->GetStringViaID(TeamService::GetInstance()->GetTeamType(session_id) == 0 ? L"STRID_SESSION_ITEM_GROUP" : L"STRID_SESSION_ITEM_TEAM");
		std::wstring you = mls->GetStringViaID(L"STRID_SESSION_ITEM_YOU");
		std::wstring admin = mls->GetStringViaID(L"STRID_SESSION_ITEM_ADMIN");

		SessionBox* session_wnd = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(session_id));

		std::wstring from_name;
		if (LoginManager::GetInstance()->IsEqual(from_account))
			from_name = you;
		else
		{
			if (session_wnd)
				from_name = nbase::UTF8ToUTF16(session_wnd->GetTeamMemberInfo(from_account).GetNick());
			if (from_name.empty())
				from_name = UserService::GetInstance()->GetUserName(from_account);
		}

		std::string obj_account = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataId].asString();
		std::wstring obj_name;
		if (obj_account.empty())
			obj_name = L"";
		else if (LoginManager::GetInstance()->IsEqual(obj_account))
			obj_name = you;
		else
		{
			if (session_wnd)
				obj_name = nbase::UTF8ToUTF16(session_wnd->GetTeamMemberInfo(obj_account).GetNick());
			if (obj_name.empty())
				obj_name = UserService::GetInstance()->GetUserName(obj_account);
		}

		if (id == nim::kNIMNotificationIdTeamMuteMember)
		{
			bool set_mute = json[nim::kNIMNotificationKeyData]["mute"].asInt() == 1;
			if (set_mute)
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_MUTED").c_str(), obj_name.c_str(), LoginManager::GetInstance()->IsEqual(from_account) ? you.c_str() : admin.c_str());
			else
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UNMUTED").c_str(), obj_name.c_str(), LoginManager::GetInstance()->IsEqual(from_account) ? you.c_str() : admin.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamApplyPass)
		{
			if (from_account == obj_account) //此群允许任何人加入，有用户通过搜索高级群加入该群，这种情况下from_account等于uid，用户直接入群。
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_WELCOME").c_str(), obj_name.c_str());
			else
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_PASS_APPLY").c_str(), from_name.c_str(), obj_name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamInviteAccept)
		{
			std::string invitor_id = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyUserNameCards][1]["accid"].asString();
			std::wstring invitor_name;
			if (invitor_id.empty())
				invitor_name = admin;
			else if (LoginManager::GetInstance()->IsEqual(invitor_id))
				invitor_name = you;
			else
			{
				if (session_wnd)
					invitor_name = nbase::UTF8ToUTF16(session_wnd->GetTeamMemberInfo(invitor_id).GetNick());
				if (invitor_name.empty())
					invitor_name = UserService::GetInstance()->GetUserName(invitor_id);
			}
			show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_ACCEPT_INVITE").c_str(), from_name.c_str(), invitor_name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamLeave)
		{
			show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_QUIT_TEAM").c_str(), from_name.c_str(), team_type.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamUpdate)
		{
			Json::Value tinfo_json = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyTeamInfo];
			if (tinfo_json.isMember(nim::kNIMTeamInfoKeyName))
			{
				std::wstring team_name = nbase::UTF8ToUTF16(tinfo_json[nim::kNIMTeamInfoKeyName].asString());
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UPDATE_TEAM_NAME").c_str(), team_type.c_str(), team_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyAnnouncement))
			{
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UPDATE_TEAM_BOARD").c_str(), from_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyJoinMode))
			{
				std::string mode = tinfo_json[nim::kNIMTeamInfoKeyJoinMode].asString();
				show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_UPDATE_VERIFY_MODE");

				if (mode == "0")
					show_text += mls->GetStringViaID(L"STRID_TEAM_INFO_ALLOW_ANYBODY");
				else if (mode == "1")
					show_text += mls->GetStringViaID(L"STRID_TEAM_INFO_NEED_VERIFY");
				else if (mode == "2")
					show_text += mls->GetStringViaID(L"STRID_TEAM_INFO_FORBID_EVERYBODY");
				else
					show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_VERIFY_MODE_UPDATED");
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyIntro))
			{
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UPDATE_TEAM_INTRO").c_str(), from_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyMuteAll))
			{
				bool mute_all = tinfo_json[nim::kNIMTeamInfoKeyMuteAll].asUInt() == 1;
				if (mute_all)
					show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_MUTE_ALL").c_str(), from_name.c_str());
				else
					show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UNMUTE_ALL").c_str(), from_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyIcon))
			{
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UPDATE_TEAM_ICON").c_str(), from_name.c_str());
			}
			else
			{
				show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_UPDATE_TEAM_INFO").c_str(), from_name.c_str(), team_type.c_str());
			}

		}
		else if (id == nim::kNIMNotificationIdTeamDismiss)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_TEAM_DISSOLVED");
		}
		else if (id == nim::kNIMNotificationIdTeamOwnerTransfer)
		{
			show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_TRANSFER_TEAM").c_str(), from_name.c_str(), obj_name.c_str());
		}
		else if (id == nim::kNIMNotificationIdNetcallBill || \
			id == nim::kNIMNotificationIdNetcallMiss || \
			id == nim::kNIMNotificationIdLocalNetcallReject || \
			id == nim::kNIMNotificationIdLocalNetcallNoResponse)
		{
			//未接电话,{"time":139323423424,"calltype":1,"from":"account_temp","channel":6144978055925334000}
			//话单,{"time":139323423424,"calltype":1,"duration":5,"channel":6144978055925334000}
			int type = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallTypeKey].asUInt();
			std::string creater = from_account;
			bool b_create = LoginManager::GetInstance()->IsEqual(creater);

			std::wstring chat_mode;
			if (type == nim::kNIMRtsVideoChatModeAudio)
			{
				chat_mode = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_AUDIO_CHAT");
			}
			else
			{
				chat_mode = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_VIDEO_CHAT");
			}

			std::wstring time_tip;
			if (id == nim::kNIMNotificationIdNetcallMiss)
			{
				time_tip = mls->GetStringViaID(L"STRID_SESSION_ITEM_UNLISTENED");
			}
			else if (id == nim::kNIMNotificationIdNetcallBill)
			{
				__int64 duration = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallDurationKey].asInt64();
				int h = (int)(duration / 3600), m = (int)((duration % 3600) / 60), s = (int)(duration % 60);
				if (h > 0)
					nbase::StringPrintf(time_tip, mls->GetStringViaID(L"STRID_SESSION_ITEM_HMS").c_str(), h, m, s);
				else if (m > 0)
				{
					nbase::StringPrintf(time_tip, mls->GetStringViaID(L"STRID_SESSION_ITEM_MINUTE_SECOND").c_str(), m, s);
				}
				else
					nbase::StringPrintf(time_tip, mls->GetStringViaID(L"STRID_SESSION_ITEM_SECOND").c_str(), s);
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallReject)
			{
				if (b_create)
				{
					time_tip = mls->GetStringViaID(L"STRID_SESSION_ITEM_OTHER_BUSY");
				}
				else
				{
					time_tip = mls->GetStringViaID(L"STRID_SESSION_ITEM_REFUSE_LISTEN");
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallNoResponse)
			{
				time_tip = mls->GetStringViaID(L"STRID_SESSION_ITEM_UNCONNECTED");
			}

			show_text = nbase::StringPrintf(L"%s %s", chat_mode.c_str(), time_tip.c_str());
		}
		else
		{
			Json::Value array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataIds];
			if (!array.empty() && array.isArray())
			{
				int len = array.size();
				std::vector<std::string> ids;
				for (int i = 0; i < len; i++)
				{
					ids.push_back(array[i].asString());
				}
				if (!ids.empty())
				{
					std::wstring obj;
					int n = ids.size();
					int i = 0;
					for (; i < n && i < 3; i++)
					{
						if (!obj.empty())
							obj.append(L", ");

						std::wstring show_name;
						if (LoginManager::GetInstance()->IsEqual(ids[i]))
							show_name = you;
						else
						{
							std::string team_nick;
							if (session_wnd)
								team_nick = session_wnd->GetTeamMemberInfo(ids[i]).GetNick();
							show_name = team_nick.empty() ? UserService::GetInstance()->GetUserName(ids[i]) : nbase::UTF8ToUTF16(team_nick);
						}
						obj.append(show_name);
					}
					if (i < n - 1)
					{
						obj.append(nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_SEVERAL").c_str(), n));
					}

					if (id == nim::kNIMNotificationIdTeamInvite)
						show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_INVITED").c_str(), from_name.c_str(), obj.c_str(), team_type.c_str());
					else if (id == nim::kNIMNotificationIdTeamKick)
						show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_KICKED").c_str(), obj.c_str(), team_type.c_str());
					else if (id == nim::kNIMNotificationIdTeamAddManager)
						show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_APPOINT_ADMIN").c_str(), obj.c_str());
					else if (id == nim::kNIMNotificationIdTeamRemoveManager)
						show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_DISMISS_ADMIN").c_str(), obj.c_str());
				}
			}
		}
	}
	if (show_text.empty())
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_TIP");
}

bool IsNetCallMsg(nim::NIMMessageType msg_type, const std::string& msg_attach)
{
	if (msg_type != nim::kNIMMessageTypeNotification)
	{
		return false;
	}
	Json::Value valus;
	Json::Reader reader;
	if (reader.parse(msg_attach, valus) && valus.isObject() && valus[nim::kNIMNotificationKeyData].isObject())
	{
		nim::NIMNotificationId id = (nim::NIMNotificationId)valus[nim::kNIMNotificationKeyId].asInt();
		if (id == nim::kNIMNotificationIdNetcallBill || \
			id == nim::kNIMNotificationIdNetcallMiss || \
			id == nim::kNIMNotificationIdLocalNetcallReject || \
			id == nim::kNIMNotificationIdLocalNetcallNoResponse)
		{
			return true;
		}
	}

	return false;
}

std::wstring GetCustomMsg(const std::string &sender_accid, const std::string &msg_attach)
{
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	std::wstring show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_CUSTOM_MSG");
	Json::Value json;
	if (StringToJson(msg_attach, json) && json.isObject())
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_Jsb) //finger
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_JSB");
		}
		else if (sub_type == CustomMsgType_SnapChat)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_SNAPCHAT");
		}
		else if (sub_type == CustomMsgType_Sticker)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_CHARTLET");
		}
		else if (sub_type == CustomMsgType_Rts)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_RTS");
			if (json["data"].isObject())
			{
				int flag = json["data"]["flag"].asInt();
				if (flag == 0)
				{
					if (LoginManager::GetInstance()->IsEqual(sender_accid))
					{
						show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_I_START_RTS");
					}
					else
					{
						nim::UserNameCard card;
						UserService::GetInstance()->GetUserInfo(sender_accid, card);
						show_text = nbase::UTF8ToUTF16(card.GetName()) + L" " + mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_OTHER_START_RTS");
					}
				}
				else if (flag == 1)
				{
					show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RTS_END");
				}
			}
		}
	}
	return show_text;
}

std::wstring GetRecallNotifyText(const std::string& session_id, nim::NIMSessionType session_type, const std::string& msg_from_id, const std::string& msg_from_nick)
{
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	std::wstring show_text;
	if (msg_from_id == LoginManager::GetInstance()->GetAccount())
	{
		show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_I_RECALL_MSG");
	}
	else
	{
		if (session_type == nim::kNIMSessionTypeP2P)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_OTHER_RECALL_MSG");
		}
		else
		{
			UTF8String name;
			if (!msg_from_nick.empty())
			{
				name = msg_from_nick;
			}
			else
			{
				auto info = nim::Team::QueryTeamMemberBlock(session_id, msg_from_id);
				name = info.GetNick();
				if (name.empty())
				{
					nim::UserNameCard name_card;
					UserService::GetInstance()->GetUserInfo(msg_from_id, name_card);
					name = name_card.GetName();
				}
				if (name.empty())
					name = msg_from_id;
			}

			show_text = nbase::UTF8ToUTF16(name) + L" " + mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_MSG");
		}
	}

	return show_text;
}
struct GetTeamMemberName
{
	GetTeamMemberName(const std::string& def_name) : def_name_(def_name){}
	virtual const std::string operator() (const std::string& tid, const std::string& accid) const
	{
		if (!def_name_.empty())
			return def_name_;
		std::string name("");
		auto info = nim::Team::QueryTeamMemberBlock(tid, accid);
		if (name.empty())
			name = info.GetNick();
		if (name.empty())
		{
			nim::UserNameCard name_card;
			UserService::GetInstance()->GetUserInfo(accid, name_card);
			name = name_card.GetName();
		}
		return name;
	}
private:
	const std::string& def_name_;
};
std::wstring GetRecallNotifyTextEx(const std::string& session_id, nim::NIMSessionType session_type, const std::string& msg_from_id, const std::string& msg_oprator_id, const std::string& msg_from_nick)
{
	ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
	std::wstring show_text;
	if (session_type == nim::kNIMSessionTypeP2P)
	{
		if (msg_from_id == LoginManager::GetInstance()->GetAccount())
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_I_RECALL_MSG");
		else
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_OTHER_RECALL_MSG");
	}
	else if (session_type == nim::kNIMSessionTypeTeam)
	{
		if (msg_oprator_id == LoginManager::GetInstance()->GetAccount())
		{
			if (msg_from_id == msg_oprator_id)
				show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_I_RECALL_MSG");
			else
				show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_BY_ME_MSG");
		}
		else
		{
			if (msg_from_id != msg_oprator_id)
			{
				auto info = nim::Team::QueryTeamMemberBlock(session_id, msg_oprator_id);
				if (info.GetUserType() == nim::kNIMTeamUserTypeCreator)
					show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_BY_CREATOR_MSG").c_str(), \
					nbase::UTF8ToUTF16(GetTeamMemberName(/*msg_from_nick*/"")(session_id, msg_oprator_id)).c_str());
				else if (info.GetUserType() == nim::kNIMTeamUserTypeManager)
					show_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_BY_ADMIN_MSG").c_str(), \
					nbase::UTF8ToUTF16(GetTeamMemberName(/*msg_from_nick*/"")(session_id, msg_oprator_id)).c_str());
				else
					show_text = nbase::UTF8ToUTF16(GetTeamMemberName(/*msg_from_nick*/"")(session_id, msg_from_id)) + L" " + \
					mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_MSG");

			}
			else
			{
				show_text = nbase::UTF8ToUTF16(GetTeamMemberName(msg_from_nick)(session_id, msg_from_id)) + L" " + \
					mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_MSG");
			}
		}
	}
	return show_text;
}
}