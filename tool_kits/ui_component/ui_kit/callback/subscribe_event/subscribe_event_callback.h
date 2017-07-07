#pragma once

namespace nim_comp
{
/** @class SubscribeEventCallback
  * @brief 事件订阅回调类
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2017/03/24
  */
class SubscribeEventCallback
{
public:
	/**
	* 接收订阅的事件的回调
	* @param[in] res_code	返回码
	* @param[in] event_data	订阅的事件
	* @return void	无返回值
	*/
	static void OnPushEventCallback(nim::NIMResCode res_code, const nim::EventData& event_data);

	/**
	* 批量接收订阅的事件的回调
	* @param[in] res_code	返回码
	* @param[in] event_list	订阅的事件
	* @return void	无返回值
	*/
	static void OnBatchPushEventCallback(nim::NIMResCode res_code, const std::list<nim::EventData>& event_list);
};
}