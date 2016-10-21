#pragma once
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