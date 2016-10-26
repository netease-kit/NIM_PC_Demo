#include "session_form.h"
#include "session_box.h"
#include "module/session/session_manager.h"
#include "gui/session/control/merge_item.h"
#include "gui/session/control/custom_layout.h"

using namespace ui;

namespace
{
	// 注册这个消息，收到这个消息后表示窗口对应的任务栏按钮被系统创建，这时候初始化ITaskbarList4接口
	UINT WM_TASKBARBUTTONCREATED = ::RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

	// 定义会话盒子针对P2P、群组会话不同状态下的尺寸
	const int kNormalSessionBoxWidth = 550;
	const int kNormalSessionBoxHeight = 600;
	const int kLargeSessionBoxWidth = 740;
	const int kLargeSessionBoxHeight = 600;

	// 窗口收到WM_CLOSE消息的原因
	enum CloseReason
	{
		kDefaultClose	 = 0,	// 在任务栏右击关闭窗口，按Alt+F4等常规原因
		kSessionBoxClose = 1	// 关闭了最后一个会话盒子导致窗口关闭
	};
}

namespace nim_comp
{
const LPCTSTR SessionForm::kClassName = L"SesssionForm";

SessionForm::SessionForm()
{
	drop_helper_ = NULL;
	icon_handle_ = NULL;

	merge_list_ = NULL;
	session_box_tab_ = NULL;
	active_session_box_ = NULL;

	is_drag_state_ = false;
	old_drag_point_.x = old_drag_point_.y = 0;
}

SessionForm::~SessionForm()
{

}

std::wstring SessionForm::GetSkinFolder()
{
	return L"session";
}

std::wstring SessionForm::GetSkinFile()
{
	return L"session_form.xml";
}

std::wstring SessionForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring SessionForm::GetWindowId() const
{
	return kClassName;
}

UINT SessionForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT SessionForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CloseReason closeReason = (CloseReason)wParam;

	// 如果是因为所有会话盒子都关闭了导致窗口关闭，则正常返回
	if (kSessionBoxClose == closeReason)
	{
		return __super::OnClose(uMsg, wParam, lParam, bHandled);
	}
	// 如果是其他原因触发了WM_CLOSE
	else
	{
		int session_count = merge_list_->GetCount();
		if (session_count > 0 && NULL != active_session_box_)
		{
			// 如果只有一个会话盒子，就直接关闭
			if (1 == session_count)
			{
				CloseSessionBox(active_session_box_->GetSessionId());
			}
			// 如果包含多个会话盒子，就询问用户
			else
			{
				MsgboxCallback cb = ToWeakCallback([this](MsgBoxRet ret)
				{
					if (ret == MB_YES)
					{
						while (merge_list_->GetCount() > 0)
						{
							Control *merge_item = merge_list_->GetItemAt(0);
							ASSERT(NULL != merge_item);
							if (NULL == merge_item)
								break;

							CloseSessionBox(merge_item->GetUTF8Name());
						}
					}				
				});
				ShowMsgBox(this->GetHWND(), L"当前窗口包含多个会话，确定要关闭窗口吗？", cb);
			}

			bHandled = TRUE;
			return 0;
		}
	}

	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

LRESULT SessionForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	if (uMsg == WM_KEYDOWN)
	{
		// 处理Ctrl+Tab快捷键
		if (wParam == VK_TAB && ::GetKeyState(VK_CONTROL) < 0)
		{
			int next = merge_list_->GetCurSel();
			next = (next+1) % merge_list_->GetCount();
			merge_list_->SelectItem(next);
			return 0;
		}
		// 处理ESC快捷键
		else if (wParam == VK_ESCAPE)
		{
			BOOL bHandled = FALSE;
			if (!SessionManager::GetInstance()->IsDragingSessionBox() && NULL != active_session_box_)
				active_session_box_->OnEsc(bHandled);

			if (!bHandled)
				this->CloseSessionBox(active_session_box_->GetSessionId());

			return 0;
		}
	}
	// 超链接消息
	else if (uMsg == WM_NOTIFY)
	{
		if (CheckRichEditLink(wParam, lParam))
			return 0;
	}
	else if (uMsg == WM_TASKBARBUTTONCREATED)
	{
		taskbar_manager_.Init(this);

		// 因为窗口刚创建时，会话盒子已经创建但是那时还没有收到WM_TASKBARBUTTONCREATED消息，导致RegisterTab函数没有被调用，所以收到消息后重新遍历一下没有被注册的Tab
		for (int i = 0; i < session_box_tab_->GetCount(); i++)
		{
			Control *box_item = session_box_tab_->GetItemAt(i);
			ASSERT(NULL != box_item);
			if (NULL == box_item)
				continue;

			SessionBox* session_box = dynamic_cast<SessionBox*>(box_item);
			if (NULL == session_box)
				continue;

			TaskbarTabItem* taskbar_item = session_box->GetTaskbarItem();
			if (NULL == taskbar_item)
				continue;;
			
			if (NULL == taskbar_item->GetTaskbarManager())
				taskbar_manager_.RegisterTab(*taskbar_item);
		}

		return TRUE;
	}

