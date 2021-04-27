#include "StdAfx.h"
#include "HelperThread.h"
#include "base/thread/thread_manager.h"

namespace ui
{
HelperThread::HelperThread(ThreadId thread_id, const char* name) :
	FrameworkThread(name),
	thread_id_(thread_id)
{

}

HelperThread::~HelperThread(void)
{

}

void HelperThread::Init()
{
	nbase::ThreadManager::RegisterThread(thread_id_);
}

void HelperThread::Cleanup()
{
	nbase::ThreadManager::UnregisterThread();
}

}  // namespace nbiz