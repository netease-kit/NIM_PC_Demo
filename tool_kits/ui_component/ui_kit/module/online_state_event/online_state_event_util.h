#pragma once
#include "module/subscribe_event/online_state_event_helper.h"
#include "module/subscribe_event/subscribe_event_manager.h"

namespace nim_comp
{
	class OnlineStateEventUtil
	{
	public:
		/**
		* 根据用户事件信息获取在线状态摘要信息
		* @param[in] online_client_type 在线客户端
		* @param[in] multi_config 多端配置信息
		* @param[in] is_simple 是否要简要信息（用于在列表展示）
		* @return std::wstring 摘要信息
		*/
		static std::wstring GetOnlineState(const std::string& accid, const EventDataEx& data, bool is_simple);

	};

}