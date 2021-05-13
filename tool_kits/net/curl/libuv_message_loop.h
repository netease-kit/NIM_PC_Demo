// Copyright (c) 2014, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2014/1/24
//
// IO message pump with libuv

#ifndef NET_CURL_LIBUV_MESSAGE_LOOP_H_
#define NET_CURL_LIBUV_MESSAGE_LOOP_H_

#include "base/framework/message_loop.h"
#include "base/framework/message_pump.h"
#include "base/framework/observer_list.h"
#include "base/time/time.h"
#include "base/framework/observer_list.h"
#include "base/thread/framework_thread.h"

struct uv_poll_s;
struct uv_handle_s;
struct uv_loop_s;
struct uv_async_s;
struct uv_timer_s;
typedef struct uv_poll_s uv_poll_t;
typedef struct uv_loop_s uv_loop_t;
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_async_s uv_async_t;
typedef struct uv_timer_s uv_timer_t;

namespace net
{

class LibuvMessagePump : public nbase::MessagePump
{
public:
	enum Event
	{
		NONE = 0,
		READ = 1 << 0,
		WRITE = 1 << 1,
		READ_WRITE = READ | WRITE
	};

	class IOObserver
	{
	public:
		IOObserver() {}
		// An IOObserver is an object that receives IO notifications
		// from the MessagePump
		virtual void WillProcessIOEvent() = 0;
		virtual void DidProcessIOEvent() = 0;
	protected:
		virtual ~IOObserver() {}
	};

	class Watcher
	{
	public:
		// |fd| can be a socket or file,
		// |socket| is used to specified whether |fd| is a socket
		Watcher(int fd, bool socket = true);

		// |CloseAndAutoRelease| must be called when you don't wanner use a
		//       instance any longer
		// NOTE: When |CloseAndAutoRelease| is called:
		//       * If opened, the instance will be deleted later automatically
		//       * otherwise, the instance will be deleted immediately
		void CloseAndAutoRelease();

		// Called when data is ready for non-blocking read or write
		virtual void OnNotification(int fd, Event mode, bool error) = 0;

		// Called when the instance is being destroyed
		virtual void OnBeingDestroyed() {}

	protected:
		// The destructor should ONLY be called in OnUVClose
		virtual ~Watcher();

	private:
		friend class LibuvMessagePump;

		// Called by libuv
		static void OnUVClose(uv_handle_t *handle);
		// Called by LibuvMessagePump
		LibuvMessagePump* pump() const { return pump_; }
		// Start polling with specified mode according to |event|
		bool StartPoll(LibuvMessagePump *pump, Event event);
		// Release the current instance
		void InternalRelease();

		// A socket or a file depends on |is_socket_|
		int fd_;
		// The watcher is in closing state?
		bool closing_;
		// |true| if |fd_| is a socket, otherwise |fd_| is a file
		bool is_socket_;
		// UV poll handle
		uv_poll_t *handle_;
		// The message pump that listening/dispatching the watcher's events
		LibuvMessagePump *pump_;
	};

	LibuvMessagePump();
	virtual ~LibuvMessagePump();

	// The following method(s) must be called on the thread
	// that the pump is running on.
	//
	// |WatchFileDescriptor| start watching of a watcher.
	// This method can be called more than once, if the watcher is already
	// running, the method won't do anything.
	//
	// The pump will NOT own the |watcher| after this method called,
	// thus to stop watching, you should call the watcher's |Close| method.
	bool WatchFileDescriptor(Watcher *watcher, Event mode);

	// Observers operation
	void AddObserver(IOObserver* obs);
	void RemoveObserver(IOObserver* obs);

	// MessagePump methods:
	virtual void Run(Delegate* delegate);
	virtual void Quit();
	virtual void ScheduleWork();
	virtual void ScheduleDelayedWork(const nbase::TimeTicks& delayed_work_time);

private:
	void WillProcessIOEvent();
	void DidProcessIOEvent();

	// Called by libuv when data is ready for non-blocking read/write
	static void OnLibuvNotification(uv_poll_t *req, int status, int events);

	bool running_;
	bool should_quit_;
	bool event_processed_;

	// The time at which we should call DoDelayedWork.
	nbase::TimeTicks delayed_work_time_;
	// Internal uv loop
	uv_loop_t *loop_;
	// Use uv_async_send to wakup the event loop
	uv_async_t *wakuper_;
	// Used by uv_timer_start
	uv_timer_t *timer_;
	// Obervers
	ObserverList<IOObserver> io_observers_;
};

class UvMessageLoop : public nbase::FrameworkThread::CustomMessageLoop
{
public:
	typedef LibuvMessagePump::Event Event;
	typedef LibuvMessagePump::Watcher Watcher;
	typedef LibuvMessagePump::IOObserver IOObserver;

	UvMessageLoop();

	static UvMessageLoop* current()
	{
		return reinterpret_cast<UvMessageLoop *>(nbase::MessageLoop::current());
	}

	virtual void AddToIncomingQueue(const PendingTask &task);

	void AddIOObserver(IOObserver *observer);
	void RemoveIOObserver(IOObserver *observer);
	bool WatchFileDescriptor(Watcher *watcher, Event mode);
};

class UvMessageLoopFactiry : public nbase::FrameworkThread::CustomMessageLoopFactory
{
public:
	virtual UvMessageLoop* CreateMessageLoop() override
	{
		return (new UvMessageLoop);
	}
};



} // namespace nbase

#endif // NET_CURL_LIBUV_MESSAGE_LOOP_H_
