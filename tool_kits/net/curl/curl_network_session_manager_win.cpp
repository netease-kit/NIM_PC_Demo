// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/29
//
// Curl network session manager for the Windows platform
// The manager MUST running on a MessageLoop based thread

#include "net/curl/curl_network_session_manager_win.h"

#include <algorithm>

#include "base/log/log.h"
#include "base/synchronization/lock.h"
#include "base/util/bind.h"
#include "base/thread/framework_thread.h"
#include "base/third_party/chrome/atomicops.h"
#include "net/socket/socket_util.h"

namespace net
{
namespace
{

const size_t kMaxSockets = WSA_MAXIMUM_WAIT_EVENTS;
const size_t kEventPoolSize = 8; // The value should be less than |kMaxSockets|

bool PeekSocketData(SOCKET socket, int action)
{
	bool would_block = false;
	if (action & CURL_CSELECT_IN) {
		char buffer;
		if (recv(socket, &buffer, 0, 0) == SOCKET_ERROR &&
			WSAGetLastError() == WSAEWOULDBLOCK)
			would_block = true;
	}
	if (action & CURL_CSELECT_OUT) {
		char buffer;
		if (send(socket, &buffer, 0, 0) == SOCKET_ERROR &&
			WSAGetLastError() == WSAEWOULDBLOCK)
			would_block = true;
	}
	return !would_block;
}

} // namespace

class CurlNetworkSessionManager::Watcher
	: public nbase::ThreadSafeRefCount,
	  protected nbase::Thread
{
public:
	explicit Watcher(CurlNetworkSessionManager *manager);
	~Watcher();

	bool Start();
	void Stop();
	// Add a session
	bool Add(SessionScopedRefPtr session, SOCKET socket, long events);
	// Remove a session from the watching list
	bool Remove(SessionScopedRefPtr session, SOCKET socket);
	// Modify a the interested events of a socket
	bool Modify(SessionScopedRefPtr session, SOCKET socket, long events);
	// A simple post task method
	void PostTask(const nbase::Closure &task);

private:
	struct WatchContext
	{
		long selected_events;
		SOCKET socket;
		SessionScopedRefPtr session;
	};

	void Run();
	void Wakeup();
	bool Select(size_t index, long &events);
	bool LookupEvents(size_t index, long &events, long &errors);

	void DoAdd(SessionScopedRefPtr session, SOCKET socket, long events);
	void DoRemove(SessionScopedRefPtr session, SOCKET socket);
	void DoRemoveHelper(size_t index);
	void DoModify(SessionScopedRefPtr session, SOCKET socket, long events);
	void WaitAndProcessEvents();
	void NotifyEvent(size_t index);
	void NotifyError(size_t index);

	WSAEVENT GetCachedEvent();
	void FreeCachedEvent(WSAEVENT event);

	bool should_quit_;
	size_t num_wait_events_;
	size_t num_cached_events_;
	WSAEVENT wait_events_[kMaxSockets];
	WatchContext wait_content_[kMaxSockets];
	CurlNetworkSessionManager *manager_;

	base::subtle::Atomic32 have_work_;
	nbase::NLock queue_lock_;
	std::list<nbase::Closure> incoming_queue_;
	std::list<nbase::Closure> task_queue_;

	DISALLOW_COPY_AND_ASSIGN(Watcher);
};

CurlNetworkSessionManager::Watcher::Watcher(CurlNetworkSessionManager *manager)
	: manager_(manager), should_quit_(false), num_wait_events_(1),
	  num_cached_events_(0), have_work_(0)
{
	memset(wait_events_, 0, sizeof(wait_events_));
	memset(wait_content_, 0, sizeof(wait_content_));
	wait_events_[0] = WSACreateEvent();
}

CurlNetworkSessionManager::Watcher::~Watcher()
{
	Stop();
	WSACloseEvent(wait_events_[0]);
	while (num_cached_events_ > 0) {
		WSACloseEvent(GetCachedEvent());
	}
}

bool CurlNetworkSessionManager::Watcher::Start()
{
	return Thread::Create();
}

void CurlNetworkSessionManager::Watcher::Stop()
{
	if (should_quit_)
		return; // |Stop| is already called
	should_quit_ = true;
	manager_ = nullptr;
	Wakeup();
	Thread::Close();
}

void CurlNetworkSessionManager::Watcher::Wakeup()
{
	WSASetEvent(wait_events_[0]);
}

void CurlNetworkSessionManager::Watcher::PostTask(const nbase::Closure &task)
{
	{
		nbase::NAutoLock lock(&queue_lock_);
		incoming_queue_.push_back(task);
	}

	if (base::subtle::NoBarrier_AtomicExchange(&have_work_, 1) == 0)
		Wakeup();
}

void CurlNetworkSessionManager::Watcher::Run()
{
#ifndef NDEBUG
#if defined(COMPILER_MSVC)
	nbase::SetThreadName(GetCurrentThreadId(), "AsyncIOWatcher");
#endif
#endif
	while (!should_quit_) {
		if (task_queue_.empty()) {
			nbase::NAutoLock lock(&queue_lock_);
			task_queue_.swap(incoming_queue_);
		}
		if (!task_queue_.empty()) {
			nbase::Closure task = task_queue_.front();
			task_queue_.pop_front();
			if (!task.is_null()) {
				task.Run();
				continue;
			}
		}
		WaitAndProcessEvents();
	}
}

void CurlNetworkSessionManager::Watcher::WaitAndProcessEvents()
{
	if (should_quit_)
		return;

	base::subtle::Release_Store(&have_work_, 0);
	WSAResetEvent(wait_events_[0]);

	if (base::subtle::NoBarrier_AtomicExchange(&have_work_, 0) == 1)
		return;

	DWORD wait = WSAWaitForMultipleEvents(num_wait_events_,
										  wait_events_,
										  FALSE,
										  INFINITE, // Timeout
										  FALSE);
	if (wait == WSA_WAIT_EVENT_0 ) {
		// Wakeup
	} else if (wait == WSA_WAIT_TIMEOUT) {
		// Wait timeout
		//DLOG_APP("Wait timeout");
	} else if (wait > WSA_WAIT_EVENT_0 &&
		wait < WSA_WAIT_EVENT_0 + num_wait_events_) {
		NotifyEvent(wait-WSA_WAIT_EVENT_0);
	} else {
		// Wait failed
		for (size_t i = 1; i < num_wait_events_; i++) {
			if (WSA_WAIT_FAILED == WSAWaitForMultipleEvents(
					1, &wait_events_[i], FALSE, 0,	FALSE)) {
				LOG_ERR("Unexpected wait %d, error %u, #0x%p/%u/0x%p",
						wait,
						::GetLastError(),
						wait_content_[i].session.get(),
						wait_content_[i].socket,
						wait_events_[i]);
				DCHECK(false);
				break;
			}
		}
	}
}


bool CurlNetworkSessionManager::Watcher::Add(
	SessionScopedRefPtr session, SOCKET socket, long events)
{
	DCHECK(session != nullptr);
	DCHECK(events != 0);
	DCHECK(socket != INVALID_SOCKET);

	if (should_quit_)
		return false;

	if (!manager_ || session == nullptr || !events || socket == INVALID_SOCKET)
		return false;
	
	DCHECK(manager_->message_loop_->BelongsToCurrentThread());

	PostTask(nbase::Bind(&CurlNetworkSessionManager::Watcher::DoAdd,
						 this,
						 session,
						 socket,
						 events));

	return true;
}

void CurlNetworkSessionManager::Watcher::DoAdd(
	SessionScopedRefPtr session, SOCKET socket, long events)
{
	if (should_quit_)
		return;

	DCHECK(events != 0);
	DCHECK(num_wait_events_ < kMaxSockets);
	DCHECK(num_wait_events_ + num_cached_events_ <= kMaxSockets);
		
	wait_content_[num_wait_events_].session = session;
	wait_content_[num_wait_events_].socket = socket;
	wait_content_[num_wait_events_].selected_events = 0;
	wait_events_[num_wait_events_] = GetCachedEvent();

	DCHECK(wait_events_[num_wait_events_]);
	if (!Select(num_wait_events_, events)) {
		NotifyError(num_wait_events_);
		DoRemoveHelper(num_wait_events_);
		return;
	}
	SetupTcpSocket(socket);
	num_wait_events_++;
	LOG_PRO("Watch +%ded, #0x%p/%u", events, session.get(), socket);
}

bool CurlNetworkSessionManager::Watcher::Select(size_t index, long &events)
{
	if (events == 0) // Use old events
		events = wait_content_[index].selected_events;
	if (wait_content_[index].selected_events != events) {
		DCHECK(events != 0);
		if (WSAEventSelect(wait_content_[index].socket,
						   wait_events_[index],
						   events|FD_CLOSE|FD_CONNECT)) {
			LOG_ERR("Select failed #0x%p/%u, Error %u",
					wait_content_[index].session.get(),
					wait_content_[index].socket,
					WSAGetLastError());
			return false;
		}
		wait_content_[index].selected_events = events;
	}
	return true;
}

bool CurlNetworkSessionManager::Watcher::Remove(
	SessionScopedRefPtr session, SOCKET socket)
{
	if (!manager_ || session == nullptr)
		return false;
	
	DCHECK(manager_->message_loop_->BelongsToCurrentThread());

	PostTask(nbase::Bind(&CurlNetworkSessionManager::Watcher::DoRemove,
						 this,
						 session,
						 socket));
	return true;
}

void CurlNetworkSessionManager::Watcher::DoRemove(
	SessionScopedRefPtr session, SOCKET socket)
{
	if (should_quit_)
		return;

	if (socket == INVALID_SOCKET) {
		for (size_t i = 1; i < num_wait_events_;) {
			if (wait_content_[i].session == session) {
				num_wait_events_--;
				DoRemoveHelper(i);
				LOG_PRO("Watch -ed I #0x%p/%u", session.get(), socket);
				continue;
			}
			i++;
		}
	} else {
		for (size_t i = 1; i < num_wait_events_; i++) {
			if (wait_content_[i].socket == socket &&
				wait_content_[i].session == session) {
				num_wait_events_--;
				DoRemoveHelper(i);
				LOG_PRO("Watch -ed II #0x%p/%u", session.get(), socket);
				break;
			}
		}
	}
}

// You MUST assure |num_wait_events_| is the last index of event
// before use the method
void CurlNetworkSessionManager::Watcher::DoRemoveHelper(size_t index)
{
	// The event handle at |num_wait_events_| may be overwrite by
	// |FreeCachedEvent| so we have to store it first.
	WSAEVENT event = wait_events_[num_wait_events_];
	// Detach the event from the socket
	WSAEventSelect(wait_content_[index].socket, NULL, 0);
	// Recyle the event object
	FreeCachedEvent(wait_events_[index]);

	if (index == num_wait_events_) {
		wait_content_[index].session = nullptr; // Release old
		return; // Need not overwrite
	}
	
	wait_content_[index].socket = wait_content_[num_wait_events_].socket;
	wait_content_[index].session = wait_content_[num_wait_events_].session;
	wait_content_[index].socket = wait_content_[num_wait_events_].socket;
	wait_content_[num_wait_events_].session = nullptr; // Release old

	wait_events_[index] = event;
}

WSAEVENT CurlNetworkSessionManager::Watcher::GetCachedEvent()
{
	DCHECK(num_wait_events_ + num_cached_events_ <= kMaxSockets);
	if (num_cached_events_ > 0) {
		WSAEVENT event = wait_events_[kMaxSockets-num_cached_events_];
		DCHECK(event != nullptr);
		wait_events_[kMaxSockets-num_cached_events_--] = 0;
		WSAResetEvent(event);
		return event;
	}
	return WSACreateEvent();
}

void CurlNetworkSessionManager::Watcher::FreeCachedEvent(WSAEVENT event)
{
	DCHECK(event);
	if (event == nullptr)
		return;

	if (num_cached_events_ >= kEventPoolSize) {
		WSACloseEvent(event);
		return;
	}
	DCHECK(num_wait_events_+num_cached_events_ < kMaxSockets);
	wait_events_[kMaxSockets-++num_cached_events_] = event;
}

bool CurlNetworkSessionManager::Watcher::Modify(
	SessionScopedRefPtr session, SOCKET socket, long events)
{
	DCHECK(session != nullptr);
	DCHECK(events != 0);

	if (should_quit_)
		return false;

	if (!manager_ || session == nullptr || !events)
		return false;
	
	DCHECK(manager_->message_loop_->BelongsToCurrentThread());

	PostTask(nbase::Bind(&CurlNetworkSessionManager::Watcher::DoModify,
						 this,
						 session,
						 socket,
						 events));

	Wakeup();

	return true;
}

void CurlNetworkSessionManager::Watcher::DoModify(
	SessionScopedRefPtr session, SOCKET socket, long events)
{
	if (should_quit_)
		return;

	for (size_t index = 1; index < num_wait_events_; index++) {
		if (wait_content_[index].socket == socket &&
			wait_content_[index].session == session) {
			if (!Select(index, events)) {
				NotifyError(index);
			}
			LOG_PRO("Watch *%ded, #0x%p/%u", events, session.get(), socket);
			return;
		}
	}

	DCHECK(false);
	LOG_ERR("Try to modify a unregistered session, #0x%p/%u",
		session.get(), socket);
}

void CurlNetworkSessionManager::Watcher::NotifyEvent(size_t index)
{
	if (manager_ == nullptr)
		return;
	long events, errors;
	if (!LookupEvents(index, events, errors)) {
		if (WSAGetLastError() != WSAENOTSOCK)
			NotifyError(index);
		LOG_ERR("Lookup events failed #0x%p/%u",
			wait_content_[index].session.get(), wait_content_[index].socket);
		return;
	}
	if (events == 0) {
		// Maybe a purious wakeup?
		// https://groups.google.com/forum/#!topic/alt.winsock.programming/55Cp1qBNaTE
		return;
	}

	//LOG_PRO("Notify %d, #0x%p/%u",
	//	events, wait_content_[index].session, wait_content_[index].socket);

	manager_->message_loop_->PostTask(
		nbase::Bind(&CurlNetworkSessionManager::OnSocketEvent,
					manager_->weak_this_,
					wait_content_[index].session,
					wait_content_[index].socket,
					events,
					errors));
}

void CurlNetworkSessionManager::Watcher::NotifyError(size_t index)
{
	manager_->message_loop_->PostTask(
		nbase::Bind(&CurlNetworkSessionManager::OnSocketWatchError,
					manager_->weak_this_,
					wait_content_[index].session));
}

bool CurlNetworkSessionManager::Watcher::LookupEvents(
	size_t index, long &events, long &errors)
{
	events = 0;
	errors = 0;

	WSANETWORKEVENTS net_events;
	if (!WSAEnumNetworkEvents(wait_content_[index].socket,
							  wait_events_[index],
							  &net_events)) {
		events = net_events.lNetworkEvents;
		if (net_events.lNetworkEvents & FD_READ) {
			if (net_events.iErrorCode[FD_READ_BIT] != 0) {
				errors |= FD_READ;
			}
		}
		if (net_events.lNetworkEvents & FD_WRITE) {
			if (net_events.iErrorCode[FD_WRITE_BIT] != 0) {
				errors |= FD_WRITE;
			}
		}
		if (net_events.lNetworkEvents & FD_CLOSE) {
			if (net_events.iErrorCode[FD_CLOSE_BIT] != 0) {
				errors |= FD_CLOSE;
			}
		}
		if (net_events.lNetworkEvents & FD_CONNECT) {
			if (net_events.iErrorCode[FD_CONNECT_BIT] != 0) {
				errors |= FD_CONNECT;
			}
		}
		return true;
	} else {
		DWORD error = WSAGetLastError();
		DLOG_PRO("Enum events failed %u #0x%p/%u", error,
			wait_content_[index].session.get(), wait_content_[index].socket);
		WSASetLastError(error);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

CurlNetworkSessionManager::CurlNetworkSessionManager(
	nbase::MessageLoopProxy *message_loop) : initialized_(false),
	still_running_(0), message_loop_(message_loop), weak_factory_(this)
{
	DCHECK(message_loop_ != nullptr);
	if (message_loop_ != nullptr) {
		message_loop_->PostTask(
			nbase::Bind(&CurlNetworkSessionManager::DoInit,
						nbase::Unretained(this)));
	}
}

void CurlNetworkSessionManager::DoInit()
{
	weak_this_ = weak_factory_.GetWeakPtr();

	// Multi-threading Issues:
	// The first basic rule is that you must never simultaneously share a
	// libcurl handle (be it easy or multi or whatever) between multiple
	// threads. Only use one handle in one thread at any time. You can pass the
	// handles around among threads, but you must never use a single handle from
	// more than one thread at any given time.
	// See below:
	// http://curl.haxx.se/libcurl/c/libcurl-tutorial.html

	multi_handle_ = curl_multi_init();
	if (multi_handle_ != nullptr) {
		curl_multi_setopt(multi_handle_,
						  CURLMOPT_SOCKETFUNCTION,
						  CurlSocketEventCB);
		curl_multi_setopt(multi_handle_, CURLMOPT_SOCKETDATA, this);
		curl_multi_setopt(multi_handle_, CURLMOPT_TIMERFUNCTION, CurlTimerCB);
		curl_multi_setopt(multi_handle_, CURLMOPT_TIMERDATA, this);
		curl_multi_setopt(multi_handle_,
						  CURLMOPT_MAX_TOTAL_CONNECTIONS,
						  kMaxSockets-1);
	}

	CreateWatcher();

	initialized_ = true;
}

CurlNetworkSessionManager::~CurlNetworkSessionManager()
{
	DestroyWatcher();
	std::for_each(
		sessions_.begin(), sessions_.end(), [&](CurlNetworkSession *session) {
		session->DestroyCurlEasyHandle(multi_handle_);
		session->manager_ = nullptr;
	});
	sessions_.clear();
	if (multi_handle_ != nullptr) {
		curl_multi_cleanup(multi_handle_);
	}
}

void CurlNetworkSessionManager::CreateWatcher()
{
	if (watcher_ == nullptr) {
		watcher_ = new Watcher(this);
		watcher_->Start();
	}
}

void CurlNetworkSessionManager::DestroyWatcher()
{
	if (watcher_ != nullptr) {
		watcher_->Stop();
		watcher_ = nullptr;
	}
}

void CurlNetworkSessionManager::ConfigureSession(
	CURL *easy_handle, CurlNetworkSession *session)
{	
#ifndef NDEBUG
	curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 1L);
#else
	curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 0L);
#endif
	curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, session);
}

