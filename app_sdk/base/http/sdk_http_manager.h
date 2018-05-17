/** @file sdk_http_manager.h
* @brief app_sdk http请求管理器
* @copyright (c) 2015-2018, NetEase Inc. All rights reserved
* @date 2018/4/26
*/
#pragma once
#include "shared/threads.h"
#include "base/callback/callback.h"
#include "base/memory/singleton.h"
#include "shared/auto_unregister.h"
#include "http_protocol_interface_define.h"

namespace app_sdk
{
	template<typename TSharedResponse>
	using RepeatHttpRequestStopCallback = std::function<bool(int cur_req_times/*当前请求的次数*/, const typename TSharedResponse& response/*此次请求的应答*/)>;
	/** @class SDKManager
	* @brief app_sdk 请求管理器
	* @copyright (c) 2018, NetEase Inc. All rights reserved
	* @date 2018/4/26
	*/
	class SDKManager : public nbase::SupportWeakCallback
	{
	public:
		SDKManager() = default;
		~SDKManager() = default;
		SINGLETON_DEFINE(SDKManager);
	public:
		/** void Invoke_Request(const typename TSharedRequest& request, const THttpCallback<typename TSharedRequest, typename TSharedResponse>& callback)
		* app sdk 发送http请求（协议类）接口，异步
		* @param[in] request 请求的数据
		* @param[in] callback 应答回调
		* @return void 
		*/
		template<typename TSharedRequest, typename TSharedResponse>
		void Invoke_Request(const typename TSharedRequest& request, const THttpCallback<typename TSharedRequest, typename TSharedResponse>& callback)
		{
			Invoke_RepeatRequest<typename TSharedRequest, typename TSharedResponse, 0>(request, callback, [](int, const typename TSharedResponse&) {return true; });
		}
	private:
		//多次请求暂时未开放出来，开发者可根据实际应用自行开放
		template<typename TSharedRequest, typename TSharedResponse, int time_interval>
		void Invoke_RepeatRequest(const typename TSharedRequest& request, \
			const typename THttpCallback<typename TSharedRequest, typename TSharedResponse>& callback, \
			const RepeatHttpRequestStopCallback<typename TSharedResponse>& stop_cb)
		{
			using TRequest = typename TSharedRequest::element_type;
			using TResponse = typename TSharedResponse::element_type;
			//类型校验
			static_assert((
				(std::is_base_of<IHttpRequest, TRequest>::value) && (std::is_base_of<IHttpResponse, TResponse>::value)
				),
				"TRequest or TResponse type error!");
			int thread_id = nbase::FrameworkThread::GetManagedThreadId();
			int interval = time_interval;
			auto task = ToWeakCallback([thread_id, stop_cb, callback, interval, request, this]() {
				DoHttpRequest<typename TSharedRequest, typename TSharedResponse>(request,
					ToWeakCallback(
						std::bind(&SDKManager::OnRepeatHttpRequest<typename TSharedRequest, typename TSharedResponse>, this, \
						1, thread_id, stop_cb, callback, interval, std::placeholders::_1, std::placeholders::_2)
					));
			});
			nbase::ThreadManager::PostTask(ThreadId::kThreadApp, task);
		}	
		template<typename TSharedRequest, typename TSharedResponse>
		void DoHttpRequest(const typename TSharedRequest& request, const THttpCallback<typename TSharedRequest, typename TSharedResponse>& callback)
		{
			using TRequest = typename TSharedRequest::element_type;
			using TResponse = typename TSharedResponse::element_type;
		
			auto request_param = std::dynamic_pointer_cast<IHttpRequest>(request);
			//组包
			std::string api(request_param->GetAPIURL());		
			if (api.empty())
				return;			
			std::string post("");
			request_param->GetRequestContent(post);
			if (!request_param->UsePostMethod() && post.empty())			
					api.append(1, '?').append(post);
			//回调
			auto http_cb_op =
				nbase::Bind(
				&SDKManager::HttpCallback<typename TSharedRequest, typename TSharedResponse>,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				request,
				callback
				);
			auto http_cb = ToWeakCallback([this,  http_cb_op](bool ret, int response_code, const std::string& reply){
				nbase::ThreadManager::PostTask(ThreadId::kThreadApp, ToWeakCallback([this, ret, response_code, reply,http_cb_op](){
					http_cb_op(ret, response_code, reply);
				}));
			});

#ifdef _DEBUG
			QLOG_APP(L"#####request api:{0} \r\n post data :{1}\r\n") << api << post;
#else
			QLOG_APP(L"#####request api:{0}") << api;			
#endif
			nim_http::HttpRequest req(api, request_param->UsePostMethod() ? post.c_str() : nullptr, request_param->UsePostMethod() ? post.size() : 0, http_cb);
			if (request_param->UsePostMethod())
				req.SetMethodAsPost();
			std::map<std::string, std::string> heads;
			request_param->GetRequestHead(heads);
			FillHttpRequestHead(req, heads);
			req.SetTimeout(request_param->GetRequestTimeout());
			nim_http::PostRequest(req);			
		}
		template<typename TSharedRequest, typename TSharedResponse>
		void HttpCallback(bool ret, int response_code, const std::string& reply, const typename TSharedRequest& request, const THttpCallback<typename TSharedRequest, typename TSharedResponse>& cb)
		{
			using TRequest = typename TSharedRequest::element_type;
			using TResponse = typename TSharedResponse::element_type;
			
#ifdef _DEBUG
			QLOG_APP(L"#####response api:{0} rescode:{1}\r\n{2}") << request->GetAPIURL() << response_code << reply;
			std::wstring test = nbase::UTF8ToUTF16(reply);
#else
			QLOG_APP(L"#####response api:{0} rescode:{1}") << request->GetAPIURL() << response_code ;
#endif
			if (cb != nullptr)
			{
				auto response = std::make_shared<TResponse>();
				response->SetResponseCode(response_code);
				response->Parse(reply);
				cb(request, response);
			}
		}
		template<typename TSharedRequest, typename TSharedResponse>
		void OnRepeatHttpRequest(int cur_times,int call_thread_id, const RepeatHttpRequestStopCallback<TSharedResponse>& stop_cb, \
			const THttpCallback<typename TSharedRequest, typename TSharedResponse>& callback, int time_interval,
			const typename TSharedRequest& request, const typename TSharedResponse& response)
		{
			if (stop_cb(cur_times, response))
			{
				auto task = ToWeakCallback([callback,request, response](){
					callback(request, response);
				});
				nbase::ThreadManager::PostTask(((call_thread_id > ThreadId::kThreadBegin && call_thread_id  < ThreadId::kThreadEnd) ? call_thread_id : ThreadId::kThreadApp), task);
			}
			else
			{
				auto task = ToWeakCallback([cur_times, call_thread_id,stop_cb, callback, time_interval, request, response, this]() {
					DoHttpRequest<typename TSharedRequest, typename TSharedResponse>(request,
						ToWeakCallback(
							std::bind(&SDKManager::OnRepeatHttpRequest<typename TSharedRequest, typename TSharedResponse>, this, \
							cur_times + 1, call_thread_id,stop_cb, callback, time_interval, std::placeholders::_1, std::placeholders::_2)
						));
				});
				if (time_interval > 0)
					nbase::ThreadManager::PostDelayedTask(ThreadId::kThreadApp, task, nbase::TimeDelta::FromMilliseconds(time_interval));
				else
					nbase::ThreadManager::PostTask(ThreadId::kThreadApp, task);
			}
		}
		private:
			void FillHttpRequestHead(nim_http::HttpRequest& request,const std::map<std::string,std::string>& heads)
			{
				for (auto it : heads)
					request.AddHeader(it.first, it.second);
			}
	};
}