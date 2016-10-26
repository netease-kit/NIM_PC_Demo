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
	static const std::wstring weekday[8] = { L"", L"星期一", L"星期二", L"星期三", L"星期四", L"星期五", L"星期六", L"星期日" };
	
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

	if (now_time_tm.year() == msg_time_tm.year() 
		&& now_time_tm.month() == msg_time_tm.month() 
		&& now_time_tm.day_of_month() == msg_time_tm.day_of_month())//today
	{
		bToday = true;
		show_data = L"今天";
	}
	else if (yesterday_time_tm.year() == msg_time_tm.year() 
		&& yesterday_time_tm.month() == msg_time_tm.month() 
		&& yesterday_time_tm.day_of_month() == msg_time_tm.day_of_month())//yesterday
	{
		show_data = L"昨天";
	}
	else if (preyesterday_time_tm.year() == msg_time_tm.year() 
		&& preyesterday_time_tm.month() == msg_time_tm.month() 
		&& preyesterday_time_tm.day_of_month() == msg_time_tm.day_of_month())//preyesterday
	{
		show_data = L"前天";
	}
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

void InsertFaceToEdit(ui::RichEdit* edit, const std::wstring &file_name, const std::wstring &tag)
{
	std::wstring file = QPath::GetAppPath();
	file.append(L"res\\emoji\\" + file_name);
	if (nbase::FilePathIsExist(file, false))
	{
		ITextServices* service = edit->GetTextServices();
		if (!Re_InsertFace(service, file, tag))
		{
			QLOG_ERR(L"insert emoj {0} {1} fail") << tag << file;
		}
		service->Release();
	}
	else
	{
		QLOG_ERR(L"emoj {0} {1} miss") << tag << file;
	}
}

//处理可能内嵌的情况，"[[[大笑]"
void _FindEmoji(ui::RichEdit* edit, const std::wstring &str)
{
	const wchar_t kCp = L'[';

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
		InsertFaceToEdit(edit, file, emo);
	else
		edit->ReplaceSel(emo, false);
}

//表情格式 "[大笑]"
void InsertTextToEdit(ui::RichEdit* edit, const std::wstring &str)
{
	const wchar_t kCp = L'[', kCq = L']';

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
				_FindEmoji(edit, emo);

				p1 = q + 1;
			}
		}
	}
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
		ret = Re_InsertImage(text_service, InsertCustomItemErrorCallback(), image_src, loading);

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
		nim::IMImage img;
		nim::Talk::ParseImageMessageAttach(msg, img);
		std::wstring filename = nbase::UTF8ToUTF16(img.md5_);
		wpath = GetUserImagePath() + filename;

		return nbase::FilePathIsExist(wpath, false);
	}
	else
	{
		return true;
	}
}

