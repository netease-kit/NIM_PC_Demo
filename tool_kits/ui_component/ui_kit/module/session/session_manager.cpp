#include "session_manager.h"
#include "export/nim_ui_session_list_manager.h"
#include "module/service/mute_black_service.h"
#include "gui/session/session_form.h"
#include "gui/session/session_box.h"

namespace nim_comp
{
SessionManager::SessionManager()
{
	enable_merge_ = true;
	use_custom_drag_image_ = true;
	ring_.Init(NULL);
}

SessionManager::~SessionManager()
{

}

SessionBox* SessionManager::OpenSessionBox(std::string session_id, nim::NIMSessionType type, bool reopen)
{
	if (session_id.empty())
	{
		ASSERT(0);
		return NULL;
	}

	auto it_box = session_box_map_.find(session_id);
	if (it_box != session_box_map_.end())
	{
		SessionForm *parent_form = it_box->second->GetSessionForm();
		if (!reopen)
		{
			parent_form->SetActiveSessionBox(session_id);
			return it_box->second;
		}
		else
		{
			parent_form->CloseSessionBox(session_id);
			RemoveSessionBox(session_id);
		}
	}

	SessionBox *session_box = CreateSessionBox(session_id, type);
	if (NULL == session_box)
		return NULL;

	session_box->InvokeShowMsgs(true);
	return session_box;
}

void SessionManager::AddNewMsg(const nim::IMMessage &msg)
{
	std::string id = GetSessionId(msg);
	SessionBox *session_box = FindSessionBox(id);

	if (session_box && !session_box->IsTeamValid())
	{
		SessionForm *parent_form = session_box->GetSessionForm();
		parent_form->CloseSessionBox(id);
		RemoveSessionBox(id);
		session_box = NULL;
	}

	bool create = false;
	bool msg_notify = true;
	if (msg.session_type_ == nim::kNIMSessionTypeTeam)
	{
		if (!IsTeamMsgNotify(id))
			msg_notify = false;
	}
	else
		msg_notify = !MuteBlackService::GetInstance()->IsInMuteList(id);

	if (NULL == session_box)
	{
		if (msg_notify)
		{
			create = true;
			// 如果使用了合并会话功能并且已经有了会话窗体，则取消create标志
			if (enable_merge_ && !session_box_map_.empty())
				create = false;

			session_box = CreateSessionBox(id, msg.session_type_);
			if (NULL == session_box)
				return;
		}
	}

	// 如果目标会话盒子不在激活状态或者第一次创建会话盒子
	if (!IsSessionBoxActive(id) || create)
	{
		// 更新对应会话中的@me消息为未读
		if (IsAtMeMsg(msg))
			seesion_id_atme_[id] = true;
	}

	if (session_box)
	{
		session_box->AddNewMsg(msg, create);

		if (msg_notify)
			ring_.Play(RING_NEW_MESSAGE);
	}
}

bool SessionManager::IsSessionBoxActive(const std::string& id)
{
	SessionBox *session_box = FindSessionBox(id);
	if (NULL != session_box)
	{
		SessionForm *parent_form = session_box->GetSessionForm();
		return parent_form->IsActiveSessionBox(session_box);
	}

	return false;
}

SessionBox* SessionManager::FindSessionBox( const std::string &id )
{
	std::map<std::string, SessionBox*>::const_iterator i = session_box_map_.find(id);
	if (i == session_box_map_.end())
		return NULL;
	else
		return i->second;
}

void SessionManager::RemoveSessionBox( std::string id, const SessionBox* box /*=NULL*/)
{
	auto it_box = session_box_map_.find(id);
	if (it_box == session_box_map_.end())
	{
		assert(0);
	}
	else
	{
		if (NULL == box || box == it_box->second)
		{
			session_box_map_.erase(it_box);
		}
		else
		{
			assert(0);
		}
	}
}

nim_comp::SessionBox* SessionManager::CreateSessionBox(const std::string &session_id, nim::NIMSessionType type)
{
	SessionForm *session_form = NULL;
	SessionBox *session_box = NULL;
	// 如果启用了窗口合并功能，就把新会话盒子都集中创建到某一个会话窗体里
	// 否则每个会话盒子都创建一个会话窗体
	if (enable_merge_)
	{
		if (session_box_map_.size() >= 1)
		{
			session_form = session_box_map_.begin()->second->GetSessionForm();
		}
		else
		{
			session_form = new SessionForm;
			HWND hwnd = session_form->Create(NULL, L"会话窗口", UI_WNDSTYLE_FRAME, 0);
			if (hwnd == NULL)
			{
				session_form = NULL;
				return NULL;
			}

			session_form->CenterWindow();
		}

		session_box = session_form->CreateSessionBox(session_id, type);
		if (NULL == session_box)
			return NULL;
	}
	else
	{
		session_form = new SessionForm;
		HWND hwnd = session_form->Create(NULL, L"会话窗口", UI_WNDSTYLE_FRAME, 0);
		if (hwnd == NULL)
			return NULL;

		session_box = session_form->CreateSessionBox(session_id, type);
		if (NULL == session_box)
			return NULL;

		session_form->CenterWindow();
	}

	session_box_map_[session_id] = session_box;
	return session_box;
}

bool SessionManager::IsContainAtMeMsg(const std::string &id)
{
	auto i = seesion_id_atme_.find(id);
	if (i != seesion_id_atme_.end())
		return i->second;
	
	return false;
}

void SessionManager::ResetUnread(const std::string &id)
{
	// 重置对应会话中的@me消息为已读
	seesion_id_atme_[id] = false;

	// 重置会话列表未读消息数
	nim_ui::SessionListManager::GetInstance()->InvokeResetSessionUnread(id);
}

void SessionManager::QueryMyTeamInfo(const std::string& tid)
{
	nim::Team::QueryTeamMemberAsync(tid, LoginManager::GetInstance()->GetAccount(), nbase::Bind(&SessionManager::OnQueryMyTeamInfo, this, tid, std::placeholders::_1));
}

void SessionManager::OnQueryMyTeamInfo(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
{
	team_list_bits_[tid] = team_member_info.GetBits();
}

bool SessionManager::IsTeamMsgNotify(const std::string& tid)
{
	auto it = team_list_bits_.find(tid);
	if (it != team_list_bits_.end())
	{
		return (it->second & nim::kNIMTeamBitsConfigMaskMuteNotify) == 0;
	}
	return true;
}

}