#ifndef NET_CURL_HTTP_TRANS_THREAD_H_
#define NET_CURL_HTTP_TRANS_THREAD_H_

#include <memory>
#include "base/thread/framework_thread.h"
#include "base/memory/singleton.h"
#include "net/curl/curl_network_session_manager.h"
#include "net/http/curl_http_request.h"
#include "net/base/proxy.h"
#include "net/export/nim_tools_http_def.h"
namespace net
{

typedef std::shared_ptr<CurlHttpRequest> HttpRequestSharedPtr;
typedef std::weak_ptr<CurlHttpRequest> HttpRequestWeakPtr;

class NET_EXPORT HttpTransThread : public nbase::FrameworkThread, public nbase::MessageLoop::DestructionObserver
{
public:
	SINGLETON_DEFINE(HttpTransThread);
	HttpTransThread();
	
	void Start(net::Proxy proxy);
	void Stop();

	HttpRequestID PostRequest(const HttpRequestSharedPtr &request);
	void RemoveRequest(HttpRequestID http_request_id);
	net::CurlHttpRequest* GetRequestByRequestID(HttpRequestID http_request_id);
	virtual void PreDestroyCurrentMessageLoop();
private:
	void Init() override;
	void Cleanup() override;
	static void DoHttpRequestTask(const HttpRequestSharedPtr &request);
	static void DoRemoveHttpRequestTask(net::CurlHttpRequest* request);
	void AddSession(const HttpRequestSharedPtr& request);
	void RemoveSession(net::CurlHttpRequest* request);

	HttpRequestID AddRequestToCatchList(const HttpRequestSharedPtr &request);
	HttpRequestID RemoveRequestFromCatchList(net::CurlHttpRequest* request);	

	void OnRequestRelease(net::CurlHttpRequest* request);
private:
	nbase::NLock message_loop_flag_lock_;
	std::unique_ptr<net::CurlNetworkSessionManager> manager_; // Http会话管理器
	net::Proxy proxy_;
	bool message_loop_destroyed_;	
	std::map<HttpRequestID, std::pair<CurlHttpRequest*,HttpRequestWeakPtr>> request_list_;
	static const int kREQUEST_ID_BASE;
	volatile int request_current_sn_;
};

}


#endif //NET_CURL_HTTP_TRANS_THREAD_H_