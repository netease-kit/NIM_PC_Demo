#include "session_manager.h"
#include "gui/session/session_form.h"
#include "gui/session/session_box.h"
#include "gui/session/dragdrop/drag_form.h"
#include "module/dragdrop/drag_drop.h"

namespace
{
	const int kSplitFormXOffset = 20;	//自动拆分会话窗口后新窗口的x偏移坐标
	const int kSplitFormYOffset = 20;	//自动拆分会话窗口后新窗口的y偏移坐标
	const int kDragFormXOffset = -100;	//拖拽出新会话窗口后的相对鼠标的x偏移坐标
	const int kDragFormYOffset = -20;	//拖拽出新会话窗口后的相对鼠标的y偏移坐标
}

namespace nim_comp
{

void SessionManager::SetEnableMerge(bool enable)
{
	if (enable_merge_ == enable)
		return;

	enable_merge_ = enable;

	if (enable_merge_)
	{
		// 如果当前只有一个会话窗口或者会话盒子，就不需要进行合并操作
		if (session_box_map_.size() <= 1)
			return;

		// 选择第一个会话盒子所属的窗口作为合并窗口
		SessionForm *merge_form = session_box_map_.begin()->second->GetSessionForm();

		// 遍历所有会话盒子，脱离原会话窗口，再附加到合并窗口里
		for (auto it_box : session_box_map_)
		{
			ASSERT(NULL != it_box.second);
			SessionForm *parent_form = it_box.second->GetSessionForm();
			if (merge_form != parent_form)
			{
				if (parent_form->DetachSessionBox(it_box.second))
				{
					merge_form->AttachSessionBox(it_box.second);
				}
			}
		}
	}
	else
	{
		// 如果当前只有一个会话盒子，就不需要进行拆分操作
		if (session_box_map_.size() <= 1)
			return;

		// 给新拆分的窗口设置坐标
		bool first_sort = true;
		ui::UiRect rect_old_form;
		SessionForm *sort_form = NULL;

		// 遍历所有会话盒子，脱离原会话窗口，创建新的会话窗口并附加会话盒子
		for (auto it_box : session_box_map_)
		{
			ASSERT(NULL != it_box.second);
			SessionForm *parent_form = it_box.second->GetSessionForm();
			if (1 == parent_form->GetSessionBoxCount())
			{
				sort_form = parent_form;
			}
			else if (parent_form->DetachSessionBox(it_box.second))
			{
				SessionForm *session_form = new SessionForm;
				HWND hwnd = session_form->Create(NULL, L"会话窗口", UI_WNDSTYLE_FRAME, 0);
				if (hwnd == NULL)
				{
					ASSERT(0);
					continue;
				}

				if (!session_form->AttachSessionBox(it_box.second))
				{
					ASSERT(0);
					continue;
				}

				sort_form = session_form;
			}

			if (NULL != sort_form)
			{
				if (first_sort)
				{
					first_sort = false;
					sort_form->CenterWindow();
					rect_old_form = sort_form->GetPos(true);
				}
				else
				{
					rect_old_form.left += kSplitFormXOffset;
					rect_old_form.top += kSplitFormXOffset;
					sort_form->SetPos(rect_old_form, SWP_NOSIZE, NULL, true);
				}
			}
		}
	}
}

bool SessionManager::IsEnableMerge() const
{
	return enable_merge_;
}

void SessionManager::SetUseCustomDragImage(bool use)
{
	use_custom_drag_image_ = use;
}

bool SessionManager::IsUseCustomDragImage() const
{
	return use_custom_drag_image_;
}

bool SessionManager::IsDragingSessionBox() const
{
	return enable_merge_ && NULL != draging_session_box_;
}

void SessionManager::SetDropSessionForm(SessionForm *session_form)
{
	if (NULL == session_form)
		return;

	drop_session_form_ = session_form;
}

bool SessionManager::DoDragSessionBox(SessionBox *session_box, HBITMAP bitmap, POINT pt_offset)
{
	if (!enable_merge_)
		return false;

	SdkDropSource* drop_src = new SdkDropSource;
	if (drop_src == NULL)
		return false;

	SdkDataObject* data_object = CreateDragDataObject(bitmap, pt_offset);
	if (data_object == NULL)
		return false;

	// 无论什么时候都让拖拽时光标显示为箭头
	drop_src->SetFeedbackCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

	OnBeforeDargSessionBox(session_box, bitmap, pt_offset);

	// 此函数会阻塞直到拖拽完成
	DWORD dwEffect;
	HRESULT hr = ::DoDragDrop(data_object, drop_src, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);

	OnAfterDragSessionBox();

	// 销毁位图
	DeleteObject(bitmap);
	drop_src->Release();
	data_object->Release();
	return true;
}

SdkDataObject* SessionManager::CreateDragDataObject(HBITMAP bitmap, POINT pt_offset)
{
	SdkDataObject* data_object = new SdkDataObject;
	if (data_object == NULL)
		return NULL;

	if (use_custom_drag_image_)
	{
		FORMATETC fmtetc = { 0 };
		fmtetc.dwAspect = DVASPECT_CONTENT;
		fmtetc.lindex = -1;
		fmtetc.cfFormat = CF_HDROP;
		fmtetc.tymed = TYMED_NULL;

		STGMEDIUM medium = { 0 };
		medium.tymed = TYMED_NULL;
		data_object->SetData(&fmtetc, &medium, FALSE);
	}
	else
	{
		FORMATETC fmtetc = { 0 };
		fmtetc.dwAspect = DVASPECT_CONTENT;
		fmtetc.lindex = -1;
		fmtetc.cfFormat = CF_BITMAP;
		fmtetc.tymed = TYMED_GDI;

		STGMEDIUM medium = { 0 };
		medium.tymed = TYMED_GDI;
		HBITMAP hBitmap = (HBITMAP)OleDuplicateData(bitmap, fmtetc.cfFormat, NULL);
		medium.hBitmap = hBitmap;
		data_object->SetData(&fmtetc, &medium, FALSE);

		BITMAP bitmap_info;
		GetObject(hBitmap, sizeof(BITMAP), &bitmap_info);
		SIZE bitmap_size = { bitmap_info.bmWidth, bitmap_info.bmHeight };
		SdkDragSourceHelper dragSrcHelper;
		dragSrcHelper.InitializeFromBitmap(hBitmap, pt_offset, bitmap_size, data_object, RGB(255, 0, 255));
	}

	return data_object;
}

void SessionManager::OnBeforeDargSessionBox(SessionBox *session_box, HBITMAP bitmap, POINT pt_offset)
{
	// 获取当前被拖拽的会话盒子所属的会话窗口
	draging_session_box_ = session_box;
	SessionForm *drag_session_form = draging_session_box_->GetSessionForm();
	ASSERT(NULL != drag_session_form);

	// 获取被拖拽会话窗口中会话盒子的数量
	int box_count = drag_session_form->GetSessionBoxCount();
	ASSERT(box_count > 0);
	drop_session_form_ = NULL;

	drag_session_form->OnBeforeDargSessionBoxCallback(nbase::UTF8ToUTF16(draging_session_box_->GetSessionId()));

	if (use_custom_drag_image_)
		DragForm::CreateCustomDragImage(bitmap, pt_offset);
}

void SessionManager::OnAfterDragSessionBox()
{
	if (use_custom_drag_image_)
		DragForm::CloseCustomDragImage();

	if (NULL == draging_session_box_)
		return;

	// 获取当前被拖拽的会话盒子所属的会话窗口
	SessionForm *drag_session_form = draging_session_box_->GetSessionForm();
	ASSERT(NULL != drag_session_form);

	// 获取被拖拽会话窗口中会话盒子的数量
	int box_count = drag_session_form->GetSessionBoxCount();
	ASSERT(box_count > 0);

	// 如果被拖拽的会话盒子放入到一个会话窗口里
	if (NULL != drop_session_form_)
	{
		if (drag_session_form == drop_session_form_)
		{
			drag_session_form->OnAfterDragSessionBoxCallback(false);
		}
		else
		{
			drag_session_form->OnAfterDragSessionBoxCallback(true);
			if (drag_session_form->DetachSessionBox(draging_session_box_))
			{
				drop_session_form_->AttachSessionBox(draging_session_box_);
			}
		}

		// 如果被拖拽的会话窗口包含多个会话盒子，就投递一个WM_LBUTTONUP消息给窗口
		// (因为窗口被拖拽时触发了ButtonDown和ButtonMove消息，但是最终的ButtonUp消息会被忽略，这里补上)
		// 如果只有一个会话盒子，则会话盒子脱离会话窗口时，会话窗体就会关闭，不需要投递
		if (box_count > 1)
			drag_session_form->PostMessage(WM_LBUTTONUP, 0, 0);
	}
	// 如果没有被拖拽到另一个会话窗口里
	else
	{
		// 如果被拖拽的会话窗口里只有一个会话盒子,则拖拽失败
		if (1 == box_count)
		{
			drag_session_form->OnAfterDragSessionBoxCallback(false);
		}
		// 如果有多个会话盒子, 就把会话盒子脱离原会话窗口，附加到新的会话窗口，拖拽成功
		else
		{
			drag_session_form->OnAfterDragSessionBoxCallback(true);

			if (drag_session_form->DetachSessionBox(draging_session_box_))
			{
				SessionForm *session_form = new SessionForm;
				HWND hwnd = session_form->Create(NULL, L"会话窗口", UI_WNDSTYLE_FRAME, 0);
				if (hwnd != NULL)
				{
					if (session_form->AttachSessionBox(draging_session_box_))
					{
						// 这里设置新会话窗口的位置，设置到偏移鼠标坐标100,20的位置
						POINT pt_mouse;
						::GetCursorPos(&pt_mouse);
						ui::UiRect rect(pt_mouse.x + kDragFormXOffset, pt_mouse.y + kDragFormYOffset, 0, 0);
						session_form->SetPos(rect, SWP_NOSIZE);
					}
				}
			}
		}

		// 如果没有被拖拽到另一个会话窗口里，这时不会有会话窗口被关闭，所以直接投递ButtonUp消息
		drag_session_form->PostMessage(WM_LBUTTONUP, 0, 0);
	}

	draging_session_box_ = NULL;
	drop_session_form_ = NULL;
}

}