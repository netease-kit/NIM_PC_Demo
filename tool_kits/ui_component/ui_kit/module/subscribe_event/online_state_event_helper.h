// Copyright (c) 2017, NetEase Inc. All rights reserved.
//
// Author: Redrain
// Date: 2017/03/31
//
// 订阅事件功能中的在线状态事件辅助类
#pragma once

namespace nim_comp
{

// 客户端自定义的在线状态事件config字段格式
struct EventConfig
{
	enum OnlineState
	{
		kOnlineStateOnline	= 0,	//在线
		kOnlineStateBusy	= 1,	//忙碌
		kOnlineStateLeave	= 2,	//离开
	};

	enum NetState
	{
		kNetStateUnknow	= 0,
		kNetStateWifi	= 1,
		kNetStateWwan	= 2,
		kNetState2G		= 3,
		kNetState3G		= 4,
		kNetState4G		= 5,
		
	};

	OnlineState	online_state_;
	NetState	net_state_;

	EventConfig()
		:online_state_(kOnlineStateOnline)
		, net_state_(kNetStateUnknow)
	{}
};

typedef std::map<nim::NIMClientType, EventConfig> EventMultiConfig;

/** @class OnlineStateEventHelper
  * @brief 在线状态事件辅助类
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2017/03/31
  */
class OnlineStateEventHelper
{
public:
	/**
	* 生成忙碌状态事件
	* @param[in] busy 是否忙碌
	* @return EventData	事件数据
	*/
	static nim::EventData CreateBusyEvent(bool busy);

	/**
	* 从EventConfig转换为kNIMEventConfig字段配置信息
	* @param[in] config 配置信息
	* @return std::string 配置信息Json
	*/
	static std::string EventConfigToJsonString(const EventConfig &config);

	/**
	* 从kNIMEventConfig转换为EventConfig配置信息
	* @param[in] config_json 配置信息Json
	* @return EventConfig 配置信息
	*/
	static EventConfig JsonStringToEventConfig(const std::string& config_json);

	/**
	* 从kNIMEventMultiConfig字段解析出多端配置信息
	* @param[in] multi_config_json 多端配置信息Json
	* @return EventMultiConfig 多端配置信息
	*/
	static EventMultiConfig JsonStringToMultiConfig(const std::string& multi_config_json);

	/**
	* 根据用户事件信息获取在线状态摘要信息
	* @param[in] online_client_type 在线客户端
	* @param[in] multi_config 多端配置信息
	* @param[in] is_simple 是否要简要信息（用于在列表展示）
	* @return std::wstring 摘要信息
	*/
	static std::wstring GetOnlineState(const nim::EventOnlineClientType& online_client_type, const EventMultiConfig& multi_config, bool is_simple);
};
}