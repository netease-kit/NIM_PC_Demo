#pragma once
#include "control/session_item.h"
#include "gui/main/multispot_form.h"
#include "module/service/photo_service.h"
#include "module/service/user_service.h"

namespace nim_comp
{

/** @class SessionListCloud
 * @brief 用于操作和维护云端会话列表
 * @copyright (c) 2015, NetEase Inc. All rights reserved
 * @author Dylan
 * @date 2019/12/2
 */
class SessionListCloud : public nbase::SupportWeakCallback
{
public:
	SessionListCloud(ui::ListBox* session_list);
	~SessionListCloud() {};

	/**
	 * 是否显示会话列表
	 * @param[in] show true 为显示，false 为不显示
	 * @return void 无返回值
	 */
	void Show(bool show);

	/**
	 * 添加一个云端会话到列表中
	 * @param[in] session_info 云端会话数据
	 * @return void 无返回值
	 */
	void AddSessionItem(const nim::SessionData& session_data);

	/**
	* 从会话列表查找会话控件
	* @param[in] session_id 会话id
	* @return SessionItem* 会话控件的指针
	*/
	SessionItem* GetSessionItem(const std::string &session_id);

	/**
	* 移除某个会话控件
	* @param[in] item 会话控件指针
	* @return void 无返回值
	*/
	void DeleteSessionItem(SessionItem* item);

	/**
	* 为某个会话控件重置未读消息数为0
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void ResetUnreadCount(const std::string &id);

	/**
	 * 添加结束标记
	 * @param[in] 是否有更多
	 * @return bool 返回 true 表示成功，返回 false 表示失败
	 */
	bool AddLoadMore(bool has_more);

	/**
	* 更新包含用户信息的会话控件的显示
	* @param[in] user_info 用户信息
	* @return void 无返回值
	*/
	void UpdateSessionInfo(const nim::UserNameCard& user_info);

	/**
	* 按每个会话最后一条消息的时间排序会话的位置
	* @param[in] msg 被排序的会话数据
	* @return int 在绘画列表中的位置
	*/
	int AdjustMsg(const nim::SessionData &msg);

private:
	/**
	* 注册给sdk的回调函数，会话消息变化（如有新消息、删除会话记录）时执行
	* @param[in] rescode 返回码
	* @param[in] data 会话消息的具体信息和内容
	* @param[in] total_unread_counts 未读消息条数
	* @return void 无返回值
	*/
	void OnSessionChangeCallback(nim::NIMResCode rescode, const nim::SessionData& data, int total_unread_counts);

	/**
	* 用户信息改变的回调函数
	* @param[in] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos);

	/**
	* 用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] accid 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path);

	/**
	* 群组头像改变的回调函数
	* @param[in] team_info 群信息
	* @return void 无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);

	/**
	* 切换本地会话列表
	* @param[in] param 发送事件的控件的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSwitchLocalSession(ui::EventArgs* msg);

	/**
	* 响应会话控件的通知事件
	* @param[in] param 发送事件的控件的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnItemNotify(ui::EventArgs* msg);

	/**
	* 响应用户列表改变的回调函数
	* @param[in] change_type 好友变化类型
	* @param[in] accid 用户id
	* @return void 无返回值
	*/
	void OnFriendListChange(FriendChangeType change_type, const std::string& accid);

	/**
	 * 禁音事件变更
	 * @param[in] id 用户id
	 * @param[in] mute 是否静音
	 */
	void OnNotifyChangeCallback(std::string id, bool mute);

	/**
	* 群组消息通知类型改变通知
	* @param[in] tid 群组ID
	* @param[in] bits 变更后的群组消息通知类型
	* @return void 无返回值
	*/
	void OnTeamNotificationModeChangeCallback(const std::string &tid, const int64_t bits);

	/**
	 * 相应回车事件
	 * @param[in] param 事件体
	 * @return bool true 继续传递控件消息，false 停止传递控件消息
	 */
	bool OnReturnEventsClick(ui::EventArgs* param);

	/**
	* 响应接收事件的回调函数
	* @param[in] event_type 事件类型
	* @param[in] accid 用户id
	* @param[in] data 事件信息
	* @return void 无返回值
	*/
	void OnReceiveEvent(int event_type, const std::string &accid, const EventDataEx &data);

private:
	ui::ListBox*		session_list_cloud_ = nullptr;

	AutoUnregister		unregister_cb;
};

}
