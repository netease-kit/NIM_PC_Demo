#include "rts_form.h"

#include "module/video/video_manager.h"
#include "module/login/login_manager.h"
#include "module/session/session_manager.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR RtsForm::kClassName = L"RtsForm";

RtsForm::RtsForm(int type, std::string uid, std::string session_id)
{
	type_ = type;
	uid_ = uid;
	session_id_ = session_id;
	uuid_ = nbase::UTF8ToUTF16(session_id);
	chat_status_ = nullptr;

	need_ack_ = false;

	talking_ = false;
	closing_ = false;
	show_endmsg_ = false;
	show_type_ = ShowOpAll;
}

RtsForm::~RtsForm()
{
}

std::wstring RtsForm::GetSkinFolder()
{
	return L"rts";
}

std::wstring RtsForm::GetSkinFile()
{
	return L"rts.xml";
}

std::wstring RtsForm::GetWindowId() const
{
	return uuid_;
}
ui::Control* RtsForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BoardBox"))
	{
		return new BoardControl();
	}
	return NULL;
}

LRESULT RtsForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		OnBtnClose();
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void RtsForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&RtsForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&RtsForm::OnClicked, this, std::placeholders::_1));

	chat_status_ = (Label*)FindControl(L"chat_status");
	chat_status2_ = (Label*)FindControl(L"chat_status2");
	ctrl_notify_ = (Label*)FindControl(L"ctrl_notify");
	speak_ = (CheckBox*)FindControl(L"speak");
	speak_->Selected(false);
	board_ = (BoardControl*)FindControl(L"board");
	board_->AttachAllEvents(nbase::Bind(&RtsForm::Notify, this, std::placeholders::_1));
	board_->SetDrawCb(nbase::Bind(&RtsForm::OnDrawCallback, this, std::placeholders::_1));

	ShowHeader();

	auto closure = nbase::Bind(&RtsForm::SendCurData, this);
	nbase::ThreadManager::PostRepeatedTask(kThreadUI, closure, nbase::TimeDelta::FromMilliseconds(60));

	if (type_ & nim::kNIMRtsChannelTypeVchat)
	{
		InitAudio();
	}
}

void RtsForm::OnFinalMessage(HWND hWnd)
{
	if (show_endmsg_)
	{
		ShowEndMsg();
	}
	if (type_ & nim::kNIMRtsChannelTypeVchat)
	{
		FreeAudio();
		FreeMic();
	}
	if (need_ack_)
	{
		nim::Rts::Ack(session_id_, type_, false, false, false, nim::Rts::AckCallback());
	} 
	else
	{
		nim::Rts::Hangup(session_id_, nim::Rts::HangupCallback());
	}
	__super::OnFinalMessage(hWnd);
}

bool RtsForm::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"board")
	{
		switch (msg->Type)
		{
		case kEventMouseButtonDown:
			board_->OnLButtonDown(msg->ptMouse);
			break;
		case kEventMouseMove:
			board_->OnMouseMove(msg->ptMouse);
			break;
		case kEventMouseButtonUp:
			board_->OnLButtonUp(msg->ptMouse);
			break;
		}
	}
	else if (name == L"speak")
	{
		if (talking_)
		{
			MutiLanSupport* mls = MutiLanSupport::GetInstance();
			if (msg->Type == kEventSelect)
			{
				InitMic();
				nim::Rts::Control(session_id_, nbase::UTF16ToUTF8(L"开启音频"), nim::Rts::ControlCallback());
			}
			else if (msg->Type == kEventUnSelect)
			{
				FreeMic();
				nim::Rts::Control(session_id_, nbase::UTF16ToUTF8(L"关闭音频"), nim::Rts::ControlCallback());
			}
		}
	}
	return true;
}

