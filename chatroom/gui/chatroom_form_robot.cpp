#include "chatroom_form.h"
#include "chatroom_frontpage.h"
#include "src/cpp_sdk/nim_chatroom/helper/nim_chatroom_helper.h"
#include "module/session/session_util.h"

using namespace nim_comp;

#define ROOMMSG_R_N _T("\r\n")
namespace nim_chatroom
{
class ScoreCompare
{
public:
	bool operator()(const Json::Value& first, const Json::Value& last)
	{
		int first_score = first["score"].asUInt();
		int last_score = last["score"].asUInt();
		return first_score < last_score;
	}
};

static void SubAddSender(ui::RichEdit* edit, const std::string &robot_id, const std::string &robot_name, bool is_history, bool first_msg_each_batch = false)
{
	long old_begin = 0, old_end = 0;
	if (edit->GetTextLength() != 0)
	{
		if (is_history)
		{
			if (first_msg_each_batch)
				edit->SetSel(0, 0);
			else
				edit->GetSel(old_begin, old_end);
			edit->ReplaceSel(ROOMMSG_R_N, false);
		}
		else
		{
			edit->SetSel(-1, -1);
			edit->ReplaceSel(ROOMMSG_R_N, false);
		}
	}

	std::wstring w_show_name = nbase::UTF8ToUTF16(robot_name);
	//
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	edit->GetDefaultCharFormat(cf); //获取消息字体
	cf.dwMask = CFM_LINK | CFM_FACE | CFM_COLOR;
	cf.dwEffects = CFE_LINK;

	// 添加发言人，并设置他的颜色
	lSelEnd = lSelBegin = is_history ? old_end + 1 : edit->GetTextLength();
	edit->SetSel(lSelEnd, lSelEnd);
	edit->ReplaceSel(w_show_name, false);

	long lNewSelBegin = 0, lNewSelEnd = 0;
	edit->GetSel(lNewSelBegin, lNewSelEnd);
	edit->ReplaceSel(L" ", false);

	edit->SetSel(lSelBegin, lNewSelEnd);
	edit->SetSelectionCharFormat(cf);

	// 设置文本的缩进
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT | PFM_LINESPACING;
	LONG lineSpace = (LONG)(1.2 * 20);//设置1.2行间距
	pf.dxStartIndent = 0;
	pf.bLineSpacingRule = 5;
	pf.dyLineSpacing = lineSpace;
	edit->SetParaFormat(pf);

	edit->SetSel(lNewSelEnd + 1, lNewSelEnd + 1);
}

static void AddSubText(ui::RichEdit* edit, const std::wstring &text, bool end_down)
{
	if (text.empty()) return;

	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;

	// 添加正文，并设置他的颜色	
	edit->ReplaceSel(ROOMMSG_R_N, false);
	edit->GetSel(lSelBegin, lSelEnd);
	edit->ReplaceSel(text, false);
	//设置文本字体
	edit->GetDefaultCharFormat(cf); //获取消息字体
	long lSelBegin2 = 0, lSelEnd2 = 0;
	edit->GetSel(lSelBegin2, lSelEnd2);
	edit->SetSel(lSelEnd, lSelEnd2);
	edit->SetSelectionCharFormat(cf);

	// 设置文本的缩进
	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT | PFM_LINESPACING;
	LONG lineSpace = (LONG)(1.2 * 20);//设置1.2行间距
	pf.dxStartIndent = 150;
	pf.bLineSpacingRule = 5;
	pf.dyLineSpacing = lineSpace;
	edit->SetSel(lSelEnd2, lSelEnd2);
	edit->SetParaFormat(pf);

	if (end_down)
		edit->EndDown();
}

void ChatroomForm::OnDownloadCallback(bool success, const std::string& file_path, bool is_complex_element, bool is_history)
{
	if (is_complex_element && complex_json_.size() > 0)
	{
		Json::Value new_complex_json;
		int len = complex_json_.size();
		for (int i = 0; i < len; i++)
		{
			bool all_ready = true;
			int link_len = complex_json_[i].size();
			Json::Value &link_elements = complex_json_[i]["element"]["content"];
			for (int j = 0; j < link_len; j++)
			{
				all_ready &= PhotoService::GetInstance()->CheckPhotoOK(nbase::UTF8ToUTF16(file_path));
			}
			if (all_ready)
			{
			}
			else
			{
				new_complex_json.append(complex_json_[i]);
			}
		}
		complex_json_ = new_complex_json;
	}
	else
	{
	}
}

void ChatroomForm::DownloadImage(const std::string &url, const std::wstring &photo_path, bool is_complex_element, bool is_history)
{
	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, photo_path, is_complex_element, is_history](int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);
			}
			std::string s_photo_path = nbase::UTF16ToUTF8(photo_path);
			OnDownloadCallback(true, s_photo_path, is_complex_element, is_history);
		}
	});
	nim::NOS::DownloadResource(url, cb);
}

LRESULT ChatroomForm::HandleDiscuzMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!is_anonymity_)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ui::Control *control = FindControl(pt);
		if (control != nullptr && control == msg_list_)
		{
			if (uMsg == WM_MOUSEMOVE)
			{
			}
			else if (uMsg == WM_LBUTTONUP)
			{
			}
		}
	}
	return 0;
}

}