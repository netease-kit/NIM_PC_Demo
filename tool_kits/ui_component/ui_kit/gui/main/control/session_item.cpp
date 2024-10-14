#include "stdafx.h"

#include "callback/session/session_callback.h"
#include "gui/main/control/session_item_helper.h"
#include "gui/profile_form/profile_form.h"
#include "gui/session/session_box.h"
#include "gui/session/session_form.h"
#include "module/online_state_event/online_state_event_util.h"
#include "module/runtime_data/runtime_data_manager.h"
#include "module/session/force_push_manager.h"
#include "module/session/session_manager.h"
#include "module/session/transfer_file_manager.h"
#include "session_item.h"
#include "shared/pin_yin_helper.h"
#include "shared/ui/toast/toast.h"
#include "shared/ui/ui_menu.h"

using namespace ui;

namespace nim_comp {
SessionItem::SessionItem()
    : is_online_session_(false) {
    if (RunTimeDataManager::GetInstance()->GetUIStyle() == UIStyle::join) {
        DetachEvent(ui::EventType::kEventSelect);
        AttachSelect(nbase::Bind(&SessionItem::OnDbClicked, this, std::placeholders::_1));
    } else {
        DetachEvent(ui::EventType::kEventMouseDoubleClick);
        AttachDoubleClick(nbase::Bind(&SessionItem::OnDbClicked, this, std::placeholders::_1));
    }
    SelectWhenInternalMenu(false);
}

SessionItem::~SessionItem() {}

void SessionItem::InitCtrl() {
    label_name_ = (Label*)this->FindSubControl(L"label_name");
    label_msg_ = (Label*)this->FindSubControl(L"label_msg");
    label_online_state_ = (Label*)this->FindSubControl(L"online_state");
    label_atme_ = (Label*)this->FindSubControl(L"at_me_label");
    label_time_ = (Label*)this->FindSubControl(L"label_time");
    box_unread_ = (Box*)this->FindSubControl(L"box_unread");
    label_unread_ = (Label*)this->FindSubControl(L"label_unread");
    head_image_ = (ButtonBox*)this->FindSubControl(L"head_image");
    label_session_item_top_ = (Box*)this->FindSubControl(L"top_icon_name");
}

void SessionItem::InitMsg(const std::shared_ptr<nim::SessionData>& msg) {
    msg_ = msg;
    if (msg_->type_ == nim::kNIMSessionTypeTeam || msg_->type_ == nim::kNIMSessionTypeP2P) {
        head_image_->DetachEvent(ui::EventType::kEventClick);
        head_image_->AttachClick(nbase::Bind(&SessionItem::OnHeadImageClicked, this, msg_->is_robot_session_, std::placeholders::_1));
        DetachEvent(ui::EventType::kEventMouseMenu);
        this->AttachMenu(nbase::Bind(&SessionItem::OnSessionItemMenu, this, std::placeholders::_1));
    }

    SetUTF8Name(msg_->id_);
    SetUTF8DataID(msg_->id_);

    InitUserProfile();   //设置用户名和头像
    UpdateMsgContent();  //更新消息内容
    UpdateUnread();      //刷新未读条数
    ShowAtmeTip(true);

#if 1
    // 显示或隐藏 Top 标志
    if (label_session_item_top_)
        label_session_item_top_->SetVisible(msg->placed_on_top_);
#else
    if (label_session_item_top_)
        label_session_item_top_->SetVisible(msg->stick_top_info_.top_);
#endif

    //更新时间
    if (msg_->msg_timetag_ > 0 && msg_->msg_status_ != nim::kNIMMsgLogStatusDeleted) {
        std::wstring str = GetMessageTime(msg_->msg_timetag_, true);
        label_time_->SetText(str);
        label_time_->SetVisible(true);
    } else
        label_time_->SetVisible(false);

    if (msg_->type_ == nim::kNIMSessionTypeTeam)  // 需要先获得群里最近一条消息中所有人的昵称，再UpdateMsg
    {
        // head_image_->SetMouseEnabled(false); //群头像不响应点击

        relate_ids.clear();
        relate_ids.insert(msg_->msg_sender_accid_);
        Json::Reader reader;
        Json::Value attach;
        if (reader.parse(msg_->msg_attach_, attach)) {
            if (attach.isObject() && attach.isMember(nim::kNIMNotificationKeyData)) {
                Json::Value data = attach[nim::kNIMNotificationKeyData];
                if (data.isObject() && data.isMember(nim::kNIMNotificationKeyDataId))
                    relate_ids.insert(data[nim::kNIMNotificationKeyDataId].asString());
                if (data.isObject() && data.isMember(nim::kNIMNotificationKeyUserNameCards) &&
                    data[nim::kNIMNotificationKeyUserNameCards].isArray()) {
                    Json::Value name_cards_json = data[nim::kNIMNotificationKeyUserNameCards];
                    for (uint32_t i = 0; i < name_cards_json.size(); i++)
                        relate_ids.insert(name_cards_json[i][nim::kNIMNameCardKeyAccid].asString());
                }
            }
        }

        if (!relate_ids.empty()) {
            std::list<std::string> uids(relate_ids.cbegin(), relate_ids.cend());
            std::list<nim::UserNameCard> uinfos;
            UserService::GetInstance()->GetUserInfos(uids, uinfos);
        }
    }
    if (msg_->type_ == nim::kNIMSessionTypeP2P) {
        if (SubscribeEventManager::GetInstance()->IsEnabled()) {
            EventDataEx data;
            SubscribeEventManager::GetInstance()->GetEventData(nim::kNIMEventTypeOnlineState, msg->id_, data);
            if (data.online_client_.online_client_type_.size() == 0 && msg->id_ == LoginManager::GetInstance()->GetAccount())
                data.online_client_.online_client_type_.insert(nim::kNIMClientTypePCWindows);
            SetOnlineState(data);
        }
        SetMute(nim_comp::MuteBlackService::GetInstance()->IsInMuteList(msg_->id_));
    } else {
        HideOnlineState();
        SetMute(nim_comp::SessionManager::GetInstance()->IsTeamMsgMuteShown(msg_->id_, -1));
    }
}

void SessionItem::SetOnlineSessionType(bool is_online_session) {
    is_online_session_ = is_online_session;
}

void SessionItem::SetMute(bool mute) {
    FindSubControl(L"not_disturb")->SetVisible(mute);
}

void SessionItem::InitUserProfile() {
    if (msg_->type_ == nim::kNIMSessionTypeP2P) {
        if (LoginManager::GetInstance()->IsEqual(msg_->id_)) {
            label_name_->SetText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MY_MOBILEPHONE"));
        } else {
            label_name_->SetText(UserService::GetInstance()->GetUserName(msg_->id_));
        }
        head_image_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(msg_->id_));
    } else {
        std::wstring tname = TeamService::GetInstance()->GetTeamName(msg_->id_);
        label_name_->SetText(tname);
        head_image_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(msg_->id_, false));
    }
}