bool RtsForm::OnClicked(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();		
	if(name == L"btn_accept")
	{
		bool data_record = atoi(GetConfigValue("rts_record").c_str()) > 0;
		bool audio_record = atoi(GetConfigValue("audio_record").c_str()) > 0;
		need_ack_ = false;
		nim::Rts::Ack(session_id_, type_, true, data_record, audio_record, nbase::Bind(&RtsForm::OnAckCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
	else if (name == L"btn_reject")
	{
		need_ack_ = false;
		nim::Rts::Ack(session_id_, type_, false, false, false, nim::Rts::AckCallback());
		DelayClose();
	}
	else if (name == L"btn_cancel")
	{
		nim::Rts::Hangup(session_id_, nim::Rts::HangupCallback());
		DelayClose();
	}
	else if (name == L"btn_undo")
	{
		board_->MyUndo();
	}
	else if (name == L"btn_clear")
	{
		DrawOpInfo info;
		info.draw_op_type_ = DrawOpClear;
		info.x_ = 0;
		info.y_ = 0;
		OnDrawCallback(info);
		board_->ReleaseAllDrawUnits(ShowOpMine);
		board_->PaintContent();
	}
	else if (name == L"btn_close")
	{
		OnBtnClose();
	}
	else if (name == L"btn_showtype")
	{
		if (talking_)
		{
			MutiLanSupport* mls = MutiLanSupport::GetInstance();
			switch (show_type_)
			{
			case ShowOpAll:
				show_type_ = ShowOpMine;
				((Button*)(arg->pSender))->SetText(mls->GetStringViaID(L"STRID_RTS_SHOW_MINE"));
				break;
			case ShowOpMine:
				show_type_ = ShowOpOther;
				((Button*)(arg->pSender))->SetText(mls->GetStringViaID(L"STRID_RTS_SHOW_OTHERS"));
				break;
			case ShowOpOther:
				show_type_ = ShowOpAll;
				((Button*)(arg->pSender))->SetText(mls->GetStringViaID(L"STRID_RTS_SHOW_ALL"));
				break;
			}
			board_->SetShowType(show_type_);
		}
	}
	return true;
}

void RtsForm::OnStartRtsCb(std::string& old_id, nim::NIMResCode res_code, const std::string& session_id, int channel_type, const std::string& uid)
{
	if (session_id_ == old_id)
	{
		StartResult(res_code, session_id);
	}
	else if (res_code == nim::kNIMResSuccess)
	{
		nim::Rts::Hangup(session_id, nim::Rts::HangupCallback());
	}
}
//callback
void RtsForm::OnAckCallback(nim::NIMResCode res_code, const std::string& session_id, int channel_type, bool accept)
{
	if (session_id == session_id_)
	{
		if (res_code == nim::kNIMResSuccess)
		{
			ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_CONNECTING"));
			ShowBoardUI();
		} 
		else
		{
			ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_ACK_FAIL"));
			DelayClose();
		}
	}
}
void RtsForm::OnAckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid)
{
	if (session_id == session_id_)
	{
		if (accept)
		{
			ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_CONNECTING"));
			ShowBoardUI();
		} 
		else
		{
			ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_REJECT"));
			DelayClose();
		}
	}
}
void RtsForm::OnSyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept)
{
	if (session_id == session_id_)
	{
		if (accept)
		{
			ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_CLIENT_ACK"));
		} 
		else
		{
			ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_CLIENT_REJECT"));
		}
		DelayClose();
	}
}
void RtsForm::OnConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json)
{
	if (session_id == session_id_)
	{
		if (channel_type == nim::kNIMRtsChannelTypeTcp)
		{
			if (code == nim::kNIMRtsConnectStatusSuccess)
			{
				if (!talking_)
				{
					ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_CONNECT_OK"));
					talking_ = true;
				}
			}
			else
			{
				ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_CONNECT_FAIL"));
				DelayClose();
			}
		}
		else if (channel_type == nim::kNIMRtsChannelTypeVchat)
		{
			SetAudioStatus(code == nim::kNIMRtsConnectStatusSuccess);
		}
	}
}
void RtsForm::OnMemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code)
{
	if (session_id == session_id_)
	{
		switch (channel_type)
		{
		case nim::kNIMRtsChannelTypeTcp:
			if (code == nim::kNIMRtsMemberStatusJoined)
			{
				ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_SESSION_ON"));
			}
			else
			{
				ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_OTHER_LEAVE"));
				DelayClose();
			}
			break;
		case nim::kNIMRtsChannelTypeVchat:
			if (code != nim::kNIMRtsMemberStatusJoined)
			{
				SetAudioStatus(false);
			}
			break;
		default:
			break;
		}
	}
}
void RtsForm::OnHangupNotifyCallback(const std::string& session_id, const std::string& uid)
{
	if (session_id == session_id_)
	{
		ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_OTHER_OTHER_HANG_UP"));
		DelayClose();
	}
}
void RtsForm::OnRecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data)
{
	if (session_id == session_id_)
	{
		if (channel_type == nim::kNIMRtsChannelTypeTcp)
		{
			OnParseTcpData(data);
		}
	}
}


