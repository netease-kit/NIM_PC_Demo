#pragma once

namespace nim_comp
{
/** @class LocalHelper
  * @brief 获取本地应用信息的辅助类
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class LocalHelper
{
public:
	/**
	* 获取应用程序版本
	* @param[out] version 版本号
	* @param[out] version_flag 版本标识
	* @return bool true 成功，false 失败
	*/
	static bool GetAppLocalVersion(int &version, std::wstring &version_flag);
};
}