#pragma once
#include "base/memory/singleton.h"

/** @class ConfigHelper
  * @brief 配置信息读写
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @date 2017/03/01
  */
class ConfigHelper
{
public:
	SINGLETON_DEFINE(ConfigHelper);
	ConfigHelper();
	virtual ~ConfigHelper();

public:

	/**
	* 设置DPI适配配置信息
	* @param[in] adapt_dpi 是否适配DPI
	* @return void 无返回值
	*/
	void SetAdaptDpi(bool adapt_dpi);

	/**
	* 获取DPI适配配置信息
	* @return bool true 适配，false 不适配
	*/
	bool IsAdaptDpi();

	/**
	* 更换全局语言
	* @param[in] language 新的语言名称
	* @return void 无返回值
	*/
	void SetLanguage(const std::string& language);

	/**
	* 获取当前语言名称
	* @return std::string 当前语言名称
	*/
	std::string GetLanguage();
	/**
	* 获取UI风格ID
	* @return int 风格ID  0 类似QQ样式的UI，1类似微信样式的UI
	*/
	int GetUIStyle();
	/**
	* 更换UI风格
	* @param[in] style_id 风格ID  0 类似QQ样式的UI，1类似微信样式的UI
	* @return void 无返回值
	*/
	void SetUIStyle(int style_id);
	/**
* 是否使用私有化配置
* @param[in] url 私有化配置地址
* @return bool 
*/
	bool UsePrivateSettings(std::string& url) const;
	/**
* 是否使用私有化配置
* @return bool
*/
	bool UsePrivateSettings() const;
	/**
	* 设置是否使用私有化配置
	* @param[in] use 
	* @param[in] url  私有化配置地址
	* @return void 无返回值
	*/
	void UsePrivateSettings(bool use,const std::string& url);
	/**
	* 设置是否使用私有化配置
	* @param[in] use
	* @return void 无返回值
	*/
	void UsePrivateSettings(bool use);
private:
	/**
	* 读取配置信息
	* @return void 无返回值
	*/
	void ReadConfig();

private:
	bool adapt_dpi_;
	std::string language_;
	int uistyle_id_;
	bool private_settings_enable_;
	std::string private_settings_url_;
};