void SessionItem::SetOnlineState(const EventDataEx& data) {
    if (label_online_state_ == nullptr)
        return;
    label_online_state_->SetText(OnlineStateEventUtil::GetOnlineState(msg_->id_, data, true));
    if (!label_online_state_->IsVisible())
        label_online_state_->SetVisible(true);
}
void SessionItem::HideOnlineState() {
    if (label_online_state_ == nullptr)
        return;
    label_online_state_->SetVisible(false);
}
void SessionItem::UpdateMsgContent(const std::string& id /*= ""*/) {
    if (!id.empty() && relate_ids.find(id) == relate_ids.cend())
        return;

    std::wstring show_text;
    if (msg_->msg_status_ != nim::kNIMMsgLogStatusDeleted) {
        SessionItemHelper::GetMsgContent(*msg_, show_text);

        bool need_prefix = true;
        if (msg_->msg_type_ == nim::kNIMMessageTypeText) {
            Json::Value values;
            Json::Reader reader;
            if (reader.parse(msg_->msg_attach_, values) && values.isObject() && values.isMember("comment") &&
                values["comment"].asString() == "is_recall_notification") {
                if (values.isMember("notify_from")) {
                    need_prefix = false;
                    std::string from_id = values["notify_from"].asString();
                    std::string from_nick = values["from_nick"].asString();
                    std::string operator_id = values["operator_id"].asString();
                    if (operator_id.empty())
                        operator_id = from_id;
                    show_text = GetRecallNotifyTextEx(msg_->id_, msg_->type_, from_id, operator_id, from_nick);
                }
            }
        }

        if (need_prefix && msg_->type_ == nim::kNIMSessionTypeTeam) {
            if (msg_->msg_type_ == nim::kNIMMessageTypeNotification && !IsNetCallMsg((nim::NIMMessageType)msg_->msg_type_, msg_->msg_attach_))
                ;  // do nothing
            else {
                switch (msg_->msg_type_) {
                    case nim::kNIMMessageTypeText:
                    case nim::kNIMMessageTypeImage:
                    case nim::kNIMMessageTypeAudio:
                    case nim::kNIMMessageTypeVideo:
                    case nim::kNIMMessageTypeLocation:
                    case nim::kNIMMessageTypeNotification:
                    case nim::kNIMMessageTypeFile:
                    case nim::kNIMMessageTypeTips:
                    case nim::kNIMMessageTypeCustom:
                    case nim::kNIMMessageTypeUnknown: {
                        std::wstring nick_name = UserService::GetInstance()->GetUserName(msg_->msg_sender_accid_);
                        if (!nick_name.empty()) {
                            show_text = nick_name + L": " + show_text;
                        }
                    } break;
                    default:
                        break;
                }
            }
        }

        if (msg_->msg_status_ == nim::kNIMMsgLogStatusSendFailed) {
            show_text = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_FAILED") + show_text;
        }
    }
    label_msg_->SetText(show_text);
}