void CurlNetworkSessionManager::AddSession(CurlNetworkSession *session)
{
	DCHECK(session);
	DCHECK(initialized_);
	DCHECK(message_loop_ != nullptr);
	// The session should not be managed
	DCHECK(session->manager_ == nullptr);

	if (!session || session->manager_) {
		LOG_WAR("0x%p is NULL or already managed", session);
		return;
	}

	message_loop_->PostTask(
		nbase::Bind(&CurlNetworkSessionManager::DoAddSession,
					weak_this_,
					SessionScopedRefPtr(session)));
}

void CurlNetworkSessionManager::DoAddSession(
	const SessionScopedRefPtr &session)
{
	DCHECK(message_loop_->BelongsToCurrentThread());
	DCHECK(session != nullptr);
	DCHECK(session->easy_handle_ == nullptr);

	if (session == nullptr || !multi_handle_)
		return;

	session->manager_ = this;

	if (!session->CreateCurlEasyHandle()) {
		session->OnError();
		return;
	}

	// We will overwrite the following options even modified by the session
	ConfigureSession(session->easy_handle_, session);

	pending_sessions_.push_back(session);
	session->OnRegisered();

	StartNextSession();
}

void CurlNetworkSessionManager::RemoveSession(CurlNetworkSession *session)
{
	DCHECK(initialized_);
	DCHECK(message_loop_ != nullptr);
	message_loop_->PostTask(
		nbase::Bind(&CurlNetworkSessionManager::DoRemoveSession,
					weak_this_,
					SessionScopedRefPtr(session)));
}

