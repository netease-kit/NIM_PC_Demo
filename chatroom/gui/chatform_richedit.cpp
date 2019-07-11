#include "resource.h"
#include "chatroom_form.h"
#include "util/user.h"
#include "shared/tool.h"

namespace nim_chatroom
{

using namespace ui;
using namespace nim_comp;

bool CheckFileSize(const std::wstring &src)
{
	int64_t sz = nbase::GetFileSize(src);
	if (sz > 15 * 1024 * 1024 || sz <= 0)
	{
		return false;
	}
	return true;
}

void ChatroomForm::OnDropFile(HDROP hDrop)
{
	const int MAX_PATH_SIZE = 1024;

	wchar_t buff[MAX_PATH_SIZE] = { 0 };
	int iFile = DragQueryFileW(hDrop, 0xFFFFFFFF, buff, MAX_PATH_SIZE);
	bool large_file = false;
	for (int i = 0; i < iFile; i++)
	{
		int len = DragQueryFileW(hDrop, i, buff, MAX_PATH_SIZE);
		std::wstring path(buff, len);

		std::wstring mime_type = ZoomImage::GetMimeType(path);
		if (!mime_type.empty())
		{
		//	SendImage(path);
		}
		else if (CheckFileSize(path))
		{
		//	SendFile(path);
		}
		else
		{
			large_file = true;
			wprintf_s(L"Send File %s\n", path.c_str());
		}
	}
	if (large_file)
	{
		ShowMsgBox(m_hWnd, MsgboxCallback(), L"STRID_CHATROOM_SUPPORT_15MB");
	}
	//目前是通过RichEdit的拖曳来处理
	//DragFinish(hDrop);
}

BOOL ChatroomForm::CheckDropEnable(POINTL pt)
{
	RECT rect = input_edit_->GetPos();
	ClientToScreen(m_hWnd, (LPPOINT)&rect);
	ClientToScreen(m_hWnd, ((LPPOINT)&rect) + 1);
	if (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HRESULT ChatroomForm::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->QueryInterface(iid, ppvObject);
}

ULONG ChatroomForm::AddRef(void)
{
	if (droptarget_ == NULL)
	{
		return 0;
	}
	return droptarget_->AddRef();
}

ULONG ChatroomForm::Release(void)
{
	if (droptarget_ == NULL)
	{
		return 0;
	}
	return droptarget_->Release();
}

HRESULT ChatroomForm::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	if (!CheckDropEnable(pt))
	{
		*pdwEffect = DROPEFFECT_NONE;
	}
	return droptarget_->DragOver(grfKeyState, pt, pdwEffect);
}

HRESULT ChatroomForm::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->DragEnter(pDataObject, grfKeyState, pt, pdwEffect);
}

HRESULT ChatroomForm::DragLeave(void)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->DragLeave();
}

HRESULT ChatroomForm::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	if (pDataObj)
	{
		STGMEDIUM stgMedium;
		FORMATETC cFmt = { (CLIPFORMAT)CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		HRESULT hr = pDataObj->GetData(&cFmt, &stgMedium);
		if (hr == S_OK)
		{
			//拖曳文件
			HDROP file = NULL;
			file = (HDROP)GlobalLock(stgMedium.hGlobal);
			GlobalUnlock(stgMedium.hGlobal);
			OnDropFile(file);
			::ReleaseStgMedium(&stgMedium);
			droptarget_->DragLeave();
			return S_OK;
		}
	}
	return droptarget_->Drop(pDataObj, grfKeyState, pt, pdwEffect);
}

std::wstring ChatroomForm::GetCurrentTimeString()
{
	SYSTEMTIME time = { 0 };
	TCHAR szTime[MAX_PATH] = { 0 };
	::GetLocalTime(&time);

	_stprintf_s(szTime, MAX_PATH, _T("   %04d-%02d-%02d %02d:%02d:%02d\r\n"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	return szTime;
}

void ChatroomForm::TimeHandle()
{
	if (input_edit_)
	{
		ITextServices * text_service = input_edit_->GetTextServices();
		if (text_service)
		{
			if (Re_AllImageOleChangeFrame(text_service))
			{
				input_edit_->Invalidate();
			}
			text_service->Release();
		}
// 		if (list_msg_)
// 		{
// 			bool refresh = false;
// 			for (int is = 0; is < list_msg_->GetCount(); is++)
// 			{
// 				Control* control = list_msg_->GetItemAt(is);
// 				if (control && typeid(*control) == typeid(MsgCell))
// 				{
// 					MsgCell* cell = (MsgCell*)control;
// 					if (cell->bubble_type_ == kBubbleTTImage)
// 					{
// 						if (cell->UpdateGifFrame())
// 						{
// 							refresh = true;
// 						}
// 					}
// 				}
// 			}
// 			if (refresh)
// 			{
// 				list_msg_->Invalidate();
// 			}
// 		}
	}

#if 0
	if (msg_list_)
	{
		ITextServices * text_service = msg_list_->GetTextServices();
		if (text_service)
		{
			if (Re_AllImageOleChangeFrame(text_service))
			{
				msg_list_->Invalidate();
			}
			text_service->Release();
		}
	}
#endif
}

}