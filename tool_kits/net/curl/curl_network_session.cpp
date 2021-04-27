// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2013/8/29
//
// Curl network session
// The session MUST work on a thread the same as the session manager

#include "net/curl/curl_network_session.h"

#include "net/log/log.h"
#include "base/thread/thread.h"

namespace net
{

CurlNetworkSession::CurlNetworkSession()
	: easy_handle_(nullptr),
	  result_(CURLE_FAILED_INIT),
	  num_active_watchers_(0),
	  low_speed_limit_(10),
	  low_speed_time_(60),
	  transfer_done_(true)
{
	//DCHECK(!num_active_watchers_);
}

CurlNetworkSession::~CurlNetworkSession()
{
	DestroyCurlEasyHandle();
}

bool CurlNetworkSession::CreateCurlEasyHandle()
{
	//DCHECK(easy_handle_ == nullptr);

	easy_handle_ = curl_easy_init();
	if (easy_handle_ == nullptr) {
		LOG_ERR(L"CurlNetworkSession create easy handle failed.");
		return false;
	}

	curl_easy_setopt(easy_handle_, CURLOPT_LOW_SPEED_TIME, low_speed_time_);
	curl_easy_setopt(easy_handle_, CURLOPT_LOW_SPEED_LIMIT, low_speed_limit_);

	return OnEasyHandleCreated();
}

void CurlNetworkSession::DestroyCurlEasyHandle()
{
	if (easy_handle_ != nullptr) {
		curl_easy_cleanup(easy_handle_);
		easy_handle_ = nullptr;
		OnEasyHandleDestroyed();
	}
}

} // namespace net