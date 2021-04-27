
#ifndef MISC_THREAD_H_
#define MISC_THREAD_H_

#include "base/thread/framework_thread.h"
#include "base/thread/thread.h"

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
#endif //
