// ImageOle.cpp : Implementation of CImageOle
#include "stdafx.h"
#include "ImageOle.h"
#include "json.h"

#pragma comment(lib,"Msimg32.lib") 

static BOOL StringToWString(const std::string &str, std::wstring &wstr)
{
	wstr.clear();
	int length = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
	wstr.resize(length);
	if (wstr.empty())
		return true;
	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		&wstr[0],
		static_cast<int>(wstr.size()));
	return true;
}

static BOOL WStringToString(const std::wstring &wstr, std::string &str)
{
	str.clear();
	int length = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
	str.resize(length);
	if (str.empty())
		return true;
	::WideCharToMultiByte(CP_UTF8,
		0,
		wstr.c_str(),
		static_cast<int>(wstr.size()),
		&str[0],
		static_cast<int>(str.size()),
		NULL,
		NULL);
	return true;
}

// CImageOle
#define MIN_FRM_DELAY	50
#define FILE_NAME_X		10
#define FILE_IMAGE_LEFT	0
#define FILE_IMAGE_TOP	10
#define FILE_IMAGE_RIGHT	10
#define FILE_TEXT_FORMAT	DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL

CImageOle::CImageOle()
{
	m_clrBackground	= RGB(255,255,255);
	m_clrText		= RGB(0,0,0);

	m_BitmapData = NULL;
	m_pFrmIndexes = NULL;
	m_nFrmCount = 0;
	m_nTimeCount = 0;
	m_iPrevFrm = 0;
	m_iCurTime = 0;
	m_nScale = 0;

	m_nFaceIndex		= -1;
	m_nFaceId		= -1;
	m_bScale			= FALSE;
	m_nScaleWidth		= 0;
	m_nScaleHeight	= 0;


	m_font_name = L"新宋体";
	m_font_size = 12;
	m_clrText = RGB(0, 0, 0);
}

HRESULT CImageOle::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT *) di.prcBounds;
	// 将剪辑区域设置为 di.prcBounds 指定的矩形

	// 保存绘制矩形区域，前面保存了容器窗口指针，这里保存了要绘制的矩形区域
	// 在更换帧时就可直接调用m_pContainer->InvalidateRect(&m_rcPos, FALSE)了
	m_rcPos = rc;
	if (m_nFrmCount > 0 && m_BitmapData) {
		HDC hMemDC = ::CreateCompatibleDC(di.hdcDraw);
		if (hMemDC)
		{
			BYTE* bitmap_data = NULL;
			int w = di.ZoomNum.cx;
			int h = di.ZoomNum.cy;
			BITMAPINFO bmi;
			::ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = w;
			bmi.bmiHeader.biHeight = h;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = w * h * 4;
			HBITMAP bitmap = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS,
				(void**)&bitmap_data, NULL, 0);
			memcpy(bitmap_data, m_BitmapData[m_iPrevFrm].c_str(), bmi.bmiHeader.biSizeImage);
			HBITMAP hOldBmps = (HBITMAP)SelectObject(hMemDC, bitmap);
			if (m_nFaceIndex == RE_OLE_TYPE_FILE || m_nFaceIndex == RE_OLE_TYPE_DESCRIPTION)
			{
				BitBlt(di.hdcDraw, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
					hMemDC, 0, 0, SRCCOPY);
			}
			else
			{
				BLENDFUNCTION blend;
				blend.AlphaFormat = AC_SRC_ALPHA;
				blend.BlendFlags = 0;
				blend.BlendOp = AC_SRC_OVER;
				blend.SourceConstantAlpha = 255;
				AlphaBlend(di.hdcDraw, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
					hMemDC, 0, 0, rc.right - rc.left, rc.bottom - rc.top, blend);
			}
			SelectObject(hMemDC, hOldBmps);
			DeleteObject(bitmap);
			::DeleteDC(hMemDC);
		}
	}

	return S_OK;
}

