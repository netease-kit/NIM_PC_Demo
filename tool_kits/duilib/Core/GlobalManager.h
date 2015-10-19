#ifndef UI_CORE_WINDOWHELPER_H_
#define UI_CORE_WINDOWHELPER_H_

#pragma once


#include "Image.h"
#include "Window.h"

namespace ui {


/////////////////////////////////////////////////////////////////////////////////////
//
typedef Control* (*LPCREATECONTROL)(const std::wstring& pstrType);


class UILIB_API GlobalManager
{
public:
	static void Startup(const std::wstring& resourcePath, const CreateControlCallback& callback);
	static void Shutdown();
	static std::wstring GetCurrentPath();
	static std::wstring GetResourcePath();
	static void SetCurrentPath(const std::wstring& strPath);
	static void SetResourcePath(const std::wstring& strPath);

	static void MessageLoop();
	static bool TranslateMessage(const LPMSG pMsg);
	static std::wstring GetDefaultFontName()
	{
		return m_pStrDefaultFontName;
	}

	static void AddTextColor(const std::wstring& strName, const std::wstring& strValue);
	static std::wstring GetTextColor(const std::wstring& strName);
	static DWORD ConvertTextColor(const std::wstring& strName);

	static void AddClass(const std::wstring& strClassName, const std::wstring& strControlAttrList);
	static std::wstring GetClassAttributes(const std::wstring& strClassName);

	static void AddPreMessage(Window* windowManagerUI)
	{
		m_aPreMessages.push_back(windowManagerUI);
	}

	static void RemovePreMessage(Window* windowManagerUI)
	{
		auto it = std::find(m_aPreMessages.begin(),  m_aPreMessages.end(), windowManagerUI);
		if (it != m_aPreMessages.end())
		{
			m_aPreMessages.erase(it);
		}
	}

	static std::shared_ptr<ImageInfo> IsImageCached(const std::wstring& bitmap);
	static void OnImageInfoDestroy(ImageInfo* imageInfo);
	static std::shared_ptr<ImageInfo> AddImageCached(const std::shared_ptr<ImageInfo>& shared_image);
	static std::shared_ptr<ImageInfo> GetImage(const std::wstring& bitmap);
	static void RemoveFromImageCache(const std::wstring& imageFullPath);

	static HFONT AddFont(const std::wstring& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	static TFontInfo* GetTFontInfo(std::size_t index);
	static HFONT GetFont(std::size_t index);
	static HFONT GetFont(const std::wstring& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	static TFontInfo* GetFontInfo(std::size_t index, HDC hDcPaint);
	static TFontInfo* GetFontInfo(HFONT hFont, HDC hDcPaint);
	static bool FindFont(HFONT hFont);
	static bool FindFont(const std::wstring& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	static bool RemoveFontAt(std::size_t index);
	static void RemoveAllFonts();

	static std::wstring GetDefaultDisabledTextColor();
	static void SetDefaultDisabledTextColor(const std::wstring& dwColor);
	static std::wstring GetDefaultTextColor();
	static void SetDefaultTextColor(const std::wstring& dwColor);
	static DWORD GetDefaultLinkFontColor();
	static void SetDefaultLinkFontColor(DWORD dwColor);
	static DWORD GetDefaultLinkHoverFontColor();
	static void SetDefaultLinkHoverFontColor(DWORD dwColor);
	static DWORD GetDefaultSelectedBkColor();
	static void SetDefaultSelectedBkColor(DWORD dwColor);

	static bool IsOsOverXp();

	static Box* CreateBox(const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());
	static Box* CreateBoxWithCache(const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());

	static void FillBox(Box* userDefinedBox, const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());
	static void FillBoxWithCache(Box* userDefinedBox, const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());

	static Control* CreateControl(const std::wstring& controlStr);

private:
	class ImageCacheKeyCompare
	{
	public:
		// 这个比较函数不是比较字典序的，而是按照以下法则：
		//		首先比较，长度小的更小
		//		再逆向比较，可以认为是逆向字典序
		bool operator()(const std::wstring& key1, const std::wstring& key2) const;
	};
	typedef std::map<std::wstring, std::weak_ptr<ImageInfo>, ImageCacheKeyCompare> MapStringToImagePtr;

	static std::vector<TFontInfo*> m_aCustomFonts;
	static MapStringToImagePtr m_mImageHash;
	static std::vector<Window*> m_aPreMessages;
	static std::map<std::wstring, std::wstring> m_mapTextColor;
	static std::map<std::wstring, std::wstring> m_GlobalClass;
	static std::wstring m_pStrResourcePath;			//全局的资源路径，换肤的时候修改这个变量
	static short m_H;
	static short m_S;
	static short m_L;
	static std::wstring m_pStrDefaultFontName;
	static std::wstring m_dwDefaultDisabledColor;
	static std::wstring m_dwDefaultFontColor;
	static DWORD m_dwDefaultLinkFontColor;
	static DWORD m_dwDefaultLinkHoverFontColor;
	static DWORD m_dwDefaultSelectedBkColor;
	static std::map<std::wstring, std::unique_ptr<WindowBuilder>> m_winowBuilderMap;
	static CreateControlCallback m_createControlCallback;
};


} // namespace ui

#endif // UI_CORE_WINDOWHELPER_H_
