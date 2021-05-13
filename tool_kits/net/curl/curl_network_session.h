// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2013/8/29
//
// Curl network session
// The session MUST work on a thread the same as the session manager

#ifndef NET_CURL_CURL_NETWORK_SESSION_H_
#define NET_CURL_CURL_NETWORK_SESSION_H_

#include <set>
#include <memory>
#include "base/base_types.h"
#include "net/base/net_export.h"
#include "third_party/curl/include/curl/curl.h"

namespace net
{

class NET_EXPORT CurlNetworkSession : public std::enable_shared_from_this<CurlNetworkSession>
{
public:
	explicit CurlNetworkSession();
	virtual ~CurlNetworkSession();

	// Called immediately after the easy handle created
	// You can do `curl_easy_setopt` work as you like,
	// such as setting url, setting cookies, etc.
	virtual bool OnEasyHandleCreated() { return true; }
	// Called after the session is finished successfully.
	virtual void OnTransferDone() {}
	// Called when a error occurred.
	// This will be called even not registering failed
	virtual void OnError() {}
	// Called after the easy handle destroyed.
	virtual void OnEasyHandleDestroyed() {}
	// Called when registered
	virtual void OnRegistered() {}
	// Called when deregistered
	virtual void OnDeregisterd() {}

protected:
	// Session result
	CURLcode result_;
	// The Curl easy handle of the session. Owned.
	CURL *easy_handle_;

	long low_speed_limit_;
	long low_speed_time_;

private:
	friend class CurlNetworkSessionManager;

	// Whether the session has been done by Curl
	bool transfer_done_;
	// Number of active watchers owned by libuv
	int num_active_watchers_;

	bool CanBeSafelyRemoved() const
	{
		return transfer_done_ && !num_active_watchers_;
	}

	bool CreateCurlEasyHandle();
	void DestroyCurlEasyHandle();

	DISALLOW_COPY_AND_ASSIGN(CurlNetworkSession);
};

} // namespace net

#endif // NET_CURL_CURL_NETWORK_SESSION_H_