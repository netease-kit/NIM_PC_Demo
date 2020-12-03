#include "business_manager.h"
#include "shared/business_action_gateway/business_action_gateway/business_action_gateway_imp.h"
//#include "tool_kits/ui_component/g2_kit/module/video_manager_g2.h"

namespace nbase {

	//static BusinessActionGateway action_gateway_ = std::make_shared<BusinessActionGatewayImp>();

	//主线程开始前init
	void BusinessManager::Init()
	{
		action_gateway_ = std::make_shared<BusinessActionGatewayImp>();
		action_gateway_->Start();

	}

	void BusinessManager::UnInit()
	{
		if (action_gateway_ != nullptr)
		{
			if (action_gateway_->IsRunning())
			{
				action_gateway_->Stop();
			}
			action_gateway_.reset();
		}
	}

	void BusinessManager::RegActions(const std::vector<std::string>& action_names, const ActionDispatcher& dispatcher)
	{
		if (action_gateway_ != nullptr)
			return std::dynamic_pointer_cast<IBusinessActionGatewayOption>(action_gateway_)->RegActions(action_names, dispatcher);
	}
	BatpCode BusinessManager::Request(const BatpPack& request, const BatpResponseCallback& call_back)
	{
		if (action_gateway_ != nullptr)
			return std::dynamic_pointer_cast<IBusinessActionGatewayOption>(action_gateway_)->Request(request, call_back);
		return BatpCode::NOTFOUND;
	}
	void BusinessManager::Response(const BatpPack& request)
	{
		if (action_gateway_ != nullptr)
			return std::dynamic_pointer_cast<IBusinessActionGatewayOption>(action_gateway_)->Response(request);
	}
	void BusinessManager::SubNotify(const std::vector<std::string>& action_names, const BatpSubNotifyCallback& cb)
	{
		if (action_gateway_ != nullptr)
			return std::dynamic_pointer_cast<IBusinessActionGatewayOption>(action_gateway_)->SubNotify(action_names, cb);
	}
	void BusinessManager::Notify(const BatpPack& request)
	{
		if (action_gateway_ != nullptr)
			return std::dynamic_pointer_cast<IBusinessActionGatewayOption>(action_gateway_)->Notify(request);
	}

// 	void BusinessManager::DispatchActionRequest(const BatpPack& request)
// 	{
// 		//QLOG_APP(L"");
// 		int a;
// 		a = 10;
// 	}
}