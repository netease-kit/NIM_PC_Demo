#pragma once

namespace nim_comp
{
/** @class TeamCallback
  * @brief 群事件回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class TeamCallback
{
public:
	/**
	* 查询群信息的结果回调
	* @param[in] info 事件信息内容
	* @param[in] tid 群组id
	* @param[in] team_info 群组信息
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static void UITeamEventCallback(const nim::TeamEvent& info, const std::string& tid, const nim::TeamInfo& team_info);

	/**
	* 群事件回调，需要在程序开始运行时就注册好。
	* @param[in] result 事件信息内容
	* @return void	无返回值
	*/
	static void OnTeamEventCallback(const nim::TeamEvent& result);
	
	/**
	* 收到系统消息的回调，需要在程序开始运行时就注册好。
	* @param[in] msg 系统消息和自定义通知数据
	* @return void	无返回值
	*/
	static void OnReceiveSysmsgCallback(const nim::SysMessage& msg);
};

}