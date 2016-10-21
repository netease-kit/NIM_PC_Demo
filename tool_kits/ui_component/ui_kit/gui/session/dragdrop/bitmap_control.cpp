#include "bitmap_control.h"

using namespace ui;
namespace nim_comp
{
BitmapControl::BitmapControl()
{
	bitmap_ = NULL;
}

void BitmapControl::Paint(HDC hDC, const UiRect& rcPaint)
{
	if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return;
	__super::Paint(hDC, rcPaint);

	if (NULL == bitmap_)
		return;

	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, bitmap_);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	::AlphaBlend(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, hCloneDC,
		0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, bf);

	::SelectObject(hCloneDC, hOldBitmap);
	::DeleteDC(hCloneDC);
}

void BitmapControl::SetBitmapImage(HBITMAP bitmap)
{
	bitmap_ = bitmap;
}
}