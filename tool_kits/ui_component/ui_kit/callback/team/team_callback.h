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
	static void UITeamEventCallback(const nim::TeamEvent& info, const std::string& tid, const nim::TeamInfo& team_info);


	/**
	* 群事件回调
	* @param[in] rescode 返回码
	* @param[in] notification_id 群事件类型
	* @param[in] tid 群id
	* @param[in] result 事件信息内容， Json数据转化成的字符串
	* @return void	无返回值
	*/
	static void OnTeamEventCallback(const nim::TeamEvent& result);
	
	/**
	* 收到系统消息的回调，需要在程序开始运行时就注册好。
	* @param[in] param 消息内容及其他信息， Json数据转化成的字符串
	* @return void	无返回值
	*/
	static void OnReceiveSysmsgCallback(const nim::SysMessage& msg);
};

}