void CImageOle::_UnLoad()
{
	if (m_BitmapData != NULL) {
		delete[] m_BitmapData;
		m_BitmapData = NULL;
	}
	if (m_pFrmIndexes != NULL) {
		delete [] m_pFrmIndexes;
		m_pFrmIndexes = NULL;
	}
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
	m_nFrmCount = 0;
	m_nTimeCount = 0;
	m_iPrevFrm = 0;
	m_iCurTime = 0;
	FinalConstruct();
}

HRESULT CImageOle::FinalConstruct()
{
	return S_OK;
}

void CImageOle::FinalRelease()
{
	_UnLoad();
}

STDMETHODIMP CImageOle::LoadFromFile(BSTR lpszPathName)
{
	ATLASSERT(lpszPathName);
	_UnLoad();
	// 保存容器窗口指针，便于更换帧时调用容器窗口的InvalidateRect函数
	//m_pContainer = pContainer;

	// 加载图像
	Image image_src(lpszPathName);
	m_sImageFile = lpszPathName;
	if (image_src.GetLastStatus() != Gdiplus::Ok) {
		return E_FAIL;
	}
	//字体
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	wcscpy_s(lf.lfFaceName, m_font_name.c_str());
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -m_font_size;
	m_hFont = ::CreateFontIndirect(&lf);

	// 获取总帧数，静态图像，如JPG、PNG等得到的是0，要改为1
	GUID pageGuid = FrameDimensionTime;
	m_nFrmCount = image_src.GetFrameCount(&pageGuid);
	if (m_nFrmCount == 0) {
		m_nFrmCount = 1;
	}

	// 得到图像尺寸
	UINT w = image_src.GetWidth();
	UINT h = image_src.GetHeight();
	UINT image_w = w;
	UINT image_h = h;
	if (w != 0 && h != 0)
	{
		if (m_bScale && m_nScaleWidth > 0)
		{
			image_h = m_nScaleHeight <= 0 ? h * m_nScaleWidth / w : m_nScaleHeight;
			image_w = m_nScaleWidth;
		}
		else if (m_nScaleWidth > 0 || m_nScaleHeight > 0)
		{
			UINT h_temp = h;
			UINT w_temp = w;
			if (m_nScaleWidth < (int)w_temp)
			{
				h_temp = h_temp * m_nScaleWidth / w_temp;
				w_temp = m_nScaleWidth;
			}
			if (m_nScaleHeight < (int)h_temp)
			{
				w_temp = w_temp * m_nScaleHeight / h_temp;
				h_temp = m_nScaleHeight;
			}
			image_w = w_temp==0?1:w_temp;
			image_h = h_temp==0?1:h_temp;
		}
	}
	if (w > 0 && image_w > 0)
	{
		m_nScale = image_w*100 / w;
		if (m_nScale == 0)
		{
			m_nScale = 1;
		}
	}
	

	w = image_w;
	h = image_h;
	HDC hDc = ::GetDC(NULL);
	std::wstring file_info;
	UINT text_h = h;
	UINT text_w = m_nScaleWidth - w - FILE_NAME_X;
	if (m_nFaceIndex == RE_OLE_TYPE_FILE && m_nScaleWidth - w > 20)
	{
		file_info = GetShowFileName(hDc, text_w, text_h);
		if (file_info.size() > 0)
		{
			w = image_w + text_w + FILE_NAME_X;
		}
	}
	UINT image_left = 0;
	UINT image_top = 0;
	UINT image_right = 0;

	if (m_nFaceIndex >= RE_OLE_TYPE_IMAGE)
	{
		image_left = FILE_IMAGE_LEFT;
		image_top = FILE_IMAGE_TOP;
		image_right = FILE_IMAGE_RIGHT;
	}
	w += image_left + image_right;
	h += image_top;
	

	// 转化成HIMETRIC，即0.01毫米单位的尺寸
	// 设置控件尺寸
	SIZEL sizel;
	sizel.cx = w;
	sizel.cy = h;
	AtlPixelToHiMetric(&sizel, &m_sizeExtent);
	m_sizeNatural = m_sizeExtent;

	if (m_nFrmCount > 1) { // 总帧数超过1时

		// 得到各帧的延迟时间
		int nSize = image_src.GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *pItem = (PropertyItem *) new BYTE[nSize];
		image_src.GetPropertyItem(PropertyTagFrameDelay, nSize, pItem);
		LONG *pDelays = (LONG *) pItem->value;

		// 计算总的延迟时间有几个100毫秒
		for (int i = 0; i < m_nFrmCount; i++) {
			// 得到的延迟时间单位是10毫秒，乘以10转换成毫秒
			// 再使用四舍五入的方法得到有几个100毫秒。
			// 例如这个值是35，那么就是35个10毫秒，乘以10得350毫秒，再四舍五入，得到4
			pDelays[i] = ((pDelays[i] * 10) + (MIN_FRM_DELAY / 2)) / MIN_FRM_DELAY;
			LONG delays = pDelays[i];
			if (delays == 0)
			{
				pDelays[i] = 2;
			}
			
			m_nTimeCount += pDelays[i];
		}

		// 再得到每个100毫秒对应的帧索引
		// 例如第1帧200毫秒，第2帧300毫秒，那么
		// m_pFrmIndexes[0] = 0
		// m_pFrmIndexes[1] = 0
		// m_pFrmIndexes[2] = 1
		// m_pFrmIndexes[3] = 1
		// m_pFrmIndexes[4] = 1
		// 这样就可通过这个数组得到当前这100毫秒应该绘制哪一帧
		m_pFrmIndexes = new int[m_nTimeCount];
		for (int i = 0, j = 0; i < m_nFrmCount; i++) {
			for (int k = 0; k < pDelays[i]; k++) {
				m_pFrmIndexes[j++] = i;
			}
		}
		// new出来的内存别忘了delete
		delete [] (BYTE *) pItem;
	} else { // 帧数为1时
		// 只是便于绘制，也为m_pFrmIndexes分配一个单元的空间，里面存0就行了
		m_nTimeCount = 1;
		m_pFrmIndexes = new int[1];
		*m_pFrmIndexes = 0;
	}

	// 创建每一帧的缓存位图
	m_BitmapData = new std::string[m_nFrmCount];
	//缓存DC
	HDC hMemDC = ::CreateCompatibleDC(hDc);
	for (int i = 0; i < m_nFrmCount; i++) {
		BYTE* bitmap_data = NULL;
		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = w;
		bmi.bmiHeader.biHeight = h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = w * h * 4;
		HBITMAP bitmap = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS,
			(void**)&bitmap_data, NULL, 0);
		HBITMAP hOldBmps = (HBITMAP)SelectObject(hMemDC, bitmap);
		// 由于InvalidateRect时指定了不擦除背景，如果使用透明背景进行绘制
		// 那么透明部分还会保留前面帧所绘制的内容，出现重影，所以填充白色背景
		// 来解决这个问题

		if (m_nFaceIndex == RE_OLE_TYPE_FILE)
		{
			HBRUSH hBrush = CreateSolidBrush(m_clrBackground);
			if (hBrush)
			{
				RECT rect = {0, 0, w, h};
				FillRect(hMemDC, &rect, hBrush);
				DeleteObject(hBrush);
			}
		}

		// 绘制当前帧
		image_src.SelectActiveFrame(&pageGuid, i);
		Graphics g(hMemDC);
		g.SetSmoothingMode(SmoothingModeHighQuality);
		g.DrawImage(&image_src, (int)image_left, (int)image_top, image_w, image_h);

		if (file_info.size() > 0)
		{
			::SetBkMode(hMemDC, TRANSPARENT);
			::SetTextColor(hMemDC, m_clrText);
			HFONT hOldFont = NULL;
			if (m_hFont)
			{
				hOldFont = (HFONT)::SelectObject(hMemDC, m_hFont);
			}
			int top = (h-text_h+image_top)/2;
			RECT rc = {image_left + image_w + FILE_NAME_X, top,0,0};
			rc.right = rc.left + text_w;
			rc.bottom = rc.top + text_h;
			::DrawText(hMemDC, file_info.c_str(), -1, &rc, FILE_TEXT_FORMAT);
			if (m_hFont)
			{
				::SelectObject(hMemDC, hOldFont);
			}
		}
		SelectObject(hMemDC, hOldBmps);
		m_BitmapData[i].append((char*)bitmap_data, bmi.bmiHeader.biSizeImage);
		DeleteObject(bitmap);
	}
	::DeleteDC(hMemDC);
	::ReleaseDC(NULL, hDc);
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
	return S_OK;
}

