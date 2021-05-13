// Copyright (c) 2015-2016, NetEase Inc. All rights reserved.
//
// Author: Redrain
// Date: 2016/9/9
//
// global thread header file

#ifndef NET_GLOBAL_MISC_THREAD_H_
#define NET_GLOBAL_MISC_THREAD_H_

#include "base/thread/framework_thread.h"
#include "base/memory/singleton.h"

namespace net
{
class GlobalMiscThread : public nbase::FrameworkThread
{
public:
	SINGLETON_DEFINE(GlobalMiscThread);
	GlobalMiscThread();
	~GlobalMiscThread(void);

private:
	virtual void Init() override;
	virtual void Cleanup() override;

private:
};

}//namespace
#endif //NET_GLOBAL_MISC_THREAD_H_