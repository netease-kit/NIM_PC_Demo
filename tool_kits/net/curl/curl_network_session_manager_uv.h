// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2014/1/27
//
// Curl network session manager for Windows & Posix platforms based on libuv
// The manager MUST running on a UvMessageLoop based thread

#ifndef NET_CURL_CURL_NETWORK_SESSION_MANAGER_UV_H_
#define NET_CURL_CURL_NETWORK_SESSION_MANAGER_UV_H_

#include <list>
#include <map>
#include <set>
#include <memory>
#include "base/framework/message_loop_proxy.h"
#include "net/curl/curl_network_session.h"

namespace net
{

typedef std::shared_ptr<CurlNetworkSession> SessionScopedRefPtr;

class CurlNetworkSessionManager : public nbase::SupportWeakCallback
{
public:
	explicit CurlNetworkSessionManager(const std::shared_ptr<nbase::MessageLoopProxy>& message_loop);
	virtual ~CurlNetworkSessionManager();

	// Once a session is added to the manager,
	// its ownership is taken by the manager
	void AddSession(const SessionScopedRefPtr &session);

	// Once a session is removed by the manager,
	// the manager will give the session's ownership up
	void RemoveSession(CurlNetworkSession *session);

	// The count of sessions still running
	int still_running() const { return still_running_; }

private:
	struct CurlWatcher;
	static int CurlSocketEventCB(CURL *e,
								 curl_socket_t s,
								 int action,
								 void *userp,
								 void *sockp);
	static void CurlTimerCB(CURLM *multi, long timeout_ms, void *userp);

	void DoInit();
	void DoAddSession(const SessionScopedRefPtr &session);
	void DoRemoveSession(CurlNetworkSession* session);

	void StartNextSession();
	void DoStartNextSession();
	void CompleteSessionAndRemoveSoon(const SessionScopedRefPtr& session,
									  CURLcode result);
	void DoCheckSessionOrRemoveSafely(const SessionScopedRefPtr& session);

	void CheckMultiRunningStatus();
	void PerformMultiSocketAction(const SessionScopedRefPtr& session,
								  SOCKET socket,
								  int action);
	void PerformTimeoutAction();
	void SetTimeout(long timeout_ms);
	void OnTimeout();

	void OnSocketEvent(const SessionScopedRefPtr& session,
					   SOCKET socket,
					   long events,
					   long error);

	// Common helpers
	void ConfigureSession(CURL *easy_handle, CurlNetworkSession *session);

	bool initialized_;
	int still_running_;
	CURLM *multi_handle_;

	// Cancelable timeout callback
	nbase::WeakCallbackFlag timeout_cb_weakflag_;

	// The sessions will be insert to the |sessions_| once AddSession called.
	// When a socket for the session is opened, the socket and session then
	// will be insert to the |sockets_| map.
	std::set<SessionScopedRefPtr> sessions_;

	// We allow WSA_MAXIMUM_WAIT_EVENTS-1 sessions to run simultaneously
	// at most, the remaining will be in the pending set
	std::list<SessionScopedRefPtr> pending_sessions_;

	// The proxy of the message loop in which we are running
	std::shared_ptr<nbase::MessageLoopProxy> message_loop_;

	DISALLOW_COPY_AND_ASSIGN(CurlNetworkSessionManager);
};

} // namespace net

#endif // NET_CURL_CURL_NETWORK_SESSION_MANAGER_UV_H_