STDMETHODIMP CImageOle::SetFaceId(LONG face_id)
{
	m_nFaceId = face_id;
	return S_OK;
}

STDMETHODIMP CImageOle::GetFaceId(LONG* face_id)
{
	*face_id = m_nFaceId;
	return S_OK;
}

STDMETHODIMP CImageOle::SetFaceIndex(LONG face_index)
{
	m_nFaceIndex = face_index;
	return S_OK;
}

STDMETHODIMP CImageOle::GetFaceIndex(LONG* face_index)
{
	*face_index = m_nFaceIndex;
	return S_OK;
}

STDMETHODIMP CImageOle::GetImageFile(BSTR* file_name)
{
	*file_name = (BSTR)m_sImageFile.c_str();
	return S_OK;
}

STDMETHODIMP CImageOle::GetImageRect(LPRECT rect)
{
	*rect = m_rcPos;
	m_rcPos.left = 0;
	m_rcPos.right = 0;
	m_rcPos.top = 0;
	m_rcPos.bottom = 0;
	return S_OK;
}

STDMETHODIMP CImageOle::GetImageFomat(GUID* guid)
{
	*guid = m_guidImageFormat;
	return S_OK;
}

STDMETHODIMP CImageOle::SaveAsFile(BSTR file_path)
{
	file_path;
	//if (m_pImage)
	//{
	//	m_pImage->Save(file_path, &m_guidImageFormat);
	//	return S_OK;
	//}
	return E_FAIL;
}

