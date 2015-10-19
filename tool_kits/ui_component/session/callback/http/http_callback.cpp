#include "http_callback.h"
#include "gui/session/session_form.h"

namespace nim_comp
{
void HttpCallback::OnHttpDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
{
	std::wstring wsid = nbase::UTF8ToUTF16(sid);
	SessionForm* session_form = dynamic_cast<SessionForm*>( WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, wsid) );
	if(session_form)
	{
		session_form->OnDownloadCallback(cid, code == nim::kNIMResSuccess);
	}
}
}