	if (NULL != active_session_box_)
	{
		bool bHandle = false;
		LRESULT ret = active_session_box_->HandleMessage(uMsg, wParam, lParam, bHandle);
		if (bHandle)
			return ret;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void SessionForm::OnFinalMessage(HWND hWnd)
{
	// 使用merge_list_来判断会话盒子总数，session_box_tab_获取的总数不准确
	int session_box_count = merge_list_->GetCount();
	for (int i = 0; i < session_box_count; i++)
	{
		Control *box_item = session_box_tab_->GetItemAt(i);
		ASSERT(NULL != box_item);
		if (NULL == box_item)
			continue;;

		SessionBox* session_box = dynamic_cast<SessionBox*>(box_item);
		if (NULL != session_box)
			session_box->UninitSessionBox();
	}

	UnInitDragDrop();

	if (icon_handle_)
	{
		::DestroyIcon(icon_handle_);
		icon_handle_ = NULL;
	}

	__super::OnFinalMessage(hWnd);
}

ui::Control* SessionForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("CustomTabBox"))
	{
		return new TabBox(new CustomLayout);
	}

	return NULL;
}

void SessionForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&SessionForm::OnNotify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&SessionForm::OnClicked, this, std::placeholders::_1));

	merge_list_ = static_cast<ListBox*>(FindControl(L"merge_list"));
	session_box_tab_ = static_cast<TabBox*>(FindControl(L"session_box_tab"));

	merge_list_->AttachSelect(nbase::Bind(&SessionForm::OnMergeItemSelected, this, std::placeholders::_1));
	
	InitDragDrop();
}

bool SessionForm::OnNotify(ui::EventArgs* param)
{
	return true;
}

bool SessionForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_max_restore")
	{
		DWORD style = GetWindowLong(GetHWND(), GWL_STYLE);
		if (style & WS_MAXIMIZE)
			::ShowWindow(GetHWND(), SW_RESTORE);
		else
			::ShowWindow(GetHWND(), SW_MAXIMIZE);
	}
	else if (name == _T("btn_close"))
	{
		if (NULL == active_session_box_)
		{
			ASSERT(0);
			return true;
		}
		
		CloseSessionBox(active_session_box_->GetSessionId());
	}
	else if (name == _T("btn_min"))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}

	return true;
}

