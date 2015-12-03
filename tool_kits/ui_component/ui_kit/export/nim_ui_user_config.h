#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{

/** @class UserConfig
  * @brief 提供用户配置信息获取接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API UserConfig
{
public:
	SINGLETON_DEFINE(UserConfig);
public:
	UserConfig();
	~UserConfig();
public:
	std::wstring GetUserDataPath();
	std::wstring GetUserImagePath();
	std::wstring GetUserAudioPath();

	void SetIcon(UINT nRes);
	UINT GetIcon();

	void Re_GetText(ITextServices * text_service, std::wstring& text);
	bool GetAppLocalVersion(int &version, std::wstring &version_flag);

private:
	UINT icon_id_;
};

}