STDMETHODIMP CImageOle::SetScaleSize(BOOL bScale, int nWidth, int nHeight)
{
	m_bScale = bScale;
	m_nScaleWidth = nWidth;
	m_nScaleHeight = nHeight;
	return S_OK;
}

STDMETHODIMP CImageOle::SetBgColor(COLORREF clrBg)
{
	m_clrBackground = clrBg;
	return E_FAIL;
}


STDMETHODIMP CImageOle::Draw(void)
{
	if (m_nFrmCount > 1) {
		// 增加当前时间
		++m_iCurTime;
		// 超过总的延迟时间时，又从0开始
		if (m_iCurTime >= m_nTimeCount) {
			m_iCurTime = 0;
		}
		// 当前时间对应的帧索引与上一次绘制的帧索引不同时才需要重绘
		if (m_pFrmIndexes[m_iCurTime] != m_iPrevFrm) {
			//m_pContainer->InvalidateRect(&m_rcPos, FALSE);
			// 更新上一次绘制的帧索引为当前的帧索引
			m_iPrevFrm = m_pFrmIndexes[m_iCurTime];
			return S_OK;
		}
	}
	return E_FAIL;
}


STDMETHODIMP CImageOle::SetFaceTag(BSTR face_tag)
{
	// TODO: 在此添加实现代码
	m_sFaceTag = face_tag;
	return S_OK;
}


