// ImageOle.h : Declaration of the CImageOle
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <string>
#include "image_ole_i.h"
#include "_IImageOleEvents_CP.h"

#include <gdiplus.h>


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;
using namespace Gdiplus;

enum
{
	RE_OLE_TYPE_TEXT = 0,
	RE_OLE_TYPE_FACE,
	RE_OLE_TYPE_IMAGE,
	RE_OLE_TYPE_IMAGELOADING,//正在加载的图片
	RE_OLE_TYPE_FILE,
	RE_OLE_TYPE_ERROR,
	RE_OLE_TYPE_DESCRIPTION, //描述
};

// CImageOle
class ATL_NO_VTABLE CImageOle :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IImageOle, &IID_IImageOle, &LIBID_image_oleLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<CImageOle>,
	public IOleObjectImpl<CImageOle>,
	public IOleInPlaceActiveObjectImpl<CImageOle>,
	public IViewObjectExImpl<CImageOle>,
	public IOleInPlaceObjectWindowlessImpl<CImageOle>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CImageOle>,
	public CProxy_IImageOleEvents<CImageOle>,
	public IQuickActivateImpl<CImageOle>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CImageOle>,
#endif
	public IProvideClassInfo2Impl<&CLSID_ImageOle, &__uuidof(_IImageOleEvents), &LIBID_image_oleLib>,
	public CComCoClass<CImageOle, &CLSID_ImageOle>,
	public CComControl<CImageOle>
{
public:
	CImageOle();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IMAGEOLE)


BEGIN_COM_MAP(CImageOle)
	COM_INTERFACE_ENTRY(IImageOle)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IQuickActivate)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CImageOle)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CImageOle)
	CONNECTION_POINT_ENTRY(__uuidof(_IImageOleEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CImageOle)
	CHAIN_MSG_MAP(CComControl<CImageOle>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_IImageOle,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IImageOle
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct();
	void FinalRelease();

public:
	STDMETHOD(LoadFromFile)(BSTR lpszPathName);

private:
	void _UnLoad();
	std::wstring GetShowFileName(HDC hDC, UINT& w, UINT& h);

private:
	void DrawRoundRectange(Graphics &g, Color pens, int x, int y, int width, int height);
	void FillRoundRectangle(Graphics &g, Color color, int x, int y, int width, int height);

private:
	COLORREF		m_clrBackground;
	COLORREF		m_clrText;
	LONG			m_font_size;
	std::wstring	m_font_name;

private:
	HFONT m_hFont;
	std::string* m_BitmapData; // 每一帧的位图数据
	int *m_pFrmIndexes; // 每个100毫秒对应的帧索引
	int m_nFrmCount; // 总帧数
	int m_nTimeCount; // 100毫秒的总数，例如GIF所有帧加起来总的延迟是1秒，则这个值为10
	volatile int m_iPrevFrm; // 前一帧，定时器触发时用于判断是否需要更换帧
	volatile int m_iCurTime; // 当前时间（即第几个100毫秒）

private:
	LONG			m_nFaceId;
	LONG			m_nFaceIndex;
	LONG			m_nScale;//缩放比例，百分制
	std::wstring	m_sFaceTag;
	std::wstring	m_sImageFile;
	std::wstring	m_sGuid;
	GUID			m_guidImageFormat;

	BOOL			m_bScale;
	int			m_nScaleWidth;
	int			m_nScaleHeight;

private:
	int	m_BubbleOffsetX = 15;
	int m_BubbleOffsetY = 15;
	int m_DesEdge = 5;
	int m_DesTextLeft = 10;
	int m_DesTextTop = 5;
	int m_DesImgTop = 10;
	int m_DesImgLeft = 20;
	int m_DesImgRight = 20;
	int m_DesTotalWidth = 300;

public:
	STDMETHOD(SetFaceId)(LONG face_id);
	STDMETHOD(GetFaceId)(LONG* face_id);
	STDMETHOD(SetFaceIndex)(LONG face_index);
	STDMETHOD(GetFaceIndex)(LONG* face_index);
	STDMETHOD(GetImageFile)(BSTR* file_name);
	STDMETHOD(GetImageRect)(LPRECT rect);
	STDMETHOD(GetImageFomat)(GUID* guid);
	STDMETHOD(SaveAsFile)(BSTR file_path);
	STDMETHOD(SetScaleSize)(BOOL bScale, int nWidth, int nHeight);
	STDMETHOD(SetBgColor)(COLORREF clrBg);
	STDMETHOD(Draw)(void);
	STDMETHOD(SetFaceTag)(BSTR face_tag);
	STDMETHOD(GetFaceTag)(BSTR* face_tag);
	STDMETHOD(SetFont)(BSTR font_name, LONG font_size, COLORREF clrText);
	STDMETHOD(SetGUID)(BSTR guid);
	STDMETHOD(GetGUID)(BSTR* guid);
	STDMETHOD(GetScale)(LONG* scale);
	STDMETHOD(LoadFromDescription)(BSTR des);
};

OBJECT_ENTRY_AUTO(__uuidof(ImageOle), CImageOle)