SessionBox* SessionForm::CreateSessionBox(const std::string &session_id, nim::NIMSessionType session_type)
{
	std::wstring id = nbase::UTF8ToUTF16(session_id);
	if (NULL != FindMergeItem(id))
	{
		ASSERT(0);
		return NULL;
	}
	if (NULL != FindSessionBox(id))
	{
		ASSERT(0);
		return NULL;
	}

	MergeItem *merge_item = new MergeItem;
	GlobalManager::FillBoxWithCache(merge_item, L"session/merge_item.xml");
	merge_list_->Add(merge_item);
	merge_item->AttachAllEvents(nbase::Bind(&SessionForm::OnProcessMergeItemDrag, this, std::placeholders::_1));
	merge_item->InitControl(session_id, session_type == nim::kNIMSessionTypeTeam);	
	Button *btn_item_close = (Button*)merge_item->FindSubControl(L"merge_item_close");
	btn_item_close->AttachClick(nbase::Bind(&SessionForm::OnMergeItemClose, this, std::placeholders::_1, session_id));

	SessionBox* session_box = new SessionBox(session_id, session_type);
	session_box_tab_->Add(session_box);
	GlobalManager::FillBoxWithCache(session_box, L"session/session_box.xml");
	session_box->SetName(id);
	session_box->InitSessionBox();
	taskbar_manager_.RegisterTab(*session_box->GetTaskbarItem());
	Button *btn_header_ = (Button*)session_box->FindSubControl(L"btn_header");
	btn_header_->AttachAllEvents(nbase::Bind(&SessionForm::OnProcessSessionBoxHeaderDrag, this, std::placeholders::_1));

	if (merge_list_->GetCount() <= 1)
		active_session_box_ = session_box;
	else
		merge_list_->SetVisible(true);

	// 切换到新的会话盒子
	// 如果merge_item处于隐藏状态，则无法顺利触发选择事件，所以这里直接切换到目标会话盒子
	merge_item->Selected(true, false);
	ChangeToSessionBox(id);
	AdjustFormSize();

	return session_box;
}

void SessionForm::CloseSessionBox(const std::string &session_id)
{
	if (session_id.empty())
		return;

	std::wstring id = nbase::UTF8ToUTF16(session_id);

	// 从左侧会话列表项移除对应item
	MergeItem *merge_item = FindMergeItem(id);
	if (NULL != merge_item)
	{
		merge_list_->Remove(merge_item);
	}

	// 在右侧Tab会话盒子列表中找到会话盒子并且移除盒子
	SessionBox *session_box = FindSessionBox(id);
	ASSERT(NULL != session_box);
	if (NULL != session_box)
	{
		taskbar_manager_.UnregisterTab(*session_box->GetTaskbarItem());
		session_box->UninitSessionBox();
		// 如果会话盒子的数量大于1就立马移除盒子，否则不移除
		// 如果最后一个会话盒子在这里立马移除，在窗口关闭时界面会因为没有控件而变成黑色
		// 窗口关闭时，会自动的移除这个会话盒子
		if (session_box_tab_->GetCount() > 1)
			session_box_tab_->Remove(session_box);
	}

	// 当会话盒子只有一个时，隐藏左侧会话列表
	if (merge_list_->GetCount() == 1)
	{
		merge_list_->SetVisible(false);
	}

	// 当会话盒子清空时，关闭会话窗口
	if (merge_list_->GetCount() == 0)
	{
		this->Close(kSessionBoxClose);
	}
	else
	{
		AdjustFormSize();
	}
}

bool SessionForm::AttachSessionBox(SessionBox *session_box)
{
	if (NULL == session_box)
		return false;

	std::wstring id = nbase::UTF8ToUTF16(session_box->GetSessionId());
	if (NULL != FindMergeItem(id))
	{
		ASSERT(0);
		return false;
	}
	if (NULL != FindSessionBox(id))
	{
		ASSERT(0);
		return false;
	}

	MergeItem *merge_item = new MergeItem;
	GlobalManager::FillBoxWithCache(merge_item, L"session/merge_item.xml");
	merge_list_->Add(merge_item);
	merge_item->AttachAllEvents(nbase::Bind(&SessionForm::OnProcessMergeItemDrag, this, std::placeholders::_1));
	merge_item->InitControl(session_box->GetSessionId(), session_box->GetSessionType() == nim::kNIMSessionTypeTeam);
	Button *btn_item_close = (Button*)merge_item->FindSubControl(L"merge_item_close");
	btn_item_close->AttachClick(nbase::Bind(&SessionForm::OnMergeItemClose, this, std::placeholders::_1, session_box->GetSessionId()));

	// 当另一个窗体创建的session_box会话盒子控件添加到另一个窗体内的容器控件时
	// Add函数会重新的修改session_box内所有子控件的m_pWindow为新的窗体指针
	session_box_tab_->Add(session_box);
	taskbar_manager_.RegisterTab(*session_box->GetTaskbarItem());
	Button *btn_header_ = (Button*)session_box->FindSubControl(L"btn_header");
	btn_header_->DetachEvent(kEventAll);
	btn_header_->AttachAllEvents(nbase::Bind(&SessionForm::OnProcessSessionBoxHeaderDrag, this, std::placeholders::_1));

	if (merge_list_->GetCount() <= 1)
		active_session_box_ = session_box;
	else
		merge_list_->SetVisible(true);

	// 切换到新的会话盒子
	// 如果merge_item处于隐藏状态，则无法顺利触发选择事件，所以这里直接切换到目标会话盒子
	merge_item->Selected(true, false);
	ChangeToSessionBox(id);
	AdjustFormSize();

	return true;
}

