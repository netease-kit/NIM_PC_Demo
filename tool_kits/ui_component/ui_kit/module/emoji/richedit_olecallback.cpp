#include "shared/image_ole_i.h"
#include "richedit_olecallback.h"
#include "richedit_util.h"

namespace nim_comp
{
IRichEditOleCallbackEx::IRichEditOleCallbackEx(ITextServices * text_services, std::function<void()> callback)
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;
	cf_nim_format_ = RegisterClipboardFormat(L"Nim_RichEdit_Format");
	cf_html_format_ = RegisterClipboardFormat(L"HTML Format");
	text_services_ = text_services;
	callback_ = callback;
	custom_mode_ = true;
}

IRichEditOleCallbackEx::~IRichEditOleCallbackEx()
{
	if (pStorage)
	{
		pStorage->Release();
	}
}
void IRichEditOleCallbackEx::SetCustomMode(bool custom)
{
	custom_mode_ = custom;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetNewStorage(LPSTORAGE* lplpstg)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}

ULONG STDMETHODCALLTYPE IRichEditOleCallbackEx::AddRef()
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE IRichEditOleCallbackEx::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	if (*lpclsid == CLSID_ImageOle)
		return S_OK;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (lpdataobj && fReally && (reco == RECO_DROP || reco == RECO_PASTE))
	{
		STGMEDIUM stgMedium;
		HRESULT ret;
		FORMATETC cFmt = {(CLIPFORMAT) cf_nim_format_, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
		ret = lpdataobj->GetData(&cFmt, &stgMedium);
		if (ret == S_OK)
		{
			std::string fragment;
			fragment = (LPSTR)GlobalLock(stgMedium.hGlobal);
			GlobalUnlock(stgMedium.hGlobal);
			Re_InsertNimTextInfo(text_services_, fragment, custom_mode_, callback_);
			::ReleaseStgMedium(&stgMedium);
			return S_FALSE;
		}
	}
	return S_OK;  
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	if (text_services_)
	{
		std::wstring strText;
		std::wstring nim_format;
		bool bCustomObject = Re_GetNimTextInfo(text_services_, lpchrg, strText, nim_format);

		if (strText.empty() && nim_format.empty())
		{
			return E_NOTIMPL;
		}
		if (*lplpdataobj == NULL)
		{
			*lplpdataobj = new SdkDataObject;
		}
		SaveStr2DataObject(*lplpdataobj, strText, CF_TEXT);
		SaveStr2DataObject(*lplpdataobj, strText, CF_UNICODETEXT);
		if (bCustomObject)
		{
			SaveStr2DataObject(*lplpdataobj, nim_format, cf_nim_format_);
		}
		return S_OK;
	} 
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	//return S_OK;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
	HMENU FAR *lphmenu)
{
	return S_OK;
}
HRESULT IRichEditOleCallbackEx::SaveStr2DataObject(LPDATAOBJECT pDataSource, std::wstring str, CLIPFORMAT cfFormat)
{
	HGLOBAL hG = NULL;
	if (cfFormat == CF_UNICODETEXT)
	{
		int  strBytes=  str.length() * sizeof(wchar_t);  
		wchar_t *temp = new wchar_t[str.length()+1];
		ZeroMemory(temp, strBytes+sizeof(wchar_t));
		memcpy(temp, str.c_str(), strBytes);
		hG = GlobalAlloc(GMEM_DDESHARE, strBytes+sizeof(wchar_t));  
		void* pBuffer = GlobalLock(hG);  
		{  
			memcpy(pBuffer, temp, strBytes+sizeof(wchar_t)); 
			GlobalUnlock(hG);  
		} 
		delete[] temp;
	} 
	else
	{
		std::string tmp;
		if (cfFormat == CF_TEXT)
		{
			int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
			if (len > 0)
			{
				std::unique_ptr<char[]> gbk_text(new char[len]);
				WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, gbk_text.get(), len, NULL, NULL);
				tmp = gbk_text.get();
			}

		}
		else
		{
			tmp = nbase::UTF16ToUTF8(str);
		}
		int  strBytes=  tmp.length() * sizeof(char);  
		char *temp = new char[tmp.length()+1];
		ZeroMemory(temp, strBytes+sizeof(char));
		memcpy(temp, tmp.c_str(), strBytes);
		hG = GlobalAlloc(GMEM_DDESHARE, strBytes+sizeof(char));  
		void* pBuffer = GlobalLock(hG);  
		{  
			memcpy(pBuffer, temp, strBytes+sizeof(char)); 
			GlobalUnlock(hG);  
		} 
		delete[] temp;
	}

	FORMATETC fmt;  
	fmt.cfFormat = cfFormat;  
	fmt.dwAspect = DVASPECT_CONTENT;  
	fmt.lindex = -1;  
	fmt.ptd = NULL;  
	fmt.tymed = TYMED_HGLOBAL;  

	STGMEDIUM stg;  
	stg.tymed = TYMED_HGLOBAL;  
	stg.hGlobal = hG;  
	stg.pUnkForRelease = NULL;  

	pDataSource->SetData(&fmt, &stg, FALSE);

	return S_OK;
}

