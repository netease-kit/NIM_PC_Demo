#ifndef UI_CORE_RENDER_H_
#define UI_CORE_RENDER_H_

#pragma once

namespace ui 
{

class UILIB_API RenderContext_GdiPlus : public IRenderContext
{
public:
	RenderContext_GdiPlus();
	virtual ~RenderContext_GdiPlus();

	virtual HDC GetDC() override;
	virtual bool Resize(int width, int height) override;

	virtual HBITMAP DetachBitmap() override;
	virtual BYTE* GetBits() override;
	virtual int	GetWidth() override;
	virtual int GetHeight() override;
	virtual void ClearAlpha(const UiRect& rcDirty) override;
	virtual void RestoreAlpha(const UiRect& rcDirty, const UiRect& rcShadowPadding = UiRect()) override;

	virtual bool IsRenderTransparent() const override;
	virtual bool SetRenderTransparent(bool bTransparent) override;

	virtual CPoint OffsetWindowOrg(CPoint ptOffset) override;
	virtual CPoint SetWindowOrg(CPoint ptOffset) override;
	virtual CPoint GetWindowOrg() const override;

	virtual void SetClip(const UiRect& rc) override;
	virtual void SetRoundClip(const UiRect& rc, int width, int height) override;
	virtual void ClearClip() override;

	virtual HRESULT BitBlt(int x, int y, int cx, int cy, HDC hdcSrc, int xSrc = 0, int yScr = 0, DWORD rop = SRCCOPY) override;
	virtual bool AlphaBlend(int xDest, int yDest, int widthDest, int heightDest, HDC hdcSrc, int xSrc, int yScr, int widthSrc, int heightSrc, BYTE uFade = 255) override;

	virtual void DrawImage(const UiRect& rcPaint, HBITMAP hBitmap, bool bAlphaChannel,
		const UiRect& rcImageDest, const UiRect& rcImageSource, const UiRect& rcCorners, BYTE uFade = 255, bool xtiled = false, bool ytiled = false) override;

	virtual void DrawColor(const UiRect& rc, DWORD dwColor, BYTE uFade = 255) override;
	virtual void DrawColor(const UiRect& rc, const std::wstring& colorStr, BYTE uFade = 255) override;

	virtual void DrawLine(const UiRect& rc, int nSize, DWORD dwPenColor) override;
	virtual void DrawRect(const UiRect& rc, int nSize, DWORD dwPenColor) override;
	virtual void DrawText(const UiRect& rc, const std::wstring& strText, DWORD dwTextColor, int iFont, UINT uStyle, BYTE uFade = 255, bool bLineLimit = false) override;

	virtual UiRect MeasureText(const std::wstring& strText, int iFont, UINT uStyle, int width = DUI_NOSET_VALUE) override;

private:
	HDC			m_hDC;
	HBITMAP		m_hOldBitmap;

	bool		m_bTransparent;
	GdiClip		m_clip;
	GdiBitmap	m_bitmap;
};

} // namespace ui

#endif // UI_CORE_RENDER_H_
