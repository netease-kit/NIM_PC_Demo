#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

namespace nim_comp
{
	typedef std::function<void(const std::list<nim::SessionData>& sessions)> OnSessionDataChangedCallback;
	typedef std::function<void(bool has_more, const nim::SessionOnLineServiceHelper::SessionList& session_list)> OnOnlineSessionDataChangedCallback;

	/** @class SessionService
	* @brief 会话列表服务
	* @copyright (c) 2016, NetEase Inc. All rights reserved
	* @date 2018/04/28
	*/
	class SessionService : public nbase::SupportWeakCallback
	{
	public:
		SINGLETON_DEFINE(SessionService);

	private:
		SessionService() {}
		~SessionService() {}

	public:
		/**
		* 主动向sdk查询之前保存的会话列表，然后逐个添加到界面的会话列表中
		* @return void 无返回值
		*/
		void InvokeLoadSessionList();

		/**
		* 主动向查询云端会话列表，然后逐个添加到界面的会话列表中
		* @return void 无返回值
		*/
		void InvokeLoadOnlineSessionList();

		/**
		* 从会话列表查找会话控件
		* @param[in] session_id 会话id
		* @return SessionItem* 会话控件的指针
		*/
		nim::SessionData GetSessionItem(const std::string &session_id);

		/**
		 * 查询本地会话列表回调接口
		 * @param[in] sessions 本地会话列表数据
		 * @return void 无返回值
		 */
		void OnQuerySessionList(const std::list<nim::SessionData>& sessions);

		/**
		 * 查询本地会话列表回调接口
		 * @param[in] res_code 查询服务器会话列表的返回值
		 * @param[in] has_more 是否还有更多数据
		 * @param[in] sessions 云端会话列表数据
		 * @return void 无返回值
		 */
		void OnQueryOnlineSessionList(int res_code, bool has_more, const nim::SessionOnLineServiceHelper::SessionList& sessions);

		/**
		* 设置本地会话列表变更的回调
		* @param[in] callback 
		* @return void 无返回值
		*/
		void RegSessionDataChangedCallback(const OnSessionDataChangedCallback& callback) { session_data_changed_callback_.reset(new OnSessionDataChangedCallback(callback)); }

		/**
		* 设置云端会话列表变更的回调
		* @param[in] callback
		* @return void 无返回值
		*/
		void RegOnlineSessionDataChangedCallback(const OnOnlineSessionDataChangedCallback& callback) { received_online_session_data_callback_.reset(new OnOnlineSessionDataChangedCallback(callback)); }

	private:
		/**
		* 向sdk查询会话列表的结果回调。
		* @param[in] unread_count 未读消息条数
		* @param[in] session_list 会话列表数据
		* @return void 无返回值
		*/
		static void OnQuerySessionListCallback(int unread_count, const nim::SessionDataList& session_list);

		/**
		 * 向 SDK 查询云端会话列表的结果回调
		 * @param[in] online_session_list session 数据（包含返回值、是否有更多数据和实际的 session 数据）
		 * @return void 无返回值
		 */
		static void OnQuerySessionOnlineListCallback(const nim::SessionOnLineServiceHelper::QuerySessionListResult& online_session_list);

	private:
		std::map<std::string, nim::SessionData> sessions_;
		std::map<std::string, nim::SessionOnLineServiceHelper::SessionInfo> online_sessions_;
		std::unique_ptr<OnSessionDataChangedCallback> session_data_changed_callback_;
		// 云端会话数据是增量查询的，这里命名不使用 changed 关键字而是 received
		std::unique_ptr<OnOnlineSessionDataChangedCallback> received_online_session_data_callback_;
		uint64_t	online_session_latest_timestamp_ = 0;
	};
}