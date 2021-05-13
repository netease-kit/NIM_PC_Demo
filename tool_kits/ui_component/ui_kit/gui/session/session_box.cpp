#include "stdafx.h"
#include "session_box.h"
#include "session_form.h"
#include "session_dock_def.h"
#include "gui/session/control/audio_capture.h"
#include "gui/session/control/atme_view.h"
#include "gui/session/atlist/at_list_form.h"
#include "gui/session/taskbar/taskbar_manager.h"
#include "module/session/session_util.h"
#include "module/session/session_manager.h"
#include "module/session/force_push_manager.h"
#include "module/audio/audio_manager.h"
#include "module/online_state_event/online_state_event_util.h"
#include "module/runtime_data/runtime_data_manager.h"
#include "module/session/transfer_file_manager.h"
#include "callback/session/session_callback.h"
#include "export/nim_ui_window_manager.h"
#include "gui/contact_select_form/contact_select_form.h"
#include "util/user.h"

using namespace ui;

namespace
{
    const int kMsgLogNumberShow = 100;
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

        mark_receipt_when_load_msgs_ = false;
    }

    SessionBox::~SessionBox()
    {
        SessionManager::GetInstance()->RemoveSessionBox(session_id_);
        /*
            ContactSelectForm *contact_select_form = (ContactSelectForm *)WindowsManager::GetInstance()->GetWindow\
                (ContactSelectForm::kClassName, nbase::UTF8ToUTF16(session_id_));
            if (contact_select_form)
                contact_select_form->Close();
        */
    }

    ISessionDock* SessionBox::GetSessionForm() const
    {
        ASSERT(NULL != session_form_);
        ASSERT(::IsWindow(session_form_->GetHWND()));
        return session_form_;
    }

    void SessionBox::InitSessionBox()
    {
        ASSERT(session_form_ != NULL);

        AttachBubbledEvent(ui::kEventAll, nbase::Bind(&SessionBox::Notify, this, std::placeholders::_1));
        AttachBubbledEvent(ui::kEventClick, nbase::Bind(&SessionBox::OnClicked, this, std::placeholders::_1));
        AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&SessionBox::OnSelChanged, this, std::placeholders::_1));
        AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&SessionBox::OnSelChanged, this, std::placeholders::_1));

        label_title_ = (Label*)FindSubControl(L"label_title");
        label_tid_ = (Label*)FindSubControl(L"label_tid");
        label_online_state_ = (Label*)FindSubControl(L"label_online_state");

        btn_header_ = (Button*)FindSubControl(L"btn_header");
        btn_header_->AttachClick(nbase::Bind(&SessionBox::OnBtnHeaderClick, this, std::placeholders::_1));

        btn_invite_ = (Button*)FindSubControl(L"btn_invite");
        btn_invite_->SetVisible(false);
        btn_invite_->AttachClick(nbase::Bind(&SessionBox::OnBtnInvite, this, std::placeholders::_1));

#if defined(USING_RTC_G2)
        Button* btn_audio_g2 = (Button*)FindSubControl(L"btn_audio_g2");
        Button* btn_video_g2 = (Button*)FindSubControl(L"btn_video_g2");
#else
        btn_capture_audio_ = (Option*)FindSubControl(L"btn_capture_audio");
        Button* btn_audio = (Button*)FindSubControl(L"btn_audio");
        Button* btn_video = (Button*)FindSubControl(L"btn_video");
        Button* btn_rts = (Button*)FindSubControl(L"btn_rts");
#endif
		
		if (session_type_ == nim::kNIMSessionTypeP2P && !IsFileTransPhone())
		{
#if defined(USING_RTC_G2)
			btn_audio_g2->SetVisible(true);
            btn_video_g2->SetVisible(true);
#else
            btn_audio->SetVisible(true);
            btn_video->SetVisible(true);
            btn_rts->SetVisible(true);
#endif
		}
#if !defined(USING_RTC_G2)
		btn_video->SetVisible(true);
