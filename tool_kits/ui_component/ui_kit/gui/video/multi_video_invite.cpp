#include "multi_video_invite.h"
#include "multi_video_form.h"
#include "module/video/video_manager.h"
#include "duilib/Utils/MultiLangSupport.h"
#include "module/session/session_manager.h"
namespace nim_comp
{
	const LPCTSTR MultiVideoInviteForm::kClassName = L"MultiVideoInviteForm";

	MultiVideoInviteForm::MultiVideoInviteForm(MultiVchatInfo vchat_info)
	{
		vchat_info_ = vchat_info;
	}

	MultiVideoInviteForm::~MultiVideoInviteForm()
	{
	}

	std::wstring MultiVideoInviteForm::GetSkinFolder()
	{
		return L"video";
	}

	std::wstring MultiVideoInviteForm::GetSkinFile()
	{
		return L"multi_video_invite.xml";
	}


	LRESULT MultiVideoInviteForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
		{
			this->Close();
			return 0;
		}
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void MultiVideoInviteForm::InitWindow()
	{
		SetTaskbarTitle(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOINVITEFORM_TASK_TITLE"));
		m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MultiVideoInviteForm::Notify, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MultiVideoInviteForm::OnClicked, this, std::placeholders::_1));
		headicon_btn_ = (ui::Button*)FindControl(L"headicon");
		lb_multi_vchat_member_info_ = (ui::Label*)FindControl(L"lb_multi_vchat_member_info");
		lb_multi_vchat_inviter_ = (ui::Label*)FindControl(L"lb_multi_vchat_inviter");
		lb_multi_vchat_member_count_ = (ui::Label*)FindControl(L"lb_multi_vchat_member_count");
		btn_jump_session_ = dynamic_cast<ui::Button*>(FindControl(L"jump_session"));
		InitControl();

		//邀请倒计时，如果MULTI_VCHAT_INVITE_TIMEOUT内无操作则自动关闭该页面
		StdClosure time_out_task = nbase::Bind(&MultiVideoInviteForm::InviteTimeOut, this);
		nbase::ThreadManager::PostDelayedTask(kThreadUI, invite_game_timer_.ToWeakCallback(time_out_task), nbase::TimeDelta::FromSeconds(MULTI_VCHAT_INVITE_TIMEOUT));
		
