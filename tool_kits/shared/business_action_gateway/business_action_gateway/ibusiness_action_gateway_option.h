#ifndef _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_GATEWAY_OPTION_H_
#define _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_GATEWAY_OPTION_H_

#include <string>
#include <functional>
#include <memory>

#include "shared/business_action_gateway/batp/batp.h"
#include "shared/business_action_gateway/business_action_gateway/action_dispatcher.h"

namespace nbase
{
	using BatpResponseCallback = std::function<void(const BatpPack& response)>;
	using BatpSubNotifyCallback = std::function<void(const BatpPack& response)>;

	class IBusinessActionGatewayOption
	{
	public:
		IBusinessActionGatewayOption() {}
		virtual ~IBusinessActionGatewayOption() {}
	public:
		virtual void RegActions(const std::vector<std::string>& action_names, const ActionDispatcher& dispatcher) = 0;
		virtual BatpCode Request(const BatpPack& request, const BatpResponseCallback& call_back) = 0;
		virtual void Response(const BatpPack& request) = 0;
		virtual void SubNotify(const std::vector<std::string>& action_names, const BatpSubNotifyCallback& cb) = 0;
		virtual void Notify(const BatpPack& request) = 0;
	};

	using BusinessActionGatewayOption = std::shared_ptr<IBusinessActionGatewayOption>;
	using WeakBusinessActionGatewayOption = std::weak_ptr<IBusinessActionGatewayOption>;

}

#endif //_COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_IBUSINESS_ACTION_GATEWAY_OPTION_H_