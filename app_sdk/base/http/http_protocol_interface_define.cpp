#include "stdafx.h"
#include "http_protocol_interface_define.h"
#include "sdk_http_manager.h"
namespace app_sdk
{
	const uint32_t IHttpRequest::request_time_out_ = 15000;

	IHttpResponse::IHttpResponse() :
		response_code_(0)
	{
	}
	IHttpResponse::~IHttpResponse()
	{
	}	
	int IHttpResponse::GetResponseCode() const
	{
		return response_code_;
	}
	void IHttpResponse::SetResponseCode(int code)
	{
		response_code_ = code;
	}
	bool IHttpRequest::UsePostMethod() const
	{
		return true;
	}
	int IHttpRequest::GetRequestTimeout() const
	{
		return request_time_out_;
	}
};