void CurlNetworkSessionManager::DoRemoveSession(
	const SessionScopedRefPtr &session)
{
	DCHECK(initialized_);
	DCHECK(message_loop_->BelongsToCurrentThread());
	session->set_completed(true);
	auto iter = sessions_.find(session);
	DCHECK(iter != sessions_.end());
	if (iter != sessions_.end()) {
		watcher_->Remove(session, INVALID_SOCKET);
		session->DestroyCurlEasyHandle(multi_handle_);
		session->manager_ = nullptr;
		sessions_.erase(iter);
	}

	StartNextSession();
}

void CurlNetworkSessionManager::StartNextSession()
{
	// Start the next task
	message_loop_->PostTask(
		nbase::Bind(&CurlNetworkSessionManager::DoStartNextSession,
		weak_this_));
}

void CurlNetworkSessionManager::DoStartNextSession()
{
	if (sessions_.size() >= kMaxSockets-1)
		return;

	if (pending_sessions_.empty())
		return;

	SessionScopedRefPtr session = *pending_sessions_.begin();
	sessions_.insert(session);
	pending_sessions_.erase(pending_sessions_.begin());

	session->attached_ = true;
	CURLMcode rc = curl_multi_add_handle(multi_handle_, session->easy_handle_);

	if (CURLM_OK != rc) {
		LOG_ERR("Add easy handle failed(%u)", rc);
		session->attached_ = false;
		DoRemoveSession(session);
		return;
	}

	// Note that the add_handle() will set a time-out to trigger
	// very soon so that the necessary socket_action() call will be called by
	// this app
}

