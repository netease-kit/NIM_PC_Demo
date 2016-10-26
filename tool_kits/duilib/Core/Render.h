#ifndef UI_CORE_RENDER_H_
#define UI_CORE_RENDER_H_

#pragma once

namespace ui {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API RenderClip
{
public:
	RenderClip() :
	  m_hDC(NULL),
	  m_hRgn(NULL),
	  m_hOldRgn(NULL),
	  m_bClip(true)
	{

	}

    ~RenderClip();
    HDC m_hDC;
    HRGN m_hRgn;
    HRGN m_hOldRgn;
	bool m_bClip;

    void GenerateClip(HDC hDC, UiRect rc, bool clip = true);
    void GenerateRoundClip(HDC hDC, UiRect rcItem, int width, int height, bool clip = true);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API RenderEngine
{
public:
	static HBITMAP CreateDIBBitmap(HDC hdc, int width, int height, LPVOID* pBits);

	static void GdiDrawImage(HDC hDC, bool bCurCanvasTransparent, const UiRect& rcPaint, HBITMAP hBitmap, bool alphaChannel, 
		const UiRect& rcImageDest, const UiRect& rcImageSource, const UiRect& rcCorners, BYTE uFade = 255, bool xtiled = false, bool ytiled = false);

    static void DrawColor(HDC hDC, const UiRect& rc, DWORD color, BYTE uFade = 255);
	static void DrawColor(HDC hDC, const UiRect& rc, const std::wstring& colorStr, BYTE uFade = 255);

    static void DrawLine(HDC hDC, const UiRect& rc, int nSize, DWORD dwPenColor);
    static void DrawRect(HDC hDC, const UiRect& rc, int nSize, DWORD dwPenColor);
	static void DrawText(HDC hDC, const UiRect& rc, const std::wstring& strText, DWORD dwTextColor, int iFont, UINT uStyle, BYTE uFade = 255, bool bLineLimit = false);

	static UiRect MeasureText(HDC hDC, const std::wstring& strText, int iFont, UINT uStyle, int width = DUI_NOSET_VALUE);
};

} // namespace ui

#endif // UI_CORE_RENDER_H_
