#include "session_box.h"
#include "session_form.h"
#include "gui/session/control/audio_capture.h"
#include "gui/session/control/atme_view.h"
#include "gui/session/atlist/at_list_form.h"
#include "gui/session/taskbar/taskbar_manager.h"
#include "module/session/session_util.h"
#include "module/session/session_manager.h"
#include "module/audio/audio_manager.h"
#include "callback/session/session_callback.h"
#include "export/nim_ui_window_manager.h"
#include "util/user.h"

using namespace ui;

namespace
{
	const int kMsgLogNumberShow = 20;
}

namespace nim_comp
{

const LPCTSTR SessionBox::kClassName = L"SessionForm";

SessionBox::SessionBox(std::string id, nim::NIMSessionType type)
{
	btn_max_restore_ = NULL;
	input_edit_ = NULL;

	session_id_ = id;
	session_type_ = type;

	is_header_enable_ = false;

	first_show_msg_ = true;
	last_msg_time_ = 0;
	farst_msg_time_ = 0;

	has_writing_cell_ = false;
	writing_time_ = 0;
	is_team_valid_ = true;

	mute_all_ = false;

	taskbar_item_ = NULL;
}

SessionBox::~SessionBox()
{

}

SessionForm* SessionBox::GetSessionForm() const
{
	ASSERT(NULL != session_form_);
	ASSERT(::IsWindow(session_form_->GetHWND()));
	return session_form_;
}

void SessionBox::InitSessionBox()
{
	ASSERT(session_form_ != NULL);

	this->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&SessionBox::Notify, this, std::placeholders::_1));
	this->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	this->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&SessionBox::OnSelChanged, this, std::placeholders::_1));
	this->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&SessionBox::OnSelChanged, this, std::placeholders::_1));

	label_title_ = (Label*)FindSubControl(L"label_title");
	label_tid_ = (Label*)FindSubControl(L"label_tid");

	btn_header_ = (Button*)FindSubControl(L"btn_header");
	btn_header_->AttachClick(nbase::Bind(&SessionBox::OnBtnHeaderClick, this, std::placeholders::_1));

	btn_invite_ = (Button*)FindSubControl(L"btn_invite");
	btn_invite_->SetVisible(false);
	btn_invite_->AttachClick(nbase::Bind(&SessionBox::OnBtnInvite, this, std::placeholders::_1));

	btn_capture_audio_ = (Option*)FindSubControl(L"btn_capture_audio");
	Button* btn_audio = (Button*)FindSubControl(L"btn_audio");
	Button* btn_video = (Button*)FindSubControl(L"btn_video");
	Button* btn_rts = (Button*)FindSubControl(L"btn_rts");
	if (session_type_ == nim::kNIMSessionTypeP2P && !IsFileTransPhone())
	{
		btn_audio->SetVisible(true);
		btn_video->SetVisible(true);
		btn_rts->SetVisible(true);
	}

	btn_max_restore_ = (Button*)FindSubControl(L"btn_max_restore");

	msg_content_ = (Box*)FindSubControl(L"msg_content");
	msg_list_ = (ListBox*)FindSubControl(L"msg_list");
	btn_face_ = (CheckBox*)FindSubControl(L"btn_face");
	input_edit_ = (RichEdit*)FindSubControl(L"input_edit");
	input_edit_->SetLimitText(5000);
	input_edit_->SetNoCaretReadonly();
	input_edit_->AttachReturn(nbase::Bind(&SessionBox::OnInputEditEnter, this, std::placeholders::_1));
	btn_send_ = (Button*)FindSubControl(L"btn_send");

	btn_new_broad_ = (Button*)FindSubControl(L"btn_new_broad");
	btn_new_broad_->SetVisible(false);
	edit_broad_ = (RichEdit*)FindSubControl(L"re_broad");
	edit_broad_->SetNoCaretReadonly();
	label_member_ = (Label*)FindSubControl(L"label_member");
	btn_refresh_member_ = (Button*)FindSubControl(L"btn_refresh_member");
	btn_refresh_member_->SetEnabled(false);
	member_list_ = (ListBox*)FindSubControl(L"member_list");

	ITextServices * text_services = input_edit_->GetTextServices();
	richedit_ole_callback_ = new IRichEditOleCallbackEx(text_services, std::function<void()>());
	text_services->Release();
	input_edit_->SetOleCallback(richedit_ole_callback_);
	richedit_ole_callback_->SetCustomMode(true);

	IDropTarget *pdt = input_edit_->GetTxDropTarget();
	if (pdt)
	{
		input_edit_droptarget_ = pdt;
	}

	atme_view_ = new AtMeView;
	atme_view_->InitControl();
	msg_content_->Add(atme_view_);
	atme_view_->AttachBubbledEvent(kEventClick, nbase::Bind(&SessionBox::OnAtMeViewClick, this, std::placeholders::_1));
	Button* button_close_at_me = static_cast<Button*>(atme_view_->FindSubControl(L"btn_close_at_me"));
	button_close_at_me->AttachClick(nbase::Bind(&SessionBox::OnCloseAtMeView, this, std::placeholders::_1));

	// 语音录制界面显示优先级高，放到最上层
	audio_capture_view_ = new AudioCaptureView;
	audio_capture_view_->InitControl(session_id_, nbase::Bind(&SessionBox::OnAudioCaptureComplete, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
	msg_content_->Add(audio_capture_view_);

	CheckHeader();
	CheckTeamType(nim::kNIMTeamTypeNormal);
	OnWndSizeMax(TRUE == IsZoomed(this->GetWindow()->GetHWND()));

	if (session_type_ == nim::kNIMSessionTypeTeam)
	{
		FindSubControl(L"btn_snapchat")->SetVisible(false);
		InvokeGetTeamInfo();

		OnSelectAtItem cb = nbase::Bind(&SessionBox::OnSelectAtItemCallback, this, std::placeholders::_1);
		AtlistForm *at_list_form = new AtlistForm(session_id_, ToWeakCallback(cb));
		at_list_form->Create(this->GetWindow()->GetHWND(), L"", WS_POPUPWINDOW, 0L);
	}
	else
	{
		is_header_enable_ = true;
		btn_invite_->SetVisible(true);
	}

	// 初始化任务栏缩略图
	{
		taskbar_item_ = new TaskbarTabItem(this);
		if (taskbar_item_)
			taskbar_item_->Init(nbase::UTF8ToUTF16(session_id_));
	}


	unregister_cb.Add(NotifyCenter::GetInstance()->RegNotify(NT_LINK, nbase::Bind(&SessionBox::OnRelink, this, std::placeholders::_1)));
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&SessionBox::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&SessionBox::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	if (session_type_ == nim::kNIMSessionTypeTeam)
	{
		unregister_cb.Add(TeamService::GetInstance()->RegAddTeamMember(nbase::Bind(&SessionBox::OnTeamMemberAdd, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeamMember(nbase::Bind(&SessionBox::OnTeamMemberRemove, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMember(nbase::Bind(&SessionBox::OnTeamMemberChange, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(TeamService::GetInstance()->RegSetTeamAdmin(nbase::Bind(&SessionBox::OnTeamAdminSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamOwner(nbase::Bind(&SessionBox::OnTeamOwnerChange, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&SessionBox::OnTeamNameChange, this, std::placeholders::_1)));
		unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeam(nbase::Bind(&SessionBox::OnTeamRemove, this, std::placeholders::_1)));
		unregister_cb.Add(TeamService::GetInstance()->RegMuteMember(nbase::Bind(&SessionBox::OnTeamMuteMember, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	}
}

void SessionBox::UninitSessionBox()
{
	AudioManager::GetInstance()->StopPlayAudio(session_id_);
	SessionManager::GetInstance()->RemoveSessionBox(session_id_, this);

	if (taskbar_item_)
	{
		taskbar_item_->UnInit();
	}

	AtlistForm* at_list_form = (AtlistForm*)WindowsManager::GetInstance()->GetWindow(AtlistForm::kClassName, nbase::UTF8ToUTF16(session_id_));
	if (at_list_form)
		at_list_form->CloseForm();

	if (input_edit_droptarget_)
	{
		input_edit_droptarget_->Release();
		input_edit_droptarget_ = NULL;
	}
}

TaskbarTabItem* SessionBox::GetTaskbarItem()
{
	return taskbar_item_;
}

void SessionBox::SetInternVisible(bool bVisible /*= true*/)
{
	Control::SetInternVisible(bVisible);
	if (m_items.empty()) return;
	for (auto it = m_items.begin(); it != m_items.end(); it++) {
		(*it)->SetInternVisible(bVisible);
	}
}

void SessionBox::Invalidate() const
{
	__super::Invalidate();

	if (taskbar_item_)
		taskbar_item_->InvalidateTab();
}

void SessionBox::SetPos(UiRect rc)
{
	__super::SetPos(rc);

	if (taskbar_item_)
		taskbar_item_->InvalidateTab();
}

void SessionBox::InvokeShowMsgs(bool first_show_msg)
{
	first_show_msg_ = first_show_msg;

	QLOG_APP(L"query begin: id={0} type={1} farst_time={2}") << session_id_ << session_type_ << farst_msg_time_;

	nim::MsgLog::QueryMsgAsync(session_id_, session_type_, kMsgLogNumberShow, farst_msg_time_,
		nbase::Bind(&TalkCallback::OnQueryMsgCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void SessionBox::AddNewMsg(const nim::IMMessage &msg, bool create)
{
	bool at_end = msg_list_->IsAtEnd();

	bool show_time = false;
	if (first_show_msg_)
	{
		show_time = true;
		farst_msg_time_ = msg.timetag_;
	}
	else
	{
		show_time = CheckIfShowTime(last_msg_time_, msg.timetag_);
	}
	ShowMsg(msg, false, show_time);

	if(!IsNoticeMsg(msg))
		last_msg_time_ = msg.timetag_;

	if (first_show_msg_)
	{
		first_show_msg_ = false;
		AddTip(STT_LOAD_MORE_MSG);
	}

	RemoveTip(STT_WRITING);
	writing_time_ = 0;

	bool flash = true;
	if (msg.feature_ == nim::kNIMMessageFeatureSyncMsg || msg.type_ == nim::kNIMMessageTypeNotification)
		flash = false;
	session_form_->OnNewMsg(*this, create, flash);
	
	if(at_end)
	{
		msg_list_->EndDown(true, false);
	}

	SendReceiptIfNeeded(true);

	// 如果当前msg包含atme消息，就显示提示条
	if (IsAtMeMsg(msg))
	{
		std::string sender_name = GetShowName(msg.sender_accid_);
		if (!sender_name.empty())
		{
			Json::Value root;
			Json::Value value;
			value["sender"] = sender_name;
			value["uuid"] = msg.client_msg_id_;
			value["msgbody"] = msg.content_;
			root[0] = value;
			atme_view_->AddMessage(root.toStyledString());
		}
	}
}

MsgBubbleItem* SessionBox::ShowMsg(const nim::IMMessage &msg, bool first, bool show_time)
{
	const std::string &bubble_id = msg.client_msg_id_;
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

	if (msg.type_ == nim::kNIMMessageTypeText || IsNetCallMsg(msg.type_, msg.attach_))
	{
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(msg.attach_, values) 
			&& values.isObject() 
			&& values.isMember("comment")
			&& values["comment"].asString() == "is_recall_notification")
		{
			std::wstring notify_text = nbase::UTF8ToUTF16(msg.content_);
			if (values.isMember("notify_from"))
			{
				std::string from_id = values["notify_from"].asString();
				notify_text = GetRecallNotifyText(from_id);
			}

			MsgBubbleNotice* cell = new MsgBubbleNotice;
			GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
			if (first)
				msg_list_->AddAt(cell, 0);
			else
				msg_list_->Add(cell);
			cell->InitControl();
			cell->InitCustomInfo(notify_text, session_id_, msg.client_msg_id_);
			return nullptr;
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
	else if (msg.type_ == nim::kNIMMessageTypeLocation)
		item = new MsgBubbleLocation;
	else if (msg.type_ == nim::kNIMMessageTypeFile)
		item = new MsgBubbleFile;
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
		return nullptr;
	}
	else if (msg.type_ == nim::kNIMMessageTypeCustom)
	{
		Json::Value json;
		if (StringToJson(msg.attach_, json))
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
						cell->InitInfo(msg, session_id_);
						return nullptr;
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

	item->InitControl(bubble_right);
	item->InitInfo(msg);
	item->SetSessionId(session_id_);
	item->SetSessionType(session_type_);
	item->SetShowTime(show_time);
	if (bubble_right || msg.session_type_ == nim::kNIMSessionTypeP2P)
		item->SetShowName(false, "");
	else
	{
		auto iter = team_member_info_list_.find(msg.sender_accid_);
		if (iter != team_member_info_list_.cend() && !iter->second.GetNick().empty())
			item->SetShowName(true, iter->second.GetNick()); //显示群名片
		else
		{
			std::string show_name = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(msg.sender_accid_));
			item->SetShowName(true, show_name); //显示备注名或昵称
		}
	}

	id_bubble_pair_[bubble_id] = item;
	if (first)//第一次打开，顺序倒序
		cached_msgs_bubbles_.insert(cached_msgs_bubbles_.begin(), 1, item);
	else
		cached_msgs_bubbles_.push_back(item);

	return item;
}

void SessionBox::ShowMsgs(const std::vector<nim::IMMessage> &msg)
{
	int pos = msg_list_->GetScrollRange().cy - msg_list_->GetScrollPos().cy;

	bool show_time = false;
	//msg倒序排列
	size_t len = msg.size();
	for (size_t i = 0; i < len; i++)
	{
		if (len == 1 || i == len - 1)
		{
			show_time = true;
		}
		else
		{
			long long older_time = 0;
			for (size_t j = i + 1; j < len; j++)
			{
				if (!IsNoticeMsg(msg[j]))
				{
					older_time = msg[j].timetag_;
					break;
				}
			}
			show_time = CheckIfShowTime(older_time, msg[i].timetag_);
		}
		ShowMsg(msg[i], true, show_time);
	}
	//加载更多历史消息
	AddTip(STT_LOAD_MORE_MSG);
	if (len < kMsgLogNumberShow)
	{
		Box* box = (Box*)msg_list_->FindSubControl(CELL_LOAD_MORE_MSG);
		assert(box);
		Button* btn = (Button*)box->FindSubControl(CELL_BTN_LOAD_MORE_MSG);
		assert(btn);
		btn->SetText(L"已显示全部历史消息");
		btn->SetEnabled(false);
	}
	//修正最近时间
	if (first_show_msg_)
	{
		first_show_msg_ = false;

		msg_list_->EndDown(true, false);

		if (len > 0 && last_msg_time_ == 0)
		{
			for (const auto &i : msg)
			{
				if (!IsNoticeMsg(i))
				{
					last_msg_time_ = i.timetag_;
					break;
				}
			}
		}

		if (session_type_ == nim::kNIMSessionTypeP2P)
		{
			//检查回执
			CheckLastReceiptMsg();
			nim::IMMessage msg;
			if (GetLastNeedSendReceiptMsg(msg))
			{
				nim::MsgLog::SendReceiptAsync(msg.ToJsonString(false), [](const nim::MessageStatusChangedResult& res) {
					auto iter = res.results_.begin();
					QLOG_APP(L"mark receipt id:{0} time:{1} rescode:{2}") << iter->talk_id_ << iter->msg_timetag_ << res.rescode_;
				});
			}
		}
	}
	else
	{
		msg_list_->SetPos(msg_list_->GetPos());

		ui::CSize sz = msg_list_->GetScrollPos();
		sz.cy = msg_list_->GetScrollRange().cy - pos - 50;
		msg_list_->SetScrollPos(sz);
	}
	//修正最远时间
	if (len > 0)
	{
		farst_msg_time_ = msg[len - 1].timetag_;
	}
}

void SessionBox::AddWritingMsg(const nim::IMMessage &msg)
{
	bool at_end = msg_list_->IsAtEnd();

	Json::Value json;
	if (StringToJson(msg.attach_, json))
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

void SessionBox::ShowMsgWriting(const nim::IMMessage &msg)
{
	cancel_writing_timer_.Cancel();
	StdClosure cb = nbase::Bind(&SessionBox::CancelWriting, this);
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

void SessionBox::CancelWriting()
{
	bool at_end = msg_list_->IsAtEnd();

	RemoveTip(STT_WRITING);

	if(at_end)
		msg_list_->EndDown(true, false);
}

void SessionBox::AddRetweetMsg(const nim::IMMessage &msg)
{
	// 如果这个消息对应的资源文件被清理掉，就重新下载
	if (!IsResourceExist(msg))
	{
		nim::NOS::FetchMedia(msg, nbase::Bind(&SessionBox::OnRetweetResDownloadCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), nim::NOS::ProgressCallback());
	}
	this->AddSendingMsg(msg);
}

void SessionBox::OnSendMsgCallback(const std::string &cid, int code, __int64 msg_timetag)
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if (it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if (item)
		{
			if (code == nim::kNIMResSuccess)
			{
				item->SetMsgStatus(nim::kNIMMsgLogStatusSent);
				item->UpdateMsgTime(msg_timetag);
			}
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

void SessionBox::OnMsgStatusChangedCallback(const std::string &from_accid, const __int64 timetag, nim::NIMMsgLogStatus status)
{
	if (!last_receipt_msg_id_.empty())
	{
		auto it = id_bubble_pair_.find(last_receipt_msg_id_);
		if (it != id_bubble_pair_.end())
		{
			MsgBubbleItem* item = it->second;
			if (item)
				item->SetMsgStatus(nim::kNIMMsgLogStatusSent);
		}
	}

	std::string my_id = LoginManager::GetInstance()->GetAccount();
	auto iter = cached_msgs_bubbles_.rbegin();
	for (; iter != cached_msgs_bubbles_.rend(); ++iter)
	{
		MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
		if (item)
		{
			nim::IMMessage msg = item->GetMsg();
			if (msg.sender_accid_ == my_id && item->IsMyMsg() &&/* msg.timetag_ <= timetag*/nim::MsgLog::QueryMessageBeReaded(msg))
			{
				item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
				last_receipt_msg_id_ = msg.client_msg_id_;
				break;
			}
		}
	}
}

void SessionBox::OnSnapchatReadCallback(const std::string& client_msg_id)
{
	RemoveMsgItem(client_msg_id);
}

void SessionBox::OnDownloadCallback(const std::string &cid, bool success)
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if (it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if (item)
		{
			item->OnDownloadCallback(success);
		}
	}
}

void SessionBox::OnRecallMsgCallback(nim::NIMResCode code, const nim::RecallMsgNotify &notify)
{
	if (code != nim::kNIMResSuccess)
	{
		std::wstring toast = nbase::StringPrintf(L"recall msg error, code:%d, id:%s", code, nbase::UTF8ToUTF16(notify.msg_id_).c_str());
		nim_ui::ShowToast(toast, 5000, this->GetWindow()->GetHWND());
		return;
	}
	int index = RemoveMsgItem(notify.msg_id_);
	if (index > -1 && notify.msglog_exist_)			//撤回本地不存在的消息的通知不在消息流中插入通知
	{
		std::wstring notify_text = GetRecallNotifyText(notify.from_id_);

		nim::IMMessage msg;
		msg.timetag_ = notify.notify_timetag_;
		msg.client_msg_id_ = QString::GetGUID();
		msg.receiver_accid_ = session_id_;
		msg.session_type_ = session_type_;
		msg.sender_accid_ = notify.from_id_;
		msg.status_ = nim::kNIMMsgLogStatusSent;
		msg.type_ = nim::kNIMMessageTypeText;
		Json::Value values;
		values["comment"] = "is_recall_notification";
		values["notify_from"] = notify.from_id_;
		msg.attach_ = values.toStyledString();
		msg.content_ = nbase::UTF16ToUTF8(notify_text);
		msg.msg_setting_.push_need_badge_ = nim::BS_FALSE; //设置会话列表不需要计入未读数
		nim::MsgLog::WriteMsglogToLocalAsync(session_id_, msg, true, nbase::Bind(&SessionBox::WriteMsglogCallback, this, notify_text, std::placeholders::_1, std::placeholders::_2));
	}
}

void SessionBox::OnRetweetResDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if (it != id_bubble_pair_.end())
	{
		MsgBubbleItem* item = it->second;
		if (item)
		{
			item->OnDownloadCallback(code == nim::kNIMResSuccess);
		}
	}
}

void SessionBox::SendText( const std::string &text )
{
	nim::IMMessage msg;
	//TODO(litianyi)
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeText;
	msg.content_ = text;
	//nickname客户端不需要填写

	AddSendingMsg(msg);

	nim::MessageSetting setting;
	//判断是否包含@某人的消息
	if (session_type_ == nim::kNIMSessionTypeTeam && !uid_at_someone_.empty())
	{
		setting.is_force_push_ = nim::BS_TRUE;
		setting.force_push_content_ = text;

		//检查文本消息中是否存在“@xxx ”的文本
		for (auto it = uid_at_someone_.begin(); it != uid_at_someone_.end(); ++it)
		{
			std::string nick_name = it->first;
			std::string at_str = "@";
			at_str.append(nick_name);
			at_str.append(" ");

			if (text.find(at_str) != std::string::npos)
				setting.force_push_ids_list_.push_back(it->second);
		}

		uid_at_someone_.clear();
	}

	std::string json_msg = nim::Talk::CreateTextMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, msg.content_, setting, msg.timetag_);
	nim::Talk::SendMsg(json_msg);
}

void SessionBox::SendImage( const std::wstring &src )
{
	nim::IMMessage msg;
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeImage;

	std::wstring filename = nbase::UTF8ToUTF16(msg.client_msg_id_);
	std::wstring dest = GetUserImagePath();
	if (!nbase::FilePathIsExist(dest, true))
		nbase::CreateDirectory(dest);
	
	dest += filename;
	GenerateUploadImage(src, dest);
	msg.local_res_path_ = nbase::UTF16ToUTF8(dest);

	nim::IMImage img;
	img.md5_ = GetFileMD5(dest);
	img.size_ = (long)nbase::GetFileSize(dest);

	Gdiplus::Image image(dest.c_str());
	if (image.GetLastStatus() != Gdiplus::Ok)
	{
		assert(0);
	}
	else
	{
		img.width_ = image.GetWidth();
		img.height_ = image.GetHeight();
	}

	msg.attach_ = img.ToJsonString();

	AddSendingMsg(msg);
	std::string json_msg = nim::Talk::CreateImageMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, img, msg.local_res_path_, nim::MessageSetting(), msg.timetag_);
	nim::Talk::SendMsg(json_msg);
}

void SessionBox::SendAudio(const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration)
{
	std::wstring wfile_path = nbase::UTF8ToUTF16(file_path);
	if (!nbase::FilePathIsExist(wfile_path, false))
		return;

	nim::IMMessage msg;
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeAudio;
	msg.local_res_path_ = file_path;

	nim::IMAudio audio;
	audio.duration_ = audio_duration;
	auto token_list = nbase::StringTokenize(wfile_path.c_str(), L"\\");
	audio.md5_ = GetFileMD5(token_list.back());
	audio.size_ = file_size;
	audio.file_extension_ = file_ext;
	msg.attach_ = audio.ToJsonString();

	AddSendingMsg(msg);
	std::string json_msg = nim::Talk::CreateAudioMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, audio, msg.local_res_path_, nim::MessageSetting(), msg.timetag_);
	nim::Talk::SendMsg(json_msg);
}

void SessionBox::SendSnapChat(const std::wstring &src)
{
	auto weak_flag = this->GetWeakFlag();
	nim::IMMessage msg;
	PackageMsg(msg);
	msg.msg_setting_.server_history_saved_ = nim::BS_FALSE;
	msg.msg_setting_.roaming_ = nim::BS_FALSE;
	msg.msg_setting_.self_sync_ = nim::BS_FALSE;
	//TODO(litianyi)
	std::wstring filename = nbase::UTF8ToUTF16(msg.client_msg_id_);
	std::wstring dest = GetUserImagePath() + filename;
	GenerateUploadImage(src, dest);
	msg.local_res_path_ = nbase::UTF16ToUTF8(dest);

	nim::NOS::UploadResource(msg.local_res_path_, [this, msg, weak_flag](int res_code, const std::string& url) {
		if (!weak_flag.expired() && res_code == nim::kNIMResSuccess)
		{
			nim::IMMessage new_msg = msg;
			std::string md5 = GetFileMD5(nbase::UTF8ToUTF16(new_msg.local_res_path_));
			int file_size = (int)nbase::GetFileSize(nbase::UTF8ToUTF16(new_msg.local_res_path_));
			new_msg.type_ = nim::kNIMMessageTypeCustom;
			Json::Value json;
			Json::Value json_data;
			Json::FastWriter writer;
			json_data["size"] = file_size;
			json_data["md5"] = md5;
			json_data["url"] = url;
			json["type"] = CustomMsgType_SnapChat;
			json["data"] = json_data;
			new_msg.content_ = nbase::UTF16ToUTF8(L"阅后即焚");
			new_msg.attach_ = writer.write(json);
			AddSendingMsg(new_msg);
	
			nim::Talk::SendMsg(new_msg.ToJsonString(true));
		}
	});
}

bool SessionBox::CheckFileSize(const std::wstring &src)
{
	int64_t sz = nbase::GetFileSize(src);
	if (sz > LoginManager::GetInstance()->GetFileSizeLimit()*1024*1024 || sz <= 0)
	{
		return false;
	}
	return true;
}

void SessionBox::SendFile(const std::wstring &src)
{
	nim::IMMessage msg;
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeFile;
	msg.local_res_path_ = nbase::UTF16ToUTF8(src);

	nim::IMFile file;
	file.md5_ = GetFileMD5(src);
	file.size_ = (long)nbase::GetFileSize(src);

	nbase::PathString file_name;
	nbase::FilePathApartFileName(src, file_name);
	std::wstring file_exten;
	nbase::FilePathExtension(file_name, file_exten);
	file.display_name_ = nbase::UTF16ToUTF8(file_name);
	file.file_extension_ = nbase::UTF16ToUTF8(file_exten);
	msg.attach_ = file.ToJsonString();

	AddSendingMsg(msg);

	nim::Talk::FileUpPrgCallback* cb_pointer = nullptr;
	MsgBubbleFile* bubble = dynamic_cast<MsgBubbleFile*>(msg_list_->FindSubControl(nbase::UTF8ToUTF16(msg.client_msg_id_)));
	if (!msg.local_res_path_.empty() && nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg.local_res_path_), false) && bubble)
	{
	}
	std::string json_msg = nim::Talk::CreateFileMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, file, msg.local_res_path_, nim::MessageSetting(), msg.timetag_);
	nim::Talk::SendMsg(json_msg, msg.client_msg_id_, cb_pointer);
}

void SessionBox::SendJsb( const std::string &attach )
{
	nim::IMMessage msg;
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeCustom;

	msg.content_ = nbase::UTF16ToUTF8(L"这是一个猜拳");
	msg.attach_ = attach;
	
	AddSendingMsg(msg);

	nim::Talk::SendMsg(msg.ToJsonString(true));
}

void SessionBox::SendSticker(const std::string &catalog, const std::string &name)
{
	nim::IMMessage msg;
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeCustom;

	Json::Value json;
	Json::FastWriter writer;
	json["type"] = CustomMsgType_Sticker;
	json["data"]["catalog"] = catalog;
	json["data"]["chartlet"] = name;

	msg.content_ = nbase::UTF16ToUTF8(L"贴图");
	msg.attach_ = writer.write(json);

	AddSendingMsg(msg);

	nim::Talk::SendMsg(msg.ToJsonString(true));
}

void SessionBox::SendTip(const std::wstring &tip)
{
	nim::IMMessage msg;
	PackageMsg(msg);
	msg.type_ = nim::kNIMMessageTypeTips;
	msg.content_ = nbase::UTF16ToUTF8(tip);
	msg.msg_setting_.need_push_ = nim::BS_FALSE;
	msg.status_ = nim::kNIMMsgLogStatusSent;

	AddSendingMsg(msg);
	nim::Talk::SendMsg(msg.ToJsonString(true));
}

void SessionBox::AddSendingMsg(const nim::IMMessage &msg)
{
	writing_time_ = 0;
	RemoveTip(STT_WRITING);

	bool show_time = false;
	if(last_msg_time_ == 0)
	{
		show_time = true;
		farst_msg_time_ = msg.timetag_;
	}
	else
	{
		show_time = CheckIfShowTime(last_msg_time_, msg.timetag_);
	}
	
	if (!IsNoticeMsg(msg))
		last_msg_time_ = msg.timetag_;

	MsgBubbleItem* item = ShowMsg(msg, false, show_time);
	msg_list_->EndDown(true, false);
}

void SessionBox::ReSendMsg(nim::IMMessage &msg)
{
	msg.msg_setting_.resend_flag_ = nim::BS_TRUE;
	msg.status_ = nim::kNIMMsgLogStatusSending;
	msg.timetag_= 1000 * nbase::Time::Now().ToTimeT();

	AddSendingMsg(msg);

	nim::Talk::SendMsg(msg.ToJsonString(true));
}

void SessionBox::PackageMsg(nim::IMMessage &msg)
{
	msg.session_type_			= session_type_;
	msg.receiver_accid_		= session_id_;
	msg.sender_accid_	= LoginManager::GetInstance()->GetAccount();
	msg.client_msg_id_   = QString::GetGUID();
	msg.msg_setting_.resend_flag_ = nim::BS_FALSE;

	//base获取的时间单位是s，服务器的时间单位是ms
	msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

	msg.status_ = nim::kNIMMsgLogStatusSending;
}

void SessionBox::CheckLastReceiptMsg()
{
	if (!last_receipt_msg_id_.empty())
		return;

	std::string my_id = LoginManager::GetInstance()->GetAccount();
	auto iter = cached_msgs_bubbles_.rbegin();
	for (; iter != cached_msgs_bubbles_.rend(); ++iter)
	{
		MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
		if (item)
		{
			nim::IMMessage msg = item->GetMsg();
			if (msg.sender_accid_ == my_id && item->IsMyMsg() && msg.status_ == nim::kNIMMsgLogStatusReceipt)
			{
				item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
				last_receipt_msg_id_ = msg.client_msg_id_;
				break;
			}
		}
	}
}

bool SessionBox::GetLastNeedSendReceiptMsg(nim::IMMessage &msg)
{
	std::string my_id = LoginManager::GetInstance()->GetAccount();
	auto iter = cached_msgs_bubbles_.rbegin();
	for (; iter != cached_msgs_bubbles_.rend(); ++iter)
	{
		MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
		if (item)
		{
			nim::IMMessage message = item->GetMsg();
			if (message.sender_accid_ != my_id || !item->IsMyMsg())
			{
				if (!nim::MsgLog::QueryMessageBeReaded(message))
				{
					msg = message;
					return true;
				}
				return false;
			}
		}
	}
	return false;
}

int SessionBox::RemoveMsgItem(const std::string& client_msg_id)
{
	int index = -1;
	MsgBubbleItem* msg_item = NULL;
	MsgBubbleNotice* msg_cell = NULL;
	for (int i = 0; i < msg_list_->GetCount(); i++)
	{
		if (msg_list_->GetItemAt(i)->GetUTF8Name() == client_msg_id)
		{
			msg_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
			msg_cell = dynamic_cast<MsgBubbleNotice*>(msg_list_->GetItemAt(i));
			index = i;
			break;
		}
	}
	if (!msg_item)
	{
		if (msg_cell) //要删除的是一个通知消息，删除后直接return
			msg_list_->Remove(msg_cell);
		return index;
	}
	
	auto iter2 = std::find(cached_msgs_bubbles_.begin(), cached_msgs_bubbles_.end(), msg_item);
	if (iter2 != cached_msgs_bubbles_.end())
	{
		if (last_receipt_msg_id_ == client_msg_id) //本条要被删除的消息是显示已读的消息，则把上一条自己发的消息显示为已读。
		{
			auto iter3 = std::reverse_iterator<decltype(iter2)>(iter2); //iter3现在指向iter2的上一个元素
			std::string my_id = LoginManager::GetInstance()->GetAccount();
			while (iter3 != cached_msgs_bubbles_.rend())
			{
				MsgBubbleItem* item = dynamic_cast<MsgBubbleItem*>(*iter3);
				if (item)
				{
					nim::IMMessage msg = item->GetMsg();
					if (msg.sender_accid_ == my_id)
					{
						item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
						last_receipt_msg_id_ = msg.client_msg_id_;
						break;
					}
				}
				iter3++;
			}
			if (iter3 == cached_msgs_bubbles_.rend())
				last_receipt_msg_id_ = "";
		}
		cached_msgs_bubbles_.erase(iter2); //从cached_msgs_bubbles_中删除
	}

	auto iter1 = id_bubble_pair_.find(client_msg_id);
	if (iter1 != id_bubble_pair_.end())
		id_bubble_pair_.erase(client_msg_id); //从id_bubble_pair_删除

	msg_list_->Remove(msg_item); //最后从msg_list_中删除并销毁该MsgBubbleItem

	return index;
}

std::wstring SessionBox::GetRecallNotifyText(const std::string& msg_from_id)
{
	std::wstring notify_text;
	if (msg_from_id == LoginManager::GetInstance()->GetAccount())
	{
		notify_text = L"我撤回了一条消息";
	}
	else
	{
		if (session_type_ == nim::kNIMSessionTypeP2P)
		{
			notify_text = L"对方撤回了一条消息";
		}
		else
		{
			auto info = GetTeamMemberInfo(msg_from_id);
			UTF8String name = info.GetNick();
			if (name.empty())
			{
				nim::UserNameCard name_card;
				UserService::GetInstance()->GetUserInfo(msg_from_id, name_card);
				name = name_card.GetName();
			}
			if (name.empty())
				name = msg_from_id;
			notify_text = nbase::UTF8ToUTF16(name) + L" 撤回了一条消息";
		}
	}

	return notify_text;
}

void SessionBox::CheckHeader()
{
	std::wstring name, photo;
	bool is_team = (session_type_ == nim::kNIMSessionTypeTeam);

	if (is_team)
	{
		label_tid_->SetUTF8Text(session_id_);
		name = TeamService::GetInstance()->GetTeamName(session_id_);
		photo = PhotoService::GetInstance()->GetTeamPhoto(session_id_, true);

	}
	else
	{
		label_tid_->SetVisible(false);
		name = IsFileTransPhone() ? L"我的手机" : UserService::GetInstance()->GetUserName(session_id_);
		photo = PhotoService::GetInstance()->GetUserPhoto(session_id_);
	}

	SetTitleName(name);
	SetHeaderPhoto(photo);
}

void SessionBox::OnRelink(const Json::Value &json)
{
	bool link = json["link"].asBool();
	if (link)
	{
		RemoveTip(STT_LINK);
	}
}

bool SessionBox::IsFileTransPhone()
{
	if (session_type_ == nim::kNIMSessionTypeP2P && LoginManager::GetInstance()->IsEqual(session_id_))
	{
		return true;
	}
	return false;
}

void SessionBox::SetTitleName(const std::wstring &name)
{
	label_title_->SetText(name);
	session_form_->SetMergeItemName(nbase::UTF8ToUTF16(session_id_), name);
	SetTaskbarTitle(name);
}

void SessionBox::SetHeaderPhoto(const std::wstring &photo)
{
	btn_header_->SetBkImage(photo);
	session_form_->SetMergeItemHeaderPhoto(nbase::UTF8ToUTF16(session_id_), photo);
	SetTaskbarIcon(photo);
}

void SessionBox::SetTaskbarTitle(const std::wstring &title)
{
	if (session_form_->GetSelectedSessionBox() == this)
	{
		session_form_->SetTaskbarTitle(title);
	}
}

void SessionBox::SetTaskbarIcon(const std::wstring &icon)
{
	if (session_form_->GetSelectedSessionBox() == this)
	{
		session_form_->SetTaskbarIcon(icon);
	}
}

void SessionBox::WriteMsglogCallback(const std::wstring &notify_text, nim::NIMResCode res_code, const std::string& msg_id)
{
	if (res_code == nim::kNIMResSuccess)
	{
		MsgBubbleNotice* cell = new MsgBubbleNotice;
		GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
		msg_list_->Add(cell);
		cell->InitControl();
		cell->InitCustomInfo(notify_text, session_id_, msg_id);
	}
}

void SessionBox::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	auto refresh_msglist = [this](const nim::UserNameCard& info) //更新消息列表中消息气泡的头像和名称
	{
		bool refresh_icon = info.ExistValue(nim::kUserNameCardKeyIconUrl);
		bool refresh_show_name = info.ExistValue(nim::kUserNameCardKeyName) && session_type_ == nim::kNIMSessionTypeTeam;
		if (refresh_show_name)
		{
			auto iter = team_member_info_list_.find(info.GetAccId());
			if (iter != team_member_info_list_.cend() && !iter->second.GetNick().empty()) //设置了群名片，不更新
				refresh_show_name = false;
		}

		if (refresh_icon)
		{
			int msg_count = msg_list_->GetCount();
			for (int i = 0; i < msg_count; i++)
			{
				MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
				if (bubble_item != NULL && bubble_item->msg_.sender_accid_ == info.GetAccId())
					bubble_item->SetShowHeader();
			}
		}

		if (refresh_show_name)
			RefreshMsglistShowname(info.GetAccId());
	};

	for (auto info : uinfos)
	{
		if (session_type_ == nim::kNIMSessionTypeP2P)
		{
			if (info.GetAccId() == session_id_)
				CheckHeader();
		}
		else if (session_type_ == nim::kNIMSessionTypeTeam)
		{
			if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
			{
				auto iter = team_member_info_list_.find(info.GetAccId());
				if (iter != team_member_info_list_.end())
				{
					//更新群成员列表信息
					TeamItem* item = (TeamItem*)member_list_->FindSubControl(nbase::UTF8ToUTF16(info.GetAccId()));
					if (item != NULL)
						item->InitInfo(iter->second);
				}
			}
		}

		refresh_msglist(info);
	}
}

void SessionBox::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	auto refresh_msglist_photo = [=]() //更新消息列表头像
	{
		int msg_count = msg_list_->GetCount();
		for (int i = 0; i < msg_count; i++)
		{
			MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
			if (bubble_item != NULL && bubble_item->msg_.sender_accid_ == accid)
				bubble_item->msg_header_button_->SetBkImage(photo_path);
		}
	};

	if (accid == session_id_)
	{
		SetHeaderPhoto(photo_path);
	}

	if (type == kUser)
	{
		// 群聊时，更新群成员列表和消息列表中用户头像
		if (session_type_ == nim::kNIMSessionTypeTeam)
		{
			if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced && team_member_info_list_.find(accid) != team_member_info_list_.cend())
			{
				TeamItem* item = (TeamItem*)member_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
				if (item != NULL)
					item->FindSubControl(L"member_icon")->SetBkImage(photo_path);

				refresh_msglist_photo();
			}
			else if (team_info_.GetType() == nim::kNIMTeamTypeNormal)
				refresh_msglist_photo();
		}
		else
			refresh_msglist_photo();
	}
}

}