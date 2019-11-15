#include "msg_record_retweet.h"
#include "module/audio/audio_manager.h"
#include "module/multi_retweet/multi_retweet_manager.h"
using namespace ui;

namespace nim_comp
{
void MsgRecordRetweetForm::ShowMsg(const nim::IMMessage &msg, bool first, bool show_time)
{
	const std::string &bubble_id = msg.client_msg_id_;
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

	if (msg.type_ == nim::kNIMMessageTypeText 
		|| IsNetCallMsg(msg.type_, msg.attach_))
	{
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(msg.attach_, values)
			&& values.isObject()
			&& values.isMember("comment")
			&& values["comment"].asString() == "is_recall_notification")
		{
			MsgBubbleNotice* cell = new MsgBubbleNotice;
			GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
			if (first)
				msg_list_->AddAt(cell, 0);
			else
				msg_list_->Add(cell);
			cell->InitControl();
			cell->InitInfo(msg, session_id_, true);
			return;
		}
		else
		{
			item = new MsgBubbleText;
		}
	}
	else if (msg.type_ == nim::kNIMMessageTypeImage)
		item = new MsgBubbleImage;
	else if (msg.type_ == nim::kNIMMessageTypeAudio)
		item = new MsgBubbleAudio;
	else if (msg.type_ == nim::kNIMMessageTypeFile)
		item = new MsgBubbleFile;
	else if (msg.type_ == nim::kNIMMessageTypeLocation)
		item = new MsgBubbleLocation;
	else if (msg.type_ == nim::kNIMMessageTypeVideo)
		item = new MsgBubbleVideo;
	else if (msg.type_ == nim::kNIMMessageTypeNotification || msg.type_ == nim::kNIMMessageTypeTips)
	{
		id_bubble_pair_[bubble_id] = NULL;

		MsgBubbleNotice* cell = new MsgBubbleNotice;
		GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
		if(first)
			msg_list_->AddAt(cell, 0);
		else
			msg_list_->Add(cell);
		cell->InitControl();
		cell->InitInfo(msg, session_id_);
		return;
	}
	else if (msg.type_ == nim::kNIMMessageTypeCustom)
	{
		Json::Value json;
		if (StringToJson(msg.attach_, json) && json.isObject())
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
			else if (sub_type == CustomMsgType_MultiRetweet) 
			{
				item = new MsgBubbleMultiRetweet;
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
						cell->InitInfo(msg, session_id_);
						return;
					}
				}
			}
		}
	}

	if (item == nullptr)
	{
		QLOG_WAR(L"unknown msg: cid={0} msg_type={1}") << bubble_id << msg.type_;
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
	std::string show_name("");
	item->SetShowName(GetUserShowName(msg, show_name), show_name);

	if (msg.type_ == nim::kNIMMessageTypeAudio)
		item->SetPlayed(true);

	if (item->NeedDownloadResourceEx())
	{
		nim::NOS::FetchMedia(msg, nbase::Bind(&MsgRecordRetweetForm::OnDownloadCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), nim::NOS::ProgressCallback());
	}
}
bool MsgRecordRetweetForm::GetUserShowName(const nim::IMMessage &msg, std::string& show_name)
{
	if (IsBubbleRight(msg) || msg.session_type_ == nim::kNIMSessionTypeP2P)
		return false;
	else
	{
		auto get_name_task = [&](void)->std::string{
			auto iter = team_member_info_list_.find(msg.sender_accid_);
			if (iter != team_member_info_list_.cend() && !iter->second.GetNick().empty())//显示群名片
				return iter->second.GetNick();
			else//显示备注名或昵称				
				return nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(msg.sender_accid_));
		};
		if (msg.type_ != nim::kNIMMessageTypeRobot)
		{
			show_name = std::move(get_name_task());
		}
		else
		{
			show_name = "";
		}
		return true;
	}
}
void MsgRecordRetweetForm::ShowMsgs(const std::list<nim::IMMessage> &msg)
{
	msg_list_->RemoveAll();
	id_bubble_pair_.clear();
	msg_list_->SetScrollPosY(0);

	bool show_time = true;
	auto it = msg.begin();
	while (it != msg.end())
	{
		long long older_time = 0;
		decltype(it) it_temp = it;
		it_temp++;
		while (it_temp != msg.end())
		{
			if (!IsNoticeMsg(*it_temp) && !IsRTSMsg(it_temp->type_, it_temp->attach_))
			{
				older_time = it_temp->timetag_;
				break;
			}
			it_temp++;
		}
		show_time = CheckIfShowTime(older_time, it->timetag_);
		ShowMsg(*it, false, show_time);
		it++;
	}	
}
void MsgRecordRetweetForm::OnDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
{
	//if (sid != session_id_)
	//	return;

	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if(item)
		{
			item->OnDownloadCallback(code == nim::kNIMResSuccess, file_path);
		}
	}
}

void MsgRecordRetweetForm::OnPlayAudioCallback( const std::string &cid, int code )
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
	if (code != nim::kNIMResSuccess)
	{
		std::wstring tip = nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSG_RECORD_PLAY_AUDIO_ERROR").c_str(), code);
		ShowMsgBox(m_hWnd, MsgboxCallback(), tip, false);
	}
}

void MsgRecordRetweetForm::OnStopAudioCallback( const std::string &cid, int code )
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
void MsgRecordRetweetForm::SetInfo(const std::shared_ptr<MultiMessageRetweetData>& data, const std::string& session_id, nim::NIMSessionType session_type)
{
	data_ = data;
	session_type_ = session_type;
	session_id_ = session_id;
	std::wstring title = nbase::UTF8ToUTF16(data_->session_name_) + MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSG_RECORD_LOGS_NAME");
	SetTaskbarTitle(title);
	auto* title_ctrl = dynamic_cast<ui::Label*> (FindControl(L"form_title"));
	if(title_ctrl != nullptr)
		title_ctrl->SetText(title);
}

void MsgRecordRetweetForm::ShowMessageLogs()
{
	nim_comp::MultiMessageRetweetManager::GetInstance()->ParseMultiMessageRetweetData(data_, 
		ToWeakCallback([this](float progress) {
		std::wstring text(std::to_wstring((int)(progress * 100)));
		text.append(L"%");
	}),
		ToWeakCallback([this](bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list) {
		if (ret)
		{
			progress_box_->SetVisible(false);
			msg_list_->SetVisible(true);
			ShowMsgs(message_list);			
		}
		else
		{

		}
	}));
}
}