bool IsAtMeMsg(const nim::IMMessage &msg)
{
	// 是否包含atme消息，如果当前msg包含atme消息，就不显示提示条，否则显示
	if (msg.session_type_ == nim::kNIMSessionTypeTeam && msg.type_ == nim::kNIMMessageTypeText && !LoginManager::GetInstance()->IsEqual(msg.sender_accid_))
	{
		if (msg.msg_setting_.is_force_push_ == nim::BS_TRUE)
		{
			//@所有人
			if (msg.msg_setting_.force_push_ids_list_.empty())
			{
				return true;
			}
			else
			{
				for (auto &id : msg.msg_setting_.force_push_ids_list_)
				{
					if (LoginManager::GetInstance()->IsEqual(id))
						return true;
				}
			}
		}
	}

	return false;
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
		if (StringToJson(msg.attach_, json))
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
	show_text.clear();

	Json::Value json;
	if (StringToJson(msg_attach, json))
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

		std::wstring team_type = TeamService::GetInstance()->GetTeamType(session_id) == 0 ? L"讨论组" : L"群";
		SessionBox* session_wnd = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(session_id));

		std::wstring from_name;
		if (LoginManager::GetInstance()->IsEqual(from_account))
			from_name = L"你";
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
			obj_name = L"你";
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
				show_text = nbase::StringPrintf(L"%s 被 %s 禁言", obj_name.c_str(), LoginManager::GetInstance()->IsEqual(from_account) ? L"你" : L"管理员");
			else
				show_text = nbase::StringPrintf(L"%s 被 %s 解除禁言", obj_name.c_str(), LoginManager::GetInstance()->IsEqual(from_account) ? L"你" : L"管理员");
		}
		else if (id == nim::kNIMNotificationIdTeamApplyPass)
		{
			if (from_account == obj_account) //此群允许任何人加入，有用户通过搜索高级群加入该群，这种情况下from_account等于uid，用户直接入群。
				show_text = nbase::StringPrintf(L"欢迎 %s 进入群聊", obj_name.c_str());
			else
				show_text = nbase::StringPrintf(L"%s 通过了 %s 的入群申请", from_name.c_str(), obj_name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamInviteAccept)
		{
			std::string invitor_id = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyUserNameCards][1]["accid"].asString();
			std::wstring invitor_name;
			if (invitor_id.empty())
				invitor_name = L"管理员";
			else if (LoginManager::GetInstance()->IsEqual(invitor_id))
				invitor_name = L"你";
			else
			{
				if (session_wnd)
					invitor_name = nbase::UTF8ToUTF16(session_wnd->GetTeamMemberInfo(invitor_id).GetNick());
				if (invitor_name.empty())
					invitor_name = UserService::GetInstance()->GetUserName(invitor_id);
			}
			show_text = nbase::StringPrintf(L"%s 接受了 %s 的入群邀请", from_name.c_str(), invitor_name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamLeave)
		{
			show_text = nbase::StringPrintf(L"%s 离开了%s", from_name.c_str(), team_type.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamUpdate)
		{
			Json::Value tinfo_json = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyTeamInfo];
			if (tinfo_json.isMember(nim::kNIMTeamInfoKeyName))
			{
				std::wstring team_name = nbase::UTF8ToUTF16(tinfo_json[nim::kNIMTeamInfoKeyName].asString());
				show_text = nbase::StringPrintf(L"%s名称被更新为 %s", team_type.c_str(), team_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyAnnouncement))
			{
				show_text = nbase::StringPrintf(L"%s 更新了群公告", from_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyJoinMode))
			{
				std::string mode = tinfo_json[nim::kNIMTeamInfoKeyJoinMode].asString();
				show_text = L"群身份验证模式更新为";

				if (mode == "0")
					show_text += L"允许任何人加入";
				else if (mode == "1")
					show_text += L"需要验证消息";
				else if (mode == "2")
					show_text += L"不允许任何人申请加入";
				else
					show_text = L"群身份验证模式已更新";
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyIntro))
			{
				show_text = nbase::StringPrintf(L"%s 更新了群介绍", from_name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyMuteAll))
			{
				bool mute_all = tinfo_json[nim::kNIMTeamInfoKeyMuteAll].asUInt() == 1;
				if (mute_all)
					show_text = nbase::StringPrintf(L"%s 设置了全员禁言", from_name.c_str());
				else
					show_text = nbase::StringPrintf(L"%s 解除了全员禁言", from_name.c_str());
			}
			else
			{
				show_text = nbase::StringPrintf(L"%s 更新了%s信息", from_name.c_str(), team_type.c_str());
			}

		}
		else if (id == nim::kNIMNotificationIdTeamDismiss)
		{
			show_text = L"群已被解散";
		}
		else if (id == nim::kNIMNotificationIdTeamOwnerTransfer)
		{
			show_text = nbase::StringPrintf(L"%s 将群转让给 %s", from_name.c_str(), obj_name.c_str());
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
			std::wstring tm;// = GetMessageTime(time, false);
			if (type == nim::kNIMRtsVideoChatModeAudio)
			{
				show_text = tm + L"[语音通话]";
			}
			else
			{
				show_text = tm + L"[视频通话]";
			}
			if (id == nim::kNIMNotificationIdNetcallMiss)
			{
				show_text += L" 未接";
			}
			else if (id == nim::kNIMNotificationIdNetcallBill)
			{
				__int64 duration = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallDurationKey].asInt64();
				int h = (int)(duration / 3600), m = (int)((duration % 3600) / 60), s = (int)(duration % 60);
				std::wstring time_tip;
				if (h > 0)
					nbase::StringPrintf(time_tip, L"%d小时%d分%d秒", h, m, s);
				else if (m > 0)
				{
					nbase::StringPrintf(time_tip, L"%d分%d秒", m, s);
				}
				else
					nbase::StringPrintf(time_tip, L"%d秒", s);
				if (type == nim::kNIMRtsVideoChatModeAudio)
				{
					show_text = nbase::StringPrintf(L"%s 时长%s", show_text.c_str(), time_tip.c_str());
				}
				else
				{
					show_text = nbase::StringPrintf(L"%s 时长%s", show_text.c_str(), time_tip.c_str());
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallReject)
			{
				if (b_create)
				{
					show_text += L" 对方正忙";
				}
				else
				{
					show_text += L" 拒绝接听";
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallNoResponse)
			{
				show_text += L" 未接通，已取消";
			}

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
							obj.append(L"，");

						std::wstring show_name;
						if (LoginManager::GetInstance()->IsEqual(ids[i]))
							show_name = L"你";
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
						obj.append(nbase::StringPrintf(L"等%d人", n));
					}

					std::wstring ttype = TeamService::GetInstance()->GetTeamType(session_id) == 0 ? L"讨论组" : L"群聊";
					if (id == nim::kNIMNotificationIdTeamInvite)
						show_text = nbase::StringPrintf(L"%s 邀请 %s 加入了%s", from_name.c_str(), obj.c_str(), ttype.c_str());
					else if (id == nim::kNIMNotificationIdTeamKick)
						show_text = nbase::StringPrintf(L"%s 已被移出%s", obj.c_str(), ttype.c_str());
					else if (id == nim::kNIMNotificationIdTeamAddManager)
						show_text = nbase::StringPrintf(L"%s 被任命为管理员", obj.c_str());
					else if (id == nim::kNIMNotificationIdTeamRemoveManager)
						show_text = nbase::StringPrintf(L"%s 被取消管理员资格", obj.c_str());
				}
			}
		}
	}
	if (show_text.empty())
		show_text = L"[通知消息]";
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

