// Copyright (c) 2014, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2014/1/26
//
// Unit test cases for Libuv Message loops

#if defined(WITH_UNITTEST)

#include "base/thread/framework_thread.h"
#include "base/log/log.h"
#include "gtest/gtest.h"

namespace
{

class UVMessageLoopTestThread:
	public nbase::FrameworkThread
{
public:
	UVMessageLoopTestThread()
		: FrameworkThread("UVMessageLoopTestThread"), ok_(false) {}

	void Init()
	{
		nbase::MessageLoop::current()->PostTask(
			std::bind(&UVMessageLoopTestThread::DoStartTimer, this));
	}

	void DoStartTimer()
	{
		nbase::MessageLoop::current()->PostDelayedTask(
			std::bind(&UVMessageLoopTestThread::DoDelayedTask, this),
			nbase::TimeDelta::FromMilliseconds(2000));
	}

	void DoDelayedTask()
	{
		ok_ = true;
		StopSoon();
	}

	bool ok_;
};

class UVMessageLoopTest : public testing::Test
{
public:
	virtual void TearDown()
	{
		thread_.Stop();
	}

	void RunUtilFinished()
	{
		thread_.StartWithLoop(nbase::MessageLoop::kUVMessageLoop);
		thread_.Close();
		ok_ = thread_.ok_;
	}

	bool ok() const { return ok_; }

private:
	bool ok_;
	UVMessageLoopTestThread thread_;
};

TEST_F(UVMessageLoopTest, Basic)
{
	RunUtilFinished();
	EXPECT_TRUE(ok());
}

}

#endif // WITH_UNITTEST