///////////////////////////////////////////////////////
#include <shlobj.h>
#include <shlwapi.h>
SdkDataObject::SdkDataObject(/*SdkDropSource *pDropSource*/)
{
	m_lRefCount = 1;
}
SdkDataObject::~SdkDataObject(void)
{
	m_lRefCount = 0;

	int nSize = (int)m_dataStorageCL.size();
	for (int i = 0; i < nSize; ++i)
	{
		DATASTORAGE_t dataEntry = m_dataStorageCL.at(i);
		ReleaseStgMedium(dataEntry.m_stgMedium);
		if (dataEntry.m_stgMedium)
		{
			delete dataEntry.m_stgMedium;
		}
		if (dataEntry.m_formatEtc)
		{
			delete dataEntry.m_formatEtc;
		}
	}
}
STDMETHODIMP SdkDataObject::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(SdkDataObject, IDataObject),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
	//return E_NOTIMPL;
}
STDMETHODIMP_(ULONG) SdkDataObject::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) SdkDataObject::Release()
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);
	if (0 == lRef)
	{
		delete this;
		return 0;
	}
	return m_lRefCount;
}
STDMETHODIMP SdkDataObject::GetData(FORMATETC *pformatetcIn, 
	STGMEDIUM *pmedium)
{
	if ( (NULL == pformatetcIn) || (NULL == pmedium) )
	{
		return E_INVALIDARG;
	}

	pmedium->hGlobal = NULL;

	int nSize = (int)m_dataStorageCL.size();
	for (int i = 0; i < nSize; ++i)
	{
		DATASTORAGE_t dataEntry = m_dataStorageCL.at(i);
		if( (pformatetcIn->tymed & dataEntry.m_formatEtc->tymed) &&
			(pformatetcIn->dwAspect == dataEntry.m_formatEtc->dwAspect) &&
			(pformatetcIn->cfFormat == dataEntry.m_formatEtc->cfFormat) )
		{
			return CopyMedium(pmedium, 
				dataEntry.m_stgMedium, dataEntry.m_formatEtc);
		}
	}

	return DV_E_FORMATETC;
}

STDMETHODIMP SdkDataObject::SetData(FORMATETC *pformatetc, 
	STGMEDIUM *pmedium, BOOL fRelease)
{
	if ( (NULL == pformatetc) || (NULL == pmedium) )
	{
		return E_INVALIDARG;
	}

	if ( pformatetc->tymed != pmedium->tymed )
	{
		return E_FAIL;
	}

	FORMATETC* fetc = new FORMATETC;
	STGMEDIUM* pStgMed = new STGMEDIUM;
	ZeroMemory(fetc, sizeof(FORMATETC));
	ZeroMemory(pStgMed, sizeof(STGMEDIUM));

	*fetc = *pformatetc;

	if ( TRUE == fRelease )
	{
		*pStgMed = *pmedium;
	}
	else
	{
		CopyMedium(pStgMed, pmedium, pformatetc);
	}

	DATASTORAGE_t dataEntry = { fetc, pStgMed };
	m_dataStorageCL.push_back(dataEntry);

	return S_OK;
}

