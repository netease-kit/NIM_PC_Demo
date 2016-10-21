#pragma once

namespace nim_comp
{
/** @class MultiportPushCallback
  * @brief 多端推送事件回调类
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/9/7
  */
class MultiportPushCallback
{
public:
	/**
	* 多端推送开关改变回调
	* @param[in] rescode 返回码
	* @param[in] switch_on 是否开启
	* @return void	无返回值
	*/
	static void OnMultiportPushConfigChange(int rescode, bool switch_on);
};
}