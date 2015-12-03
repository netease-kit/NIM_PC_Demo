#ifndef RICHEDIT_OLECALLBACK_H
#define RICHEDIT_OLECALLBACK_H
#include <atlbase.h>
#include <richedit.h>
#include <richole.h>
#include <textserv.h>

namespace nim_comp
{
interface IRichEditOleCallbackEx : public IRichEditOleCallback
{
public:
	IRichEditOleCallbackEx(ITextServices * text_services, std::function<void()> callback);
	virtual ~IRichEditOleCallbackEx();
	//CRichEditCtrl *m_pRichEdit;
	void SetCustomMode(bool custom = false);

	virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
		LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
	virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
	virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
	virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
		DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
	virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
	virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
		HMENU FAR *lphmenu);

private:
	HRESULT SaveStr2DataObject(LPDATAOBJECT pDataSource, std::wstring str, CLIPFORMAT cfFormat);
	int m_iNumStorages;
	IStorage* pStorage;
	DWORD m_dwRef;
	UINT cf_html_format_;
	UINT cf_nim_format_;
	ITextServices * text_services_;
	std::function<void()> callback_;
	bool custom_mode_;
};
}
#ifdef __cplusplus
#ifndef _SDKDATAOBJECT_H_
#define _SDKDATAOBJECT_H_


typedef struct _DATASTORAGE
{
	FORMATETC *m_formatEtc;
	STGMEDIUM *m_stgMedium;

} DATASTORAGE_t, *LPDATASTORAGE_t;

namespace nim_comp
{
class SdkDataObject : public IDataObject
{
public:

	SdkDataObject(/*SdkDropSource *pDropSource = NULL*/);
	BOOL IsDataAvailable(CLIPFORMAT cfFormat);
	BOOL GetGlobalData(CLIPFORMAT cfFormat, void **ppData);
	BOOL GetGlobalDataArray(CLIPFORMAT cfFormat, 
		HGLOBAL *pDataArray, DWORD dwCount);
	BOOL SetGlobalData(CLIPFORMAT cfFormat, void *pData, BOOL fRelease = TRUE);
	BOOL SetGlobalDataArray(CLIPFORMAT cfFormat, 
		HGLOBAL *pDataArray, DWORD dwCount, BOOL fRelease = TRUE);
	//BOOL SetDropTip(DROPIMAGETYPE type, PCWSTR pszMsg, PCWSTR pszInsert);

	// The com interface.
	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();
	IFACEMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	IFACEMETHODIMP SetData(FORMATETC *pformatetc,
		STGMEDIUM *pmedium, BOOL fRelease);
	IFACEMETHODIMP GetDataHere(FORMATETC *pformatetc , STGMEDIUM *pmedium );
	IFACEMETHODIMP QueryGetData(FORMATETC *pformatetc);
	IFACEMETHODIMP GetCanonicalFormatEtc(FORMATETC *pformatetcIn,  
		FORMATETC *pformatetcOut);
	IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection, 
		IEnumFORMATETC **ppenumFormatEtc);
	IFACEMETHODIMP DAdvise(FORMATETC *pformatetc , DWORD advf , 
		IAdviseSink *pAdvSnk , DWORD *pdwConnection);
	IFACEMETHODIMP DUnadvise(DWORD dwConnection);
	IFACEMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);

private:

	~SdkDataObject(void);
	SdkDataObject(const SdkDataObject&);
	SdkDataObject& operator = (const SdkDataObject&);
	HRESULT CopyMedium(STGMEDIUM* pMedDest, 
		STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);
	HRESULT SetBlob(CLIPFORMAT cf, const void *pvBlob, UINT cbBlob);

private:

	//!< The reference of count
	volatile LONG           m_lRefCount;      
	//!< The pointer to CDropSource object  
	//SdkDropSource          *m_pDropSource;  
	//!< The collection of DATASTORAGE_t structure    
	std::vector<DATASTORAGE_t>   m_dataStorageCL;    
};
}
#endif // _SDKDATAOBJECT_H_
#endif // __cplusplus

#endif //RICHEDIT_OLECALLBACK_H