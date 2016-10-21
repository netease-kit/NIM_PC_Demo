#pragma once

#include "nim_ui_dll.h"
#include "module/session/session_manager.h"
#include "module/service/team_service.h"

namespace nim_ui
{

/** @class SessionManager
  * @brief 提供会话窗体控制接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API SessionManager
{
public:
	SINGLETON_DEFINE(SessionManager);
	SessionManager(){};
	~SessionManager(){};
public:
	/**
	* 是否开启会话合并功能
	* @param[in] enable true:开启合并功能,自动合并当前所有会话盒子
	*					false:关闭合并功能,自动拆分当前所有会话盒子
	* @return 无返回值
	*/
	void SetEnableMerge(bool enable);
	bool IsEnableMerge() const;

	void SetUseCustomDragImage(bool use);
	bool IsUseCustomDragImage() const;


	/**
	* 根据会话id，打开会话盒子
	* @param[in] id 会话id
	* @param[in] type 会话类型（P2P或群聊）
	* @param[in] reopen 如果为true，且该会话窗口当前已打开，则关闭该窗口，重新打开
	* @return SessionBox* 返回会话盒子的指针
	*/
	nim_comp::SessionBox* OpenSessionBox(std::string id, nim::NIMSessionType type, bool reopen = false);

	/**
	* 判断某个会话盒子是否处于激活状态
	* @param[in] id 会话id
	* @return bool true 激活，false 非激活
	*/
	bool IsSessionBoxActive(const std::string& id);

	/**
	* 根据会话id，找回会话盒子控件
	* @param[in] id 会话id
	* @return SessionBox* 返回会话盒子的指针
	*/
	nim_comp::SessionBox* FindSessionBox(const std::string &id);

	/**
	* 添加消息到正确的会话窗口
	* @param[in] msg 消息体
	* @return void 无返回值
	*/
	void AddNewMsg(const nim::IMMessage &msg);

	/**
	* 重置某个会话的未读消息数为0
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void ResetUnread(const std::string &id);
	
	/**
	* 根据会话id，移除本类中会话盒子信息
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void RemoveSessionBox(std::string id);
};

}