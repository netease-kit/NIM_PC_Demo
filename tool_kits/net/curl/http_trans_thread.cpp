#include "http_trans_thread.h"
#include "base/memory/singleton.h"
#include "net/curl/libuv_message_loop.h"

namespace net
{
	const int HttpTransThread::kREQUEST_ID_BASE = 0x7FFFFFF;

	HttpTransThread::HttpTransThread() : FrameworkThread("http thread"), message_loop_destroyed_(true), request_current_sn_(kREQUEST_ID_BASE)
{
	
}

void HttpTransThread::Start(net::Proxy proxy)
{
	proxy_ = proxy;
	StartWithLoop(new net::UvMessageLoopFactiry);	
}

void HttpTransThread::Stop()
{
	FrameworkThread::Stop();
}

void HttpTransThread::Init()
{
	manager_.reset(new net::CurlNetworkSessionManager(nbase::MessageLoop::current()->message_loop_proxy()));
	message_loop()->AddDestructionObserver(this);
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	message_loop_destroyed_ = false;
}

void HttpTransThread::Cleanup()
{
	manager_.reset(nullptr);
}
void HttpTransThread::PreDestroyCurrentMessageLoop()
{
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	message_loop_destroyed_ = true;
}
HttpRequestID HttpTransThread::PostRequest(const HttpRequestSharedPtr &request)
{
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	if (!message_loop_destroyed_)
		message_loop()->PostTask(nbase::Bind(&HttpTransThread::DoHttpRequestTask, request));	
	return AddRequestToCatchList(request);
}

void HttpTransThread::RemoveRequest(HttpRequestID http_request_id)
{
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	if (!message_loop_destroyed_)
	{
		net::CurlHttpRequest* request = GetRequestByRequestID(http_request_id);// (net::CurlHttpRequest*)http_request_id;
		if (request != nullptr)
			message_loop()->PostTask(nbase::Bind(&HttpTransThread::DoRemoveHttpRequestTask, request));
	}	
}

void HttpTransThread::DoHttpRequestTask(const HttpRequestSharedPtr &request)
{
	HttpTransThread *thread = static_cast<HttpTransThread*>(nbase::FrameworkThread::current());
	thread->AddSession(request);
}

void HttpTransThread::DoRemoveHttpRequestTask(net::CurlHttpRequest* request)
{
	HttpTransThread *thread = static_cast<HttpTransThread*>(nbase::FrameworkThread::current());
	thread->RemoveSession(request);
}

void HttpTransThread::AddSession(const HttpRequestSharedPtr& request)
{
	//因为有如下的关系
	//std::unique_ptr<net::CurlNetworkSessionManager> manager_;
	//std::shared_ptr<nbase::MessageLoopProxy> manager_::message_loop_;   
	//MessageLoop* manager_::message_loop_::target_message_loop_ = this(代码在message_loop.cpp里 MessageLoop*类型);
	//如果message_loop 被销毁 会引起崩溃,不过在整个工程里搜了下，没发现对这个接口的调用
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	if (!message_loop_destroyed_)
	{
		if (!proxy_.IsEmpty()) {
			request->SetProxy(proxy_);
		}
		manager_->AddSession(request); // 添加一个Http会话任务
	}	
}

void HttpTransThread::RemoveSession(net::CurlHttpRequest* request)
{
	//修改原因同上
	if (request == nullptr)
		return;
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	if (!message_loop_destroyed_)
	{
		manager_->RemoveSession(request);
	}	
	RemoveRequestFromCatchList(request);
}
HttpRequestID HttpTransThread::AddRequestToCatchList(const HttpRequestSharedPtr &request)
{
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	request_current_sn_ -= 1;
	if (request_current_sn_ == INVALID_HTTPREQUESTID)
		request_current_sn_ = kREQUEST_ID_BASE;
	request_list_.insert(std::make_pair(request_current_sn_, std::make_pair(request.get(), request)));
	request->attach_release(ToWeakCallback(std::bind(&HttpTransThread::OnRequestRelease, this, request.get())));
	return request_current_sn_;
}
HttpRequestID HttpTransThread::RemoveRequestFromCatchList(net::CurlHttpRequest* request)
{
	HttpRequestID ret = INVALID_HTTPREQUESTID;
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	auto it = std::find_if(request_list_.begin(), request_list_.end(), [&](const std::pair<HttpRequestID, std::pair<CurlHttpRequest*, HttpRequestWeakPtr>>& item){	
		return item.second.first == request;
	});
	if (it != request_list_.end())
	{
		ret = it->first;
		request_list_.erase(it);
	}
	return ret;
}
net::CurlHttpRequest* HttpTransThread::GetRequestByRequestID(HttpRequestID http_request_id)
{
	net::CurlHttpRequest* ret = nullptr;
	nbase::NAutoLock autolock(&message_loop_flag_lock_);
	auto it = request_list_.find(http_request_id);
	if (it != request_list_.end())
	{
		if (!it->second.second.expired())
			ret = it->second.first;
		else
			request_list_.erase(it);
	}
	return ret;
}
void HttpTransThread::OnRequestRelease(net::CurlHttpRequest* request)
{
	RemoveRequestFromCatchList(request);
}
}