void RtsForm::ShowStartUI(bool creater)
{
	need_ack_ = !creater;
	if (creater)
	{
		ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_INVITING_OTHER"));
	} 
	else
	{
		ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_INVITING_YOU"));
		show_endmsg_ = true;
	}
	Control* accept = FindControl(L"btn_accept");
	accept->SetVisible(!creater);
	Control* reject = FindControl(L"btn_reject");
	reject->SetVisible(!creater);
	Control* cancel = FindControl(L"btn_cancel");
	cancel->SetVisible(creater);

	if (creater)
	{
		bool data_record = atoi(GetConfigValue("rts_record").c_str()) > 0;
		bool audio_record = atoi(GetConfigValue("audio_record").c_str()) > 0;
		bool webrtc = VideoManager::GetInstance()->GetWebrtc();
		nim::RtsStartInfo info;
		info.apns_ = nbase::UTF16ToUTF8(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_INVITE_TEST"));
		info.custom_info_ = "rts custom info";
		info.data_record_ = data_record;
		info.audio_record_ = audio_record;
		info.webrtc_ = webrtc;
		info.session_id_ = session_id_;
		nim::Rts::StartChannelCallback cb = nbase::Bind(&RtsForm::OnStartRtsCb, this, session_id_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		nim::Rts::StartChannel(type_, uid_, info, cb);
	}
	auto closure = nbase::Bind(&RtsForm::NoActiveTimer, this);
	nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(40));
}
void RtsForm::StartResult(nim::NIMResCode code, std::string session_id)
{
	session_id_ = session_id;
	if (code == nim::kNIMResSuccess)
	{
		//ShowTip(L"等待对方接受");
		SendCreateMsg();
	} 
	else
	{
		ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_START_FAIL"));
		DelayClose();
	}
}

void RtsForm::ShowBoardUI()
{
	Box* status_page = (Box*)FindControl(L"status_page");
	status_page->SetVisible(false);
	Box* board_page = (Box*)FindControl(L"board_page");
	board_page->SetVisible(true);
}
void RtsForm::ShowHeader()
{
	std::wstring name = UserService::GetInstance()->GetUserName(uid_);
	Label* friend_name = (Label*)FindControl(L"friend_name");
	friend_name->SetText(name);

	std::wstring title_text = nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_RTS_WITH").c_str(), name.c_str());
	SetTaskbarTitle(title_text);
	Label* title = (Label*)FindControl(L"title");
	title->SetText(title_text);

	std::wstring photo = PhotoService::GetInstance()->GetUserPhoto(uid_);
	Button* headicon = (Button*)FindControl(L"headicon");
	headicon->SetBkImage(photo);
}
void RtsForm::DelayClose()
{
	closing_ = true;
	talking_ = false;
	Box* ack_box = (Box*)FindControl(L"ack_box");
	ack_box->SetVisible(false);
	auto closure = nbase::Bind(&RtsForm::Close, this, IDOK);
	nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(2));
}
void RtsForm::NoActiveTimer()
{
	if (!talking_)
	{
		ShowTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_TIMEOUT"));
		DelayClose();
	}
}
void RtsForm::ShowTip(std::wstring text)
{
	chat_status_->SetText(text);
	chat_status2_->SetText(text);
}
void RtsForm::OnDrawCallback(ui::DrawOpInfo info)
{
	std::string buffer_temp = nbase::StringPrintf("%d:%f,%f;", info.draw_op_type_, info.x_, info.y_);
	cur_buffer_.append(buffer_temp);
	if (info.draw_op_type_ != ui::DrawOpMove)
	{
		SendCurData();
	}
}
void RtsForm::SendCurData()
{
	if (cur_buffer_.size() > 0)
	{
		//OnParseTcpData(cur_buffer_);
		nim::Rts::SendData(session_id_, nim::kNIMRtsChannelTypeTcp, cur_buffer_);
		cur_buffer_.clear();
	}
}
void RtsForm::OnParseTcpData(std::string data)
{
	int pos = (int)data.find(';');
	std::list<ui::DrawOpInfo> info_list;
	while (pos != -1)
	{
		ui::DrawOpInfo info;
		std::string cur_data = data.substr(0, pos);
		sscanf(cur_data.c_str(), "%d:%f,%f", &info.draw_op_type_, &info.x_, &info.y_);
		info_list.push_back(info);

		data = data.substr(pos + 1);
		pos = (int)data.find(';');
	}
	if (info_list.size() > 0)
	{
		board_->OnOtherDrawInfos(info_list);
	}
}
bool RtsForm::InitMic()
{
	std::string def_audio;
	int no = 0;
	VideoManager::GetInstance()->GetDefaultDevicePath(no, def_audio, nim::kNIMDeviceTypeAudioIn);
	VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioIn, def_audio, kDeviceSessionTypeRts);
	if (def_audio.empty())
	{
		return false;
	}
	return true;
}

