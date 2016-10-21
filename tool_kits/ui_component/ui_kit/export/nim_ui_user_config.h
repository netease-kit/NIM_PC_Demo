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
	UserConfig(){};
	~UserConfig(){};
public:
	/**
	* 获取用户数据目录位置
	* @return wstring 用户数据目录
	*/
	std::wstring GetUserDataPath();

	/**
	* 获取用户图片数据目录位置
	* @return wstring 用户图片数据目录
	*/
	std::wstring GetUserImagePath();

	/**
	* 获取用户语音数据目录位置
	* @return wstring 用户语音数据目录
	*/
	std::wstring GetUserAudioPath();

	/**
	* 设置应用程序默认显示的图标(各种窗口在任务栏显示的图标)
	* @param[in] nRes 图标资源ID
	* @return void 无返回值
	*/
	void SetDefaultIcon(UINT nRes);

	/**
	* 获取应用程序默认显示的图标
	* @return UINT 图标资源ID
	*/
	UINT GetDefaultIcon();

	/**
	* 获取应用程序版本
	* @param[out] version 版本号
	* @param[out] version_flag 版本标识
	* @return bool true 成功，false 失败
	*/
	bool GetAppLocalVersion(int &version, std::wstring &version_flag);

private:
	UINT icon_id_;
};

}