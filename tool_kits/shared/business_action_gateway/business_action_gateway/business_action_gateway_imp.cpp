#include "business_action_gateway/business_action_gateway/business_action_gateway_imp.h"

namespace nbase
{
	bool BusinessActionGatewayImp::Start()
	{
		working_thread_.Start();
		notify_thread_.Start();
		return true;
	}
	bool BusinessActionGatewayImp::Stop()
	{
		working_thread_.Stop();
		notify_thread_.Stop();
		return true;
	}
	bool BusinessActionGatewayImp::IsRunning() const
	{
		return working_thread_.IsRunning() && notify_thread_.IsRunning();
	}
	BusinessActionGatewayOption BusinessActionGatewayImp::GetOption() {
		return shared_from_this();
	}
	//TODO: 线程安全
	void BusinessActionGatewayImp::RegActions(const std::vector<std::string>& action_names, const ActionDispatcher& dispatcher)
	{
		//这里认为在使用前由core进行统一加载，不会产生线程安全的问题
		for (auto it : action_names)
		{
			action_dispatcher_list_[it] = dispatcher;
		}
	}
	BatpCode BusinessActionGatewayImp::Request(const BatpPack& request, const BatpResponseCallback& call_back)
	{
		BatpCode ret = BatpCode::NOTFOUND;
		auto dispatcher = GetActionDispatcher(request.head_.action_name_);
		if (dispatcher != nullptr)
		{
			ret = BatpCode::OK;
			DoRequest(request, dispatcher, call_back);
		}
		return ret;
	}
	void BusinessActionGatewayImp::Response(const BatpPack& request)
	{
		DoResponse(request);
	}
	//TODO: SubNotify在编译期即可决定，有优化空间
	//TODO: 要不要加反注册？
	void BusinessActionGatewayImp::SubNotify(const std::vector<std::string>& action_names, const BatpSubNotifyCallback& cb)
	{
		std::lock_guard<std::recursive_mutex> auto_lock(notify_list_lock_);
		for (auto it : action_names)
		{
			notify_list_[it].emplace_back(cb);
		}
	}
	void BusinessActionGatewayImp::Notify(const BatpPack& notify)
	{
		notify_thread_.message_loop()->PostTask(ToWeakCallback([this, notify]() {
			std::lock_guard<std::recursive_mutex> auto_lock(notify_list_lock_);
			auto action_list = notify_list_.find(notify.head_.action_name_);
			if (action_list != notify_list_.end())
			{
				std::for_each(action_list->second.begin(), action_list->second.end(), [&](const BatpSubNotifyCallback& cb) {
					cb(notify);
					});
			}
			}));
	}
	ActionDispatcher BusinessActionGatewayImp::GetActionDispatcher(const std::string& action_name) const
	{
		auto it = action_dispatcher_list_.find(action_name);
		if (it != action_dispatcher_list_.end() && !it->second.expired())
			return it->second.lock();
		return nullptr;
	}
	void BusinessActionGatewayImp::DoRequest(const BatpPack& request, const ActionDispatcher& dispatcher, const BatpResponseCallback& call_back)
	{
		if (request.head_.meth_ == BatpRequestMeth::POST)
		{
			{
				std::lock_guard<std::recursive_mutex> auto_lock(wait_response_list_lock_);
				wait_response_list_[request.head_.sn_] = call_back;
				//wait_response_list_.insert(std::make_pair(request.head_.sn_, call_back));
			}
			working_thread_.message_loop()->PostTask(ToWeakCallback([request, dispatcher]() {
				dispatcher->DispatchActionRequest(request);
				}));
		}
		else if (request.head_.meth_ == BatpRequestMeth::SEND)
		{

		}
		else if (request.head_.meth_ == BatpRequestMeth::UNIDIRECTIONAL)
		{
			working_thread_.message_loop()->PostTask(ToWeakCallback([request, dispatcher]() {
				dispatcher->DispatchActionRequest(request);
				}));
		}
	}
	void BusinessActionGatewayImp::DoResponse(const BatpPack& response)
	{
		if (response.head_.meth_ == BatpRequestMeth::POST)
		{
			auto task = ToWeakCallback([this, response]() {
				BatpResponseCallback cb = nullptr;
				{
					std::lock_guard<std::recursive_mutex> auto_lock(wait_response_list_lock_);
					auto it = wait_response_list_.find(response.head_.sn_);
					if (it != wait_response_list_.end())
					{
						cb = it->second;
						wait_response_list_.erase(it);
					}
				}
				if (cb != nullptr)
					cb(response);
				});
			working_thread_.message_loop()->PostTask(task);
		}
	}
}