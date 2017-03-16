#include "http_callback.h"
#include "module/session/session_manager.h"
#include "gui/session/msg_record.h"

namespace nim_comp
{
void HttpCallback::OnHttpDownloadCallback(nim::NIMResCode res_code, const std::string& file_path, const std::string& sid, const std::string& cid)
{
	SessionBox* session_box = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(sid));

	if (session_box)
	{
		session_box->OnDownloadCallback(cid, res_code == nim::kNIMResSuccess, file_path);
	}

	MsgRecordForm* msg_record = dynamic_cast<MsgRecordForm*>(WindowsManager::GetInstance()->GetWindow(MsgRecordForm::kClassName, MsgRecordForm::kClassName));
	if (msg_record)
	{
		msg_record->OnDownloadCallback(res_code, file_path, sid, cid);
	}
}

void HttpCallback::OnHttpUploadCallback(nim::NIMResCode res_code, const nim::UploadMediaResult& result)
{
	SessionBox* session_box = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(result.call_id_));

	if (session_box)
	{
		session_box->OnUploadCallback(result.res_id_, res_code == nim::kNIMResSuccess, result.url_);
	}
}

}