STDMETHODIMP SdkDataObject::GetDataHere(
	FORMATETC *pformatetc , STGMEDIUM *pmedium)
{
	UNREFERENCED_PARAMETER(pformatetc);
	UNREFERENCED_PARAMETER(pmedium);
	return E_NOTIMPL;
}
STDMETHODIMP SdkDataObject::QueryGetData(FORMATETC *pformatetc)
{
	if ( NULL == pformatetc )
	{
		return E_INVALIDARG;
	}
	if ( !(DVASPECT_CONTENT & pformatetc->dwAspect) )
	{
		return DV_E_DVASPECT;
	}
	HRESULT hr = DV_E_TYMED;
	int nSize = m_dataStorageCL.size();
	for (int i = 0; i < nSize; ++i)
	{
		DATASTORAGE_t dataEnrty = m_dataStorageCL.at(i);
		if ( dataEnrty.m_formatEtc->tymed & pformatetc->tymed )
		{
			if ( dataEnrty.m_formatEtc->cfFormat == pformatetc->cfFormat )
			{
				return S_OK;
			}
			else
			{
				hr = DV_E_CLIPFORMAT;
			}
		}
		else
		{
			hr = DV_E_TYMED;
		}
	}
	return hr;
}
STDMETHODIMP SdkDataObject::GetCanonicalFormatEtc(FORMATETC *pformatetcIn, FORMATETC *pformatetcOut)
{
	return E_NOTIMPL;
}
STDMETHODIMP SdkDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	if ( NULL == ppenumFormatEtc )
	{
		return E_INVALIDARG;
	}
	*ppenumFormatEtc = NULL;
	HRESULT hr = E_NOTIMPL;
	if ( DATADIR_GET == dwDirection )
	{
		int nSize = m_dataStorageCL.size();
		FORMATETC* rgfmtetc = new FORMATETC[nSize];

		for (int i = 0; i < nSize; ++i)
		{
			DATASTORAGE_t dataEnrty = m_dataStorageCL.at(i);
			rgfmtetc[i] = *dataEnrty.m_formatEtc;
		}
		hr = SHCreateStdEnumFmtEtc(nSize, rgfmtetc, ppenumFormatEtc);
	}
	return hr;
}
STDMETHODIMP SdkDataObject::DAdvise(FORMATETC *pformatetc , DWORD advf , IAdviseSink *pAdvSnk , DWORD *pdwConnection)
{
	UNREFERENCED_PARAMETER(pformatetc);
	UNREFERENCED_PARAMETER(advf);
	UNREFERENCED_PARAMETER(pAdvSnk);
	UNREFERENCED_PARAMETER(pdwConnection);
	return E_NOTIMPL;
}

STDMETHODIMP SdkDataObject::DUnadvise(DWORD dwConnection)
{
	UNREFERENCED_PARAMETER(dwConnection);
	return E_NOTIMPL;
}

STDMETHODIMP SdkDataObject::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
	UNREFERENCED_PARAMETER(ppenumAdvise);
	return E_NOTIMPL;
}
HRESULT SdkDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
{
	if ( (NULL == pMedDest) || (NULL ==pMedSrc) || (NULL == pFmtSrc) )
	{
		return E_INVALIDARG;
	}
	switch(pMedSrc->tymed)
	{
	case TYMED_HGLOBAL:
		pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_GDI:
		pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_MFPICT:
		pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_ENHMF:
		pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_FILE:
		pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_ISTREAM:
		pMedDest->pstm = pMedSrc->pstm;
		pMedSrc->pstm->AddRef();
		break;
	case TYMED_ISTORAGE:
		pMedDest->pstg = pMedSrc->pstg;
		pMedSrc->pstg->AddRef();
		break;
	case TYMED_NULL:
	default:
		break;
	}
	pMedDest->tymed = pMedSrc->tymed;
	pMedDest->pUnkForRelease = NULL;
	if(pMedSrc->pUnkForRelease != NULL)
	{
		pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
		pMedSrc->pUnkForRelease->AddRef();
	}
	return S_OK;
}
HRESULT SdkDataObject::SetBlob(CLIPFORMAT cf, const void *pvBlob, UINT cbBlob)
{
	void *pv = GlobalAlloc(GPTR, cbBlob);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if ( SUCCEEDED(hr) )
	{
		CopyMemory(pv, pvBlob, cbBlob);
		FORMATETC fmte = {cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
		// The STGMEDIUM structure is used to define how to handle a global memory transfer.
		// This structure includes a flag, tymed, which indicates the medium
		// to be used, and a union comprising pointers and a handle for getting whichever
		// medium is specified in tymed.
		STGMEDIUM medium = {};
		medium.tymed = TYMED_HGLOBAL;
		medium.hGlobal = pv;
		hr = this->SetData(&fmte, &medium, TRUE);
		if (FAILED(hr))
		{
			GlobalFree(pv);
		}
	}
	return hr;
}
HGLOBAL CreateGlobalHandle(IN void* ptr, int size)
{
	HGLOBAL hGlobal = NULL;
	hGlobal = GlobalAlloc(GMEM_FIXED, size);
	if (NULL != hGlobal)
	{
		LPVOID pdest = GlobalLock(hGlobal);
		if (NULL != pdest)
		{
			memcpy_s(pdest, size, ptr, size);
		}
		GlobalUnlock(hGlobal);
	}
	return hGlobal;
}
}