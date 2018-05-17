#pragma once
/** @file http_protocol_interface_define.h
* @brief app_sdk 基于http请求的协议基础数据类型定义
* @copyright (c) 2015-2018, NetEase Inc. All rights reserved
* @date 2018/4/26
*/
namespace app_sdk
{	
	template<typename THttpRequest>
	using TSharedHttpRequest = std::shared_ptr<typename THttpRequest>;

	template<typename THttpResponse>
	using TSharedHttpResponse = std::shared_ptr<typename THttpResponse>;

	template<typename TSharedHttpRequest, typename TSharedHttpResponse>
	using THttpCallback = std::function<void(const typename TSharedHttpRequest&, const typename TSharedHttpResponse&)>;
	
	class SDKManager;
	/** @class IHttpRequest
	* @brief app_sdk 请求接口定义
	* @copyright (c) 2018, NetEase Inc. All rights reserved
	* @date 2018/4/26
	*/
	class IHttpRequest
	{
	public:
		IHttpRequest() = default;
		virtual ~IHttpRequest() = default;
		//是否使用post方法
		virtual bool UsePostMethod() const;
		//http请求地址
		virtual std::string GetAPIURL() const = 0;	
		//请求的超时时间
		virtual int GetRequestTimeout() const;
		//获取请求内容
		virtual void GetRequestContent(std::string& content) const = 0;
		//获取请求时http头要加入的信息
		virtual void GetRequestHead(std::map<std::string, std::string>& heads) = 0;
	private:
		static const uint32_t request_time_out_;
	};
	/** @class IHttpRequest
	* @brief app_sdk 应答接口定义
	* @copyright (c) 2018, NetEase Inc. All rights reserved
	* @date 2018/4/26
	*/
	class IHttpResponse
	{
		friend class SDKManager;
	public:	
		IHttpResponse();
		virtual ~IHttpResponse();
		virtual void Parse(const std::string& response) = 0;	//解析应答		
		//获取http请求应答码
		int GetResponseCode() const;
	private:
		void SetResponseCode(int code);
	protected:
		int response_code_;//http请求的应答码
	};	

	//创建一个继承于IHttpRequest的http请求
	template<typename TSharedHttpRequest, typename... TParams>
	static auto CreateHttpRequest(const TParams&... params)->
		std::shared_ptr<typename TSharedHttpRequest::element_type>
	{
		using TRequest = TSharedHttpRequest::element_type;
		static_assert(std::is_base_of<IHttpRequest, TRequest>::value, "app_sdk::IHttpRequest should be base of TRequest");
		return std::make_shared< TRequest>(params...);
	}
}