bool SessionForm::DetachSessionBox(SessionBox *session_box)
{
	if (NULL == session_box)
		return false;

	std::wstring id = nbase::UTF8ToUTF16(session_box->GetSessionId());

	// 从左侧会话列表项移除对应item
	MergeItem *session_item = FindMergeItem(id);
	if (NULL == session_item)
		return false;

	merge_list_->Remove(session_item);

	taskbar_manager_.UnregisterTab(*session_box->GetTaskbarItem());
	// 在右侧Tab会话盒子列表中找到会话盒子并且移除盒子
	// 在这里不能delete session_box
	bool auto_destroy = session_box_tab_->IsAutoDestroy();
	session_box_tab_->SetAutoDestroy(false);
	if (!session_box_tab_->Remove(session_box))
	{
		session_box_tab_->SetAutoDestroy(auto_destroy);
		return false;
	}
	session_box_tab_->SetAutoDestroy(auto_destroy);

	// 当会话盒子只有一个时，隐藏左侧会话列表
	if (merge_list_->GetCount() == 1)
	{
		merge_list_->SetVisible(false);
	}

	// 当会话盒子清空时，关闭会话窗口
	if (merge_list_->GetCount() == 0)
	{
		this->Close(kSessionBoxClose);
	}
	else
	{
		AdjustFormSize();
	}

	return true;
}

SessionBox* SessionForm::GetSelectedSessionBox()
{
	return active_session_box_;
}

void SessionForm::SetActiveSessionBox(const std::string &session_id)
{
	if (session_id.empty())
		return;

	ActiveWindow();

	// 从窗口左侧会话列表找到要激活的会话盒子项
	std::wstring id = nbase::UTF8ToUTF16(session_id);
	MergeItem *merge_item = FindMergeItem(id);
	if (NULL == merge_item)
		return;

	// 如果merge_item处于隐藏状态，则无法顺利触发选择事件，所以这里直接切换到目标会话盒子
	merge_item->Selected(true, false);
	ChangeToSessionBox(id);
}

bool SessionForm::IsActiveSessionBox(const SessionBox *session_box)
{
	ASSERT(NULL != session_box);
	return (::GetForegroundWindow() == GetHWND() && session_box == active_session_box_);
}

bool SessionForm::IsActiveSessionBox(const std::wstring &session_id)
{
	ASSERT(!session_id.empty());
	return (::GetForegroundWindow() == GetHWND() && FindSessionBox(session_id) == active_session_box_);
}

int SessionForm::GetSessionBoxCount() const
{
	return session_box_tab_->GetCount();
}

void SessionForm::OnBeforeDargSessionBoxCallback(const std::wstring &session_id)
{
	// 如果当前被拖拽的会话盒子所属的会话窗口只有一个会话盒子，则在拖拽时隐藏会话窗口
	int box_count = this->GetSessionBoxCount();
	if (1 == box_count)
	{
		this->ShowWindow(false);
	}
	// 否则隐藏被拖拽的会话盒子和合并项
	else
	{
		SessionBox *session_box = FindSessionBox(session_id);
		if (NULL != session_box)
			session_box->SetVisible(false);

		MergeItem *merge_item = FindMergeItem(session_id);
		if (NULL != merge_item)
			merge_item->SetVisible(false);

		// 找到新的被显示的会话盒子
		int index = merge_item->GetIndex();
		if (index > 0)
			index--;
		else
			index++;
		MergeItem *new_merge_item = static_cast<MergeItem*>(merge_list_->GetItemAt(index));
		if (NULL != new_merge_item)
		{
			new_merge_item->Selected(true, false);
			ChangeToSessionBox(new_merge_item->GetName());
		}

		draging_session_id_ = session_id;
	}
}