void SessionItem::ClearMsg() {
    label_msg_->SetText(L"");
    label_time_->SetVisible(false);
    ResetUnread();
}

long long SessionItem::GetMsgTime() {
    return msg_->msg_timetag_;
}

int SessionItem::GetUnread() {
    return msg_->unread_count_;
}

void SessionItem::SetUnread(int unread) {
    if (is_online_session_)
        msg_->unread_count_ = unread;
    UpdateUnread();
}

void SessionItem::AddUnread() {
    if (is_online_session_)
        msg_->unread_count_ += 1;
    UpdateUnread();
    ShowAtmeTip(true);
}

bool SessionItem::ResetUnread() {
    if (is_online_session_ && msg_->unread_count_ != 0)
        msg_->unread_count_ = 0;
    UpdateUnread();
    ShowAtmeTip(false);
    InvokeResetUnread(msg_->id_, msg_->type_);
    Invalidate();
    return true;
}

void SessionItem::DeleteRecentSessionCb(nim::NIMResCode code, const nim::SessionData& result, int total_unread_counts) {
    QLOG_APP(L"delete recent session, code={0} command={1} total_un_cn={2}") << code << result.command_ << total_unread_counts;
}

void SessionItem::BatchStatusDeleteCb(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type) {
    QLOG_APP(L"batch delete msg, id={0} type={1} code={2}") << uid << to_type << res_code;
    Post2UI(ToWeakCallback([=]() {
        auto session_box = SessionManager::GetInstance()->FindSessionBox(msg_->id_);
        if (session_box)
            session_box->RemoveAllMsgs();
    }));
}

void SessionItem::ShowAtmeTip(bool show) {
    if (show) {
        label_atme_->SetVisible((ForcePushManager::GetInstance()->IsContainAtMeMsg(msg_->id_)));
    } else {
        label_atme_->SetVisible(false);
    }
}

void SessionItem::UpdateUnread() {
    if (msg_->unread_count_ > 0) {
        if (msg_->unread_count_ < 100) {
            label_unread_->SetText(nbase::StringPrintf(L"%d", msg_->unread_count_));
        } else {
            label_unread_->SetText(L"99+");
        }
        box_unread_->SetVisible(true);
    } else {
        box_unread_->SetVisible(false);
        label_unread_->SetText(L"0");
    }

    if (msg_->unread_count_ == 0)
        // 重置对应会话中的@me消息为已读
        ForcePushManager::GetInstance()->ResetAtMeMsg(msg_->id_);

    // 通知会话窗口中的会话合并项
    SessionBox* session_box = SessionManager::GetInstance()->FindSessionBox(msg_->id_);
    if (session_box)
        session_box->GetSessionForm()->InvokeSetSessionUnread(msg_->id_, msg_->unread_count_);
}

