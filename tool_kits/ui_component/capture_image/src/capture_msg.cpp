#include "capture_wnd.h"
#include <time.h>

LRESULT CaptureWindow::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	bool edit = false;
	point_offset_.x = GET_X_LPARAM(lParam);
	point_offset_.y = GET_Y_LPARAM(lParam);
	switch (DeTrackState(track_state_))
	{
	case NOTRACK:
		{
			AdjustOffset(track_rect_, point_offset_, point_offset_, NOTRACK);
			SetTrackState(TRACKING|SIZESE);

			bHandled = TRUE;
		}
		break;
	case TRACKING:
	case TRACKED:
		{
			EditMode edit_mode = GetCaptureEditMode();
			if (edit_mode == EM_NONE)
			{
				int state = TrackerHitTest(ClipTrackRect(track_rect_), point_offset_);
				SetHitTestCursor(state);
				if (state>NOTRACK)
				{
					SetTrackState(TRACKING | state);
					bHandled = TRUE;
				}
			}
			else
			{
				// »æÖÆ½ØÍ¼±à¼­, ÕâÊÇÒ»´Î±à¼­µÄ¿ªÊ¼
				capture_edit_->SetTrackRect(track_rect_); 
				//capture_edit_->SetEditMode(capture_edit_mode_);
				edit = capture_edit_->OnLButtonDownInCaptureEditMode(point_offset_, toolbar_->GetPos()) == TRUE;
				// »æÖÆ½ØÍ¼±à¼­
				capture_edit_->OnMouseMoveInCaptureEditMode(point_offset_, toolbar_->GetPos());
			}
		}
		break;
	}
	ShowToolBar(edit);
	return 0;
}

LRESULT CaptureWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//POINT point;
	//GetCursorPos(&point);
	POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	switch(DeTrackState(track_state_))
	{
	case TRACKING:
		{
			if (point_offset_.x != point.x || point_offset_.y != point.y)
			{
				AdjustOffset(track_rect_, point, point_offset_, DeSizeState(track_state_));
				DrawTrack(track_rect_);
				ShowSizeTip();
				SetHitTestCursor(DeSizeState(track_state_));
				point_offset_ = point;
			}
			bHandled = TRUE;
		}
		break;
	case TRACKED:
		{
			EditMode edit_mode = GetCaptureEditMode();
			if(edit_mode == EM_NONE)
			{
				int state = TrackerHitTest(ClipTrackRect(track_rect_), point);
				SetHitTestCursor(state);
			}
			else
			{
				// »æÖÆ½ØÍ¼±à¼­
				capture_edit_->OnMouseMoveInCaptureEditMode(point, toolbar_->GetPos());
			}
		}
		break;
	}
	return 0;
}

LRESULT CaptureWindow::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	switch(DeTrackState(track_state_))
	{
	case TRACKING:
		{
			NormalizeRect(track_rect_);
			if (track_rect_.right>track_rect_.left || track_rect_.bottom>track_rect_.top)
			{
				int state = TrackerHitTest(ClipTrackRect(track_rect_), point_offset_);
				SetTrackState(TRACKED);
				SetHitTestCursor(state);
				DrawTrack(track_rect_);
			}
			else
			{
				SetTrackState(NOTRACK);
				SetHitTestCursor(NOTRACK);
			}
			ShowSizeTip();
			bHandled = TRUE;
		}
		break;
	case TRACKED:
		{
			capture_edit_->OnLButtonUpInCaptureEditMode(point);
		}
		break;
	}
	ShowToolBar();
	return 0;
}


LRESULT CaptureWindow::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (WM_LBUTTONDBLCLK == uMsg)
	{
		if (TRACKED == track_state_ && SIZEALL == TrackerHitTest(ClipTrackRect(track_rect_), point_offset_))
		{
			SaveTrackImage();
			Close();
		}
	}
	else if( uMsg == WM_KEYDOWN)
	{
		if( wParam == VK_ESCAPE)
		{
			SetFocus(GetRoot());
			Close();
		}
	}
	else if ( uMsg == WM_SETCURSOR)
	{
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}