#endif
		btn_max_restore_ = (Button*)FindSubControl(L"btn_max_restore");
        msg_content_ = (Box*)FindSubControl(L"msg_content");
        msg_list_ = (ListBox*)FindSubControl(L"msg_list");
        btn_face_ = (CheckBox*)FindSubControl(L"btn_face");
        input_edit_ = (RichEdit*)FindSubControl(L"input_edit");
        btn_team_ack_ui_ = (CheckBox*)FindSubControl(L"btn_team_ack_ui");
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
        member_list_ = (VirtualListBox*)FindSubControl(L"member_list");
        int kRoomMemberItemHeight = 50;
        member_list_->SetElementHeight(ui::DpiManager::GetInstance()->ScaleInt(kRoomMemberItemHeight));
        member_list_->SetDataProvider(this);
        member_list_->InitElement(30);
        bottom_panel_ = (VBox*)FindSubControl(L"bottom_panel");

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

        //群回执消息
        if (session_type_ == nim::kNIMSessionTypeTeam || session_type_ == nim::kNIMSessionTypeSuperTeam) {
            btn_team_ack_ui_->SetVisible(true);
        }
        else {
            btn_team_ack_ui_->SetVisible(false);
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

        // 初始化任务栏缩略图
        {
            taskbar_item_ = new TaskbarTabItem(this);
            if (taskbar_item_)
                taskbar_item_->Init(nbase::UTF8ToUTF16(session_id_));
        }

        OnSelectAtItem cb = nbase::Bind(&SessionBox::OnSelectAtItemCallback, this, std::placeholders::_1);
        AtlistForm *at_list_form = new AtlistForm(session_id_, session_type_, ToWeakCallback(cb));
        at_list_form->Create(this->GetWindow()->GetHWND(), L"", WS_POPUPWINDOW, 0L);

        CheckHeader();
        //CheckTeamType(nim::kNIMTeamTypeNormal);
        OnWndSizeMax(TRUE == IsZoomed(this->GetWindow()->GetHWND()));

        if (session_type_ == nim::kNIMSessionTypeTeam)
        {
            FindSubControl(L"btn_snapchat")->SetVisible(false);
            FindSubControl(L"btn_transfer_file")->SetVisible(false);
            InvokeGetTeamMember();
            InvokeGetTeamInfo();
        }
        else
        {
            is_header_enable_ = true;
        }

        if (RunTimeDataManager::GetInstance()->GetUIStyle() == UIStyle::join)
            FindSubControl(L"sysbar")->SetFixedWidth(0);


        unregister_cb.Add(NotifyCenter::GetInstance()->RegNotify(NT_LINK, nbase::Bind(&SessionBox::OnRelink, this, std::placeholders::_1)));
        unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&SessionBox::OnUserInfoChange, this, std::placeholders::_1)));
        unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&SessionBox::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
        if (session_type_ == nim::kNIMSessionTypeTeam)
        {
            unregister_cb.Add(TeamService::GetInstance()->RegAddTeamMember(nbase::Bind(&SessionBox::OnTeamMemberAdd, this, std::placeholders::_1, std::placeholders::_2)));
            unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeamMemberList(nbase::Bind(&SessionBox::OnTeamMemberRemove, this, std::placeholders::_1, std::placeholders::_2)));
            unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMember(nbase::Bind(&SessionBox::OnTeamMemberChange, this, std::placeholders::_1, std::placeholders::_2)));
            unregister_cb.Add(TeamService::GetInstance()->RegSetTeamAdmin(nbase::Bind(&SessionBox::OnTeamAdminSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
            unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamOwner(nbase::Bind(&SessionBox::OnTeamOwnerChange, this, std::placeholders::_1, std::placeholders::_2)));
            unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&SessionBox::OnTeamNameChange, this, std::placeholders::_1)));
            unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeam(nbase::Bind(&SessionBox::OnTeamRemove, this, std::placeholders::_1)));
            unregister_cb.Add(TeamService::GetInstance()->RegAddTeam(nbase::Bind(&SessionBox::OnTeamAdd, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
            unregister_cb.Add(TeamService::GetInstance()->RegMuteMember(nbase::Bind(&SessionBox::OnTeamMuteMember, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
            unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamNotification(nbase::Bind(&SessionBox::OnTeamNotificationModeChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));
        }
        else
        {
            auto receive_event_cb = nbase::Bind(&SessionBox::OnReceiveEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            unregister_cb.Add(SubscribeEventManager::GetInstance()->RegReceiveEventCallback(receive_event_cb));

            FindSubControl(L"not_disturb")->SetVisible(nim_comp::MuteBlackService::GetInstance()->IsInMuteList(session_id_));
            unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetMuteCallback(nbase::Bind(&SessionBox::OnNotifyChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));

            auto friend_list_change_cb = nbase::Bind(&SessionBox::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
            unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));
        }

        auto hied_right = dynamic_cast<ui::Button*>(FindSubControl(L"hide_right"));
        auto show_right = dynamic_cast<ui::Button*>(FindSubControl(L"show_right"));
        hied_right->AttachClick([this, show_right, hied_right](ui::EventArgs* param) {
            hied_right->SetVisible(false);
            show_right->SetVisible(true);
            auto frame_right = FindSubControl(L"frame_right");
            frame_right->SetVisible(false);
            auto spliter = FindSubControl(L"frame_mid_split");
            spliter->SetVisible(false);
            return true;
        });
        show_right->AttachClick([this, show_right, hied_right](ui::EventArgs* param) {
            hied_right->SetVisible(true);
            show_right->SetVisible(false);
            auto frame_right = FindSubControl(L"frame_right");
            frame_right->SetVisible(true);
            auto spliter = FindSubControl(L"frame_mid_split");
            spliter->SetVisible(true);
            return true;
        });
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

    void SessionBox::OnNotifyChangeCallback(std::string id, bool mute)
    {
        if (session_type_ == nim::kNIMSessionTypeP2P && id == session_id_)
            FindSubControl(L"not_disturb")->SetVisible(mute);
    }

    void SessionBox::EndDown(bool withAnimation /*= false*/)
    {
        if (msg_list_)
            msg_list_->EndDown(true, withAnimation);
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

        // 如果收到的消息是 P2P 传送文件消息，那么抛到注入的命令通道来执行
        Json::Value values;
        Json::Reader reader;
        bool parse_success = reader.parse(msg.attach_, values);
        bool is_transfer_file = values.isMember("type") && values["type"].asInt() == nim_comp::CustomMsgType_TransferFile;

        if (msg.type_ == nim::kNIMMessageTypeCustom && parse_success && is_transfer_file)
        {
            QLOG_APP(L"Receive transfer file notification, command type = {0}") << values[kJsonKeyCommand].asString();
            // 协商成功后才能收到这个自定义的传送文件请求
            nim_p2p::NimP2PDvelopKit::GetInstance()->OnReceiveChannelCommand((RemoteFlagType)msg.sender_accid_.c_str(), msg.attach_);
        }

        ShowMsg(msg, false, show_time);

        if (!IsNoticeMsg(msg) && !IsRTSMsg(msg.type_, msg.attach_))
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
        {
            flash = false;
        }
        session_form_->OnNewMsg(*this, create, flash);

        if (at_end && SessionManager::GetInstance()->IsSessionBoxActive(session_id_))
        {
            msg_list_->EndDown(true, false);
        }

        if (session_type_ == nim::kNIMSessionTypeTeam &&
            !IsBubbleRight(msg) &&
            msg.type_ != nim::kNIMMessageTypeNotification &&
            msg.type_ != nim::kNIMMessageTypeTips &&
            msg.msg_setting_.team_msg_need_ack_ == BS_TRUE &&
            msg.msg_setting_.team_msg_ack_sent_ != BS_TRUE)
        {
            new_msgs_need_to_send_mq_.push_back(msg);
        }
        SendTeamReceiptIfNeeded(true);
        SendReceiptIfNeeded(true);
        AddAtMessage(msg);
    }

    MsgBubbleItem* SessionBox::ShowMsg(const nim::IMMessage &msg, bool first, bool show_time)
    {
        const std::string &bubble_id = msg.client_msg_id_;
        if (bubble_id.empty())
        {
            QLOG_WAR(L"msg id empty");
            return nullptr;
        }

        IdBubblePair::iterator it = id_bubble_pair_.find(bubble_id);
        if (it != id_bubble_pair_.end())
        {
            QLOG_WAR(L"repeat msg: {0}") << bubble_id;
            return nullptr;
        }

        MsgBubbleItem* item = NULL;
        if (IsRecallMsg(msg.type_, msg.attach_))
        {
            Json::Value values;
            if (Json::Reader().parse(msg.attach_, values))
            {
                nim::IMMessage notify_msg = msg;
                if (values.isMember("notify_from"))
                {
                    std::string from_id = values["notify_from"].asString();
                    std::string from_nick = values["from_nick"].asString();
                    std::string operator_id = values["operator_id"].asString();
                    if (values.isMember("feature"))
                        notify_msg.feature_ = (nim::NIMMessageFeature)values["feature"].asInt();
                    if (operator_id.empty())
                        operator_id = from_id;
                    notify_msg.content_ = nbase::UTF16ToUTF8(GetRecallNotifyTextEx(GetSessionId(), GetSessionType(), from_id, operator_id, from_nick));
                    if (notify_msg.feature_ == nim::kNIMMessageFeatureRoamMsg)
                        notify_msg.content_.append(" [roaming]");
                    else if (notify_msg.feature_ == nim::kNIMMessageFeatureLeaveMsg)
                        notify_msg.content_.append(" [leave]");
                    else if (notify_msg.feature_ == nim::kNIMMessageFeatureSyncMsg)
                        notify_msg.content_.append(" [sync]");
                    else;
                }
                MsgBubbleNotice* cell = new MsgBubbleNotice;
                GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
                if (first)
                    msg_list_->AddAt(cell, 0);
                else
                    msg_list_->Add(cell);
                cell->InitControl();
                cell->InitInfo(notify_msg, session_id_, true);
                return nullptr;
            }
        }

        if (msg.type_ == nim::kNIMMessageTypeText ||
            IsNetCallMsg(msg.type_, msg.attach_))
        {
            item = new MsgBubbleText;
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
            if (first)
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
                            return nullptr;
                        }
                    }
                }
                else if (sub_type == CustomMsgType_TransferFile)
                {
                    // 跳过一些前期协商不带 session id 和 params 属性的消息
                    if (json.isMember("session_id") && json["params"].isObject())
                    {
                        std::string transfer_file_session_id = json["session_id"].asString();
                        item = new MsgBubbleTransferFile;
                        if (!first)
                        {
                            transfer_file_bubble_list_[transfer_file_session_id] = item;
                            TransferFileManager::GetInstance()->AddItem(session_id_, transfer_file_session_id, item);
                        }
                    }
                    else
                    {
                        return nullptr;
                    }
                }
            }
        }
        else if (msg.type_ == nim::kNIMMessageTypeRobot)
        {
            return nullptr;
        }
		else if (msg.type_ == nim::kNIMMessageTypeG2NetCall)
		{
			//将G2话单显示成一条普通消息
			item = new MsgBubbleText;
		}
        if (item == nullptr)
        {
            QLOG_WAR(L"unknown msg: cid={0} msg_type={1}") << bubble_id << msg.type_;
            item = new MsgBubbleUnknown;
        }

        bool bubble_right = IsBubbleRight(msg);
        if (bubble_right)
            GlobalManager::FillBoxWithCache(item, L"session/bubble_right.xml");
        else
            GlobalManager::FillBoxWithCache(item, L"session/bubble_left.xml");

        if (first)
            msg_list_->AddAt(item, 0);
        else
            msg_list_->Add(item);

        item->InitControl(bubble_right);
        item->InitInfo(msg);
        if (msg.session_type_ == nim::kNIMSessionTypeTeam)
            item->SetTeamMemberGetter([this]()->const decltype(team_member_info_list_)&{
            return team_member_info_list_;
        });
        item->SetSessionId(session_id_);
        item->SetSessionType(session_type_);
        item->SetShowTime(show_time);
        if (bubble_right || msg.session_type_ == nim::kNIMSessionTypeP2P)
            item->SetShowName(false, "");
        else
        {
            auto iter = team_member_info_list_.find(msg.sender_accid_);
            if (iter != team_member_info_list_.cend() && !iter->second->GetNick().empty())
                item->SetShowName(true, iter->second->GetNick()); //显示群名片
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
        auto it_closure_befor_item_add = closure_befor_item_add_.find(bubble_id);
        if (it_closure_befor_item_add != closure_befor_item_add_.end())
        {
            QLOG_WAR(L"it_closure_befor_item_add != closure_befor_item_add_.end() task count {0}") << it_closure_befor_item_add->second.size();
            for (auto & it : it_closure_befor_item_add->second)
                it();
            closure_befor_item_add_.erase(it_closure_befor_item_add);
        }
        return item;
    }
    void SessionBox::InvokeShowSpecifiedCountMsgs(unsigned count)
    {
        QLOG_APP(L"query begin: id={0} type={1} farst_time={2}") << session_id_ << session_type_ << farst_msg_time_;

        nim::MsgLog::QueryMsgAsync(session_id_, session_type_, count, farst_msg_time_,
            nbase::Bind(&TalkCallback::OnQueryMsgCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void SessionBox::ShowMsgs(const std::vector<nim::IMMessage> &msg)
    {
        int pos = msg_list_->GetScrollRange().cy - msg_list_->GetScrollPos().cy;

        bool show_time = false;
        bool show_fast_message = false;
        //msg倒序排列
        size_t len = msg.size();
        for (size_t i = 0; i < len; i++)
        {
            const nim::IMMessage &message = msg[i];
            if (i == len - 1)
            {
                //如果最后一条是提示消息被拒收的消息，要把被拒收的消息加载出来
                show_fast_message = (message.type_ == nim::kNIMMessageTypeTips && message.status_ == nim::kNIMMsgLogStatusRefused);
            }
            if (len == 1 || i == len - 1)
            {
                show_time = true;
            }
            else
            {
                long long older_time = 0;
                for (size_t j = i + 1; j < len; j++)
                {
                    if (!IsNoticeMsg(msg[j]) && !IsRTSMsg(msg[j].type_, msg[j].attach_))
                    {
                        older_time = msg[j].timetag_;
                        break;
                    }
                }
                if (message.type_ == nim::kNIMMessageTypeTips && message.status_ == nim::kNIMMsgLogStatusRefused)
                    show_time = false;
                else
                    show_time = CheckIfShowTime(older_time, message.timetag_);
            }
            ShowMsg(message, true, show_time);

            if (session_type_ == nim::kNIMSessionTypeTeam &&
                message.type_ != nim::kNIMMessageTypeNotification &&
                message.type_ != nim::kNIMMessageTypeTips)
            {
                if (!IsBubbleRight(message))
                {
                    if (message.msg_setting_.team_msg_need_ack_ == BS_TRUE && message.msg_setting_.team_msg_ack_sent_ != BS_TRUE)
                        new_msgs_need_to_send_mq_.push_back(message);
                }
            }
        }

        if (session_type_ == nim::kNIMSessionTypeTeam)
        {
            SendTeamReceiptIfNeeded();
        }

        //加载更多历史消息
        AddTip(STT_LOAD_MORE_MSG);
        if (len < kMsgLogNumberShow)
        {
            Box* box = (Box*)msg_list_->FindSubControl(CELL_LOAD_MORE_MSG);
            assert(box);
            Button* btn = (Button*)box->FindSubControl(CELL_BTN_LOAD_MORE_MSG);
            assert(btn);
            btn->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_ALL_MSGS_SHOWN"));
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
                    if (!IsNoticeMsg(i) && !IsRTSMsg(i.type_, i.attach_))
                    {
                        last_msg_time_ = i.timetag_;
                        break;
                    }
                }
            }

            SendReceiptIfNeeded(false);
        }
        else
        {
            msg_list_->SetPos(msg_list_->GetPos());

            ui::CSize sz = msg_list_->GetScrollPos();
            sz.cy = msg_list_->GetScrollRange().cy - pos - 50;
            msg_list_->SetScrollPos(sz);
        }

        if (session_type_ == nim::kNIMSessionTypeP2P && !mark_receipt_when_load_msgs_)
            ResetLastMsgNeedMarkReceipt();

        //修正最远时间
        if (len > 0)
        {
            farst_msg_time_ = msg[len - 1].timetag_;
        }
        if (show_fast_message)
            InvokeShowSpecifiedCountMsgs(1);
    }

    void SessionBox::AddWritingMsg(const nim::IMMessage &msg)
    {
        bool at_end = msg_list_->IsAtEnd();

        Json::Value json;
        if (StringToJson(msg.attach_, json) && json.isObject())
        {
            std::string id = json["id"].asString();
            if (id == "1")
            {
                ShowMsgWriting(msg);
            }
        }

        if (at_end && SessionManager::GetInstance()->IsSessionBoxActive(session_id_))
        {
            msg_list_->EndDown(true, false);
        }
    }
    void SessionBox::OnDeleteMsglogNotfiy(const std::string &msg_id, const std::string& ext)
    {
        RemoveMsgItem(msg_id);
    }
    void SessionBox::TryToTransferFile(const std::wstring& src)
    {
        received_p2p_reply_ = false;
        transfer_file_ = src;

        Json::Value json;
        Json::FastWriter writer;
        json[kJsonKeyCommand] = kJsonKeyTryTransferFileRequest;

        nim::SysMessageSetting setting;
        setting.need_offline_ = BS_FALSE;

        auto str = nim::SystemMsg::CreateCustomNotificationMsg(session_id_, nim::kNIMSysMsgTypeCustomP2PMsg, nim::Tool::GetUuid(), writer.write(json), setting);
        nim::SystemMsg::SendCustomNotificationMsg(str);

        auto closure = [this]() {
            if (!received_p2p_reply_)
            {
                AddTextTip(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_THE_OTHER_UNOL_OR_UNSUPPOT_P2P"));
            }
        };

        nbase::ThreadManager::PostDelayedTask(kThreadUI, ToWeakCallback(closure), nbase::TimeDelta::FromSeconds(2));
    }

    void SessionBox::ReplyTransferFileRequest(const std::string& sender_accid_)
    {
        Json::Value json;
        Json::FastWriter writer;
        json[kJsonKeyCommand] = kJsonKeySupportedTransferFile;

        nim::SysMessageSetting setting;
        setting.need_offline_ = BS_FALSE;

        auto str = nim::SystemMsg::CreateCustomNotificationMsg(sender_accid_, nim::kNIMSysMsgTypeCustomP2PMsg, nim::Tool::GetUuid(), writer.write(json), setting);
        nim::SystemMsg::SendCustomNotificationMsg(str);
    }

    void SessionBox::TransferFile()
    {
        received_p2p_reply_ = true;

        if (transfer_file_.empty())
            return;

        nim::IMMessage msg;
        PackageMsg(msg);

        msg.status_ = nim::kNIMMsgLogStatusSent;
        msg.type_ = nim::kNIMMessageTypeCustom;
        msg.local_res_path_ = nbase::UTF16ToUTF8(transfer_file_);

        nim_p2p::FILE_INFO file;

        nbase::PathString file_name;
        nbase::FilePathApartFileName(transfer_file_, file_name);
        std::wstring file_exten;
        nbase::FilePathExtension(file_name, file_exten);
        if (!file_exten.empty())
            file_exten = file_exten.substr(1);


        // 传入 SDK 使用
        file.kFilePath = nbase::UTF16ToUTF8(transfer_file_);
        nbase::StringReplaceAll("\\", "/", file.kFilePath);
        // file.kFileMD5 = GetFileMD5(transfer_file_);
        file.kFileName = nbase::UTF16ToUTF8(file_name);
        file.kFileUserExt = nbase::UTF16ToUTF8(file_exten);
        file.kFileSize = nbase::GetFileSize(transfer_file_);

        TransferFileSessionID transfer_file_session_id = nullptr;
        if (!msg.local_res_path_.empty() && nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg.local_res_path_), false))
        {
            transfer_file_session_id = nim_p2p::NimP2PDvelopKit::GetInstance()->TransferFile((RemoteFlagType)session_id_.c_str(), file);

            if (!transfer_file_session_id)
            {
                QLOG_ERR(L"Failed to transfer file to {0}") << session_id_.c_str();
                AddTextTip(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_THE_OTHER_UNSUPPOT_P2P_OR_MODULE_INVALID"));
                return;
            }
        }

        // 添加自定义类型的 sub type
        Json::Value root;
        Json::Value json_file_info;

        json_file_info["file_name"] = file.kFileName;
        json_file_info["file_ext"] = file.kFileUserExt;
        json_file_info["file_size"] = file.kFileSize;
        json_file_info["file_path"] = file.kFilePath;
        json_file_info["file_md5"] = file.kFileMD5;

        root["type"] = CustomMsgType_TransferFile;
        root["params"]["file_info"] = json_file_info;
        root["session_id"] = transfer_file_session_id;

        // 让 msg id 与传输文件的 msg 对应
        msg.client_msg_id_ = transfer_file_session_id;
        msg.attach_ = root.toStyledString();

        AddSendingMsg(msg);

        transfer_file_.clear();
    }

    void SessionBox::ShowMsgWriting(const nim::IMMessage &msg)
    {
        cancel_writing_timer_.Cancel();
        StdClosure cb = nbase::Bind(&SessionBox::CancelWriting, this);
        auto weak_cb = cancel_writing_timer_.ToWeakCallback(cb);
        nbase::ThreadManager::PostDelayedTask(weak_cb, nbase::TimeDelta::FromSeconds(kCellCancelWriting));

        if (has_writing_cell_)
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

        if (at_end)
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
                else if (code == nim::kNIMResInBlack)
                {
                    MutiLanSupport* mls = MutiLanSupport::GetInstance();
                    SendRefusedTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_MSG_SENT_BUT_REJECTED"));
                    item->SetMsgStatus(nim::kNIMMsgLogStatusRefused);
                }
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
                if (msg.sender_accid_ == my_id && item->IsMyMsg() && msg.status_ == nim::kNIMMsgLogStatusSent && nim::MsgLog::QuerySentMessageBeReaded(msg))
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

    void SessionBox::OnDownloadCallback(const std::string &res_id, bool success, const std::string& file_path)
    {
        static auto OnDownloadCallback_task = [](MsgBubbleItem* item, bool success, const std::string& file_path) {
            item->OnDownloadCallback(success, file_path);
        };
        IdBubblePair::iterator it = id_bubble_pair_.find(res_id);
        if (it != id_bubble_pair_.end())
        {
            OnDownloadCallback_task(it->second, success, file_path);
        }
        else
        {
            QLOG_WAR(L"MsgRecordForm::OnDownloadCallback cid {0}") << res_id;
            closure_befor_item_add_[res_id].emplace_back(ToWeakCallback(
                [this, res_id, success, file_path]() {
                IdBubblePair::iterator it = id_bubble_pair_.find(res_id);
                if (it != id_bubble_pair_.end())
                    OnDownloadCallback_task(it->second, success, file_path);
            })
            );
        }
    }

    void SessionBox::OnUploadCallback(const std::string &res_id, bool success, const std::string& url)
    {
        IdBubblePair::iterator it = id_bubble_pair_.find(res_id);
        if (it != id_bubble_pair_.end())
        {
            MsgBubbleItem* item = it->second;
            if (item)
            {
                item->OnUploadCallback(success, url);
            }
        }
    }

    void SessionBox::OnRecallMsgCallback(nim::NIMResCode code, const nim::RecallMsgNotify &notify)
    {
        if (code != nim::kNIMResSuccess)
        {
            std::wstring toast = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_RECALL_MSG_OVERTIME");
            nim_ui::ShowToast(toast, 5000, this->GetWindow()->GetHWND());
            return;
        }

        bool is_at_end = msg_list_->IsAtEnd();
        int index = RemoveMsgItem(notify.msg_id_);
        if (is_at_end)
            msg_list_->EndDown(true, false);

        // 	if (index < 0)
        // 	{
        // 		QLOG_APP(L"SessionBox::OnRecallMsgCallback RemoveMsgItem faild: code:{0} from_id:{1} to_id:{2} msg_time:{3} msglog_createtime:{4} msg_id:{5}")
        // 			<< code << notify.from_id_ << notify.to_id_ << notify.notify_timetag_ << notify.msglog_timetag_ << notify.msg_id_;

        index = FindIndexByMsgTime(notify.msglog_timetag_);
        // 		if (index < 0)
        // 		{
        // 			QLOG_APP(L"SessionBox::OnRecallMsgCallback FindIndexByMsgTime faild: code:{0} from_id:{1} to_id:{2} msg_time:{3} msglog_createtime:{4} msg_id:{5}")
        // 				<< code << notify.from_id_ << notify.to_id_ << notify.notify_timetag_ << notify.msglog_timetag_ << notify.msg_id_;
        // 		}
        // 	}

        if (index > -1)
        {
            std::wstring notify_text = GetRecallNotifyTextEx(GetSessionId(), GetSessionType(), notify.from_id_, notify.operator_id_, "");

            nim::IMMessage msg;
            msg.timetag_ = notify.msglog_timetag_;
            msg.client_msg_id_ = nim::Tool::GetUuid();
            msg.receiver_accid_ = session_id_;
            msg.session_type_ = session_type_;
            msg.sender_accid_ = notify.from_id_;
            msg.status_ = nim::kNIMMsgLogStatusSent;
            msg.type_ = nim::kNIMMessageTypeText;
            msg.feature_ = notify.notify_feature_;

            Json::Value values;
            values["comment"] = "is_recall_notification";
            values["notify_from"] = notify.from_id_;
            values["from_nick"] = notify.from_nick_;
            values["operator_id"] = notify.operator_id_;
            values["attach"] = notify.attach_;
            values["callback_ext"] = notify.callback_ext_;
            msg.attach_ = values.toStyledString();
            msg.content_ = nbase::UTF16ToUTF8(notify_text);
            msg.msg_setting_.push_need_badge_ = BS_FALSE; //设置会话列表不需要计入未读数
            nim::MsgLog::WriteMsglogToLocalAsync(session_id_, msg, true, nbase::Bind(&SessionBox::WriteMsglogCallback, this, std::placeholders::_1, std::placeholders::_2, msg, index, is_at_end));
        }
    }

    void SessionBox::OnRetweetResDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
    {
        OnDownloadCallback(cid, code == nim::kNIMResSuccess, file_path);
    }

    MsgBubbleItem* SessionBox::FindBubbleByTransferFileSID(TransferFileSessionID transfer_file_session_id)
    {
        MsgBubbleItem* bubble = nullptr;

#if 0
        for (auto i = 0; i < msg_list_->GetCount(); i++)
        {
            MsgBubbleTransferFile* bubble_item = dynamic_cast<MsgBubbleTransferFile*>(msg_list_->GetItemAt(i));

            if (!bubble_item)
                continue;

            std::string session_id = bubble_item->GetTransferFileSessionId();
            if (session_id == transfer_file_session_id)
            {
                bubble = bubble_item;
                break;
            }
        }
#else
        auto bubble_iter = transfer_file_bubble_list_.find(transfer_file_session_id);
        if (bubble_iter != transfer_file_bubble_list_.end())
        {
            bubble = bubble_iter->second;
        }
#endif

        if (!bubble)
        {
            QLOG_ERR(L"[SessionBox::FindBubbleByTransferFileSessionId] Can not fount bubble by session id {0}") << transfer_file_session_id;
        }

        return bubble;
    }

    void SessionBox::SendText(const std::string &text, bool team_msg_need_ack/* = false*/)
    {
        nim::IMMessage msg;
        PackageMsg(msg);
        std::string robot_accid;
        //判断是否包含@某人的消息
        if (!uid_at_someone_.empty())
        {
            //检查文本消息中是否存在“@xxx ”的文本
            for (auto it = uid_at_someone_.begin(); it != uid_at_someone_.end(); ++it)
            {
                std::string nick_name = it->first;
                std::string at_str = "@";
                at_str.append(nick_name);
                at_str.append(" ");

                int index = text.find(at_str);
                if (index != std::string::npos)
                {
                    if (!it->second.is_robot_)
                    {
                        msg.msg_setting_.force_push_ids_list_.push_back(it->second.uid_);
                    }
                    else
                    {
                        msg.type_ = nim::kNIMMessageTypeRobot;
                        if (robot_accid.empty())//只允许第一个
                            robot_accid = it->second.uid_;
                    }
                }
            }

            if (!msg.msg_setting_.force_push_ids_list_.empty())
            {
                msg.msg_setting_.is_force_push_ = BS_TRUE;
                msg.msg_setting_.force_push_content_ = text;
            }

            uid_at_someone_.clear();
        }

        if (team_msg_need_ack)
            msg.msg_setting_.team_msg_need_ack_ = BS_TRUE;

        std::string json_msg;
        msg.content_ = text;
        if (msg.type_ != nim::kNIMMessageTypeRobot)
        {
            msg.type_ = nim::kNIMMessageTypeText;
            if (GetConfigValue("client_antispam") == "1")
            {
                nim::Tool::FilterClientAntispam(msg.content_, "*", "", ToWeakCallback([this, msg](bool succeed, int ret, const std::string &content)
                {
                    nim::IMMessage msgex = msg;
                    if (ret == 1)
                    {
                        msgex.content_ = content;
                    }
                    else if (ret == 2)
                    {
                        AddTextTip(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_MESSAGE_INCLUDE_PROHIBITED_WORDS"));
                        return;
                    }
                    else if (ret == 3)
                    {
                        msgex.msg_setting_.anti_spam_enable_ = BS_TRUE;
                        msgex.msg_setting_.anti_spam_content_ = msg.content_;
                        msgex.msg_setting_.client_anti_spam_hitting_ = BS_TRUE;
                    }
                    std::string json_msg = nim::Talk::CreateTextMessage(msgex.receiver_accid_, msgex.session_type_, msgex.client_msg_id_, msgex.content_, msgex.msg_setting_, msgex.timetag_);
                    AddSendingMsg(msgex);
                    nim::Talk::SendMsg(json_msg);
                }));
                return;
            }
            json_msg = nim::Talk::CreateTextMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, msg.content_, msg.msg_setting_, msg.timetag_);
        }

        AddSendingMsg(msg);
        nim::Talk::SendMsg(json_msg);
    }

    void SessionBox::SendImage(const std::wstring &src)
    {
        nim::IMMessage msg;
        PackageMsg(msg);
        msg.type_ = nim::kNIMMessageTypeImage;

        //先以消息id为图片名，生成用于上传的图片
        std::wstring image_dir = GetUserImagePath();
        if (!nbase::FilePathIsExist(image_dir, true))
            nbase::CreateDirectory(image_dir);
        std::wstring zoom_path = image_dir + nbase::UTF8ToUTF16(msg.client_msg_id_);
        if (!nbase::FilePathIsExist(zoom_path, false))
        {
            GenerateUploadImage(src, zoom_path);
            if (!nbase::FilePathIsExist(zoom_path, false))
            {
                QLOG_ERR(L"Zoomed image does not exist.");
                return;
            }
        }

        //再计算用于上传的图片的md5，以md5重命名之
        std::string image_md5 = GetFileMD5(zoom_path);
        std::wstring dest = image_dir + nbase::UTF8ToUTF16(image_md5);
        if (!nbase::FilePathIsExist(dest, false))
        {
            if (!::MoveFileEx(zoom_path.c_str(), dest.c_str(), 0))
            {
                QLOG_ERR(L"Rename image error: {0}.") << ::GetLastError();
                return;
            }
        }
        else
            nbase::DeleteFile(zoom_path);

        msg.local_res_path_ = nbase::UTF16ToUTF8(dest);

        nim::IMImage img;
        img.md5_ = image_md5;
        img.size_ = (long)nbase::GetFileSize(dest);
        auto task = [&]() {
            Gdiplus::Image image(dest.c_str());
            if (image.GetLastStatus() != Gdiplus::Ok)
            {
                return false;
            }
            else
            {
                img.width_ = image.GetWidth();
                img.height_ = image.GetHeight();
                return true;
            }
            return false;
        };
        if (!task())
        {
            nbase::DeleteFile(dest);
            nbase::CopyFile(src, dest);
            if (!task())
            {
                assert(0);
            }
        }
        std::wstring file_extension;
        if (nbase::FilePathExtension(src, file_extension))
            img.file_extension_ = nbase::UTF16ToUTF8(file_extension);
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
        //auto token_list = nbase::StringTokenize(wfile_path.c_str(), L"\\");
        audio.md5_ = GetFileMD5(wfile_path/*token_list.back()*/);
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
        msg.msg_setting_.server_history_saved_ = BS_FALSE;
        msg.msg_setting_.roaming_ = BS_FALSE;
        msg.msg_setting_.self_sync_ = BS_FALSE;
        //TODO(litianyi)
        std::wstring zoom_path = GetUserOtherResPath() + nbase::UTF8ToUTF16(msg.client_msg_id_);
        if (!nbase::FilePathIsExist(zoom_path, false))
        {
            GenerateUploadImage(src, zoom_path);
            if (!nbase::FilePathIsExist(zoom_path, false))
            {
                QLOG_ERR(L"Zoomed image does not exist.");
                return;
            }
        }
        std::string image_md5 = GetFileMD5(zoom_path);
        std::wstring dest = GetUserOtherResPath() + nbase::UTF8ToUTF16(image_md5);
        if (!nbase::FilePathIsExist(dest, false))
        {
            if (!::MoveFileEx(zoom_path.c_str(), dest.c_str(), 0))
            {
                QLOG_ERR(L"Rename image error: {0}.") << ::GetLastError();
                return;
            }
        }
        else
            nbase::DeleteFile(zoom_path);

        msg.local_res_path_ = nbase::UTF16ToUTF8(dest);

        nim::NOS::UploadResource2(msg.local_res_path_, nim::kNIMNosDefaultTagIM, [this, msg, image_md5, weak_flag](int res_code, const std::string& url) {
            if (!weak_flag.expired() && res_code == nim::kNIMResSuccess)
            {
                nim::IMMessage new_msg = msg;
                int file_size = (int)nbase::GetFileSize(nbase::UTF8ToUTF16(new_msg.local_res_path_));
                new_msg.type_ = nim::kNIMMessageTypeCustom;
                Json::Value json;
                Json::Value json_data;
                Json::FastWriter writer;
                json_data["size"] = file_size;
                json_data["md5"] = image_md5;
                json_data["url"] = url;
                json["type"] = CustomMsgType_SnapChat;
                json["data"] = json_data;
                new_msg.content_ = nbase::UTF16ToUTF8(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_SNAPCHAT"));
                new_msg.attach_ = writer.write(json);
                nbase::DeleteFile(nbase::UTF8ToUTF16(new_msg.local_res_path_));
                new_msg.local_res_path_ = "";
                AddSendingMsg(new_msg);

                nim::Talk::SendMsg(new_msg.ToJsonString(true));
            }
        });
    }

    bool SessionBox::CheckFileSize(const std::wstring &src)
    {
        int64_t sz = nbase::GetFileSize(src);

        int64_t p2p_file_limit = 2.0 * 1024 * 1024 * 1024;
        int64_t nos_file_limit = LoginManager::GetInstance()->GetFileSizeLimit() * 1024 * 1024;

        if (sz > (use_p2p_transfer_file_ ? p2p_file_limit : nos_file_limit) || sz <= 0)
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
        if (!file_exten.empty())
            file_exten = file_exten.substr(1);
        file.display_name_ = nbase::UTF16ToUTF8(file_name);
        file.file_extension_ = nbase::UTF16ToUTF8(file_exten);
        msg.attach_ = file.ToJsonString();
        DoSendFile(msg, &file);

    }
    void SessionBox::DoSendFile(nim::IMMessage &msg, nim::IMFile* file)
    {
        nim::IMFile temp_file;
        if (file != nullptr)
        {
            temp_file = *file;
        }
        else
        {
            Json::Value json_file;
            if (Json::Reader().parse(msg.attach_, json_file))
            {
                if (json_file.isMember(nim::kNIMMsgAttachKeyExt))
                    temp_file.file_extension_ = json_file[nim::kNIMMsgAttachKeyExt].asString();
                if (json_file.isMember(nim::kNIMMsgAttachKeyMd5))
                    temp_file.md5_ = json_file[nim::kNIMMsgAttachKeyMd5].asString();
                if (json_file.isMember(nim::kNIMMsgAttachKeyDisplayName))
                    temp_file.display_name_ = json_file[nim::kNIMMsgAttachKeyDisplayName].asString();
                if (json_file.isMember(nim::kNIMMsgAttachKeyTag))
                    temp_file.msg_attachment_tag_ = json_file[nim::kNIMMsgAttachKeyTag].asString();
                if (json_file.isMember(nim::kNIMMsgAttachKeySize))
                    temp_file.size_ = json_file[nim::kNIMMsgAttachKeySize].asInt64();
            }
        }
        AddSendingMsg(msg);

        nim::Talk::FileUpPrgCallback* cb_pointer = nullptr;
        MsgBubbleFile* bubble = dynamic_cast<MsgBubbleFile*>(msg_list_->FindSubControl(nbase::UTF8ToUTF16(msg.client_msg_id_)));
        if (!msg.local_res_path_.empty() && nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg.local_res_path_), false) && bubble)
        {
            cb_pointer = new nim::Talk::FileUpPrgCallback(bubble->GetFileUpPrgCallback());
        }
        std::string json_msg = nim::Talk::CreateFileMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, temp_file, msg.local_res_path_, nim::MessageSetting(), msg.timetag_);
        nim::Talk::SendMsg(json_msg, msg.client_msg_id_, cb_pointer);
    }
    void SessionBox::SendJsb(const std::string &attach)
    {
        nim::IMMessage msg;
        PackageMsg(msg);
        msg.type_ = nim::kNIMMessageTypeCustom;

        msg.content_ = nbase::UTF16ToUTF8(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_THIS_IS_A_JSB"));
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

        msg.content_ = nbase::UTF16ToUTF8(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_CHARLET"));
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
        msg.msg_setting_.need_push_ = BS_FALSE;
        msg.status_ = nim::kNIMMsgLogStatusSending;

        AddSendingMsg(msg);
        nim::Talk::SendMsg(msg.ToJsonString(true));
    }

    void SessionBox::AddTextTip(const std::wstring text)
    {
        nim::IMMessage notice_msg;
        notice_msg.client_msg_id_ = nim::Tool::GetUuid();
        notice_msg.content_ = nbase::UTF16ToUTF8(text);
        MsgBubbleNotice* cell = new MsgBubbleNotice;
        GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
        msg_list_->Add(cell);
        cell->InitControl();
        cell->InitInfo(notice_msg, session_id_, true);
    }

    void SessionBox::SendRefusedTip(const std::wstring &tip)
    {
        nim::IMMessage msg;
        PackageMsg(msg);
        msg.msg_setting_.server_history_saved_ = BS_FALSE;//不存云端
        msg.msg_setting_.roaming_ = BS_FALSE;//不漫游
        msg.msg_setting_.self_sync_ = BS_FALSE;//不进行多端同步
        msg.msg_setting_.need_push_ = BS_FALSE;//不推送
        msg.msg_setting_.push_need_badge_ = BS_FALSE;//不计数
        msg.msg_setting_.need_offline_ = BS_FALSE;//不需要支持离线
        msg.msg_setting_.routable_ = BS_FALSE;//不需要抄送
        msg.type_ = nim::kNIMMessageTypeTips;
        msg.content_ = nbase::UTF16ToUTF8(tip);
        msg.msg_setting_.need_push_ = BS_FALSE;
        msg.status_ = nim::kNIMMsgLogStatusSent;
        AddSendingMsg(msg);
        nim::Talk::SendMsg(msg.ToJsonString(true));
    }
    void SessionBox::AddSendingMsg(const nim::IMMessage &msg)
    {
        writing_time_ = 0;
        RemoveTip(STT_WRITING);

        bool show_time = false;
        if (last_msg_time_ == 0)
        {
            show_time = true;
            farst_msg_time_ = msg.timetag_;
        }
        else
        {
            show_time = CheckIfShowTime(last_msg_time_, msg.timetag_);
        }

        if (!IsNoticeMsg(msg) && !IsRTSMsg(msg.type_, msg.attach_))
            last_msg_time_ = msg.timetag_;

        MsgBubbleItem* item = ShowMsg(msg, false, show_time);
        msg_list_->EndDown(true, false);
    }

    void SessionBox::ReSendMsg(nim::IMMessage &msg)
    {
        msg.msg_setting_.resend_flag_ = BS_TRUE;
        msg.status_ = nim::kNIMMsgLogStatusSending;
        msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
        if (msg.type_ == nim::kNIMMessageTypeFile)
        {
            DoSendFile(msg, nullptr);
        }
        else
        {
            AddSendingMsg(msg);
            nim::Talk::SendMsg(msg.ToJsonString(true));
        }
    }

    void SessionBox::PackageMsg(nim::IMMessage &msg)
    {
        msg.session_type_ = session_type_;
        msg.receiver_accid_ = session_id_;
        msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
        msg.client_msg_id_ = nim::Tool::GetUuid();
        msg.msg_setting_.resend_flag_ = BS_FALSE;

        //base获取的时间单位是s，服务器的时间单位是ms
        msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

        msg.status_ = nim::kNIMMsgLogStatusSending;
        if (session_type_ == nim::kNIMSessionTypeTeam || session_type_ == nim::kNIMSessionTypeSuperTeam)
        {
            if (std::atoi(app_sdk::AppSDKInterface::GetInstance()->GetConfigValue(nim::kNIMTeamMessageAckEnabled).c_str()) != 0 && team_msg_need_ack_)
                msg.msg_setting_.team_msg_need_ack_ = BS_TRUE;
        }

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
                    if (!nim::MsgLog::QueryReceivedMsgReceiptSent(message))
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

    bool SessionBox::ResetLastMsgNeedMarkReceipt()
    {
        std::string my_id = LoginManager::GetInstance()->GetAccount();
        auto iter = cached_msgs_bubbles_.rbegin();
        for (; iter != cached_msgs_bubbles_.rend(); ++iter)
        {
            MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
            if (item)
            {
                nim::IMMessage message = item->GetMsg();
                if (message.sender_accid_ == my_id && item->IsMyMsg())
                {
                    if (nim::MsgLog::QuerySentMessageBeReaded(message))
                    {
                        item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
                        last_receipt_msg_id_ = message.client_msg_id_;
                        mark_receipt_when_load_msgs_ = true;
                        return true;
                    }
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
                        if (msg.sender_accid_ == my_id && item->IsMyMsg())
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
        closure_befor_item_add_.erase(client_msg_id);
        msg_list_->Remove(msg_item); //最后从msg_list_中删除并销毁该MsgBubbleItem

        return index;
    }

    int SessionBox::FindIndexByMsgTime(int64_t msg_time)
    {
        int index = -1;
        MsgBubbleItem* msg_item = NULL;
        MsgBubbleNotice* msg_cell = NULL;

        for (int i = 0; i < msg_list_->GetCount(); i++)
        {
            msg_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
            if (msg_item)
            {
                if (msg_time <= msg_item->GetMsgTimeTag())
                {
                    index = i;
                    break;
                }
            }
            else
            {
                msg_cell = dynamic_cast<MsgBubbleNotice*>(msg_list_->GetItemAt(i));
                if (msg_cell && msg_time <= msg_cell->GetMsgTimeTag())
                {
                    index = i;
                    break;
                }
            }
        }

        if (-1 == index)
            return msg_list_->GetCount();
        else
            return index;
    }

    std::wstring SessionBox::GetRecallNotifyText(const std::string& msg_from_id, const std::string& msg_from_nick)
    {
        ui::MutiLanSupport* mls = ui::MutiLanSupport::GetInstance();
        std::wstring notify_text;
        if (msg_from_id == LoginManager::GetInstance()->GetAccount())
        {
            notify_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_I_RECALL_MSG");
        }
        else
        {
            if (session_type_ == nim::kNIMSessionTypeP2P)
            {
                notify_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_OTHER_RECALL_MSG");
            }
            else
            {
                UTF8String name = msg_from_nick;
                auto info = GetTeamMemberInfo(msg_from_id);
                if (name.empty())
                    name = info.GetNick();
                if (name.empty())
                {
                    nim::UserNameCard name_card;
                    UserService::GetInstance()->GetUserInfo(msg_from_id, name_card);
                    name = name_card.GetName();
                }
                if (info.GetUserType() == nim::kNIMTeamUserTypeCreator)
                    notify_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_BY_CREATOR_MSG").c_str(), nbase::UTF8ToUTF16(name).c_str());
                else if (info.GetUserType() == nim::kNIMTeamUserTypeManager)
                    notify_text = nbase::StringPrintf(mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_BY_ADMIN_MSG").c_str(), nbase::UTF8ToUTF16(name).c_str());
                else
                    notify_text = nbase::UTF8ToUTF16(name) + L" " + mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_RECALL_MSG");
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
            label_tid_->SetVisible(true);
            label_tid_->SetUTF8Text(session_id_);
            label_online_state_->SetVisible(false);
            name = TeamService::GetInstance()->GetTeamName(session_id_);
            photo = PhotoService::GetInstance()->GetTeamPhoto(session_id_, true);
        }
        else
        {
            if (SubscribeEventManager::GetInstance()->IsEnabled())
            {
                label_online_state_->SetVisible(true);

                EventDataEx data;
                SubscribeEventManager::GetInstance()->GetEventData(nim::kNIMEventTypeOnlineState, session_id_, data);
                if (data.online_client_.online_client_type_.size() == 0 && session_id_ == LoginManager::GetInstance()->GetAccount())
                    data.online_client_.online_client_type_.insert(nim::kNIMClientTypePCWindows);
                SetOnlineState(data);
            }

            btn_invite_->SetVisible(true);
            label_tid_->SetVisible(false);
            if (IsFileTransPhone())
                name = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_MY_MOBILEPHONE");
            else
                name = UserService::GetInstance()->GetUserName(session_id_);

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

    void SessionBox::SetOnlineState(const EventDataEx &data)
    {
        if (session_type_ != nim::kNIMSessionTypeTeam)
        {
            label_online_state_->SetText(OnlineStateEventUtil::GetOnlineState(session_id_, data, false));
        }
    }

    void SessionBox::SetTitleName(const std::wstring &name)
    {
        if (label_title_)
            label_title_->SetText(name);
        if (session_form_)
            session_form_->SetMergeItemName(nbase::UTF8ToUTF16(session_id_), name);

        if (taskbar_item_)
            taskbar_item_->SetTaskbarTitle(name);

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
        if (session_form_ == nullptr)
        {
            return;
        }

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

    void SessionBox::WriteMsglogCallback(nim::NIMResCode res_code, const std::string& msg_id, const nim::IMMessage& msg, int index, bool is_keep_end)
    {
        if (res_code == nim::kNIMResSuccess)
        {
            MsgBubbleNotice* cell = new MsgBubbleNotice;
            GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
            bool succeed = false;
            if (index > -1)
                succeed = msg_list_->AddAt(cell, index);
            else
                succeed = msg_list_->Add(cell);

            cell->InitControl();
            cell->InitInfo(msg, session_id_, true);

            if (is_keep_end)
                msg_list_->EndDown(true, false);

            if (!succeed)
            {
                QLOG_APP(L"SessionBox::WriteMsglogCallback Add faild: code:{0} index:{1} msg_id:{2} sender:{3} reveiver:{4}")
                    << res_code << index << msg_id << msg.sender_accid_ << msg.receiver_accid_;
            }
        }
        else
        {
            QLOG_APP(L"SessionBox::WriteMsglogCallback Faild: code:{0} index:{1} msg_id:{2} sender:{3} reveiver:{4}")
                << res_code << index << msg_id << msg.sender_accid_ << msg.receiver_accid_;
        }
    }

    void SessionBox::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
    {
        bool need_refresh_member_list = false;
        auto refresh_msglist = [this](const nim::UserNameCard& info) //更新消息列表中消息气泡的头像和名称
        {
            bool refresh_icon = info.ExistValue(nim::kUserNameCardKeyIconUrl);
            bool refresh_show_name = info.ExistValue(nim::kUserNameCardKeyName) && session_type_ == nim::kNIMSessionTypeTeam;
            if (refresh_show_name)
            {
                auto iter = team_member_info_list_.find(info.GetAccId());
                if (iter != team_member_info_list_.cend() && !iter->second->GetNick().empty()) //设置了群名片，不更新
                    refresh_show_name = false;
            }

            if (refresh_icon)
            {
                int msg_count = msg_list_->GetCount();
                for (int i = 0; i < msg_count; i++)
                {
                    MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
                    if (bubble_item)
                    {
                        bool refresh_header = bubble_item->msg_.sender_accid_ == info.GetAccId();
                        if (bubble_item->GetMsg().type_ == nim::kNIMMessageTypeRobot)
                        {
                            nim::IMBotRobot robot;
                            nim::Talk::ParseBotRobotMessageAttach(bubble_item->GetMsg(), robot);
                            refresh_header = !robot.out_msg_;
                        }
                        if (refresh_header)
                            bubble_item->SetShowHeader();
                    }
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
                        ////更新群成员列表信息
                        //TeamItem* item = (TeamItem*)member_list_->FindSubControl(nbase::UTF8ToUTF16(info.GetAccId()));
                        //if (item != NULL)
                        //	item->InitInfo(iter->second);
                        need_refresh_member_list = true;
                    }
                }
            }

            refresh_msglist(info);
        }
        if (need_refresh_member_list)
            RefreshMemberList(true);
    }

    void SessionBox::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
    {
        auto refresh_msglist_photo = [=]() //更新消息列表头像
        {
            int msg_count = msg_list_->GetCount();
            for (int i = 0; i < msg_count; i++)
            {
                MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
                if (bubble_item != NULL)
                {
                    if (bubble_item->msg_.sender_accid_ == accid)
                    {
                        bool refresh_header = true;
                        if (bubble_item->msg_.type_ == nim::kNIMMessageTypeRobot)
                        {
                            nim::IMBotRobot robot_attach;
                            nim::Talk::ParseBotRobotMessageAttach(bubble_item->msg_, robot_attach);
                            refresh_header = !robot_attach.out_msg_;
                        }
                        if (refresh_header)
                        {
                            bubble_item->SetShowHeader();
                        }
                    }
                }
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

    void SessionBox::OnReceiveEvent(int event_type, const std::string &accid, const EventDataEx &data)
    {
        if (event_type == nim::kNIMEventTypeOnlineState)
        {
            if (session_type_ == nim::kNIMSessionTypeP2P && session_id_ == accid)
            {
                SetOnlineState(data);
            }
        }
    }

    void SessionBox::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
    {
        if (accid == session_id_ && change_type == kChangeTypeUpdate || change_type == kChangeTypeAdd)
        {
            CheckHeader();
        }
    }

}