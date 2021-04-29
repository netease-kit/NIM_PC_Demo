// Copyright (c) 2014, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2014/1/24
//
// IO message pump with libuv

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif // To fix Warning C2894

#include "net/curl/libuv_message_loop.h"

#include <process.h>
#include "net/log/log.h"
#include "base/util/string_util.h"

#include "base/compiler_specific.h"
MSVC_PUSH_DISABLE_WARNING(4201)
#undef _WINSOCKAPI_
#include "third_party/libuv/include/uv.h"
MSVC_POP_WARNING()

namespace net
{

static void OnUVAsync(uv_async_t* handle, int status)
{
	// Nothing to do
}

LibuvMessagePump::LibuvMessagePump()
	: running_(false), should_quit_(false), event_processed_(false)
{
	loop_ = uv_loop_new();
	wakuper_ = new uv_async_t;
	timer_ = new uv_timer_t;
	uv_async_init(loop_, wakuper_, OnUVAsync);
	uv_timer_init(loop_, timer_);
}

LibuvMessagePump::~LibuvMessagePump()
{
	// TODO(rtwang):
	// Recycle running watcher's to avoid memory leak

	uv_timer_stop(timer_);
	delete timer_;
	delete wakuper_;
	uv_loop_delete(loop_);
	loop_ = nullptr;
	timer_ = nullptr;
	wakuper_ = nullptr;
}

bool LibuvMessagePump::WatchFileDescriptor(Watcher *watcher, Event mode)
{
	if (watcher == nullptr)
		return false;
	if (!watcher->StartPoll(this, mode)) {
		return false;
	}
	return true;
}

void LibuvMessagePump::AddObserver(IOObserver* obs)
{
	io_observers_.AddObserver(obs);
}

void LibuvMessagePump::RemoveObserver(IOObserver* obs)
{
	io_observers_.RemoveObserver(obs);
}

static void OnUVTimer(uv_timer_t* handle, int status)
{
	// Nothing to do
}

void LibuvMessagePump::Run(Delegate* delegate)
{
	running_ = true;

	for (;;) {
		bool did_work = delegate->DoWork();
		if (should_quit_ || loop_ == nullptr)
			break;
		// Poll for new events once but don't block if there are no pending
		// events
		uv_run(loop_, UV_RUN_NOWAIT);
		did_work |= event_processed_;
		event_processed_ = false;
		if (should_quit_)
			break;

		did_work |= delegate->DoDelayedWork(&delayed_work_time_);
		if (should_quit_)
			break;

		if (did_work)
			continue;

		did_work = delegate->DoIdleWork();
		if (should_quit_)
			break;

		if (did_work)
			continue;

		// UV_RUN_ONCE tells libuv to only block once,
		// but to service all pending events when it wakes up.
		if (delayed_work_time_.is_null()) {
			uv_run(loop_, UV_RUN_ONCE);
		} else {
			nbase::TimeDelta delay = delayed_work_time_ - nbase::TimeTicks::Now();
			if (delay > nbase::TimeDelta()) {
				uv_timer_start(timer_, OnUVTimer, delay.ToMilliseconds(), 0);
				uv_run(loop_, UV_RUN_ONCE);
				uv_timer_stop(timer_);
			} else {
				// It looks like delayed_work_time_ indicates a time in the
				// past, so we need to call DoDelayedWork now.
				delayed_work_time_ = nbase::TimeTicks();
			}
		}
	}

	should_quit_ = false;
}

void LibuvMessagePump::Quit()
{
	if (running_) {
		should_quit_ = true;
		ScheduleWork();
	}
}

void LibuvMessagePump::ScheduleWork()
{
	uv_async_send(wakuper_);
}

void LibuvMessagePump::ScheduleDelayedWork(const nbase::TimeTicks& delayed_work_time)
{
	// We know that we can't be blocked on Wait right now since this method can
	// only be called on the same thread as Run, so we only need to update our
	// record of how long to sleep when we do sleep.
	delayed_work_time_ = delayed_work_time;
}

void LibuvMessagePump::WillProcessIOEvent()
{
	AutoLazyEraser lazy_eraser(&io_observers_);
	size_t index = 0;
	IOObserver* observer;
	while (index < io_observers_.GetObserverCount())
	{
		observer = io_observers_.GetObserver(index++);
		if (observer == NULL)
			continue;
		observer->WillProcessIOEvent();
	}
}

void LibuvMessagePump::DidProcessIOEvent()
{
	AutoLazyEraser lazy_eraser(&io_observers_);
	size_t index = 0;
	IOObserver* observer;
	while (index < io_observers_.GetObserverCount())
	{
		observer = io_observers_.GetObserver(index++);
		if (observer == NULL)
			continue;
		observer->DidProcessIOEvent();
	}
}

void LibuvMessagePump::OnLibuvNotification(uv_poll_t *req,
										   int status,
										   int events)
{
	Watcher *watcher = static_cast<Watcher *>(req->data);
	LibuvMessagePump *pump = watcher->pump();

	//	DCHECK(watcher);
	//	DCHECK(pump != nullptr);
	pump->WillProcessIOEvent();
	watcher->OnNotification(watcher->fd_, (Event)events, status == -1);
	pump->event_processed_ = true;
	pump->DidProcessIOEvent();
}

LibuvMessagePump::Watcher::Watcher(int fd, bool socket)
	: fd_(fd), is_socket_(socket),
	  closing_(false), pump_(nullptr), handle_(nullptr)
{

}

LibuvMessagePump::Watcher::~Watcher()
{
	// We should not call the destructor directly
	//	DCHECK(closing_);
	// Free the uv handle
	delete handle_;
}

bool LibuvMessagePump::Watcher::StartPoll(LibuvMessagePump *pump, Event mode)
{
	//	DCHECK(!closing_);
	if (pump_ != nullptr) {
		//		DCHECK(pump_ == pump);
		if (pump_ != pump) {
			LOG_ERR(L"Alreay attached to another pump");
			return false;
		}
	}
	// Allocate a handle if we have not do so.
	if (handle_ == NULL) {
		handle_ = new uv_poll_t;
		int r = is_socket_ ?
			uv_poll_init_socket(pump->loop_, handle_, fd_) :
			uv_poll_init(pump->loop_, handle_, fd_);
		if (r) {
			LOG_ERR(L"Init socket failed");
			return false;
		}
		handle_->data = this;
	}
	// Change poll mode
	if (uv_poll_start(handle_, mode, LibuvMessagePump::OnLibuvNotification)) {
		LOG_ERR(L"Poll start failed");
		return false;
	}
	pump_ = pump;
	return true;
}

void LibuvMessagePump::Watcher::OnUVClose(uv_handle_t *handle)
{
	LibuvMessagePump::Watcher *self =
		reinterpret_cast<LibuvMessagePump::Watcher *>(handle->data);
	self->InternalRelease();
}

void LibuvMessagePump::Watcher::CloseAndAutoRelease()
{
	if (closing_)
		return;
	closing_ = true;
	if (pump_ == nullptr) {
		InternalRelease();
		return;
	}

	// The watcher is being referenced by libuv, so we should close it.
	if (!uv_is_closing((uv_handle_t *)handle_)) {
		uv_poll_stop(handle_);
		// Delayed close & delete
		uv_close((uv_handle_t *)handle_, OnUVClose);
	}
}

void LibuvMessagePump::Watcher::InternalRelease()
{
	OnBeingDestroyed();
	delete this;
}


UvMessageLoop::UvMessageLoop()
{ 
	pump_.reset(new LibuvMessagePump);
	type_ = kCustomMessageLoop;
}

void UvMessageLoop::AddToIncomingQueue(const PendingTask &task)
{
	// 本方法可能会在另一个线程中被执行，所以必须线程安全
	std::shared_ptr<nbase::MessagePump> pump;
	{
		nbase::NAutoLock lock(&incoming_queue_lock_);
		incoming_queue_.push(task);
		// 因为这函数可能是间接地在另一个线程中被调用的，
		// 此时MessageLoop中可能正有任务在运行，
		// 这些任务中可能包含销毁MessageLoop的任务，
		// 为了保证对MessageLoop中的MessagePump引用有效，
		// 这里需要用到引用指针
		pump = pump_;
	}
	pump->ScheduleWork();
}

void UvMessageLoop::AddIOObserver(IOObserver* observer)
{
	//	DCHECK(this == current());
	static_cast<LibuvMessagePump *>(pump())->AddObserver(observer);
}

void UvMessageLoop::RemoveIOObserver(IOObserver *observer)
{
	//	DCHECK(this == current());
	static_cast<LibuvMessagePump *>(pump())->RemoveObserver(observer);
}

bool UvMessageLoop::WatchFileDescriptor(Watcher *watcher, Event event)
{
	//	DCHECK(this == current());
	return static_cast<LibuvMessagePump *>(pump())->WatchFileDescriptor(watcher, event);
}


} // namespace nbase
