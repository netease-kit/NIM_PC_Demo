#include "licence_box.h"

const LPCTSTR LicenceForm::kClassName = L"LicenceForm";

LicenceForm::LicenceForm()
{
}

LicenceForm::~LicenceForm()
{
	
}

LRESULT LicenceForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN)
	{
		if(wParam == VK_ESCAPE)
		{
			Close();
			return 1;
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

LicenceForm* LicenceForm::ShowLicence() {
    auto* window = LicenceForm::GetInstance();
    // LicenceForm* window = nbase::Singleton<LicenceForm>::get();
    if (!::IsWindow(window->m_hWnd)) {
        window = new LicenceForm();
        window->GetLicenceText();
        window->Create(NULL, LicenceForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX, 0);
        window->CenterWindow();
        window->ShowWindow();
    } else {
        if (::IsWindow(window->m_hWnd)) {
            if (::IsIconic(window->m_hWnd))
                ::ShowWindow(window->m_hWnd, SW_RESTORE);
            else
                ::SetForegroundWindow(window->m_hWnd);
        }
    }
    return window;
}

std::wstring LicenceForm::GetSkinFolder()
{
	return L"form";
}

std::wstring LicenceForm::GetSkinFile()
{
	return L"licence.xml";
}

ui::UILIB_RESOURCETYPE LicenceForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring LicenceForm::GetZIPFileName() const
{
	return (L"LicenceForm.zip");
}

std::wstring LicenceForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring LicenceForm::GetWindowId() const
{
	return kClassName;
}

UINT LicenceForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LicenceForm::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
}
std::wstring LicenceForm::GetLicenceText()
{
	std::wstring path = ui::GlobalManager::GetResourcePath() + L"licence.rtf";
	std::string text;
	if (ui::GlobalManager::IsUseZip())
	{
		HGLOBAL hGlobal = ui::GlobalManager::GetData(path);
		if (hGlobal)
		{
			text.append((LPSTR)GlobalLock(hGlobal), GlobalSize(hGlobal));
			GlobalUnlock(hGlobal);
			GlobalFree(hGlobal);
		}
	}
	else
	{
		HANDLE hFile = ::CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwRead = 0;
			BYTE* pByte = new BYTE[1024*1024];
			::ReadFile(hFile, pByte, 1024 * 1024, &dwRead, NULL);
			text.append((char*)pByte, dwRead);
			delete[] pByte;
			::CloseHandle(hFile);
		}
	}
	edit_text_ = nbase::UTF8ToUTF16(text);
	return edit_text_;
}
DWORD CALLBACK EditStreamCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	std::wstring *pstr = (std::wstring *)dwCookie;
	uint32_t size = pstr->size();
	if (size >= cb)
	{
		size = cb;
		*pcb = cb;
	}
	WideCharToMultiByte(CP_OEMCP, NULL, pstr->c_str(), size, (LPSTR)pbBuff, cb, NULL, FALSE);
	*pstr = pstr->substr(size);
	return 0;
}
void LicenceForm::InitWindow()
{
    // GetLicenceText();
    // edit_text_ = L"abcdefg";
	m_pRoot->AttachBubbledEvent(ui::EventType::kEventAll, std::bind(&LicenceForm::Notify, this, std::placeholders::_1));
	edit_ = (ui::RichEdit*)FindControl(L"info");
	EDITSTREAM es = { (DWORD_PTR)&edit_text_, 0, EditStreamCallBack };
	edit_->StreamIn(SF_RTF | SFF_SELECTION, es);
}

bool LicenceForm::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (msg->Type == ui::EventType::kEventClick)
	{
		if (name == L"button_1")
		{
			Close();
		}
	}
	return true;
}