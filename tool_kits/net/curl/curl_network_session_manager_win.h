// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/29
//
// Curl network session manager for the Windows platform
// The manager MUST running on a MessageLoop based thread

#ifndef NET_CURL_CURL_NETWORK_SESSION_MANAGER_WIN_H_
#define NET_CURL_CURL_NETWORK_SESSION_MANAGER_WIN_H_

#include <list>
#include <map>
#include <set>
#include "base/framework/message_loop_proxy.h"
#include "base/memory/weak_ptr.h"
#include "base/util/cancelable_callback.h"
#include "net/curl/curl_network_session.h"

namespace net
{

class NET_EXPORT CurlNetworkSessionManager
{
public:
	explicit CurlNetworkSessionManager(nbase::MessageLoopProxy *message_loop);
	virtual ~CurlNetworkSessionManager();

	// Once a session is added to the manager,
	// its ownership is taken by the manager
	void AddSession(CurlNetworkSession *session);

	// Once a session is removed by the manager,
	// the manager will give the session's ownership up
	void RemoveSession(CurlNetworkSession *session);

	// The count of sessions still running
	int still_running() const { return still_running_; }

private:
	typedef nbase::scoped_refptr<CurlNetworkSession> SessionScopedRefPtr;
	class Watcher;
	static int CurlSocketEventCB(CURL *e,
								 curl_socket_t s,
								 int action,
								 void *userp,
								 void *sockp);
	static void CurlTimerCB(CURLM *multi, long timeout_ms, void *userp);

	void DoInit();
	void DoAddSession(const SessionScopedRefPtr &session);
	void DoRemoveSession(const SessionScopedRefPtr &session);

	void StartNextSession();
	void DoStartNextSession();
	void CompleteSessionAndRemoveSoon(const SessionScopedRefPtr &, bool error);
	void DoCompleteSessionAndRemove(const SessionScopedRefPtr &, bool error);

	void CheckMultiRunningStatus();
	void PerformMultiSocketAction(const SessionScopedRefPtr &session,
								  SOCKET socket,
								  int action);
	void PerformTimeoutAction();
	void SetTimeout(long timeout_ms);
	void OnTimeout();

	void CreateWatcher();	
	void DestroyWatcher();
	void AddSocketEventWatch(const SessionScopedRefPtr &session,
							 SOCKET socket,
							 long events);
	void RemoveSocketEventWatch(const SessionScopedRefPtr &session,
								SOCKET socket);
	void ModifySocketEventWatch(const SessionScopedRefPtr &session,
								SOCKET socket,
								long events);
	void OnSocketEvent(const SessionScopedRefPtr &session,
					   SOCKET socket,
					   long events,
					   long errors);
	void OnSocketWatchError(const SessionScopedRefPtr &session);

	// Common helpers
	void ConfigureSession(CURL *easy_handle, CurlNetworkSession *session);

	bool initialized_;
	int still_running_;
	CURLM *multi_handle_;

	// Cancelable timeout callback
	nbase::CancelableClosure timeout_cb_;

	// The socket events watcher
	nbase::scoped_refptr<Watcher> watcher_;

	// The sessions will be insert to the |sessions_| once AddSession called.
	// When a socket for the session is opened, the socket and session then
	// will be insert to the |sockets_| map.
	std::set<SessionScopedRefPtr> sessions_;

	// We allow WSA_MAXIMUM_WAIT_EVENTS-1 sessions to run simultaneously
	// at most, the remaining will be in the pending set
	std::list<SessionScopedRefPtr> pending_sessions_;

	// The proxy of the message loop in which we are running
	nbase::scoped_refptr<nbase::MessageLoopProxy> message_loop_;

	nbase::WeakPtrFactory<CurlNetworkSessionManager> weak_factory_;
	nbase::WeakPtr<CurlNetworkSessionManager> weak_this_;

	std::map<int,int> socket_actions_;

	DISALLOW_COPY_AND_ASSIGN(CurlNetworkSessionManager);
};

} // namespace net

#endif // NET_CURL_CURL_NETWORK_SESSION_MANAGER_WIN_H_