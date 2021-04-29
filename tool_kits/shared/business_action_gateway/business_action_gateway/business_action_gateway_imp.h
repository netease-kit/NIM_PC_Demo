#ifndef _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_BUSINESS_ACTION_GATEWAY_IMP_H_
#define _COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_BUSINESS_ACTION_GATEWAY_IMP_H_

//#include "config/build_config.h"

#include <map>
#include <mutex>

#include "base/thread/threadex.h"
#include "base/thread/thread.h"
#include "base/callback/callback.h"

#include "shared/business_action_gateway/business_action_gateway/ibusiness_action_gateway.h"
#include "shared/business_action_gateway/business_action_gateway/ibusiness_action_gateway_option.h"
#include "base/thread/framework_thread.h"

namespace nbase
{
	class BusinessActionGatewayImp :
		public SupportWeakCallback,
		public IBusinessActionGatewayOption,
		public IBusinessActionGateway,
		public nbase::Thread,
		public std::enable_shared_from_this<BusinessActionGatewayImp>
	{
	public:
		BusinessActionGatewayImp():working_thread_("working_thread"), notify_thread_("notify_thread"){}
		virtual ~BusinessActionGatewayImp() {}
	public:
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool IsRunning() const override;
		virtual BusinessActionGatewayOption GetOption() override;
		virtual void RegActions(const std::vector<std::string>& action_names, const ActionDispatcher& dispatcher) override;
		virtual BatpCode Request(const BatpPack& request, const BatpResponseCallback& call_back) override;
		virtual void Response(const BatpPack& response) override;
		virtual void SubNotify(const std::vector<std::string>& action_names, const BatpSubNotifyCallback& cb) override;
		virtual void Notify(const BatpPack& notify) override;
	private:
		ActionDispatcher GetActionDispatcher(const std::string& action_name) const;
		void DoRequest(const BatpPack& request, const ActionDispatcher& dispatcher, const BatpResponseCallback& call_back);
		void DoResponse(const BatpPack& response);
	private:
		std::recursive_mutex wait_response_list_lock_;
		std::map<uint64_t, BatpResponseCallback> wait_response_list_;
		std::recursive_mutex sync_response_list_lock_;
		std::map<uint64_t, BatpResponseCallback> sync_response_list_;
		std::recursive_mutex notify_list_lock_;
		std::map<std::string, std::list<BatpSubNotifyCallback>> notify_list_;
	private:
		std::map<std::string, WeakActionDispatcher> action_dispatcher_list_;
		nbase::FrameworkThread working_thread_;
		nbase::FrameworkThread notify_thread_;
	};
}

#endif //_COMPILE_SWITCH_BUSINESS_ACTION_GATEWAY_BUSINESS_ACTION_GATEWAY_IMP_H_