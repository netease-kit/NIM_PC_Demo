#pragma once
#include "gui/session/session_form.h"
#include "ring.h"

namespace nim_comp
{
/** @class SessionManager
* @brief 会话管理类，用于统一管理会话窗口和消息。收到会话消息以及其他会话事件，先经过会话管理器，然后分发到正确的会话窗口。
* @copyright (c) 2015, NetEase Inc. All rights reserved
* @author towik
* @date 2015/1/1
*/
class SessionManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(SessionManager);
public:
	SessionManager();
	~SessionManager();
	/**
	* 根据会话id，打开会话窗口
	* @param[in] id 会话id
	* @param[in] type 会话类型（P2P或群聊）
	* @param[in] reopen 如果为true，且该会话窗口当前已打开，则关闭该窗口，重新打开
	* @return SessionForm* 返回会话窗体的句柄
	*/
	SessionForm* OpenSessionForm(std::string id, nim::NIMSessionType type, bool reopen = false);
	bool IsSessionWndActive(const std::string& id);

	/**
	* 添加消息到正确的会话窗口
	* @param[in] msg 消息体
	* @return void 无返回值
	*/
	void AddNewMsg(const nim::IMMessage &msg);
	void ResetUnread(const std::string &id);

	SessionForm* Find(const std::string &id);
	void RemoveForm(std::string id, const SessionForm* form = NULL);

	void AddFileUpProgressCb(std::string msg_id, nim::Talk::FileUpPrgCallback* cb);
	void RemoveFileUpProgressCb(std::string msg_id);
	void QueryMyTList(const std::string& tid);

private:
	void OnQueryMyTList(const std::string& tid, const nim::TeamMemberProperty& team_member_info);
	bool IsTeamMsgNotify(const std::string& tid);

private:
	std::map<std::string, SessionForm*>	id_form_;
	std::map<std::string, nim::Talk::FileUpPrgCallback*>	id_up_prg_cb_;
	std::map<std::string, uint64_t> tlist_bits_;

	Ring ring_;
};
}