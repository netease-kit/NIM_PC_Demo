#include "session_box.h"
#include "shared/zoom_image.h"

using namespace ui;


namespace nim_comp
{

bool SessionBox::CheckDropEnable(POINTL pt)
{
	RECT rect = input_edit_->GetPos();
	ClientToScreen(this->GetWindow()->GetHWND(), (LPPOINT)&rect);
	ClientToScreen(this->GetWindow()->GetHWND(), ((LPPOINT)&rect) + 1);
	if (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SessionBox::OnDropFile(HDROP hDrop)
{
	const int MAX_PATH_SIZE = 1024;

	wchar_t buff[MAX_PATH_SIZE] = { 0 };
	int iFile = DragQueryFileW(hDrop, 0xFFFFFFFF, buff, MAX_PATH_SIZE);
	bool large_file = false;
	for (int i = 0; i < iFile; i++)
	{
		int len = DragQueryFileW(hDrop, i, buff, MAX_PATH_SIZE);
		std::wstring path(buff, len);
		if (nbase::FilePathIsExist(path, true))
		{
			ShowMsgBox(this->GetWindow()->GetHWND(), MsgboxCallback(), L"STRID_SESSION_CANT_SEND_FOLDER");
			continue;
		}
		if (!nbase::FilePathIsExist(path, false))
		{
			continue;
		}

		std::wstring mime_type = ZoomImage::GetMimeType(path);
		if (!mime_type.empty())
		{
			OnImageSelected(false, TRUE, path);
		}
		else if (CheckFileSize(path))
		{
			OnFileSelected(TRUE, path);
		}
		else
		{
			large_file = true;
			wprintf_s(L"Send File %s\n", path.c_str());
		}
	}
	if (large_file)
	{
		ShowMsgBox(this->GetWindow()->GetHWND(), MsgboxCallback(), L"STRID_SESSION_SUPPORT_15MB");
	}

}

HRESULT SessionBox::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (input_edit_droptarget_ == NULL)
	{
		return -1;
	}
	if (!CheckDropEnable(pt))
	{
		*pdwEffect = DROPEFFECT_NONE;
	}
	return input_edit_droptarget_->DragOver(grfKeyState, pt, pdwEffect);
}

HRESULT SessionBox::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if (input_edit_droptarget_ == NULL)
	{
		return -1;
	}
	return input_edit_droptarget_->DragEnter(pDataObject, grfKeyState, pt, pdwEffect);
}

HRESULT SessionBox::DragLeave(void)
{
	if (input_edit_droptarget_ == NULL)
	{
		return -1;
	}
	return input_edit_droptarget_->DragLeave();
}

HRESULT SessionBox::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	if (input_edit_droptarget_ == NULL)
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
			input_edit_droptarget_->DragLeave();
			return S_OK;
		}
	}
	return input_edit_droptarget_->Drop(pDataObj, grfKeyState, pt, pdwEffect);
}


} // namespace nim_comp
