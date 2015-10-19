#ifndef UI_UTILS_UTILS_H_
#define UI_UTILS_UTILS_H_

#pragma once

#include <oaidl.h> // for VARIANT

namespace ui
{

/////////////////////////////////////////////////////////////////////////////////////
//

class STRINGorID
{
public:
	STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
	{ }
	STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
	{ }
	LPCTSTR m_lpstr;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CPoint : public tagPOINT
{
public:
	CPoint()
	{
		x = y = 0;
	}

	CPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}

	CPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	CPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}

	inline void Offset(int offsetX, int offsetY)
	{
		x += offsetX;
		y += offsetY;
	}

	inline void Offset(CPoint offsetPoint)
	{
		x += offsetPoint.x;
		y += offsetPoint.y;
	}
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CSize : public tagSIZE
{
public:
	CSize()
	{
		cx = cy = 0;
	}

	CSize(const CSize& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CSize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}

	inline void Offset(int offsetCX, int offsetCY)
	{
		cx += offsetCX;
		cy += offsetCY;
	}

	inline void Offset(CSize offsetPoint)
	{
		cx += offsetPoint.cx;
		cy += offsetPoint.cy;
	}
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API UiRect : public tagRECT
{
public:
	inline UiRect()
	{
		left = top = right = bottom = 0;
	}

	inline UiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	inline UiRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

	inline int GetWidth() const
	{
		return right - left;
	}

	inline int GetHeight() const
	{
		return bottom - top;
	}

	inline void Clear()
	{
		left = top = right = bottom = 0;
	}

	inline bool IsRectEmpty() const
	{
		return ::IsRectEmpty(this) == TRUE; 
	}

	inline void ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	inline void Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	inline void Offset(int cx, int cy)
	{
		::OffsetRect(this, cx, cy);
	}

	inline void Offset(const CPoint& offset)
	{
		::OffsetRect(this, offset.x, offset.y);
	}

	inline void Inflate(int cx, int cy)
	{
		::InflateRect(this, cx, cy);
	}

	inline void Inflate(const UiRect& rect)
	{
		this->left -= rect.left;
		this->top -= rect.top;
		this->right += rect.right;
		this->bottom += rect.bottom;
	}

	inline void Deflate(int cx, int cy)
	{
		::InflateRect(this, -cx, -cy);
	}

	inline void Deflate(const UiRect& rect)
	{
		this->left += rect.left;
		this->top += rect.top;
		this->right -= rect.right;
		this->bottom -= rect.bottom;
	}

	inline void Union(const UiRect& rc)
	{
		::UnionRect(this, this, &rc);
	}

	inline void Intersect(const UiRect& rc)
	{
		::IntersectRect(this, this, &rc);
	}

	inline void Subtract(const UiRect& rc)
	{
		::SubtractRect(this, this, &rc);
	}

	inline bool IsPointIn(const CPoint& point) const
	{
		return ::PtInRect(this, point) == TRUE;
	}

	inline bool Equal(const UiRect& rect) const
	{
		return this->left == rect.left && this->top == rect.top 
			&& this->right == rect.right && this->bottom == rect.bottom;
	}
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CVariant : public VARIANT
{
public:
	CVariant() 
	{ 
		VariantInit(this); 
	}
	CVariant(int i)
	{
		VariantInit(this);
		this->vt = VT_I4;
		this->intVal = i;
	}
	CVariant(float f)
	{
		VariantInit(this);
		this->vt = VT_R4;
		this->fltVal = f;
	}
	CVariant(LPOLESTR s)
	{
		VariantInit(this);
		this->vt = VT_BSTR;
		this->bstrVal = s;
	}
	CVariant(IDispatch *disp)
	{
		VariantInit(this);
		this->vt = VT_DISPATCH;
		this->pdispVal = disp;
	}

	~CVariant() 
	{ 
		VariantClear(this); 
	}
};





class PathUtil
{
public:
	static std::wstring GetCurrentModuleDir()
	{
		std::wstring moduleFilePath;
		moduleFilePath.resize(MAX_PATH);
		::GetModuleFileNameW(::GetModuleHandle(NULL), &moduleFilePath[0], moduleFilePath.length());
		return moduleFilePath.substr(0, moduleFilePath.find_last_of(L"\\") + 1);
	}
};


	


}// namespace ui

#endif // UI_UTILS_UTILS_H_