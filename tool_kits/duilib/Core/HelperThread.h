#ifndef UI_CORE_HELPER_THREAD_H_
#define UI_CORE_HELPER_THREAD_H_

#include "base/thread/framework_thread.h"
#include "shared/threads.h"

namespace ui
{
class UILIB_API HelperThread : public nbase::FrameworkThread
{
public:
	HelperThread(ThreadId thread_id, const char *name);
	~HelperThread(void);

private:
	virtual void Init() override;
	virtual void Cleanup() override;

private:
	ThreadId thread_id_;
};
}  
#endif //UI_CORE_HELPER_THREAD_H_