STDMETHODIMP CImageOle::GetFaceTag(BSTR* face_tag)
{
	// TODO: 在此添加实现代码
	*face_tag = (BSTR)m_sFaceTag.c_str();
	return S_OK;
}
std::wstring CImageOle::GetShowFileName(HDC hDC, UINT& w, UINT& h)
{
	std::wstring file_name_ret;
	std::wstring file_name, file_exten;
	file_name = m_sFaceTag;
	int pos1 = (int)file_name.rfind(L'\\');
	int pos2 = (int)file_name.rfind(L'/');
	if (pos1 > 0 || pos2 > 0)
	{
		file_name = file_name.substr(max(pos1, pos2)+1, file_name.size());
	}
	file_name_ret = file_name;
	int pos3 = (int)file_name.rfind(L'.');
	if (pos3 > 0)
	{
		file_exten = file_name.substr(pos3, file_name.size());
		file_name = file_name.substr(0, pos3);
	}

	if (file_name_ret.size() > 0)
	{
		HFONT hOldFont = NULL;
		if (m_hFont)
		{
			hOldFont = (HFONT)::SelectObject(hDC, m_hFont);
		}
		int name_max_length = min(30, (int)file_name_ret.size());
		RECT rc = {0,0,w,h+100};
		::DrawText(hDC, file_name_ret.c_str(), -1, &rc, DT_CALCRECT | FILE_TEXT_FORMAT);
		bool two_line = (rc.bottom > (int)h/2);
		while ((rc.bottom > (int)h || rc.right > (int)w)&& name_max_length > 0)
		{
			file_name_ret = file_name.substr(0, --name_max_length);
			if (file_exten.size() < 8)
			{
				file_name_ret = file_name_ret + L".." + file_exten;
			} 
			else
			{
				file_name_ret = file_name_ret + L"..." ;
			}
			rc.bottom = h+1000;
			::DrawText(hDC, file_name_ret.c_str(), -1, &rc, DT_CALCRECT | FILE_TEXT_FORMAT);
		}
		UINT w_temp = rc.right;
		UINT h_temp = rc.bottom;
		if (name_max_length < 1)
		{
			file_name_ret.clear();
		}
		if (file_name_ret.size() > 0 && two_line)
		{
			do
			{
				w_temp = rc.right;
				h_temp = rc.bottom;
				rc.right = w_temp-1;
				rc.bottom = h+1000;
				::DrawText(hDC, file_name_ret.c_str(), -1, &rc, DT_CALCRECT | FILE_TEXT_FORMAT);
			}
			while ((rc.bottom <= (int)h && rc.right < (int)w_temp)&& name_max_length > 0 && rc.right > 0);
		}
		if (m_hFont)
		{
			::SelectObject(hDC, hOldFont);
		}
		w = w_temp;
		h = h_temp;
	}
	return file_name_ret;
}


STDMETHODIMP CImageOle::SetFont(BSTR font_name, LONG font_size, COLORREF clrText)
{
	// TODO: 在此添加实现代码
	m_font_name = font_name;
	m_font_size = font_size;
	m_clrText = clrText;
	return S_OK;
}


STDMETHODIMP CImageOle::SetGUID(BSTR guid)
{
	// TODO: 在此添加实现代码
	m_sGuid = guid;
	return S_OK;
}


STDMETHODIMP CImageOle::GetGUID(BSTR* guid)
{
	// TODO: 在此添加实现代码
	*guid = (BSTR)m_sGuid.c_str();
	return S_OK;
}


STDMETHODIMP CImageOle::GetScale(LONG* scale)
{
	// TODO: 在此添加实现代码
	*scale = m_nScale;
	return S_OK;
}

static void ScaleImage(UINT& width, UINT& height, const int scale_width, const int scale_height)
{
	UINT image_w = width;
	UINT image_h = height;
	if (image_w > 0)
	{
		image_h = scale_height <= 0 ? image_h * scale_width / image_w : scale_height;
		image_w = scale_width;
	}
	else if (scale_width > 0 || scale_height > 0)
	{
		UINT h_temp = image_h;
		UINT w_temp = image_w;
		if (scale_width < (int)w_temp)
		{
			h_temp = h_temp * scale_width / w_temp;
			w_temp = scale_width;
		}
		if (scale_height < (int)h_temp)
		{
			w_temp = w_temp * scale_height / h_temp;
			h_temp = scale_height;
		}
		image_w = w_temp == 0 ? 1 : w_temp;
		image_h = h_temp == 0 ? 1 : h_temp;
	}
	width = image_w;
	height = image_h;
}

