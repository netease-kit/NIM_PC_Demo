#include "http_callback.h"
#include "module/session/session_manager.h"

namespace nim_comp
{
void HttpCallback::OnHttpDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
{
	SessionBox* session_box = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(sid));

	if (session_box)
	{
		session_box->OnDownloadCallback(cid, code == nim::kNIMResSuccess);
	}
}
}