void RtsForm::FreeMic()
{
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeRts);
}
bool RtsForm::InitAudio()
{
	std::string def_audio;
	int no = 0;
	VideoManager::GetInstance()->GetDefaultDevicePath(no, def_audio, nim::kNIMDeviceTypeAudioOutChat);
	VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioOutChat, def_audio, kDeviceSessionTypeRts);
	if (def_audio.empty())
	{
		return false;
	}
	return true;
}
void RtsForm::FreeAudio()
{
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeRts);
}
void RtsForm::OnBtnClose()
{
	if (!closing_)
	{
		MsgboxCallback mb = nbase::Bind(&RtsForm::OnQuitMsgBox, this, std::placeholders::_1);
		ShowMsgBox(m_hWnd, mb, L"STRID_RTS_QUIT_TIP", true, L"STRID_RTS_QUIT_TIP_TITLE", true, L"STRING_OK", true, L"STRING_NO", true);
	} 
	else
	{
		OnQuitMsgBox(MB_YES);
	}
}
void RtsForm::OnQuitMsgBox(MsgBoxRet ret)
{
	if (ret == MB_YES)
	{
		this->Close();
	}
}
void RtsForm::SetAudioStatus(bool show)
{
	ui::Control* speak = FindControl(L"speak");
	speak->SetVisible(show);
	if (show)
	{
		if (speak_->IsSelected())
		{
			InitMic();
		}
	} 
	else
	{
		FreeMic();
	}
}
void RtsForm::SendCreateMsg()
{
	show_endmsg_ = true;
	nim::IMMessage msg;
	msg.session_type_ = nim::kNIMSessionTypeP2P;
	msg.receiver_accid_ = uid_;
	msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
	msg.client_msg_id_ = QString::GetGUID();

	//base获取的时间单位是s，服务器的时间单位是ms
	msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
	msg.status_ = nim::kNIMMsgLogStatusSending;
	msg.type_ = nim::kNIMMessageTypeCustom;

	Json::Value json;
	Json::FastWriter writer;
	json["type"] = CustomMsgType_Rts;
	json["data"]["flag"] = 0;

	msg.content_ = nbase::UTF16ToUTF8(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_RTS"));
	msg.attach_ = writer.write(json);

	nim::Talk::SendMsg(msg.ToJsonString(true));
	SessionBox* session = SessionManager::GetInstance()->FindSessionBox(uid_);
	if (session)
	{
		session->AddNewMsg(msg, false);
	}
}
void RtsForm::ShowEndMsg()
{
	nim::IMMessage msg;
	msg.session_type_ = nim::kNIMSessionTypeP2P;
	msg.receiver_accid_ = uid_;
	msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
	msg.client_msg_id_ = QString::GetGUID();
	//base获取的时间单位是s，服务器的时间单位是ms
	msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
	msg.status_ = nim::kNIMMsgLogStatusSent;
	msg.type_ = nim::kNIMMessageTypeCustom;

	Json::Value json;
	Json::FastWriter writer;
	json["type"] = CustomMsgType_Rts;
	json["data"]["flag"] = 1;

	msg.content_ = nbase::UTF16ToUTF8(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_RTS_RTS"));
	msg.attach_ = writer.write(json);

	nim::MsgLog::WriteMsglogToLocalAsync(uid_, msg, false, nim::MsgLog::WriteMsglogCallback());
	SessionBox* session = SessionManager::GetInstance()->FindSessionBox(uid_);
	if (session)
	{
		session->AddNewMsg(msg, false);
	}
}

void RtsForm::OnControlNotify(const std::string& session_id, const std::string& uid, const std::string& text)
{
	std::wstring name = UserService::GetInstance()->GetUserName(uid);
	if (name.empty())
		name = nbase::UTF8ToUTF16(uid);
	ctrl_notify_->SetText(name + L": " + nbase::UTF8ToUTF16(text));
	ctrl_notify_->SetVisible(true);
	auto closure = nbase::Bind(&RtsForm::HideCtrlNotifyTip, this);
	nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(3));
}

void RtsForm::HideCtrlNotifyTip()
{
	ctrl_notify_->SetVisible(false);
}
}