void SessionItem::PopupSessionItemMenu(POINT point) {
    if (is_online_session_)  //因为涉及到位置切换，所以取消右键
        return;
    CMenuWnd* pMenu = new CMenuWnd(NULL);
    STRINGorID xml(L"session_item_menu.xml");
    pMenu->Init(xml, _T("xml"), point);

    CMenuElementUI* del_session_item = (CMenuElementUI*)pMenu->FindControl(L"del_session_item");
    del_session_item->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));
    if (is_online_session_) {
        auto del_session_label = dynamic_cast<Label*>(del_session_item->FindSubControl(L"del_session_item_text"));
        if (del_session_label)
            del_session_label->SetTextId(L"STRID_SESSION_ITEM_DELETE_ONLINE_SESSION");
    }

    CMenuElementUI* top_item = (ui::CMenuElementUI*)pMenu->FindControl(L"top_item");
    top_item->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

    CMenuElementUI* cancel_top_item = (ui::CMenuElementUI*)pMenu->FindControl(L"cancel_top_item");
    cancel_top_item->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));
#if 1
    if (GetSessionData()->placed_on_top_) {
#else
    if (GetSessionData()->stick_top_info_.top_) {
#endif
        top_item->SetVisible(false);
    } else {
        cancel_top_item->SetVisible(false);
    }

    CMenuElementUI* del_session_msg = (CMenuElementUI*)pMenu->FindControl(L"del_session_msg");
    del_session_msg->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

    ui::ListContainerElement* del_session_msg_online = (ui::ListContainerElement*)pMenu->FindControl(L"del_session_msg_online");
    if (del_session_msg_online != nullptr)
        if (msg_->type_ == nim::kNIMSessionTypeP2P) {
            del_session_msg_online->SetVisible(true);
            del_session_msg_online->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));
        } else {
            del_session_msg_online->SetVisible(false);
        }

    pMenu->Show();
}