void SessionForm::OnAfterDragSessionBoxCallback(bool drop_succeed)
{
	is_drag_state_ = false;

	if (drop_succeed)
	{
		int box_count = this->GetSessionBoxCount();
		// 如果当前被拖拽的会话盒子所属的会话窗口只有一个会话盒子，并且拖拽到新的会话窗口里，这个会话窗口就会关闭
		if (1 == box_count)
		{

		}
		else
		{

		}
	}
	else
	{
		int box_count = this->GetSessionBoxCount();
		// 如果当前被拖拽的会话盒子所属的会话窗口只有一个会话盒子，并且没有拖拽到新的会话窗口里
		// 就显示会话窗口
		if (1 == box_count)
		{
			this->ShowWindow(true);
		}
		// 如果当前被拖拽的会话盒子所属的会话窗口有多个会话盒子，并且没有拖拽到新的会话窗口里
		// 就显示之前被隐藏的会话盒子和合并项
		else
		{
			SessionBox *session_box = FindSessionBox(draging_session_id_);
			if (NULL != session_box)
				session_box->SetVisible(true);

			MergeItem *merge_item = FindMergeItem(draging_session_id_);
			if (NULL != merge_item)
			{
				merge_item->SetVisible(true);
				merge_item->Selected(true, false);
				ChangeToSessionBox(draging_session_id_);
			}

			draging_session_id_.clear();
		}
	}
}

void SessionForm::InvokeSetSessionUnread(const std::string &id, int unread)
{
	MergeItem *merge_item = FindMergeItem(nbase::UTF8ToUTF16(id));
	if (NULL != merge_item)
	{
		merge_item->SetUnread(unread);
	}
}

bool SessionForm::OnMergeItemSelected(ui::EventArgs* param)
{
	if (kEventSelect == param->Type)
	{
		std::wstring name = param->pSender->GetName();

		if (name == L"merge_list")
		{
			// 如果单击了左侧会话盒子项，则找到右侧Tab里对应的会话盒子并选中
			Control *select_item = merge_list_->GetItemAt(merge_list_->GetCurSel());
			ASSERT(NULL != select_item);
			if (NULL == select_item)
				return true;

			std::wstring session_id = select_item->GetName();
			ChangeToSessionBox(session_id);
		}
	}

	return true;
}

bool SessionForm::OnMergeItemClose(ui::EventArgs* param, const std::string& session_id)
{
	if (param->pSender->GetName() == L"merge_item_close")
	{
		CloseSessionBox(session_id);
	}

	return false;
}

SessionBox* SessionForm::FindSessionBox(const std::wstring &session_id)
{
	for (int i = 0; i < session_box_tab_->GetCount(); i++)
	{
		Control *box_item = session_box_tab_->GetItemAt(i);
		ASSERT(NULL != box_item);
		if (NULL == box_item)
			return NULL;

		if (box_item->GetName() == session_id)
			return dynamic_cast<SessionBox*>(box_item);
	}

	return NULL;
}

MergeItem* SessionForm::FindMergeItem(const std::wstring &session_id)
{
	for (int i = 0; i < merge_list_->GetCount(); i++)
	{
		Control *merge_item = merge_list_->GetItemAt(i);
		ASSERT(NULL != merge_item);
		if (NULL == merge_item)
			return NULL;

		if (merge_item->GetName() == session_id)
			return static_cast<MergeItem*>(merge_item);
	}

	return NULL;
}

bool SessionForm::ChangeToSessionBox(const std::wstring &session_id)
{
	if (session_id.empty())
		return false;

	SessionBox *box_item = FindSessionBox(session_id);
	if (NULL == box_item)
		return false;

	session_box_tab_->SelectItem(box_item);

	active_session_box_ = box_item;

	// 根据当前激活的会话盒子，更新任务栏的图标和标题
	active_session_box_->OnActivate();

	return true;
}

