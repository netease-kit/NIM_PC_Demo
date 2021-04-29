#pragma once

#include "shared/business_action_gateway/business_action_gateway/ibusiness_action_gateway.h"
#include "shared/business_action_gateway/business_action_gateway/ibusiness_action_gateway_option.h"
#include "base/thread/threadex.h"
#include "base/memory/singleton.h"

namespace nbase {
	class BusinessManager :public nbase::Singleton<BusinessManager>
		,public nbase::IBusinessActionGatewayOption
		,public BaseThread
		//,public IActionDispatcher
	{
	public:
		virtual void Init();
		virtual void UnInit();
		virtual void RegActions(const std::vector<std::string>& action_names, const ActionDispatcher& dispatcher) override;
		virtual BatpCode Request(const BatpPack& request, const BatpResponseCallback& call_back) override;
		virtual void Response(const BatpPack& request) override;
		virtual void SubNotify(const std::vector<std::string>& action_names, const BatpSubNotifyCallback& cb) override;
		virtual void Notify(const BatpPack& request) override;

		//virtual void DispatchActionRequest(const BatpPack& request) override;
	private:
		BusinessActionGateway action_gateway_;
	};
}