// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/29
//
// Making http requests using Curl on a MessageLoop based thread

#ifndef NET_HTTP_CURL_HTTP_REQUEST_BASE_H_
#define NET_HTTP_CURL_HTTP_REQUEST_BASE_H_

#include <list>
#include "net/base/proxy.h"
#include "net/curl/curl_network_session.h"

namespace net
{

class NET_EXPORT CurlHttpRequestBase : public CurlNetworkSession
{
public:
	explicit CurlHttpRequestBase(const std::string &url, bool post = false);
	virtual ~CurlHttpRequestBase();

	bool IsRunning() const { return !!easy_handle_; }
	bool IsPostMethod() const { return post_; }
	CURL* easy_handle() const { return easy_handle_; }
	long response_code() const { return response_code_; }
	const std::string& url() const { return url_; }

	void AddHeaderField(const char *name, const char *value);
	void AddHeaderField(const std::string &name, const std::string &value);
	void ClearHeaderFields();
	// NOTE:
	// * SetPostFields will post a clone of |data|,
	//   you can do whatever you want to |data| before the posting progress is
	//   performed.
	// * You must make sure that |data| is formatted as
	//   "Content-Type: application/x-www-form-urlencoded".
	// * You cannot use both SetPostFields*** and AddForm, or post fields will
	//   be thrown.
	bool SetPostFields(const void *data, size_t size);
	// |value| will be copied
	bool AddForm(const char *name, const char *value);
	// |data| will be copied
	bool AddForm(const char *name, const void *data, size_t data_length);
#if defined(OS_WIN)
	// |filename_encoding| is the codepage which the filename will be encoded to
	bool AddForm(const char *name,
				 const wchar_t *file_path,
				 int filename_encoding = CP_ACP);
#else
	bool AddForm(const char *name, const char *file_path);
#endif // OS_WIN
	void ClearForms();
	// |cookie| should be in the format of
	// "name1=content1[;name2=content2[;name3=content3...]]"
	bool SetCookie(const std::string &cookie);
	bool GetCookieList(std::list<std::string> &cookies);
	bool SetCookieList(const std::list<std::string> &cookies);
	bool SetReferer(const char *referer);
	bool SetReferer(const std::string &referer);
	void SetProxy(const Proxy &proxy);
	// Set the transfer timeout in milliseconds.
	void SetTimeout(long timeout_ms);

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

	bool post_;
	long response_code_;
	long timeout_ms_;
	std::string url_;
	Proxy proxy_;
	curl_slist *header_list_;
	curl_httppost *form_post_;
	curl_httppost *form_post_last_;
	std::string post_fields_;
	std::list<FILE *> upload_file_handles_;
	char error_buffer_[CURL_ERROR_SIZE];
};

} // namespace net

#endif // NET_HTTP_CURL_HTTP_REQUEST_BASE_H_
