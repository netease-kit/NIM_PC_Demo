#include "session_manager.h"
#include "export/nim_ui_session_list_manager.h"
#include "module/service/mute_black_service.h"

namespace nim_comp
{
SessionManager::SessionManager()
{
	ring_.Init(NULL);
}

SessionManager::~SessionManager()
{

}

SessionForm* SessionManager::OpenSessionForm(std::string id, nim::NIMSessionType type, bool reopen)
{
	if(id.empty())
	{
		assert(0);
		return NULL;
	}

	std::map<std::string, SessionForm*>::const_iterator i = id_form_.find(id);
	if(i != id_form_.end())
	{
		if (!reopen) {
			i->second->ActiveWindow();
			return i->second;
		}
		else {
			i->second->Close();
			RemoveForm(id);
		}
	}

	SessionForm* sess = new SessionForm(id, type);
	HWND hwnd = sess->Create(NULL, L"会话窗口", UI_WNDSTYLE_FRAME, 0);
	if(hwnd == NULL)
		return NULL;
	sess->CenterWindow();

	id_form_[id] = sess;

	sess->InvokeShowMsgs(true);
	return sess;
}

bool SessionManager::IsSessionWndActive(const std::string& id)
{
	SessionForm* sess = Find(id);
	if (sess)
	{
		HWND hwnd = sess->GetHWND();
		if (::GetForegroundWindow() == hwnd)
		{
			return true;
		}
	}

	return false;
}

void SessionManager::AddNewMsg(const nim::IMMessage &msg)
{
	bool create = false;

	std::string id = GetSessionId(msg);
	SessionForm* sess = Find(id);
	if (sess && !sess->IsValid())
	{
		sess->Close();
		RemoveForm(id);
		sess = NULL;
	}
	bool msg_notify = true;
	if (msg.session_type_ == nim::kNIMSessionTypeTeam)
	{
		if (!IsTeamMsgNotify(id))
		{
			msg_notify = false;
		}
	}
	else
		msg_notify = !MuteBlackService::GetInstance()->IsInMuteList(id);

	if (sess == NULL)
	{
		if (msg_notify)
		{
			sess = new SessionForm(id, msg.session_type_);
			HWND hwnd = sess->Create(NULL, L"会话窗口", WS_OVERLAPPEDWINDOW, 0);
			if (hwnd == NULL)
				return;
			sess->CenterWindow();

			id_form_[id] = sess;

			create = true;
		}
	}

	if (!IsSessionWndActive(id))
	{
		nim_ui::SessionListManager::GetInstance()->AddUnreadCount(id);
	}
	if (sess)
	{
		sess->AddNewMsg(msg, create);
		if (msg_notify)
		{
			ring_.Play(RING_NEW_MESSAGE);
		}
	}
}

SessionForm* SessionManager::Find( const std::string &id )
{
	std::map<std::string, SessionForm*>::const_iterator i = id_form_.find(id);
	if(i == id_form_.end())
		return NULL;
	else
		return i->second;
}

void SessionManager::RemoveForm( std::string id, const SessionForm* form /*=NULL*/)
{
	std::map<std::string, SessionForm*>::const_iterator i = id_form_.find(id);
	if(i == id_form_.end())
	{
		assert(0);
	}
	else
	{
		if (form == NULL || form == i->second)
		{
			id_form_.erase(i);
		}
	}
}

void SessionManager::ResetUnread( const std::string &id )
{
	nim_ui::SessionListManager::GetInstance()->ResetSessionUnread(id);
}
void SessionManager::AddFileUpProgressCb(std::string msg_id, nim::Talk::FileUpPrgCallback* cb)
{
	id_up_prg_cb_[msg_id] = cb;
}
void SessionManager::RemoveFileUpProgressCb(std::string msg_id)
{
	auto it = id_up_prg_cb_.find(msg_id);
	if (it != id_up_prg_cb_.end())
	{
		delete it->second;
		it->second = nullptr;
		id_up_prg_cb_.erase(it);
	}
}
void SessionManager::QueryMyTList(const std::string& tid)
{
	nim::Team::QueryTeamMemberAsync(tid, LoginManager::GetInstance()->GetAccount(), nbase::Bind(&SessionManager::OnQueryMyTList, this, tid, std::placeholders::_1));
}

void SessionManager::OnQueryMyTList(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
{
	tlist_bits_[tid] = team_member_info.GetBits();
}

bool SessionManager::IsTeamMsgNotify(const std::string& tid)
{
	auto it = tlist_bits_.find(tid);
	if (it != tlist_bits_.end())
	{
		return (it->second & nim::kNIMTeamBitsConfigMaskMuteNotify) == 0;
	}
	return true;
}
}