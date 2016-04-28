#include "nim_tools_http.h"
#include "net/curl/http_trans_thread.h"
#include "base/util/string_util.h"
#include "base/file/file_util.h"

struct HttpRequest
{
	HttpRequest() :
		is_set_as_post(false),
		url(),
		download_file_path(),
		post_body(),
		range_start(-1),
		complete_cb(nullptr),
		completed_cb_user_data(nullptr),
		response_cb(nullptr),
		response_cb_user_data(nullptr),
		progress_cb(nullptr),
		progress_cb_user_data(nullptr),
		header_map()
	{
	}
	bool is_set_as_post;
	std::string url;
	std::string download_file_path;
	std::string post_body;
	long long range_start;
	int timeout_ms;
	nim_http_request_completed_cb complete_cb;
	const void* completed_cb_user_data;
	nim_http_request_response_cb response_cb;
	const void* response_cb_user_data;
	nim_http_request_progress_cb progress_cb;
	const void* progress_cb_user_data;
	std::map<std::string, std::string> header_map;
	net::Proxy proxy;
};

void nim_http_init()
{
	static bool bIsInited = false;
	if (!bIsInited)
	{
		bIsInited = true;
		net::HttpTransThread::GetInstance()->Start(net::Proxy());
	}
}

void nim_http_uninit()
{
	static bool bIsUnInited = false;
	if (!bIsUnInited)
	{
		bIsUnInited = true;
		net::HttpTransThread::GetInstance()->Stop();
	}
}

int nim_http_post_request(HttpRequestHandle request_handle)
{
	HttpRequest* http_request = (HttpRequest*)request_handle;
	std::wstring ws_file_path = nbase::UTF8ToUTF16(http_request->download_file_path);
	net::CompletionCallback completed_cb;
	if (http_request->complete_cb) {
		completed_cb = nbase::Bind(http_request->complete_cb, http_request->completed_cb_user_data, std::placeholders::_1, std::placeholders::_2);
	}
	net::CurlHttpRequest::ContentCallback content_cb;
	if (http_request->response_cb) {
		nim_http_request_response_cb response_cb = http_request->response_cb;
		const void* http_user_data = http_request->response_cb_user_data;
		content_cb = [response_cb, http_user_data](const std::shared_ptr<std::string> & response_content, bool res, int response_code) {
			response_cb(http_user_data, res, response_code, response_content->c_str());
		};
	}
	net::CurlHttpRequest::ProgressCallback progress_cb;
	if (http_request->progress_cb) {
		progress_cb = nbase::Bind(http_request->progress_cb, http_request->progress_cb_user_data,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}

	net::CurlHttpRequest* curl_http_request = nullptr;
	bool is_post = !http_request->post_body.empty() || http_request->is_set_as_post;
	if (!ws_file_path.empty())
	{
		if (http_request->range_start < 0) {
			curl_http_request = new net::CurlHttpRequest(http_request->url, ws_file_path.c_str(),
				completed_cb, progress_cb, is_post);
		}
		else {
			curl_http_request = new net::CurlHttpRequest(http_request->url, ws_file_path.c_str(),
				http_request->range_start, completed_cb, progress_cb, is_post);
		}
	}
	else {
		curl_http_request = new net::CurlHttpRequest(http_request->url, content_cb,
			progress_cb, is_post);
	}

	net::HttpRequestSharedPtr shared_request(curl_http_request);
	if (!http_request->post_body.empty()) {
		shared_request->SetPostFields(http_request->post_body.c_str(), http_request->post_body.size());
	}
	for (auto it = http_request->header_map.begin(); it != http_request->header_map.end(); it++)
	{
		shared_request->AddHeaderField(it->first, it->second);
	}
	shared_request->SetTimeout(http_request->timeout_ms);
	if (http_request->proxy.type != net::ProxyNone)
	{
		shared_request->SetProxy(http_request->proxy);
	}
	int http_request_id = net::HttpTransThread::GetInstance()->PostRequest(shared_request);
	delete http_request;

	return http_request_id;
}

void nim_http_remove_request(int http_request_id)
{
	net::HttpTransThread::GetInstance()->RemoveRequest(http_request_id);
}

HttpRequestHandle nim_http_create_download_file_request(const char* url, const char *download_file_path, 
	nim_http_request_completed_cb complete_cb, const void* user_data)
{
	HttpRequest* http_request = new HttpRequest();
	http_request->url = url;
	http_request->download_file_path = download_file_path;
	http_request->complete_cb = complete_cb;
	http_request->completed_cb_user_data = user_data;
	return http_request;
}

HttpRequestHandle nim_http_create_download_file_range_request(const char* url, const char *download_file_path,
	__int64 range_start, nim_http_request_completed_cb complete_cb, const void* user_data)
{
	HttpRequest* http_request = new HttpRequest();
	http_request->url = url;
	http_request->download_file_path = download_file_path;
	if (range_start < 0)
	{
		range_start = 0;
	}
	http_request->range_start = range_start;
	http_request->complete_cb = complete_cb;
	http_request->completed_cb_user_data = user_data;
	return http_request;
}

HttpRequestHandle nim_http_create_request(const char* url, const char* post_body, size_t post_body_size, 
	nim_http_request_response_cb response_callback, const void* user_data)
{
	HttpRequest* http_request = new HttpRequest();
	http_request->url = url;
	http_request->post_body.assign(post_body, post_body_size);
	http_request->response_cb = response_callback;
	http_request->response_cb_user_data = user_data;
	return http_request;
}

void nim_http_add_request_header(HttpRequestHandle request_handle, const char* key, const char* value)
{
	HttpRequest* http_request = (HttpRequest*)request_handle;
	http_request->header_map[key] = value;
}

void nim_http_set_request_range_start(HttpRequestHandle request_handle, int range_start)
{
	HttpRequest* http_request = (HttpRequest*)request_handle;
	http_request->range_start = range_start;
}

void nim_http_set_request_progress_cb(HttpRequestHandle request_handle, 
	nim_http_request_progress_cb progress_callback, const void* user_data)
{
	HttpRequest* http_request = (HttpRequest*)request_handle;
	http_request->progress_cb = progress_callback;
	http_request->progress_cb_user_data = user_data;
}

void nim_http_set_request_method_as_post(HttpRequestHandle request_handle)
{
	HttpRequest* http_request = (HttpRequest*)request_handle;
	http_request->is_set_as_post = true;
}

NET_EXPORT void nim_http_set_timeout(HttpRequestHandle request_handle, int timeout_ms)
{

	HttpRequest* http_request = (HttpRequest*)request_handle;
	http_request->timeout_ms = timeout_ms;
}
NET_EXPORT void nim_http_set_proxy(HttpRequestHandle request_handle, int type, const char* host, short port, const char* user, const char* pass)
{
	HttpRequest* http_request = (HttpRequest*)request_handle;
	switch (type)
	{
	case kNIMProxyHttp11:
		http_request->proxy.type = net::ProxyHttp11;
		break;
	case kNIMProxySocks4:
		http_request->proxy.type = net::ProxySock4;
		break;
	case kNIMProxySocks4a:
		http_request->proxy.type = net::ProxySock4a;
		break;
	case kNIMProxySocks5:
		http_request->proxy.type = net::ProxySock5;
		break;
	default:
		http_request->proxy.type = net::ProxyNone;
		break;
	}
	http_request->proxy.host = host;
	http_request->proxy.port = port;
	http_request->proxy.user = user;
	http_request->proxy.pass = pass;
}
