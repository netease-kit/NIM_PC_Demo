#pragma once

#include "nim_ui_dll.h"
#include "module/session/session_manager.h"
#include "module/service/team_service.h"

namespace nim_ui
{

/** @class SessionManager
  * @brief 提供会话窗体控制接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API SessionManager
{
public:
	SINGLETON_DEFINE(SessionManager);
public:
	SessionManager();
	~SessionManager();
public:
	nim_comp::SessionForm* OpenSessionForm(std::string id, nim::NIMSessionType type, bool reopen = false);
	bool IsSessionWndActive(const std::string& id);
	nim_comp::SessionForm* Find(const std::string &id);

public:
	void AddNewMsg(const nim::IMMessage &msg);
	void ResetUnread(const std::string &id);
	
	void RemoveForm(std::string id);

	void AddFileUpProgressCb(std::string msg_id, nim::Talk::FileUpPrgCallback* cb);
	void RemoveFileUpProgressCb(std::string msg_id);
	void QueryMyTList(const std::string& tid);

};

}