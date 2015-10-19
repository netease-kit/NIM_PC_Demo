#pragma once
#include "shared/threads.h"

class DBThread : public nbase::FrameworkThread
{
public:
	DBThread(ThreadId thread_id, const char *name);
	~DBThread(void);

private:
	virtual void Init() override;
	virtual void Cleanup() override;

private:
	ThreadId thread_id_;
};