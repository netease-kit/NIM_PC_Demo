#ifndef UI_RENDER_IRENDER_H_
#define UI_RENDER_IRENDER_H_

#pragma once

namespace ui 
{

class UILIB_API IClip : public nbase::SupportWeakCallback
{
public:
	virtual void CreateClip(HDC hDC, UiRect rc) = 0;
	virtual void CreateRoundClip(HDC hDC, UiRect rc, int width, int height) = 0;
	virtual void ClearClip(HDC hDC) = 0;
};

class IRenderContext;
class UILIB_API AutoClip : public nbase::SupportWeakCallback
{
public:
	AutoClip(IRenderContext* pRender, const UiRect& rc, bool bClip = true);
	AutoClip(IRenderContext* pRender, const UiRect& rcRound, int width, int height, bool bClip = true);
	~AutoClip();

private:
	IRenderContext *m_pRender;
	bool			m_bClip;
};

class UILIB_API IBitmap : public nbase::SupportWeakCallback
{
	virtual void Init(HDC hSrcDC, int width, int height) = 0;
	virtual HBITMAP DetachBitmap() = 0;

	virtual HBITMAP GetBitmap() = 0;
	virtual BYTE* GetBits() = 0;
	virtual int	GetWidth() = 0;
	virtual int GetHeight() = 0;

	virtual void ClearAlpha(const UiRect& rcDirty) = 0;
	virtual void RestoreAlpha(const UiRect& rcDirty, const UiRect& rcShadowPadding) = 0;
};

class UILIB_API IRenderContext : public nbase::SupportWeakCallback
{
public:
	virtual HDC GetDC() = 0;
	virtual bool Resize(int width, int height) = 0;

	virtual HBITMAP DetachBitmap() = 0;
	virtual BYTE* GetBits() = 0;
	virtual int	GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual void ClearAlpha(const UiRect& rcDirty) = 0;
	virtual void RestoreAlpha(const UiRect& rcDirty, const UiRect& rcShadowPadding = UiRect()) = 0;

	virtual bool IsRenderTransparent() const = 0;
	virtual bool SetRenderTransparent(bool bTransparent) = 0;

	virtual CPoint OffsetWindowOrg(CPoint ptOffset) = 0;
	virtual CPoint SetWindowOrg(CPoint ptOffset) = 0;
	virtual CPoint GetWindowOrg() const = 0;

	virtual void SetClip(const UiRect& rc) = 0;
	virtual void SetRoundClip(const UiRect& rcItem, int width, int height) = 0;
	virtual void ClearClip() = 0;

	virtual HRESULT BitBlt(int x, int y, int cx, int cy, HDC hdcSrc, int xSrc = 0, int yScr = 0, DWORD rop = SRCCOPY) = 0;
	virtual bool AlphaBlend(int xDest, int yDest, int widthDest, int heightDest, HDC hdcSrc, int xSrc, int yScr, int widthSrc, int heightSrc, BYTE uFade = 255) = 0;

	virtual void DrawImage(const UiRect& rcPaint, HBITMAP hBitmap, bool bAlphaChannel,
		const UiRect& rcImageDest, const UiRect& rcImageSource, const UiRect& rcCorners, BYTE uFade = 255, bool xtiled = false, bool ytiled = false) = 0;

	virtual void DrawColor(const UiRect& rc, DWORD dwColor, BYTE uFade = 255) = 0;
	virtual void DrawColor(const UiRect& rc, const std::wstring& colorStr, BYTE uFade = 255) = 0;

	virtual void DrawLine(const UiRect& rc, int nSize, DWORD dwPenColor) = 0;
	virtual void DrawRect(const UiRect& rc, int nSize, DWORD dwPenColor) = 0;
	virtual void DrawText(const UiRect& rc, const std::wstring& strText, DWORD dwTextColor, int iFont, UINT uStyle, BYTE uFade = 255, bool bLineLimit = false) = 0;

	virtual UiRect MeasureText(const std::wstring& strText, int iFont, UINT uStyle, int width = DUI_NOSET_VALUE) = 0;
};

} // namespace ui

#endif // UI_RENDER_IRENDER_H_
