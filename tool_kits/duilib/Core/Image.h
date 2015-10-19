#ifndef UI_CORE_IMAGEDECODE_H_
#define UI_CORE_IMAGEDECODE_H_

#pragma once

#include <GdiPlus.h>
#include "Utils/Utils.h"

namespace ui {

class UILIB_API ImageInfo
{
public:
	ImageInfo();
	~ImageInfo();
	
	int nX;
	int nY;
	std::wstring sImageFullPath;


	void SetAlpha(bool bAlphaChannel) {
		m_bAlphaChannel = bAlphaChannel;
	}

	bool IsAlpha() {
		return m_bAlphaChannel;
	}

	void SetPropertyItem(Gdiplus::PropertyItem* propertyItem) {
		m_propertyItem.reset(propertyItem);
	}

	void PushBackHBitmap(HBITMAP hBitmap) {
		m_vecBitmap.push_back(hBitmap);
	}

	HBITMAP GetHBitmap(int index) {
		return m_vecBitmap[index];
	}

	int GetFrameCount() {
		return m_vecBitmap.size();
	}

	int IsGif() {
		return m_vecBitmap.size() > 1;
	}

	//ºÁÃëÎªµ¥Î» 
	int GetInterval(int index);
	 
	static std::unique_ptr<ImageInfo> LoadImage(const std::wstring& imageFullPath);

	bool IsCached()
	{
		return m_bCached;
	}
	void SetCached(bool bCached)
	{
		m_bCached = bCached;
	}
private:
	bool m_bAlphaChannel;
	bool m_bCached;
	std::unique_ptr<Gdiplus::PropertyItem> m_propertyItem;
	std::vector<HBITMAP> m_vecBitmap;
};


struct UILIB_API ImageAttribute
{
public:
	ImageAttribute()
	{
		Init();
	}

	void Init()
	{
		imageString.clear();
		sImageName.clear();
		bFade = 0xFF;
		bTiledX = false;
		bTiledY = false;
		rcDest.left = rcDest.top = rcDest.right = rcDest.bottom = DUI_NOSET_VALUE;
		rcSource.left = rcSource.top = rcSource.right = rcSource.bottom = DUI_NOSET_VALUE;
		rcCorner.left = rcCorner.top = rcCorner.right = rcCorner.bottom = 0;
	}

	void SetImageString (const std::wstring& imageStr);
	static void ModifyAttribute(ImageAttribute& imageAttribute, const std::wstring& imageStr);

	std::wstring imageString;
	std::wstring sImageName;
	UiRect rcDest;
	UiRect rcSource;
	UiRect rcCorner;
	BYTE bFade;
	bool bTiledX;
	bool bTiledY;
};


class UILIB_API Image
{
public:
	Image()
		: imageAttribute(),
		m_iCurrentFrame(0),
		m_bPlaying(false),
		imageCache()
	{

	}

	bool IsValid()
	{
		return (bool)imageCache;
	}

	void SetImageString(const std::wstring& imageString)
	{
		ClearCache();
		imageAttribute.SetImageString(imageString);
	}

	void ClearCache()
	{
		m_iCurrentFrame = 0;
		m_bPlaying = false;
		imageCache.reset();
	}

	bool IncrementCurrentFrame() {
		if (!imageCache) {
			return false;
		}
		m_iCurrentFrame++;
		if (m_iCurrentFrame == imageCache->GetFrameCount()) {
			m_iCurrentFrame = 0;
		}
		return true;
	}

	void SetCurrentFrame(int iCurrentFrame) {
		m_iCurrentFrame = iCurrentFrame;
	}

	bool IsPlaying()
	{
		return m_bPlaying;
	}

	void SetPlaying(bool playing)
	{
		m_bPlaying = playing;
	}

	HBITMAP GetCurrentHBitmap() {
		if (!imageCache) {
			return NULL;
		}
		return imageCache->GetHBitmap(m_iCurrentFrame);
	}

	int GetCurrentInterval() {
		if (!imageCache) {
			return 0;
		}
		return imageCache->GetInterval(m_iCurrentFrame);
	}

	ImageAttribute imageAttribute;
	int m_iCurrentFrame;
	bool m_bPlaying;
	std::shared_ptr<ImageInfo> imageCache;
};

class UILIB_API StateImage
{
public:
	StateImage() :
		m_pControl(nullptr),
		m_stateImageMap()
	{

	}

	void SetControl(Control* control)
	{
		m_pControl = control;
	}

	Image& operator[](ControlStateType stateType)
	{
		return m_stateImageMap[stateType];
	}

	bool PaintStatusImage(HDC hDC, ControlStateType stateType, const std::wstring& m_sImageModify = L"");
	Image* GetEstimateImage();

	void ClearCache();

private:
	Control* m_pControl;
	std::map<ControlStateType, Image> m_stateImageMap;
};

class UILIB_API StateImageMap
{
public:
	StateImageMap()
	{

	}

	void SetControl(Control* control);

	void SetImage(StateImageType stateImageType, ControlStateType stateType, const std::wstring& image_path)
	{
		m_stateImageMap[stateImageType][stateType].SetImageString(image_path);
	}

	std::wstring GetImagePath(StateImageType stateImageType, ControlStateType stateType)
	{
		return m_stateImageMap[stateImageType][stateType].imageAttribute.imageString;
	}

	bool PaintStatusImage(HDC hDC, StateImageType stateImageType, ControlStateType stateType, const std::wstring& m_sImageModify = L"");
	Image* GetEstimateImage(StateImageType stateImageType);

	void ClearCache();

private:
	std::map<StateImageType, StateImage> m_stateImageMap;
};

class UILIB_API StateColorMap
{
public:
	StateColorMap() :
		m_pControl(nullptr),
		m_stateColorMap()
	{

	}

	void SetControl(Control* control)
	{
		m_pControl = control;
	}

	std::wstring& operator[](ControlStateType stateType)
	{
		return m_stateColorMap[stateType];
	}

	void PaintStatusColor(HDC hDC, UiRect rcPaint, ControlStateType stateType);

private:
	Control* m_pControl;
	std::map<ControlStateType, std::wstring> m_stateColorMap;
};


} // namespace ui

#endif // UI_CORE_IMAGEDECODE_H_
