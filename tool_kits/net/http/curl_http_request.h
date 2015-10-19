// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/9/25
//
// Making http requests using Curl on a MessageLoop based thread

#ifndef NET_HTTP_CURL_HTTP_REQUEST_H_
#define NET_HTTP_CURL_HTTP_REQUEST_H_

#include <memory>
#include "base/memory/deleter.h"
#include "net/base/completion_callback.h"
#include "net/http/curl_http_request_base.h"

namespace nbase
{
class MessageLoopProxy;
}

namespace net
{

class NET_EXPORT CurlHttpRequest : public CurlHttpRequestBase
{
public:
	// The first parameter tells you the content of the server's response.
	// The second parameter tells you whether an error occurred. If it is
	// non-zero, you should ignore the first parameter.
	typedef std::function<void(const std::shared_ptr<std::string> &, bool, int)>
		ContentCallback;

	// The four parameters of ProgressCallback represent (in order):
	// * Total bytes expected to upload,
	// * Bytes uploaded now
	// * Total bytes expected to download
	// * Bytes downloaded now
	typedef std::function<void(double,double,double,double)> ProgressCallback;

	// * This version of constructor create an instance which will make a http
	//   or https request and store the server's response to the file whose path
	//   is specified by |file_path|. Thus |file_path| should NOT be NULL.
	// * |callback| will be invoked when the request finished, if a ZERO is
	//   passed to its unique parameter if the request succeeded, or the
	//   request failed.
	explicit CurlHttpRequest(const std::string &url,
							const wchar_t *download_file_path,
							const CompletionCallback &file_callback,
							const ProgressCallback &progress_callback
							= ProgressCallback(),
							bool post = false);
	
	// *This version of constructor extends the upper constructor to support continue
	// transferring from breakpoint
	explicit CurlHttpRequest(const std::string &url,
							const wchar_t *download_file_path,
							long long range_start,
							const CompletionCallback &file_callback,
							const ProgressCallback &progress_callback
							   = ProgressCallback(),
							bool post = false);

	// * This version of constructor create an instance which will make a http
	//   or https request and store the server's response to the first
	//   parameter of |callback|.
	// * |callback| will be invoked when the request finished, if a ZERO is
	//   passed to its second parameter if the request is succeeded, or the
	//   request failed. Its first paremeter stores the server's response.
	explicit CurlHttpRequest(const std::string &url,
							 const ContentCallback &content_callback,
							 const ProgressCallback &progress_callback
								= ProgressCallback(),
							 bool post = false);

	virtual ~CurlHttpRequest() {}

	// The following methods MUST be called on the thread really do the request
	bool download_ok() const { return result_ == CURLE_OK; }
	const void* content() const;
	size_t content_length() const;
	const std::wstring& download_file_path() const { return download_file_path_; }

protected:
	// Called immediately after the easy handle created
	// You can do `curl_easy_setopt` work as you like,
	// such as setting url, setting cookies, etc.
	virtual bool OnEasyHandleCreated();
	// Called after the session is finished successfully.
	virtual void OnTransferDone();
	// Called when a error occurred.
	virtual void OnError();
	// Called after the easy handle destroyed.
	virtual void OnEasyHandleDestroyed();

	static size_t WriteHeader(void *ptr, size_t size, size_t count, void *data);
	static size_t WriteOSFile(void *ptr, size_t size, size_t count, void *param);
	static size_t WriteOSFileRange(void *ptr, size_t size, size_t count, void *param);
	static size_t WriteMemory(void *ptr, size_t size, size_t count, void *data);
	static int ProgressCB(void *clientp,
		double dltotal, double dlnow, double ultotal, double ulnow);

	bool OpenFileForWrite();
	bool OpenFileForRangeWrite();
	size_t WriteCfgFile();

	void NotifyCompletion();
	void NotifyProgress(double, double, double, double);

	bool memory_;
	long long range_start_;
	std::wstring download_file_path_;
	std::unique_ptr<FILE, nbase::DeleterFileClose> file_handle_;
	std::unique_ptr<FILE, nbase::DeleterFileClose> cfg_file_handle_;
	int response_code_;
	std::shared_ptr<std::string> content_;
	ContentCallback content_callback_;
	CompletionCallback file_callback_;
	ProgressCallback progress_callback_;
	std::shared_ptr<nbase::MessageLoopProxy> callback_message_loop_;

	DISALLOW_COPY_AND_ASSIGN(CurlHttpRequest);
};

} // namespace net

#endif // NET_HTTP_CURL_HTTP_REQUEST_H_