std::wstring GetCustomMsg(const std::string &msg_attach)
{
	std::wstring show_text = L"[自定义消息]";
	Json::Value json;
	if (StringToJson(msg_attach, json))
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_Jsb) //finger
		{
			show_text = L"[猜拳]";
		}
		else if (sub_type == CustomMsgType_SnapChat)
		{
			show_text = L"[阅后即焚]";
		}
		else if (sub_type == CustomMsgType_Sticker)
		{
			show_text = L"[贴图]";
		}
		else if (sub_type == CustomMsgType_Rts)
		{
			show_text = L"[白板]";
			if (json["data"].isObject())
			{
				int flag = json["data"]["flag"].asInt();
				if (flag == 0)
				{
					show_text = L"我已发起了[白板演示]";
				}
				else if (flag == 1)
				{
					show_text = L"白板演示已结束";
				}
			}
		}
	}
	return show_text;
}

std::wstring GetRecallNotifyText(const std::string& session_id, nim::NIMSessionType session_type, const std::string& msg_from_id)
{
	std::wstring show_text;
	if (msg_from_id == LoginManager::GetInstance()->GetAccount())
	{
		show_text = L"我撤回了一条消息";
	}
	else
	{
		if (session_type == nim::kNIMSessionTypeP2P)
		{
			show_text = L"对方撤回了一条消息";
		}
		else
		{
			auto info = nim::Team::QueryTeamMemberBlock(session_id, msg_from_id);
			UTF8String name = info.GetNick();
			if (name.empty())
			{
				nim::UserNameCard name_card;
				UserService::GetInstance()->GetUserInfo(msg_from_id, name_card);
				name = name_card.GetName();
			}
			if (name.empty())
				name = msg_from_id;
			show_text = nbase::UTF8ToUTF16(name) + L" 撤回了一条消息";
		}
	}

	return show_text;
}

}