#pragma once
#include "shared/threads.h"

class MiscThread : public nbase::FrameworkThread
{
public:
	MiscThread(ThreadId thread_id, const char *name);
	~MiscThread(void);

private:
	virtual void Init() override;
	virtual void Cleanup() override;

private:
	ThreadId thread_id_;
};