#include "db_thread.h"


DBThread::DBThread(ThreadId thread_id, const char* name)
	: FrameworkThread(name), thread_id_(thread_id)
{

}

DBThread::~DBThread(void)
{

}

void DBThread::Init()
{
	nbase::ThreadManager::RegisterThread(thread_id_);
}

void DBThread::Cleanup()
{
	nbase::ThreadManager::UnregisterThread();

	QLOG_APP(L"DBThread Cleanup");
}