		//响铃
		voip_ring_.Init(this->GetHWND());
		voip_ring_.Play(RING_VOIP_RING, true);
	}

	void MultiVideoInviteForm::OnFinalMessage(HWND hWnd)
	{
		invite_game_timer_.Cancel();
		voip_ring_.Stop();
		__super::OnFinalMessage(hWnd);
	}

	bool MultiVideoInviteForm::Notify(ui::EventArgs* msg)
	{
		return true;
	}

	bool MultiVideoInviteForm::OnClicked(ui::EventArgs* arg)
	{
		std::wstring name = arg->pSender->GetName();
		if (name == L"accept")
		{
			invite_game_timer_.Cancel();
			SendMultiVchatNotify(true);
			VideoManager::GetInstance()->StartJoinTimer();
		
		}
		else if (name == L"refuse")
		{
			invite_game_timer_.Cancel();
			SendMultiVchatNotify(false);
		}
		else if (name == L"jump_session")
		{
			JumpSession();
		}
		return true;
	}

	void MultiVideoInviteForm::InitControl()
	{
		std::wstring inviter_info = L"";
		std::string vchat_member_info = "";
		std::string utf8_inviter_info = "";
		auto user_info_change_cb = nbase::Bind(&MultiVideoInviteForm::OnUserInfoChange, this, std::placeholders::_1);
		unregister_cb_.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));

		nim::UserNameCard info;
		if (!UserService::GetInstance()->GetUserInfo(vchat_info_.creator_id_, info))
		{
			QLOG_APP(L"get user info  failed uid:{0}") << vchat_info_.creator_id_;
		}
		std::wstring user_name = nbase::UTF8ToUTF16(info.GetName());
		if (user_name.length() > 6)
		{
			user_name = (user_name.substr(0, 6)).append(L"...");
		}
		inviter_info = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOCHATFORM_INVITE_PROMPT").c_str(), user_name.c_str());
		lb_multi_vchat_inviter_->SetText(inviter_info);
		//vchat_member_info = vchat_info_.team_name_;
		//std::wstring vchat_info = nbase::UTF8ToUTF16(vchat_member_info);
		//lb_multi_vchat_member_info_->SetText(vchat_info);
		//lb_multi_vchat_member_count_->SetText(nbase::StringPrintf(L"(%d人)", vchat_info_.member_count_));
		CheckHeadIcon();
	}

	void MultiVideoInviteForm::CheckHeadIcon()
	{
		std::wstring photo = PhotoService::GetInstance()->GetTeamPhoto(vchat_info_.team_id_);
		headicon_btn_->SetBkImage(photo);
	}

	void MultiVideoInviteForm::SendMultiVchatNotify(bool accept)
	{
		Json::FastWriter fw;
		int index = 0;
		UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
		Json::Value value;
		value["teamId"] = vchat_info_.team_id_;
		value["channelId"] = vchat_info_.channel_id_;
		value["opt"] = accept ? 1 : 2;
		value["id"] = kMsgMultiVchatAck;
		value["roomName"] = vchat_info_.room_name_;
		std::string json_value = fw.write(value);
		//发送自定义消息给用户

		nim::SysMessage msg;
		msg.sender_accid_ = current_user_id;
		msg.client_msg_id_ = QString::GetGUID();
		msg.attach_ = json_value;
		msg.type_ = nim::kNIMSysMsgTypeCustomP2PMsg;
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
		msg.receiver_accid_ = vchat_info_.creator_id_;
		QLOG_APP(L"invite accept:{0} send msg:{1}") << accept << msg.ToJsonString();
		nim::SystemMsg::SendCustomNotificationMsg(msg.ToJsonString());
		if (accept)
		{
			VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatJoin);
		}
		else
		{
			VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatEnd);
		}
		Close();
	}

	void MultiVideoInviteForm::JumpSession()
	{
		//打开会话页面
		SessionManager::GetInstance()->OpenSessionBox(vchat_info_.team_id_, nim::kNIMSessionTypeTeam);
	}

	void MultiVideoInviteForm::InviteTimeOut()
	{
		SendMultiVchatNotify(false);
	}

	void MultiVideoInviteForm::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
	{
		for (const auto &info : uinfos)
		{
			if (info.GetAccId() == vchat_info_.creator_id_)
			{
				if (info.ExistValue(nim::kUserNameCardKeyName))
				{
					std::wstring user_name = nbase::UTF8ToUTF16(info.GetName());
					if (user_name.length() > 6)
					{
						user_name = (user_name.substr(0, 6)).append(L"...");
					}
					std::wstring inviter_info = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOCHATFORM_INVITE_PROMPT").c_str(), user_name.c_str());
					size_t len = inviter_info.length();
					lb_multi_vchat_inviter_->SetText(inviter_info);
				}
				else
				{
					QLOG_ERR(L"get user info cb error {0}") << vchat_info_.creator_id_;
					std::wstring inviter_info = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOCHATFORM_INVITE_PROMPT").c_str(), nbase::UTF8ToUTF16(info.GetAccId()).c_str());
					lb_multi_vchat_inviter_->SetText(inviter_info);
				}
				return;
			}
		}
		SetInviteName();
	}

	void MultiVideoInviteForm::SetInviteName()
	{
		std::string invite_name = "";
		for (const auto &uid :invite_members_)
		{
			nim::UserNameCard user_name;
			UserService::GetInstance()->GetUserInfo(uid, user_name);
			if (!invite_name.empty())
			{
				invite_name.append(nbase::UTF16ToUTF8(L"、"));
			}
			invite_name.append(user_name.GetName());
		}
		lb_multi_vchat_member_info_->SetText(nbase::UTF8ToUTF16(invite_name));
		lb_multi_vchat_member_count_->SetText(nbase::StringPrintf(L"(%d人)", invite_members_.size()));
	}

	void MultiVideoInviteForm::AddInviteMembers(std::set<std::string> uids)
	{
		invite_members_ = uids;
		SetInviteName();
	}
}