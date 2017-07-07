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

private:
	/**
	* 读取配置信息
	* @return void 无返回值
	*/
	void ReadConfig();

private:
	bool adapt_dpi_;
	std::string language_;
};