void CurlNetworkSessionManager::CompleteSessionAndRemoveSoon(
	const SessionScopedRefPtr &session, bool error)
{
	if (session->completed())
		return;
	session->set_completed(true);
	message_loop_->PostTask(
		nbase::Bind(&CurlNetworkSessionManager::DoCompleteSessionAndRemove,
		weak_this_,
		session,
		error));
}

void CurlNetworkSessionManager::DoCompleteSessionAndRemove(
	const SessionScopedRefPtr &session, bool error)
{
	if (error)
		session->OnError();
	else
		session->OnTransferDone();
	DoRemoveSession(session);
}

int CurlNetworkSessionManager::CurlSocketEventCB(CURL *e,
												 curl_socket_t s,
												 int action,
												 void *userp,
												 void *sockp)
{
	CurlNetworkSessionManager *manager =
		reinterpret_cast<CurlNetworkSessionManager *>(userp);
	CurlNetworkSession *session = nullptr;
	if (CURLE_OK != curl_easy_getinfo(e, CURLINFO_PRIVATE, &session))
		return 0;

	DCHECK(manager);
	DCHECK(session);

	if (session == nullptr) {
		DCHECK(false);
		return 0;
	}

	LOG_PRO("cURL action %d, #0x%p/%u", action, session, s);

	long events = 0;
	switch (action) {
	case CURL_POLL_IN:
		events = FD_READ;
		break;
	case CURL_POLL_OUT:
		events = FD_WRITE;
		break;
	case CURL_POLL_INOUT:
		events = FD_READ|FD_WRITE;
		break;
	case CURL_POLL_REMOVE:
		manager->RemoveSocketEventWatch(SessionScopedRefPtr(session), s);
		curl_multi_assign(manager->multi_handle_, s, nullptr);
		LOG_PRO("cURL will remove #0x%p/%u", session, s);
		break;
	}

	if (events != 0) {
		if (sockp == nullptr) {
			// New socket
			curl_multi_assign(manager->multi_handle_, s, (void *)action);
			manager->AddSocketEventWatch(
				SessionScopedRefPtr(session), s, events);
		} else if ((int)sockp != action) {
			curl_multi_assign(manager->multi_handle_, s, (void *)action);
			manager->ModifySocketEventWatch(
				SessionScopedRefPtr(session), s, events);
		}
	}

	return 0;
}

