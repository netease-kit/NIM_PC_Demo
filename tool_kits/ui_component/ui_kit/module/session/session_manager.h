#pragma once
#include "gui/session/session_box.h"
#include "ring.h"

namespace nim_comp
{
/** @class SessionManager
  * @brief 会话管理类，用于统一管理会话盒子、会话窗口、消息。
  *        收到会话消息以及其他会话事件，先经过会话管理器，然后分发到正确的会话盒子。
  *        会话盒子的合并、拆分、拖拽功能的控制。
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/8/27
  */
class SdkDataObject;
class SessionManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(SessionManager);
public:
	SessionManager();
	~SessionManager();
	/**
	* 根据会话id，打开会话盒子
	* @param[in] id 会话id
	* @param[in] type 会话类型（P2P或群聊）
	* @param[in] reopen 如果为true，且该会话盒子当前已打开，则关闭该盒子，重新打开
	* @return SessionBox* 返回会话盒子的指针
	*/
	SessionBox* OpenSessionBox(std::string session_id, nim::NIMSessionType type, bool reopen = false);

	/**
	* 添加消息到正确的会话盒子
	* @param[in] msg 消息体
	* @return void 无返回值
	*/
	void AddNewMsg(const nim::IMMessage &msg);

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
	SessionBox* FindSessionBox(const std::string &id);

	/**
	* 根据会话id，移除本类中会话盒子信息
	* @param[in] id 会话id
	* @param[in] box 会话对应的会话盒子指针
	* @return void 无返回值
	*/
	void RemoveSessionBox(std::string id, const SessionBox* box = NULL);

	/**
	* 重置某个会话的未读消息数为0
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void ResetUnread(const std::string &id);

	/**
	* 查找某个会话中是否包含未读的@我的消息
	* @param[in] id 会话id
	* @return bool true 包含，false 不包含
	*/
	bool IsContainAtMeMsg(const std::string &id);

	/**
	* 查询当前登录用户在某个群中的信息
	* @param[in] tid 群组id
	* @return void 无返回值
	*/
	void QueryMyTeamInfo(const std::string& tid);
public:
	/**
	* 设置是否开启会话合并功能
	* @param[in] enable true:开启合并功能,自动合并当前所有会话盒子
	*					false:关闭合并功能,自动拆分当前所有会话盒子
	* @return 无返回值
	*/
	void SetEnableMerge(bool enable);

	/**
	* 是否开启会话合并功能
	* @return bool true 是，false 否
	*/
	bool IsEnableMerge() const;

	/**
	* 设置是否使用自定义的拖拽缩略图
	* @param[in] use true:使用自定义拖拽缩略图，会使用低级键盘钩子，一些安全软件会拦截
	*				 false:使用系统的拖拽缩略图，效果上不如自定义的好
	* @return 无返回值
	*/
	void SetUseCustomDragImage(bool use);

	/**
	* 是否使用自定义的拖拽缩略图
	* @return bool true 是，false 否
	*/
	bool IsUseCustomDragImage() const;

	/**
	* 当前是否正在拖拽会话盒子
	* @return bool true 是，false 否
	*/
	bool IsDragingSessionBox() const;

	/**
	* 设置被放入会话盒子的会话窗体
	* @param[in] session_form 会话窗体
	* @return void	无返回值
	*/
	void SetDropSessionForm(SessionForm *session_form);

	/**
	* 执行拖拽会话盒子的操作
	* @param[in] session_box 会话盒子
	* @param[in] bitmap 会话盒子生成的位图
	* @param[in] pt_offset 拖拽图像相对于鼠标的偏移
	* @return bool true 成功，false 失败
	*/
	bool DoDragSessionBox(SessionBox *session_box, HBITMAP bitmap, POINT pt_offset);

private:
	/**
	* 创建一个会话盒子
	* @param[in] session_id 会话id
	* @param[in] type 会话类型
	* @return SessionBox*	会话盒子指针
	*/
	SessionBox* CreateSessionBox(const std::string &session_id, nim::NIMSessionType type);

	/**
	* 创建一个用于拖拽的IDataObject对象
	* @param[in] bitmap 用于生成拖拽效果的位图
	* @param[in] pt_offset 拖拽图像相对于鼠标的偏移
	* @return SdkDataObject* IDataObject对象
	*/
	SdkDataObject* CreateDragDataObject(HBITMAP bitmap, POINT pt_offset);

	/**
	* 执行拖拽操作前被调用
	* @return void	无返回值
	*/
	void OnBeforeDargSessionBox(SessionBox *session_box, HBITMAP bitmap, POINT pt_offset);

	/**
	* 执行拖拽操作后被调用
	* @return void	无返回值
	*/
	void OnAfterDragSessionBox();

private:
	/**
	* 查询当前登录用户在某个群中信息的回调函数
	* @param[in] tid 群组id
	* @param[in] team_member_info 群成员信息
	* @return void 无返回值
	*/
	void OnQueryMyTeamInfo(const std::string& tid, const nim::TeamMemberProperty& team_member_info);

	/**
	* 查找某个群的消息是否需要通知提示
	* @param[in] tid 群组id
	* @return bool true 需要，false 不需要
	*/
	bool IsTeamMsgNotify(const std::string& tid);

private:
	bool		enable_merge_;
	bool		use_custom_drag_image_;
	SessionBox	*draging_session_box_;	// 当下正在被拖拽的会话盒子
	SessionForm	*drop_session_form_;	// 拖入会话盒子的会话窗口，用于附加拖拽来的会话盒子

	std::map<std::string, SessionBox*>	session_box_map_;
	std::map<std::string, bool>			seesion_id_atme_;	// 会话是否包含未读的@me消息

	std::map<std::string, uint64_t>		team_list_bits_;
	Ring ring_;
};
}