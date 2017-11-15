#include "session_form.h"
#include "session_box.h"
#include "module/session/session_manager.h"
#include "gui/session/control/merge_item.h"
#include "OleIdl.h"
#include "ShObjIdl.h"
#include <shlobj.h>

using namespace ui;

namespace
{
	const int kDragImageWidth = 300;
	const int kDragImageHeight = 300;
}

namespace nim_comp
{

bool SessionForm::InitDragDrop()
{
	if (NULL != drop_helper_)
		return false;

	if (FAILED(CoCreateInstance(CLSID_DragDropHelper, NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper,
		(void**)&drop_helper_)))
	{
		QLOG_ERR(L"SessionForm::InitDragDrop Create CLSID_DragDropHelper faild");
		return false;
	}

	if (FAILED(RegisterDragDrop(this->GetHWND(), this)))
	{
		QLOG_ERR(L"SessionForm::InitDragDrop RegisterDragDrop faild");
		return false;
	}

	QLOG_APP(L"SessionForm::InitDragDrop succeed");
	return true;
}

void SessionForm::UnInitDragDrop()
{
	if (NULL != drop_helper_)
		drop_helper_->Release();

	RevokeDragDrop(this->GetHWND());
}

HRESULT SessionForm::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (NULL == drop_helper_)
		return E_NOINTERFACE;

	return drop_helper_->QueryInterface(iid, ppvObject);
}

ULONG SessionForm::AddRef(void)
{
	if (NULL == drop_helper_)
		return 0;

	return drop_helper_->AddRef();
}

ULONG SessionForm::Release(void)
{
	if (NULL == drop_helper_)
		return 0;

	return drop_helper_->Release();
}

HRESULT SessionForm::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if (NULL == drop_helper_)
		return S_OK;

	// 如果不是拖拽会话盒子
	if (!SessionManager::GetInstance()->IsDragingSessionBox())
	{
		if (NULL != active_session_box_)
		{
			active_session_box_->DragEnter(pDataObject, grfKeyState, pt, pdwEffect);
			ActiveWindow();
		}
	}
	else
	{
		QLOG_APP(L"SessionForm::Drop receive a SessionBox");
		*pdwEffect = DROPEFFECT_MOVE;
	}

	drop_helper_->DragEnter(this->GetHWND(), pDataObject, (LPPOINT)&pt, *pdwEffect);
	return S_OK;
}

HRESULT SessionForm::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == drop_helper_)
		return S_OK;

	// 如果不是拖拽会话盒子
	if (!SessionManager::GetInstance()->IsDragingSessionBox())
	{
		if (NULL != active_session_box_)
		{
			active_session_box_->DragOver(grfKeyState, pt, pdwEffect);
		}
	}
	else
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}

	drop_helper_->DragOver((LPPOINT)&pt, *pdwEffect);
	return S_OK;
}

HRESULT SessionForm::DragLeave(void)
{
	if (NULL == drop_helper_)
		return S_OK;

	// 如果不是拖拽会话盒子
	if (!SessionManager::GetInstance()->IsDragingSessionBox())
	{
		if (NULL != active_session_box_)
		{
			active_session_box_->DragLeave();
		}
	}

	drop_helper_->DragLeave();
	return S_OK;
}

HRESULT SessionForm::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	// 如果不是拖拽会话盒子
	if (!SessionManager::GetInstance()->IsDragingSessionBox())
	{
		if (NULL != active_session_box_ && active_session_box_->CheckDropEnable(pt))
		{
			active_session_box_->Drop(pDataObj, grfKeyState, pt, pdwEffect);
		}
	}
	else
	{
		QLOG_APP(L"SessionForm::Drop drop a SessionBox");
		*pdwEffect = DROPEFFECT_MOVE;
		SessionManager::GetInstance()->SetDropSessionForm(this);
	}

	drop_helper_->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);
	return S_OK;
}