void CImageOle::DrawRoundRectange(Graphics &g, Color pens, int x, int y, int width, int height)
{
	//设置画图时的滤波模式为消除锯齿现象  
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	//创建一个红色的画笔  
	Pen *pen = new Pen(pens, 1);
	//画矩形上面的边  
	g.DrawLine(pen, x + m_BubbleOffsetX, y, x + width - m_BubbleOffsetX, y);
	//画矩形下面的边  
	g.DrawLine(pen, x + m_BubbleOffsetX, y + height, x + width - m_BubbleOffsetX, y + height);
	//画矩形左面的边  
	g.DrawLine(pen, x, y + m_BubbleOffsetY, x, y + height - m_BubbleOffsetY);
	//画矩形右面的边  
	g.DrawLine(pen, x + width, y + m_BubbleOffsetY, x + width, y + height - m_BubbleOffsetY);
	//画矩形左上角的圆角  
	g.DrawArc(pen, x, y, m_BubbleOffsetX * 2, m_BubbleOffsetY * 2, 180, 90);
	//画矩形右下角的圆角  
	g.DrawArc(pen, x + width - m_BubbleOffsetX * 2, y + height - m_BubbleOffsetY * 2, m_BubbleOffsetX * 2, m_BubbleOffsetY * 2, 360, 90);
	//画矩形右上角的圆角  
	g.DrawArc(pen, x + width - m_BubbleOffsetX * 2, y, m_BubbleOffsetX * 2, m_BubbleOffsetY * 2, 270, 90);
	//画矩形左下角的圆角  
	g.DrawArc(pen, x, y + height - m_BubbleOffsetY * 2, m_BubbleOffsetX * 2, m_BubbleOffsetY * 2, 90, 90);
	delete pen;
}

/* 填充颜色 */
void CImageOle::FillRoundRectangle(Graphics &g, Color color, int x, int y, int width, int height)
{
	//矩形填充的步骤：
	//1、把圆角矩形画分为四个圆角上分成四个同等的扇形外加三个直角矩形
	//2、先填充三个直角矩形
	//3、然后填充四个角上的扇形

	//创建画刷
	Brush *brush = new SolidBrush(color);

	//填充三个直角矩形
	g.FillRectangle(brush, x, y + m_BubbleOffsetY, width, height - m_BubbleOffsetY * 2);
	g.FillRectangle(brush, x + m_BubbleOffsetX, y, width - m_BubbleOffsetX * 2, height);

	//填充四个角上的扇形区
	//填充左上角扇形
#define PIE_OFFSET 2
	g.FillPie(brush, x, y, m_BubbleOffsetX * 2 + PIE_OFFSET, m_BubbleOffsetY * 2 + PIE_OFFSET, 180, 90);
	//填充右下角的扇形
	g.FillPie(brush, x + width - (m_BubbleOffsetX * 2 + PIE_OFFSET), y + height - (m_BubbleOffsetY * 2 + PIE_OFFSET), m_BubbleOffsetX * 2 + PIE_OFFSET, m_BubbleOffsetY * 2 + PIE_OFFSET, 360, 90);
	//填充右上角的扇形
	g.FillPie(brush, x + width - (m_BubbleOffsetX * 2 + PIE_OFFSET), y, (m_BubbleOffsetX * 2 + PIE_OFFSET), (m_BubbleOffsetY * 2 + PIE_OFFSET), 270, 90);
	//填充左下角的扇形
	g.FillPie(brush, x, y + height - (m_BubbleOffsetX * 2 + PIE_OFFSET), (m_BubbleOffsetX * 2 + PIE_OFFSET), (m_BubbleOffsetY * 2 + PIE_OFFSET), 90, 90);
	delete brush;
}

