#pragma once
#include "base/callback/callback.h"
#include "base/memory/singleton.h"
#include <list>
#include "shared/auto_unregister.h"
#include "define.h"

namespace app_sdk
{
	enum SERVER_TYPE
	{
		kST_DEFAULT,
		kST_TEST,
		kST_PRE,
		kST_ONLINE,
	};

	class SDKManager : public nbase::SupportWeakCallback
	{
	public:
		SDKManager();
		~SDKManager();
		SINGLETON_DEFINE(SDKManager);

		typedef std::function<void(ResponseBaseParam *)> ResponseCallback;
		typedef std::function<void(std::shared_ptr<ResponseBaseParam>)> ResponseCallbackEx;
	public:
		void SetServerType(SERVER_TYPE type);
		bool IsOnlineServer() { return server_type_ == kST_ONLINE; }
		std::string GetAppkey(SERVER_TYPE type = kST_DEFAULT);
		std::string GetHost(SERVER_TYPE type = kST_DEFAULT);
		std::string GetToken() { return token_; }

	public:
		template<typename TRequest, typename TResponse>
		void HttpCallback(bool ret, int response_code, const std::string& reply, const std::shared_ptr<TRequest>& request, const std::function<void(const std::shared_ptr<TRequest>&, const std::shared_ptr<TResponse>&)>& cb)
		{
			QLOG_APP(L"#####response api:{0} rescode:{1}\r\n{2}") << request->GetAPI() << response_code << reply;
#ifdef _DEBUG
			std::wstring test = nbase::UTF8ToUTF16(reply);
#endif
			if (cb != nullptr)
			{
				auto response = std::make_shared<TResponse>();
				response->response_code_ = response_code;
				response->Parse(reply);
				cb(request, response);
			}
		}
		template<typename TRequest, typename TResponse>
		void HttpRequest(const std::shared_ptr<TRequest>& request, \
			const std::function<void(const std::shared_ptr<TRequest>&, const std::shared_ptr<TResponse>&)>& callback)
		{
			//类型校验
			static_assert((
				(std::is_base_of<RequestBaseParam, TRequest>::value) && (std::is_base_of<ResponseBaseParam, TResponse>::value)
				),
				"TRequest or TResponse type error!");
			//组包
			std::string api(GetHost());
			std::string post;
			auto request_param = std::dynamic_pointer_cast<RequestBaseParam>(request);
			if (request_param->GetAPI().empty())
				return;
			api.append(request_param->GetAPI());
			post.append(request_param->ToJsonString());
			if (!request_param->UsePostMethod())
				api.append(1, '?').append(post);
			//回调
			auto http_cb = ToWeakCallback(
				std::bind(
				&SDKManager::HttpCallback<TRequest, TResponse>,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				request,
				callback
				)
				);
			QLOG_APP(L"#####request api:{0} \r\n post data :{1}\r\n") << api << post;
			nim_http::HttpRequest req(api, request_param->UsePostMethod() ? post.c_str() : nullptr, request_param->UsePostMethod() ? post.size() : 0, http_cb);
			if (!token_.empty())
				req.AddHeader("token", token_);
			if (request_param->UsePostMethod())
				req.SetMethodAsPost();
			nim_http::PostRequest(req);
		}

	private:
		SERVER_TYPE server_type_ = kST_ONLINE;
		std::string token_;
	};
}