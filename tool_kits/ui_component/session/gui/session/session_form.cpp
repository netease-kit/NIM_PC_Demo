#include "session_form.h"
#include "custom_msg_form.h"
#include "module/session/session_manager.h"
#include "callback/session/session_callback.h"
#include "util/user.h"

using namespace ui;

namespace nim_comp
{
const int kMsgLogNumberShow = 20;

void SessionForm::AddNewMsg(const MsgData &msg, bool create)
{
	bool at_end = msg_list_->IsAtEnd();

	bool show_time = false;
	if (first_show_msg_)
	{
		show_time = true;
		farst_msg_time_ = msg.msg_time;
	}
	else
	{
		show_time = CheckIfShowTime(last_msg_time_, msg.msg_time);
	}
	ShowMsg(msg, false, show_time);
	last_msg_time_ = msg.msg_time;

	if (first_show_msg_)
	{
		first_show_msg_ = false;
		AddTip(STT_LOAD_MORE_MSG);
	}

	RemoveTip(STT_WRITING);
	writing_time_ = 0;

	bool flash = true;
	if (msg.feature == nim::kNIMMessageFeatureSyncMsg || msg.msg_type == nim::kNIMMessageTypeNotification)
		flash = false;
	OnNewMsg(create, flash);
	
	if(at_end)
	{
		msg_list_->EndDown(true, false);
	}
}

void SessionForm::AddWritingMsg(const MsgData &msg)
{
	bool at_end = msg_list_->IsAtEnd();

	Json::Value json;
	if (StringToJson(msg.msg_attach, json))
	{
		std::string id = json["id"].asString();
		if (id == "1")
		{
			ShowMsgWriting(msg);
		}
	}

	if (at_end)
	{
		msg_list_->EndDown(true, false);
	}
}

MsgBubbleItem* SessionForm::ShowMsg(const MsgData &msg, bool first, bool show_time)
{
	const std::string &bubble_id = msg.client_msg_id;
	if(bubble_id.empty())
	{
		QLOG_WAR(L"msg id empty");
		return nullptr;
	}

	IdBubblePair::iterator it = id_bubble_pair_.find(bubble_id);
	if(it != id_bubble_pair_.end())
	{
		QLOG_WAR(L"repeat msg: {0}") <<bubble_id;
		return nullptr;
	}

	MsgBubbleItem* item = NULL;

	int type = msg.msg_type;
	if (type == nim::kNIMMessageTypeText || IsNetCallMsg((nim::NIMMessageType)msg.msg_type, msg.msg_attach))
		item = new MsgBubbleText;
	else if (type == nim::kNIMMessageTypeImage)
		item = new MsgBubbleImage;
	else if (type == nim::kNIMMessageTypeAudio)
		item = new MsgBubbleAudio;
	else if (type == nim::kNIMMessageTypeLocation)
		item = new MsgBubbleLocation;
	else if (type == nim::kNIMMessageTypeFile)
		item = new MsgBubbleFile;
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
		return nullptr;
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
						return nullptr;
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

	item->InitControl(bubble_right);
	item->InitInfo(msg);
	item->SetSessionId(session_id_);
	item->SetSessionType(session_type_);
	item->SetShowTime(show_time);
	if (bubble_right || msg.to_type == nim::kNIMSessionTypeP2P)
		item->SetShowName(false, "");
	else
		item->SetShowName(true, msg.from_nick);

	id_bubble_pair_[bubble_id] = item;

	return item;
}

void SessionForm::ShowMsgWriting(const MsgData &msg)
{
	cancel_writing_timer_.Cancel();
	StdClosure cb = nbase::Bind(&SessionForm::CancelWriting, this);
	auto weak_cb = cancel_writing_timer_.ToWeakCallback(cb);
	nbase::ThreadManager::PostDelayedTask(weak_cb, nbase::TimeDelta::FromSeconds(kCellCancelWriting));

	if(has_writing_cell_)
		return;

	MsgBubbleWriting* item = new MsgBubbleWriting;
	GlobalManager::FillBoxWithCache(item, L"session/bubble_left.xml");
	msg_list_->Add(item);

	item->InitControl(false);
	item->InitInfo(msg);
	item->SetShowTime(false);
	item->SetShowName(false, "");

	item->SetName(CELL_WRITING);

	has_writing_cell_ = true;
}

void SessionForm::CancelWriting()
{
	bool at_end = msg_list_->IsAtEnd();

	RemoveTip(STT_WRITING);

	if(at_end)
		msg_list_->EndDown(true, false);
}

void SessionForm::SendText( const std::string &text )
{
	MsgData msg;
	PackageMsg(msg);
	msg.msg_type = nim::kNIMMessageTypeText;
	msg.msg_body = text;
	UserInfo user_info;
	UserService::GetInstance()->GetUserInfo(msg.from_account, user_info);
	msg.from_nick = user_info.name;

	SendMsg(msg);
}

void SessionForm::SendImage( const std::wstring &src )
{
	MsgData msg;
	PackageMsg(msg);
	msg.msg_type = nim::kNIMMessageTypeImage;

	std::wstring filename = nbase::UTF8ToUTF16(msg.client_msg_id);

	std::wstring dest = GetUserImagePath() + filename;
	GenerateUploadImage(src, dest);
	msg.local_file_path = nbase::UTF16ToUTF8(dest);

	std::string md5 = GetFileMD5(dest);

	//image key
	{
		Json::Value image_key;
		image_key[nim::kNIMImgMsgKeyMd5] = md5;

		long sz = (long)nbase::GetFileSize(dest);
		image_key[nim::kNIMImgMsgKeySize] = sz;

		Gdiplus::Image image(dest.c_str());
		if (image.GetLastStatus() == Gdiplus::Ok)
		{
			image_key[nim::kNIMImgMsgKeyWidth] = image.GetWidth();
			image_key[nim::kNIMImgMsgKeyHeight] = image.GetHeight();
		}

		msg.msg_attach = image_key.toStyledString();
	}

	SendMsg(msg);
}

void SessionForm::SendSnapChat(const std::wstring &src)
{
	auto weak_flag = this->GetWeakFlag();
	MsgData msg;
	PackageMsg(msg);
	msg.history_save = 0;
	msg.msg_roaming = 0;
	msg.msg_sync = 0;
	std::wstring filename = nbase::UTF8ToUTF16(msg.client_msg_id);
	std::wstring dest = GetUserImagePath() + filename;
	GenerateUploadImage(src, dest);
	msg.local_file_path = nbase::UTF16ToUTF8(dest);

	nim::Http::UploadResource(msg.local_file_path, [this, msg, weak_flag](nim::NIMResCode res_code, const std::string& url) {
		if (!weak_flag.expired() && res_code == nim::kNIMResSuccess)
		{
			MsgData new_msg = msg;
			std::string md5 = GetFileMD5(nbase::UTF8ToUTF16(new_msg.local_file_path));
			int file_size = (int)nbase::GetFileSize(nbase::UTF8ToUTF16(new_msg.local_file_path));
			new_msg.msg_type = nim::kNIMMessageTypeCustom;
			Json::Value json;
			Json::Value json_data;
			json_data["size"] = file_size;
			json_data["md5"] = md5;
			json_data["url"] = url;
			json["type"] = CustomMsgType_SnapChat;
			json["data"] = json_data;
			new_msg.msg_body = nbase::UTF16ToUTF8(L"阅后即焚");
			new_msg.msg_attach = json.toStyledString();
			SendMsg(new_msg);
		}
	});
}

bool SessionForm::CheckFileSize(const std::wstring &src)
{
	int64_t sz = nbase::GetFileSize(src);
	if (sz > 15*1024*1024 || sz <= 0)
	{
		return false;
	}
	return true;
}
void SessionForm::SendFile(const std::wstring &src)
{
	MsgData msg;
	PackageMsg(msg);
	msg.msg_type = nim::kNIMMessageTypeFile;

	msg.local_file_path = nbase::UTF16ToUTF8(src);

	std::string md5 = GetFileMD5(src);

	//file key
	{
		Json::Value image_key;
		image_key[nim::kNIMImgMsgKeyMd5] = md5;

		long sz = (long)nbase::GetFileSize(src);
		image_key[nim::kNIMImgMsgKeySize] = sz;

		nbase::PathString file_name;
		nbase::FilePathApartFileName(src, file_name);
		std::wstring file_exten;
		nbase::FilePathExtension(file_name, file_exten);
		image_key["name"] = nbase::UTF16ToUTF8(file_name);
		image_key[nim::kNIMImgMsgKeyExt] = nbase::UTF16ToUTF8(file_exten);

		msg.msg_attach = image_key.toStyledString();
	}

	SendMsg(msg);
}

void SessionForm::SendJsb( const std::string &attach )
{
	MsgData msg;
	PackageMsg(msg);
	msg.msg_type = nim::kNIMMessageTypeCustom;

	msg.msg_body = nbase::UTF16ToUTF8(L"这是一个猜拳");
	msg.msg_attach = attach;
	
	SendMsg(msg);
}
void SessionForm::SendSticker(const std::string &catalog, const std::string &name)
{
	MsgData msg;
	PackageMsg(msg);
	msg.msg_type = nim::kNIMMessageTypeCustom;

	Json::Value json;
	json["type"] = CustomMsgType_Sticker;
	json["data"]["catalog"] = catalog;
	json["data"]["chartlet"] = name;

	msg.msg_body = nbase::UTF16ToUTF8(L"贴图");
	msg.msg_attach = json.toStyledString();

	SendMsg(msg);
}

void SessionForm::SendMsg( const MsgData &msg )
{
	writing_time_ = 0;
	RemoveTip(STT_WRITING);

	bool show_time = false;
	if(last_msg_time_ == 0)
	{
		show_time = true;
		farst_msg_time_ = msg.msg_time;
	}
	else
	{
		show_time = CheckIfShowTime(last_msg_time_, msg.msg_time);
	}
	last_msg_time_ = msg.msg_time;

	MsgBubbleItem* item = ShowMsg(msg, false, show_time);
	msg_list_->EndDown(true, false);

	Json::Value value;
	MsgToJson(msg, value);

	if (msg.msg_type == nim::kNIMMessageTypeFile)
	{
		if (!msg.local_file_path.empty() && nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg.local_file_path), false))
		{
			nim::Talk::FileUpPrgCallback cb = ((MsgBubbleFile*)item)->GetFileUpPrgCallback();
			nim::Talk::FileUpPrgCallback* cb_pointer = new nim::Talk::FileUpPrgCallback(cb);

			SessionManager::GetInstance()->AddFileUpProgressCb(msg.client_msg_id, cb_pointer);
			nim::Talk::SendMsg(value.toStyledString(), msg.client_msg_id, cb_pointer);
		}
	} 
	else
	{
		nim::Talk::SendMsg(value.toStyledString());
	}
}

