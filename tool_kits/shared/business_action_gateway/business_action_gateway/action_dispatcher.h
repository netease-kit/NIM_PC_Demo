#ifndef _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_DISPATCHER_H_
#define _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_DISPATCHER_H_

#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace nbase
{
	class IActionDispatcher;
	using ActionDispatcher = std::shared_ptr<IActionDispatcher>;
	using WeakActionDispatcher = std::weak_ptr<IActionDispatcher>;
	class IActionDispatcher
	{
	public:
		IActionDispatcher() {}
		virtual ~IActionDispatcher() {}
	public:
		virtual void DispatchActionRequest(const BatpPack& request) = 0;
	};

}

#endif //_COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_DISPATCHER_H_