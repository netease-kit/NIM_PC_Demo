#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

namespace nim_comp
{
	typedef std::function<void(const std::list<nim::SessionData>& sessions)> OnSessionDataChangedCallback;

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
		* 从会话列表查找会话控件
		* @param[in] session_id 会话id
		* @return SessionItem* 会话控件的指针
		*/
		nim::SessionData GetSessionItem(const std::string &session_id);

		void OnQuerySessionList(const std::list<nim::SessionData>& sessions);

		/**
		* 
		* @param[in] callback 
		* @return 
		*/
		void RegSessionDataChangedCallback(const OnSessionDataChangedCallback& callback) { session_data_changed_callback_.reset(new OnSessionDataChangedCallback(callback)); }

	private:
		/**
		* 向sdk查询会话列表的结果回调。
		* @param[in] unread_count 未读消息条数
		* @param[in] session_list 会话列表数据
		* @return void	无返回值
		*/
		static void OnQuerySessionListCallback(int unread_count, const nim::SessionDataList& session_list);


	private:
		std::map<std::string, nim::SessionData> sessions_;
		std::unique_ptr<OnSessionDataChangedCallback> session_data_changed_callback_;
	};
}