void CurlNetworkSessionManager::AddSocketEventWatch(
	const SessionScopedRefPtr &session, SOCKET socket, long events)
{
	if (session->completed()) {
		return;
	}

	if (watcher_ == nullptr)
		return; // The manager is being destroyed
	socket_actions_[socket] = events;
	watcher_->Add(session, socket, events);
	LOG_PRO("Watch +%d, #0x%p/%u",
			events,
			session.get(),
			socket);
}

void CurlNetworkSessionManager::RemoveSocketEventWatch(
	const SessionScopedRefPtr &session, SOCKET socket)
{
	if (watcher_ == nullptr)
		return; // The manager is being destroyed
	auto iter = socket_actions_.find(socket);
	if (iter != socket_actions_.end())
		socket_actions_.erase(iter);
	watcher_->Remove(session, socket);
	LOG_PRO("Watch -, #0x%p/%u",
			session.get(),
			socket);
}

void CurlNetworkSessionManager::ModifySocketEventWatch(
	const SessionScopedRefPtr &session, SOCKET socket, long events)
{
	if (session->completed()) {
		return;
	}

	if (watcher_ == nullptr)
		return; // The manager is being destroyed
	socket_actions_[socket] = events;
	watcher_->Modify(session, socket, events);
	LOG_PRO("Watch *%d, #0x%p/%u",
			events,
			session.get(),
			socket);
}