STDMETHODIMP CImageOle::LoadFromDescription(BSTR des)
{
	// TODO:  在此添加实现代码
	ATLASSERT(des);

	Json::Reader reader;
	Json::Value values;
	std::string description;
	WStringToString(des, description);
	if (reader.parse(description, values) && values.isObject())
	{
		//字体，暂时用全局字体
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		wcscpy_s(lf.lfFaceName, m_font_name.c_str());
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -m_font_size;
		m_hFont = ::CreateFontIndirect(&lf);

		Json::Value draw_data;

		Json::Value &config = values["config"];
		if (!config.isNull())
		{
			if (config.isMember("bubble_offset_x"))
				m_BubbleOffsetX = config["bubble_offset_x"].asInt();
			if (config.isMember("bubble_offset_y"))
				m_BubbleOffsetY = config["bubble_offset_y"].asInt();
			if (config.isMember("line_space"))
				m_DesEdge = config["line_space"].asInt();
			if (config.isMember("text_left"))
				m_DesTextLeft = config["text_left"].asInt();
			if (config.isMember("text_top"))
				m_DesTextTop = config["text_top"].asInt();
			if (config.isMember("img_top"))
				m_DesImgTop = config["img_top"].asInt();
			if (config.isMember("img_left"))
				m_DesImgLeft = config["img_left"].asInt();
			if (config.isMember("img_right"))
				m_DesImgRight = config["img_right"].asInt();
			if (config.isMember("total_width"))
				m_DesTotalWidth = config["total_width"].asInt();
		}
		UINT max_width = m_DesTotalWidth;

		Json::Value &content = values["content"];
		HDC hDC = ::GetDC(NULL);
		UINT total_w = 0;
		UINT total_h = m_DesEdge;
		int obj_len = content.size();
		for (int i = 0; i < obj_len; i++)
		{
			Json::Value &obj = content[i];
			std::string obj_type = obj["type"].asString();
			if (obj_type == "img")
			{
				std::wstring file_path;
				StringToWString(obj["file"].asString(), file_path);
				Image image(file_path.c_str());
				if (image.GetLastStatus() != Gdiplus::Ok)
					continue;
				UINT w = image.GetWidth();
				UINT h = image.GetHeight();
				int scale_width = obj["width"].asInt();
				int scale_height = obj["height"].asInt();
				if (scale_width > 0 || scale_height > 0)
				{
					ScaleImage(w, h, scale_width, scale_height);
				}
				obj["draw_width"] = w;
				obj["draw_height"] = h;
				obj["draw_left"] = m_DesImgLeft + (i == 0 ? m_DesEdge : 0);
				obj["draw_top"] = total_h + m_DesImgTop + (i == 0 ? m_DesEdge : 0);
				UINT image_w = m_DesImgLeft + w + m_DesImgRight;
				if (image_w > total_w)
					total_w = image_w;
				if (total_w > max_width)
					max_width = total_w;
				draw_data.append(obj);
				total_h = total_h + m_DesImgTop + h;
			}
			else if (obj_type == "text")
			{
				std::wstring content;
				StringToWString(obj["content"].asString(), content);
				HFONT hOldFont = NULL;
				if (m_hFont)
				{
					hOldFont = (HFONT)::SelectObject(hDC, m_hFont);
				}
				UINT width = max_width;
				RECT rc = { 0, 0, width, 999 };
				::DrawText(hDC, content.c_str(), -1, &rc, DT_CALCRECT | FILE_TEXT_FORMAT | DT_VCENTER);
				UINT text_width = rc.right + m_DesTextLeft * 2/*> 40 ? rc.right : 40*/;
				UINT text_height = rc.bottom + m_DesTextTop * 2/*> 30 ? rc.bottom : 30*/;
 				obj["draw_width"] = text_width;
				obj["draw_height"] = text_height;
				obj["draw_left"] = m_DesTextLeft + (i == 0 ? m_DesEdge : 0);
				obj["draw_top"] = total_h + m_DesTextTop + (i == 0 ? m_DesEdge : 0);
				draw_data.append(obj);
				text_width = m_DesTextLeft + text_width;
				if (text_width > total_w)
					total_w = text_width;
				if (total_w > max_width)
					max_width = total_w;
				total_h = total_h + text_height;
			}
		}
		
		total_h += m_DesEdge;
		total_w += m_DesEdge * 2;

		// 转化成HIMETRIC，即0.01毫米单位的尺寸
		// 设置控件尺寸
		SIZEL sizel;
		sizel.cx = total_w;
		sizel.cy = total_h;
		AtlPixelToHiMetric(&sizel, &m_sizeExtent);
		m_sizeNatural = m_sizeExtent;

		//只支持静态图片
		m_nFrmCount = 1;
		// 创建每一帧的缓存位图
		m_BitmapData = new std::string[m_nFrmCount];
		//缓存DC
		HDC hMemDC = ::CreateCompatibleDC(hDC);

		BYTE* bitmap_data = NULL;
		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = total_w;
		bmi.bmiHeader.biHeight = total_h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = total_w * total_h * 4;
		HBITMAP bitmap = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS,
			(void**)&bitmap_data, NULL, 0);
		HBITMAP hOldBmps = (HBITMAP)SelectObject(hMemDC, bitmap);
		// 由于InvalidateRect时指定了不擦除背景，如果使用透明背景进行绘制
		// 那么透明部分还会保留前面帧所绘制的内容，出现重影，所以填充白色背景
		// 来解决这个问题

		HBRUSH hBrush = CreateSolidBrush(m_clrBackground);
		if (hBrush)
		{
			RECT rect = { 0, 0, total_w, total_h };
			FillRect(hMemDC, &rect, hBrush);
			DeleteObject(hBrush);
		}

		Graphics g(hMemDC);
		FillRoundRectangle(g, Color(255, 255, 255), m_DesEdge, m_DesEdge, total_w - m_DesEdge * 2, total_h - m_DesEdge * 2);
		DrawRoundRectange(g, Color(207, 212, 217), m_DesEdge, m_DesEdge, total_w - m_DesEdge * 2, total_h - m_DesEdge * 2);

		obj_len = draw_data.size();
		g.SetSmoothingMode(SmoothingModeHighQuality);
		for (int i = 0; i < obj_len; i++)
		{
			Json::Value obj = draw_data[i];
			std::string obj_type = obj["type"].asString();
			if (obj_type == "img")
			{
				std::wstring file_path;
				StringToWString(obj["file"].asString(), file_path);
				Image image(file_path.c_str());
				GUID pageGuid = FrameDimensionTime;
				image.GetFrameCount(&pageGuid);
				image.SelectActiveFrame(&pageGuid, 0);
				int left = obj["draw_left"].asInt();
				int top = obj["draw_top"].asInt();
				int width = obj["draw_width"].asInt();
				int height = obj["draw_height"].asInt();
				g.DrawImage(&image, left, top, width, height);
			}
			else if (obj_type == "text")
			{
				::SetBkMode(hMemDC, TRANSPARENT);
				::SetTextColor(hMemDC, m_clrText);
				std::wstring content;
				StringToWString(obj["content"].asString(), content);
				HFONT hOldFont = NULL;
				if (m_hFont)
				{
					hOldFont = (HFONT)::SelectObject(hDC, m_hFont);
				}
				int left = obj["draw_left"].asInt();
				int top = obj["draw_top"].asInt();
				int width = obj["draw_width"].asInt();
				int height = obj["draw_height"].asInt();
				RECT rc = { left, top, left + width, top + height };
				::DrawText(hMemDC, content.c_str(), -1, &rc, FILE_TEXT_FORMAT | DT_VCENTER);
				if (m_hFont)
				{
					::SelectObject(hMemDC, hOldFont);
				}
			}	
		}
		SelectObject(hMemDC, hOldBmps);
		m_BitmapData[0].append((char*)bitmap_data, bmi.bmiHeader.biSizeImage);
		DeleteObject(bitmap);

		::DeleteDC(hMemDC);
		::ReleaseDC(NULL, hDC);
		if (m_hFont)
		{
			DeleteObject(m_hFont);
			m_hFont = NULL;
		}
	}

	return S_OK;
}