void SessionForm::AdjustFormSize()
{
	if (IsZoomed(m_hWnd))
		return;

	bool has_advanced_session = false;
	for (int i = 0; i < session_box_tab_->GetCount(); i++)
	{
		Control *box_item = session_box_tab_->GetItemAt(i);
		if (NULL == box_item)
			continue;

		SessionBox *session_box = dynamic_cast<SessionBox*>(box_item);
		if (NULL == session_box)
			continue;

		if (session_box->IsAdvancedTeam())
		{
			has_advanced_session = true;
			break;
		}
	}

	int extend_width = 0;
	if (merge_list_->IsVisible())
		extend_width = merge_list_->GetFixedWidth();

	if (has_advanced_session)
	{
		int width = kLargeSessionBoxWidth + extend_width;
		int height = kLargeSessionBoxHeight;
		this->SetMinInfo(kLargeSessionBoxWidth, kLargeSessionBoxHeight);
		this->SetPos(ui::UiRect(0, 0, width, height), SWP_NOMOVE | SWP_NOACTIVATE);
	}
	else
	{
		int width = kNormalSessionBoxWidth + extend_width;
		int height = kNormalSessionBoxHeight;
		this->SetMinInfo(kNormalSessionBoxWidth, kNormalSessionBoxHeight);
		this->SetPos(ui::UiRect(0, 0, width, height), SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

void SessionForm::SetMergeItemName(const std::wstring &session_id, const std::wstring &name)
{
	MergeItem *merge_item = FindMergeItem(session_id);
	if (NULL != merge_item)
	{
		merge_item->SetTitle(name);
	}
}

void SessionForm::SetMergeItemHeaderPhoto(const std::wstring &session_id, const std::wstring &photo)
{
	MergeItem *merge_item = FindMergeItem(session_id);
	if (NULL != merge_item)
	{
		merge_item->SetIcon(photo);
	}
}

void SessionForm::OnNewMsg(SessionBox &session_box, bool create, bool flash)
{
	if (flash || create)
	{
		bool need = false;

		if (create)
		{
			RECT rc;
			::GetWindowRect(m_hWnd, &rc);

			WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
			::GetWindowPlacement(m_hWnd, &wp);
			wp.showCmd = SW_SHOWMINNOACTIVE;
			wp.rcNormalPosition = rc;
			::SetWindowPlacement(m_hWnd, &wp);

			need = true;
		}
		else
		{
			if (::IsIconic(m_hWnd))
				need = true;
			else if (!IsActiveSessionBox(&session_box))
				need = true;
		}

		if (need)
			FlashTaskbar(session_box);
	}
}

void SessionForm::FlashTaskbar(SessionBox &session_box)
{
	taskbar_manager_.SetTabActive(*session_box.GetTaskbarItem());

	FLASHWINFO flash_info = { sizeof(FLASHWINFO), m_hWnd, FLASHW_TRAY, 3, 0 };
	::FlashWindowEx(&flash_info);
}

void SessionForm::SetTaskbarIcon(const std::wstring &icon)
{
	if (!nbase::FilePathIsExist(icon, false))
		return;

	if (icon_handle_)
	{
		::DestroyIcon(icon_handle_);
	}

	Gdiplus::Bitmap bm(icon.c_str());
	bm.GetHICON(&icon_handle_);
	if (icon_handle_)
	{
		::SendMessage(this->GetHWND(), WM_SETICON, (WPARAM)TRUE, (LPARAM)icon_handle_);
		::SendMessage(this->GetHWND(), WM_SETICON, (WPARAM)FALSE, (LPARAM)icon_handle_);
	}
}

bool SessionForm::CheckRichEditLink(WPARAM wParam, LPARAM lParam)
{
	if (wParam == EN_LINK)
	{
		std::wstring url = *(std::wstring*)lParam;
		if (!url.empty())
		{
			std::wstring ws = url;
			nbase::LowerString(ws);
			// 以"file:"开头 或者 包含".." 的超链接不允许打开
			std::size_t k = ws.find(L"file:", 0, 5);
			if (k == 0)
				return true;

			k = ws.find(L"..");
			if (k != std::wstring::npos)
				return true;

			Post2GlobalMisc(nbase::Bind(&shared::tools::SafeOpenUrl, url, SW_SHOW));
			return true;
		}
	}
	return false;
}

}