#ifndef  _COMPILE_SWITCH_TASK_INTERFACE_H_
#define _COMPILE_SWITCH_TASK_INTERFACE_H_

#include <functional>

namespace nbase
{
	using RuntimeCallback = std::function<void()>;
	using Task = std::function<void()>;

	class ITaskLoop
	{
	public:
		ITaskLoop() = default;
		virtual ~ITaskLoop() = default;
	public:
		virtual void PostTask(const Task& task) = 0;
		virtual void PostHighPriorityTask(const Task& task) = 0;
		virtual void PostDelayTask(int time_block, const Task& task) = 0;
	};

}

#endif//_COMPILE_SWITCH_TASK_INTERFACE_H_
