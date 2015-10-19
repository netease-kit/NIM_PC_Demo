#include "msg_record.h"

using namespace ui;

namespace nim_comp
{
const int kMsgLogNumberShow = 20;

void MsgRecordForm::ShowMsg(const MsgData &msg, bool first, bool show_time)
{
	const std::string &bubble_id = msg.client_msg_id;
	if(bubble_id.empty())
	{
		QLOG_WAR(L"msg id empty");
		return;
	}

	IdBubblePair::iterator it = id_bubble_pair_.find(bubble_id);
	if(it != id_bubble_pair_.end())
	{
		QLOG_WAR(L"repeat msg: {0}") <<bubble_id;
		return;
	}

	MsgBubbleItem* item = NULL;

	int type = msg.msg_type;
	if (type == nim::kNIMMessageTypeText)
		item = new MsgBubbleText;
	else if (type == nim::kNIMMessageTypeImage)
		item = new MsgBubbleImage;
	else if (type == nim::kNIMMessageTypeAudio)
		item = new MsgBubbleAudio;
	else if (type == nim::kNIMMessageTypeFile)
		item = new MsgBubbleFile;
	else if (type == nim::kNIMMessageTypeLocation)
		item = new MsgBubbleLocation;
	else if (type == nim::kNIMMessageTypeNotification)
	{
		id_bubble_pair_[bubble_id] = NULL;

		MsgBubbleNotice* cell = new MsgBubbleNotice;
		GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
		if(first)
			msg_list_->AddAt(cell, 0);
		else
			msg_list_->Add(cell);
		cell->InitControl();
		cell->InitInfo(msg);
		return;
	}
	else if (type == nim::kNIMMessageTypeCustom)
	{
		Json::Value json;
		if (StringToJson(msg.msg_attach, json))
		{
			int sub_type = json["type"].asInt();
			if (sub_type == CustomMsgType_Jsb) //finger
			{
				item = new MsgBubbleFinger;
			}
			else if (sub_type == CustomMsgType_SnapChat)
			{
				item = new MsgBubbleSnapChat;
			}
			else if (sub_type == CustomMsgType_Sticker)
			{
				item = new MsgBubbleSticker;
			}
			else if (sub_type == CustomMsgType_Rts)
			{
				if (json["data"].isObject())
				{
					int flag = json["data"]["flag"].asInt();
					if (flag == 0)
					{
						item = new MsgBubbleText;
					}
					else if (flag == 1)
					{
						id_bubble_pair_[bubble_id] = NULL;

						MsgBubbleNotice* cell = new MsgBubbleNotice;
						GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
						if (first)
							msg_list_->AddAt(cell, 0);
						else
							msg_list_->Add(cell);
						cell->InitControl();
						cell->InitInfo(msg);
						return;
					}
				}
			}
		}
	}

	if (item == nullptr)
	{
		QLOG_WAR(L"unknown msg: cid={0} msg_type={1}") << bubble_id << type;
		item = new MsgBubbleUnknown;
	}

	bool bubble_right = IsBubbleRight(msg);
	if(bubble_right)
		GlobalManager::FillBoxWithCache( item, L"session/bubble_right.xml" );
	else
		GlobalManager::FillBoxWithCache( item, L"session/bubble_left.xml" );

	if(first)
		msg_list_->AddAt(item, 0);
	else
		msg_list_->Add(item);

	id_bubble_pair_[bubble_id] = item;

	std::string sid = nbase::UTF16ToUTF8(GetWindowClassName());

	item->InitControl(bubble_right);
	item->InitInfo(msg);
	item->SetSessionId(sid);
	item->SetSessionType(session_type_);
	item->SetActionMenu(false);
	item->SetShowTime(show_time);
	if (bubble_right || msg.to_type == nim::kNIMSessionTypeP2P)
		item->SetShowName(false, "");
	else
		item->SetShowName(true, msg.from_nick);


	if (type == nim::kNIMMessageTypeAudio)
		item->SetPlayed(true);

	if( item->NeedDownloadResource() )
	{
		Json::Value json;
		MsgToJson(msg, json);
		nim::Http::FetchMedia(json.toStyledString(), nbase::Bind(&MsgRecordForm::OnDownloadCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), nim::Http::FetchMediaProgressCallback());
	}
}

void MsgRecordForm::RefreshRecord(std::string id, nim::NIMSessionType type)
{
	if(id == session_id_)
		return;
	session_id_ = id;
	session_type_ = type;

	//初始化
	first_show_msg_ = true;
	last_msg_time_ = 0;
	farst_msg_time_ = 0;
	last_server_id_ = 0;

	has_more_ = true;
	is_loading_ = false;

	std::wstring name;
	if (type == nim::kNIMSessionTypeP2P)
		name = UserService::GetInstance()->GetUserName(id);
	else
		name = TeamService::GetInstance()->GetTeamName(id);
	label_title_->SetText(name);
	SetTaskbarTitle(name);

	msg_list_->RemoveAll();
	id_bubble_pair_.clear();

	std::string play_sid = AudioCallback::GetPlaySid();
	std::wstring wsid = nbase::UTF8ToUTF16(play_sid);
	if(GetWindowId() == wsid)
	{
		AudioCallback::SetPlaySid("");
		AudioCallback::SetPlayCid("");

		nim::Audio::StopPlayAudio();
	}

	ShowMore(false);
}

void MsgRecordForm::ShowMore(bool more)
{
	is_loading_ = true;
	QLOG_APP(L"query online msg begin: id={0} type={1} last_time={2} last_server_id={3}") <<session_id_ <<session_type_ <<farst_msg_time_ <<last_server_id_;
	nim::MsgLog::QueryMsgCallback cb = nbase::Bind(&MsgRecordForm::QueryMsgOnlineCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	nim::MsgLog::QueryMsgOnlineAsync(session_id_, session_type_, kMsgLogNumberShow, 0, farst_msg_time_, last_server_id_, false, true, cb);
}

void MsgRecordForm::ShowMsgs(const std::vector<MsgData> &msg)
{
	int pos = msg_list_->GetScrollRange().cy - msg_list_->GetScrollPos().cy;

	bool show_time = false;
	//msg倒序排列
	size_t len = msg.size();
	for(size_t i = 0; i < len; i++)
	{
		if(len == 1 || i == len-1)
		{
			show_time = true;
		}
		else
		{
			show_time = CheckIfShowTime(msg[i+1].msg_time, msg[i].msg_time);
		}
		ShowMsg(msg[i], true, show_time);
	}
	//修正最近时间
	if(first_show_msg_)
	{
		first_show_msg_ = false;

		msg_list_->EndDown(true, false);

		if(len > 0 && last_msg_time_ == 0)
			last_msg_time_ = msg[0].msg_time;
	}
	else
	{
		msg_list_->SetPos( msg_list_->GetPos() );

		ui::CSize sz = msg_list_->GetScrollPos();
		sz.cy = msg_list_->GetScrollRange().cy - pos;
		msg_list_->SetScrollPos(sz);
	}
	//修正最远时间
	if(len > 0)
	{
		farst_msg_time_ = msg[len-1].msg_time;
		last_server_id_ = msg[len-1].server_msg_id;
	}
}

void MsgRecordForm::QueryMsgOnlineCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type, const std::string& result)
{
	QLOG_APP(L"query online msg end: code={0} id={1} type={2}") <<code <<id <<type;


	if (id != session_id_)
		return;
	is_loading_ = false;
	LoadingTip(false);

	if (code == nim::kNIMResSuccess)
	{
		Json::Value json;
		if (StringToJson(result, json))
		{
			std::vector<MsgData> vec;

			Json::Value& msg = json[nim::kNIMMsglogQueryKeyContent];
			for (size_t i = 0; i < msg.size(); i++)
			{
				MsgData md;
				JsonToMsg(msg[i], md);
				vec.push_back(md);
			}

			ShowMsgs(vec);

			if (vec.size() < kMsgLogNumberShow)
			{
				has_more_ = false;
				return;
			}
		}
	}
}

void MsgRecordForm::OnDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if(item)
		{
			item->OnDownloadCallback(code == nim::kNIMResSuccess);
		}
	}
}

void MsgRecordForm::OnPlayAudioCallback( const std::string &cid, int code )
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleAudio* item = dynamic_cast<MsgBubbleAudio*>(it->second);
		if(item)
		{
			item->OnPlayCallback(code);
		}
	}
	if (code != nim::kSuccess)
	{
		std::wstring tip = nbase::StringPrintf(L"语音播放失败，错误码：%d", code);
		ShowMsgBox(m_hWnd, tip, MsgboxCallback(), L"提示", L"确定", L"");
	}
}

void MsgRecordForm::OnStopAudioCallback( const std::string &cid, int code )
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleAudio* item = dynamic_cast<MsgBubbleAudio*>(it->second);
		if(item)
		{
			item->OnStopCallback(code);
		}
	}
}
}