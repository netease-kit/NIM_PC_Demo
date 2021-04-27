#include "global_misc_thread.h"
#include "base/thread/thread_manager.h"

namespace net
{

GlobalMiscThread::GlobalMiscThread()
	: FrameworkThread("net_global_misc")
{
}

GlobalMiscThread::~GlobalMiscThread(void)
{
}

void GlobalMiscThread::Init()
{
}

void GlobalMiscThread::Cleanup()
{
}

}