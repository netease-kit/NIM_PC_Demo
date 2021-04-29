#ifndef _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_GATEWAY_H_
#define _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_GATEWAY_H_

#include <string>
#include <functional>
#include <memory>
#include <vector>

#include "shared/business_action_gateway/business_action_gateway/ibusiness_action_gateway_option.h"

namespace nbase
{
	class IBusinessActionGateway
	{
	public:
		virtual BusinessActionGatewayOption GetOption() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool IsRunning() const = 0;
	};

	using BusinessActionGateway = std::shared_ptr<IBusinessActionGateway>;

}

#endif //_COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_GATEWAY_H_