void CurlNetworkSessionManager::OnSocketEvent(
	const SessionScopedRefPtr &session, SOCKET socket, long events, long errors)
{
	if (session == nullptr || session->completed()) {
		return;
	}

	LOG_PRO("Event %d/%d, #0x%p/%u", events, errors, session.get(), socket);

	int action = 0;
	if (errors != 0) {
		action = CURL_CSELECT_ERR;
	} else {
		if (events & FD_READ)
			action |= CURL_CSELECT_IN;
		if (events & CURL_CSELECT_OUT)
			action |= CURL_CSELECT_OUT;
		//if (events == FD_CONNECT) {
		//	return; // Notification only
		//}
		//if (events == FD_CLOSE)
		//	action = CURL_CSELECT_ERR;
	}

	// When the events on a socket are unknown, pass 0 instead,
	// and libcurl will test the descriptor internally.
	PerformMultiSocketAction(session, socket, action);
}

void CurlNetworkSessionManager::PerformMultiSocketAction(
	const SessionScopedRefPtr &session, SOCKET socket, int action)
{
	if (session->completed())
		return;

	CURLMcode rc = curl_multi_socket_action(multi_handle_,
											socket,
											action,
											&still_running_);

	CheckMultiRunningStatus();

	if (still_running_ <= 0) {
		LOG_PRO("The last transfer done, kill timeout");
		timeout_cb_.Cancel();
	}

	if (rc != CURLM_OK && rc != CURLM_CALL_MULTI_PERFORM) {
		LOG_ASS("Multi action failed: %d", rc);
		CompleteSessionAndRemoveSoon(session, true);
		return;
	}

	int action_to_lookup = action & (CURL_CSELECT_IN|CURL_CSELECT_OUT);
	if (action_to_lookup != 0) {
		auto iter = socket_actions_.find(socket);
		if (iter != socket_actions_.end()) {
			action_to_lookup &= iter->second;
		}
	}

	if (action_to_lookup != 0) {
		if (PeekSocketData(socket, action_to_lookup)) {
			message_loop_->PostTask(nbase::Bind(
				&CurlNetworkSessionManager::PerformMultiSocketAction,
				weak_this_,
				session,
				socket,
				action));
		}
	}
}