void SessionForm::ReSendMsg( MsgData &msg )
{
	msg.resend_flag = 1;
	msg.msg_status = nim::kNIMMsgLogStatusSending;
	msg.msg_time	= 1000 * nbase::Time::Now().ToTimeT();

	SendMsg(msg);
}

void SessionForm::PackageMsg( MsgData &msg )
{
	msg.to_type			= session_type_;
	msg.to_account		= session_id_;
	msg.from_account	= LoginManager::GetInstance()->GetAccount();
	msg.client_msg_id   = QString::GetGUID();
	msg.resend_flag		= 0;
	
	//base获取的时间单位是s，服务器的时间单位是ms
	msg.msg_time = 1000 * nbase::Time::Now().ToTimeT();

	msg.msg_status = nim::kNIMMsgLogStatusSending;
}

void SessionForm::InvokeShowMsgs(bool first_show_msg)
{
	first_show_msg_ = first_show_msg;

	QLOG_APP(L"query begin: id={0} type={1} farst_time={2}") <<session_id_ <<session_type_ <<farst_msg_time_;

	nim::MsgLog::QueryMsgAsync(session_id_, session_type_, kMsgLogNumberShow, farst_msg_time_, 
		nbase::Bind(&TalkCallback::OnQueryMsgCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void SessionForm::ShowMsgs(const std::vector<MsgData> &msg)
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
	//加载更多历史消息
	AddTip(STT_LOAD_MORE_MSG);
	if(len < kMsgLogNumberShow)
	{
		Box* box = (Box*) msg_list_->FindSubControl(CELL_LOAD_MORE_MSG);
		assert(box);
		Button* btn = (Button*) box->FindSubControl(CELL_BTN_LOAD_MORE_MSG);
		assert(btn);
		btn->SetText(L"已显示全部历史消息");
		btn->SetEnabled(false);
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
		sz.cy = msg_list_->GetScrollRange().cy - pos - 50;
		msg_list_->SetScrollPos(sz);
	}
	//修正最远时间
	if(len > 0)
	{
		farst_msg_time_ = msg[len-1].msg_time;
	}
}

void SessionForm::OnSendMsgCallback( const std::string &cid, int code )
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if(item)
		{
			if (code == nim::kNIMResSuccess)
				item->SetMsgStatus(nim::kNIMMsgLogStatusSent);
			else if (code == nim::kNIMLocalResMsgFileNotExist)
				item->SetMsgStatus(nim::kNIMMsgLogStatusSendFailed);
			else if (code == nim::kNIMLocalResMsgNosUploadCancel)
				item->SetMsgStatus(nim::kNIMMsgLogStatusSendCancel);
			else
			{
				QLOG_WAR(L"unknown send msg callback code {0}") << code;
				item->SetMsgStatus(nim::kNIMMsgLogStatusSendFailed);
			}
		}
	}
}

void SessionForm::OnDownloadCallback( const std::string &cid, bool success )
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if(item)
		{
			item->OnDownloadCallback(success);
		}
	}
}

void SessionForm::OnRelink( const Json::Value &json )
{
	bool link = json["link"].asBool();
	if(link)
	{
		RemoveTip(STT_LINK);
	}
}
void SessionForm::ShowCustomMsgForm()
{
	CustomMsgForm* f = WindowsManager::SingletonShow<CustomMsgForm>(CustomMsgForm::kClassName);
	f->SetSession(session_id_, session_type_, label_title_->GetText());
}
}