bool SessionItem::DelSessionItemMenuItemClick(ui::EventArgs* param) {
    std::wstring name = param->pSender->GetName();
    if (name == L"del_session_item") {
        bool has_transfer_task = TransferFileManager::GetInstance()->HasTransferTask(msg_->id_);

        auto closure = [this, has_transfer_task](MsgBoxRet ret) {
            if (ret == MB_YES) {
                if (is_online_session_) {
                    nim::SessionOnLineServiceHelper::DeleteSessionParam delete_param;
                    delete_param.AddSession(msg_->type_, msg_->id_);
                    nim::SessionOnLineService::DeleteSession(
                        delete_param, [](nim::NIMResCode res_code) { QLOG_APP(L"delete online session, code={0}") << res_code; });
                } else {
                    nim::Session::DeleteRecentSession(
                        msg_->type_, msg_->id_,
                        nbase::Bind(&SessionItem::DeleteRecentSessionCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                }
                SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(*msg_);
                m_pWindow->SendNotify(this, ui::kEventNotify, SET_DELETE, 0);

                if (has_transfer_task) {
                    TransferFileManager::GetInstance()->RemoveAllTaskBySessionBoxId(msg_->id_);
                }
            }
        };

        if (has_transfer_task) {
            ShowMsgBox(nullptr, closure, L"STRID_SESSION_HAS_TRANSFER_FILE_TASK", true, L"STRING_MULTIVIDEOCHATFORM_TITLE_PROMPT", true, L"STRING_OK",
                       true, L"STRING_NO", true);
        } else {
            closure(MB_YES);
        }
    } else if (name == L"del_session_msg") {
        nim::Session::SetUnreadCountZeroAsync(
            msg_->type_, msg_->id_, ToWeakCallback([this](nim::NIMResCode res_code, const nim::SessionData&, int) {
                if (res_code == nim::kNIMResSuccess) {
                    nim::MsgLog::BatchStatusDeleteAsyncEx(
                        msg_->id_, msg_->type_, (atoi(GetConfigValue("kNIMMsglogRevert").c_str()) != 0),
                        nbase::Bind(&SessionItem::BatchStatusDeleteCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                }
            }));
    } else if (name == L"del_session_msg_online") {
        auto box = dynamic_cast<ui::ListContainerElement*>(param->pSender);
        bool delete_roaming = dynamic_cast<ui::CheckBox*>(box->FindSubControl(L"chkbox_delete_roaming"))->IsSelected();
        auto task = [this, delete_roaming]() {
            nim::MsgLog::DeleteHistoryOnlineAsync(
                msg_->id_, delete_roaming, "", ToWeakCallback([](nim::NIMResCode res_code, const std::string& session_id) {
                    ShowMsgBox(nullptr, nullptr,
                               (res_code == nim::NIMResCode::kNIMResSuccess ? L"STRID_SESSION_ITEM_DELETE_ONLINE_MSGLOG_SUCCESS"
                                                                            : L"STRID_SESSION_ITEM_DELETE_ONLINE_MSGLOG_FAILED"),
                               true, L"STRING_MULTIVIDEOCHATFORM_TITLE_PROMPT", true, L"STRING_OK", true);
                }));
        };
        Post2UI(ToWeakCallback(task));
        box->GetWindow()->Close();
    } else if (name == L"top_item") {
#if 1
        nim::Session::SetSessionTop(GetSessionData()->type_, GetSessionData()->id_, true,
                                    [this](nim::NIMResCode rescode, const nim::SessionData& session, int) {
                                        if (rescode == nim::kNIMResSuccess) {
                                            GetSessionData()->placed_on_top_ = true;
                                            m_pWindow->SendNotify(this, ui::kEventNotify, SET_TO_TOP, 0);
                                        }
                                    });
#else
        nim::Session::SetToStickTopSession(GetSessionData()->id_, GetSessionData()->type_, "",
                                           [this](nim::NIMResCode rescode, const nim::StickTopSession& top_session) {
                                               if (rescode == nim::kNIMResSuccess) {
                                                   GetSessionData()->stick_top_info_.top_ = true;
                                                   m_pWindow->SendNotify(this, ui::kEventNotify, SET_TO_TOP, 0);
                                               }
                                           });
#endif
    } else if (name == L"cancel_top_item") {
#if 1
        nim::Session::SetSessionTop(GetSessionData()->type_, GetSessionData()->id_, false,
                                    [this](nim::NIMResCode rescode, const nim::SessionData& session, int) {
                                        if (rescode == nim::kNIMResSuccess) {
                                            GetSessionData()->placed_on_top_ = false;
                                            m_pWindow->SendNotify(this, ui::kEventNotify, SET_CANCEL_FROM_TOP, 0);
                                        }
                                    });
#else
        nim::Session::CancelToStickTopSession(GetSessionData()->id_, GetSessionData()->type_,
                                              [this](nim::NIMResCode rescode, const std::string& session_id, nim::NIMSessionType session_type) {
                                                  if (rescode == nim::kNIMResSuccess) {
                                                      GetSessionData()->stick_top_info_.top_ = false;
                                                      m_pWindow->SendNotify(this, ui::kEventNotify, SET_CANCEL_FROM_TOP, 0);
                                                  }
                                              });
#endif
        return true;
    }
}

bool SessionItem::OnDbClicked(ui::EventArgs* arg) {
    if (msg_->type_ == nim::kNIMSessionTypeTeam || msg_->type_ == nim::kNIMSessionTypeP2P) {
        SessionManager::GetInstance()->OpenSessionBox(msg_->id_, msg_->type_);
    } else {
        shared::Toast::ShowToast(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_ITEM_NON_TEAM_P2P_NOT_SUPPORTED_TIP"), 1000);
    }
    return true;
}

bool SessionItem::OnSessionItemMenu(ui::EventArgs* arg) {
    POINT point;
    ::GetCursorPos(&point);
    PopupSessionItemMenu(point);
    return true;
}

bool SessionItem::OnHeadImageClicked(bool is_robot, ui::EventArgs* arg) {
    if (msg_->type_ == nim::kNIMSessionTypeTeam) {
        auto team_info = nim::Team::QueryTeamInfoBlock(msg_->id_);
        TeamInfoForm::ShowTeamInfoForm(false, team_info.GetType(), msg_->id_, team_info);
    } else if (msg_->type_ == nim::kNIMSessionTypeP2P) {
        ProfileForm::ShowProfileForm(msg_->id_, is_robot);
    }
    return true;
}

void InvokeResetUnread(const std::string& id, nim::NIMSessionType type) {
    nim::Session::SetUnreadCountZeroAsync(type, id, nim::Session::SetUnreadCountZeroCallback());
    nim::MsgLog::BatchStatusReadAsync(id, type, nim::MsgLog::BatchStatusReadCallback());
}
}  // namespace nim_comp