bool SessionForm::OnProcessSessionBoxHeaderDrag(ui::EventArgs* param)
{
	if (!SessionManager::GetInstance()->IsEnableMerge())
		return true;

	switch (param->Type)
	{
	case kEventMouseMove:
	{
		if (::GetKeyState(VK_LBUTTON) >= 0)
			break;

		LONG cx = abs(param->ptMouse.x - old_drag_point_.x);
		LONG cy = abs(param->ptMouse.y - old_drag_point_.y);

		if (!is_drag_state_ && (cx > 5 || cy > 5))
		{
			if (NULL == active_session_box_)
				break;

			is_drag_state_ = true;

			// 把被拖拽的会话盒子生成一个宽度300的位图
			HBITMAP bitmap = GenerateSessionBoxBitmap(session_box_tab_->GetPos(true));

			// pt应该指定相对bitmap位图的左上角(0,0)的坐标,这里设置为bitmap的中上点
			POINT pt = { kDragImageWidth / 2, 0 };

			StdClosure cb = [=]{
				SessionManager::GetInstance()->DoDragSessionBox(active_session_box_, bitmap, pt);
			};
			nbase::ThreadManager::PostTask(ThreadId::kThreadUI, cb);
		}
	}
	break;
	case kEventMouseButtonDown:
	{
		// 如果当前会话窗口里只有一个会话盒子，则可以通过拖拽头像来拖拽会话盒子
		// 否则只能拖拽merge_list的merge_item来拖拽
		if (1 == session_box_tab_->GetCount())
		{
			old_drag_point_ = param->ptMouse;
			is_drag_state_ = false;
		}
	}
	break;
	}
	return true;
}

bool SessionForm::OnProcessMergeItemDrag(ui::EventArgs* param)
{
	if (!SessionManager::GetInstance()->IsEnableMerge())
		return true;

	switch (param->Type)
	{
	case kEventMouseMove:
	{
		if (::GetKeyState(VK_LBUTTON) >= 0)
			break;

		LONG cx = abs(param->ptMouse.x - old_drag_point_.x);
		LONG cy = abs(param->ptMouse.y - old_drag_point_.y);

		if (!is_drag_state_ && (cx > 5 || cy > 5))
		{
			if (NULL == active_session_box_)
				break;

			is_drag_state_ = true;

			// 把被拖拽的会话盒子生成一个宽度300的位图
			HBITMAP bitmap = GenerateSessionBoxBitmap(session_box_tab_->GetPos(true));

			// pt应该指定相对bitmap位图的左上角(0,0)的坐标,这里设置为bitmap的中上点
			POINT pt = { kDragImageWidth / 2, 0 };

			StdClosure cb = [=]{
				SessionManager::GetInstance()->DoDragSessionBox(active_session_box_, bitmap, pt);
			};
			nbase::ThreadManager::PostTask(ThreadId::kThreadUI, cb);
		}
	}
	break;
	case kEventMouseButtonDown:
	{
		old_drag_point_ = param->ptMouse;
		is_drag_state_ = false;
	}
	break;
	}
	return true;
}

HBITMAP SessionForm::GenerateSessionBoxBitmap(const UiRect &src_rect)
{
	ASSERT(!src_rect.IsRectEmpty());
	int src_width = src_rect.right - src_rect.left;
	int src_height = src_rect.bottom - src_rect.top;

	auto render = GlobalManager::CreateRenderContext();
	if (render->Resize(kDragImageWidth, kDragImageHeight))
	{
		int dest_width = 0;
		int dest_height = 0;
		float scale = (float)src_width / (float)src_height;
		if (scale >= 1.0)
		{
			dest_width = kDragImageWidth;
			dest_height = (int)(kDragImageWidth * (float)src_height / (float)src_width);
		}
		else
		{
			dest_height = kDragImageHeight;
			dest_width = (int)(kDragImageHeight * (float)src_width / (float)src_height);
		}

		render->AlphaBlend((kDragImageWidth - dest_width) / 2, 0, dest_width, dest_height, this->GetRenderContext()->GetDC(),
			src_rect.left, src_rect.top, src_rect.right - src_rect.left, src_rect.bottom - src_rect.top);
	}

	return render->DetachBitmap();
}

}