void CurlNetworkSessionManager::OnSocketWatchError(
	const SessionScopedRefPtr &session)
{
	watcher_->Remove(session, INVALID_SOCKET);
	if (sessions_.find(session) != sessions_.end())
		CompleteSessionAndRemoveSoon(session, true);
}

void CurlNetworkSessionManager::CurlTimerCB(CURLM *multi,
											long timeout_ms,
											void *userp)
{
	LOG_PRO("cURL set timeout: %ld", timeout_ms);
	CurlNetworkSessionManager *manager =
		reinterpret_cast<CurlNetworkSessionManager *>(userp);
	DCHECK(manager);
	DCHECK(manager->multi_handle_ == multi);

	manager->SetTimeout(timeout_ms);
}

void CurlNetworkSessionManager::SetTimeout(long timeout_ms)
{
	timeout_cb_.Cancel();
	if (timeout_ms <= 0) {
		OnTimeout();
	} else {
		timeout_cb_.Reset(nbase::Bind(&CurlNetworkSessionManager::OnTimeout,
									  weak_this_));
		message_loop_->PostTask(timeout_cb_.callback());
	}
}

void CurlNetworkSessionManager::OnTimeout()
{
	PerformTimeoutAction();
}

void CurlNetworkSessionManager::PerformTimeoutAction()
{
	CURLMcode rc = curl_multi_socket_action(multi_handle_,
											CURL_SOCKET_TIMEOUT,
											0,
											&still_running_);

	if (rc != CURLM_OK) {
		LOG_ASS("Timeout action failed: %d", rc);
		return;
	}

	CheckMultiRunningStatus();
}

void CurlNetworkSessionManager::CheckMultiRunningStatus()
{
	int msgs_left = 0;
	CURLMsg *msg = nullptr;
	while (msg = curl_multi_info_read(multi_handle_, &msgs_left)) {
		if (msg->msg == CURLMSG_DONE) {
			CurlNetworkSession *session = nullptr;
			if (CURLE_OK != curl_easy_getinfo(msg->easy_handle,
											  CURLINFO_PRIVATE,
											  &session)) {
				LOG_ERR("Read multi info failed.");
				break;
			}
			if (session != nullptr) {
				if (msg->data.result == CURLE_OK) {
					CompleteSessionAndRemoveSoon(session, false);
					LOG_PRO("#0x%p completed, %d running",
							session,
							still_running_);
				} else {
					CompleteSessionAndRemoveSoon(session, true);
					LOG_ERR("#0x%p error %d, %d running",
							session,
							msg->data.result,
							still_running_);
				}
			}
		}
	}
}

} // namespace net
