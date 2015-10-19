#include "session_util.h"
#include "shared/zoom_image.h"
#include "util/user.h"
#include "module/emoji/richedit_util.h"

namespace nim_comp
{
SessionType GetSessionType(const MsgData &msg)
{
	if (msg.to_type == nim::kNIMSessionTypeP2P)
		return kSessionDouble;
	else if (msg.to_type == nim::kNIMSessionTypeTeam)
		return kSessionTeam;
	else
	{
		assert(0);
		return kSessionDouble;
	}
}

std::string GetSessionId(const MsgData &msg)
{
	if (msg.to_type == nim::kNIMSessionTypeP2P)
	{
		if (LoginManager::GetInstance()->IsEqual(msg.from_account))
			return msg.to_account;
		else
			return msg.from_account;
	}
	else
		return msg.to_account;
}

bool IsBubbleRight( const MsgData &msg )
{
	if (LoginManager::GetInstance()->IsEqual(msg.from_account))
	{
		if (msg.to_account == msg.from_account)//给自己的其他端的消息
		{
			//nim::kNIMClientTypePCWindows
			if (msg.from_client_type != nim::kNIMClientTypePCWindows && msg.from_client_type != 0)
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
		show_data = GetWeekOfDate(msg_time_tm.day_of_week());
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


std::string GetFileMD5( const std::wstring &file )
{
	std::string utf8 = nbase::UTF16ToUTF8(file);
	const char* md5 = nim::Tool::GetFileMd5( utf8 );
	std::string out = std::string(md5);
	QString::NIMFreeBuf((char*)md5);
	return out;
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

void OpenMap( const std::string& title, const std::string& content, const std::string& point )
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

}