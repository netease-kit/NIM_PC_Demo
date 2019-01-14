// Copyright (c) 2011, NetEase Inc. All rights reserved.
//
// Author: Ruan Liang <ruanliang@corp.netease.com>
// Date: 2011/10/8
//

#ifndef BASE_TIME_TIMER_H_
#define BASE_TIME_TIMER_H_

#include "base/base_export.h"
#include "base/time/time.h"
#include "base/framework/task.h"
#include "base/thread/thread_manager.h"
#include <assert.h>
namespace nbase
{

//-----------------------------------------------------------------------------
// This class is an implementation detail of OneShotTimer and RepeatingTimer.
template <bool repeating>
class Timer 
{
public:
//	typedef void (Receiver::*ReceiverMethod)();

	bool IsRunning()
	{
		if (timer_flag_.HasUsed())
		{
			return true;
		}
		return false;
	}
	// Call this method to start the timer
	void Start(TimeDelta delay,const StdClosure& callback)
	{
		if (timer_flag_.HasUsed())
		{
			return;
		}
		delay_ = delay;
		callback_ = callback;
		InitiateDelayedTask(delay, callback);
	}

	// Call this method to stop the timer.  It is a no-op if the timer is not
	// running.
	void Stop()
	{
		OrphanDelayedTask();
	}

	// Call this method to reset the timer delay of an already running timer.
	void Reset()
	{
		InitiateDelayedTask(delay_, callback_);
	}
private:
	void OrphanDelayedTask()
	{
		if (timer_flag_.HasUsed())
		{
			timer_flag_.Cancel();
		}
	}

	void InitiateDelayedTask(TimeDelta delay, const StdClosure& callback)
	{
		OrphanDelayedTask();

		auto cb = timer_flag_.ToWeakCallback(callback);
		ThreadManager::GetTimerMessageLoop()->PostDelayedTask(cb, delay);
	}

private:
	typedef Timer<repeating> SelfType;
	nbase::WeakCallbackFlag timer_flag_;
	TimeDelta delay_;
	StdClosure callback_;

};
template <>
void Timer<true>::InitiateDelayedTask(TimeDelta delay, const StdClosure& callback)
{
	OrphanDelayedTask();

	auto cb = timer_flag_.ToWeakCallback(callback);
	ThreadManager::PostRepeatedTask(cb, delay,-1);
}
//-----------------------------------------------------------------------------
// A simple, one-shot timer.  See usage notes at the top of the file.
class OneShotTimer : public Timer<false> {};

//-----------------------------------------------------------------------------
// A simple, repeating timer.  See usage notes at the top of the file.
class RepeatingTimer : public Timer< true> {};

	
}

#endif  // BASE_TIME_TIMER_H_