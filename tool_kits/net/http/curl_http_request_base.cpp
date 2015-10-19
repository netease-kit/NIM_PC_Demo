// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/29
//
// Making http requests using Curl on a MessageLoop based thread

#include "net/http/curl_http_request_base.h"

#include "base/file/file_util.h"
//#include "base/log/log.h"

#if defined(OS_WIN)
#include "base/win32/platform_string_util.h"
#endif // OS_WIN

namespace net
{

namespace
{

static size_t ReadOSFile(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t bytes_to_read = size * nmemb;
	std::list<FILE *> *handle_list = (std::list<FILE *> *)userdata;
	if (!handle_list || handle_list->empty()) {
		//LOG_ERR("Invalid handle list");
		return CURL_READFUNC_ABORT;
	}
	if (fread(ptr, 1, bytes_to_read, handle_list->front()) == bytes_to_read)
		return bytes_to_read;

	int error = errno;
	
	// We automatically remove the file handle when EOF is met
	// or an error occurred
	fclose(handle_list->front());
	handle_list->pop_front();

	if (feof(handle_list->front())) {
		return 0;
	}
	//LOG_ERR("Read file failed(%d)", error);
	return CURL_READFUNC_ABORT; // on error
}

} // anonymous namespace

CurlHttpRequestBase::CurlHttpRequestBase(const std::string &url, bool post)
	: url_(url),
	  post_(post),
	  response_code_(0),
	  header_list_(NULL),
	  form_post_(NULL),
	  form_post_last_(NULL),
	  timeout_ms_(0)
{

}

CurlHttpRequestBase::~CurlHttpRequestBase()
{
	ClearHeaderFields();
	ClearForms();
}

void CurlHttpRequestBase::AddHeaderField(const char *name, const char *value)
{
	std::string header_field;
	header_field.append(name);
	header_field.append(": ");
	header_field.append(value);
	header_list_ = curl_slist_append(header_list_, header_field.c_str());
}

void CurlHttpRequestBase::AddHeaderField(
	const std::string &name, const std::string &value)
{
	std::string header_field;
	header_field.append(name);
	header_field.append(": ");
	header_field.append(value);
	header_list_ = curl_slist_append(header_list_, header_field.c_str());
}

void CurlHttpRequestBase::ClearHeaderFields()
{
	if (header_list_ != NULL) {
		curl_slist_free_all(header_list_);
		header_list_ = NULL;
	}
}

bool CurlHttpRequestBase::SetPostFields(const void *data, size_t size)
{
	if (!data || size == 0)
		post_fields_.clear();
	else {
		post_fields_.append((const char *)data, size);
	}
	return true;
}

bool CurlHttpRequestBase::AddForm(const char *name, const char *value)
{
	if (!name || !value)
		return false;
	return CURL_FORMADD_OK == curl_formadd(&form_post_,
										   &form_post_last_,
										   CURLFORM_COPYNAME,
										   name,
										   CURLFORM_COPYCONTENTS,
										   value,
										   CURLFORM_END);
}

bool CurlHttpRequestBase::AddForm(const char *name,
								  const void *data,
								  size_t data_length)
{
	if (!name || !data || data_length == 0)
		return false;
	return CURL_FORMADD_OK == curl_formadd(&form_post_,
										   &form_post_last_,
										   CURLFORM_COPYNAME,
										   name,
										   CURLFORM_COPYCONTENTS,
										   data,
										   CURLFORM_CONTENTSLENGTH,
										   data_length,
										   CURLFORM_END);
}

//static void EncodedWord(const std::string &input, std::string &output)
//{
//	// http://www.whatwg.org/specs/web-apps/current-work/multipage/association-of-controls-and-forms.html#multipart-form-data
//	output.append("=?utf-8?q?");
//	char hex[4] = {'='};
//	for (size_t i = 0; i < input.size(); i++) {
//		unsigned char ch = (unsigned char)input[i];
//		if (ch != '?' && ch != ' ' && isprint(ch)) {
//			output.push_back(ch);
//		} else {
//			static const char kDigits[] = "0123456789ABCDEF";
//			hex[1] = kDigits[ch >> 4];
//			hex[2] = kDigits[ch & 0xf];
//			output.append(hex);
//		}
//	}
//	output.append("?=");
//}

bool CurlHttpRequestBase::AddForm(const char *name,
#if defined(OS_WIN)
								  const wchar_t *file_path,
								  int filename_encoding
#else
								  const char *file_path,
#endif
							  )
{
	if (!name || !file_path)
		return false;

	std::string filename;

#if defined(OS_WIN)
	std::wstring wfilename;
	nbase::FilePathApartFileName(file_path, wfilename);
	nbase::win32::UnicodeToMBCS(wfilename, filename, filename_encoding);
#else
	nbase::FilePathApartFileName(file_path, filename);
#endif // OS_WIN

	FILE *file = nullptr;

#if defined(OS_WIN)
	if (_wfopen_s(&file, file_path, L"rb") != 0)
		file = nullptr;
#else
	file = fopen(file_path, "rb");
#endif // OS_WIN

	if (file == nullptr) {
		int err = errno;
#if defined(OS_WIN)
		//LOG_ERR(L"Open file failed(%d): `%s`", err, file_path);
#else
		//LOG_ERR("Open file failed(%d): `%s`", err, file_path);
#endif // OS_WIN
		return false;
	}

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	CURLFORMcode rc = curl_formadd(&form_post_,
								   &form_post_last_,
								   CURLFORM_COPYNAME,
								   name,
								   CURLFORM_FILENAME,
								   filename.c_str(),
								   CURLFORM_STREAM,
								   &upload_file_handles_,
								   CURLFORM_CONTENTTYPE,
								   "application/octet-stream",
								   CURLFORM_CONTENTSLENGTH,
								   file_size,
								   CURLFORM_END);
	if (CURL_FORMADD_OK != rc) {
		//LOG_ERR("Add form error %d", rc);
		fclose(file);
		return false;
	}

	upload_file_handles_.push_back(file);

	return true;
}

void CurlHttpRequestBase::ClearForms()
{
	if (form_post_ != NULL) {
		curl_formfree(form_post_);
		form_post_ = NULL;
		form_post_last_ = NULL;
	}
	for (auto iter = upload_file_handles_.begin();
		iter != upload_file_handles_.end(); iter++) {
		fclose(*iter);
	}
	upload_file_handles_.clear();
}

bool CurlHttpRequestBase::SetCookie(const std::string &cookie)
{
	//	DCHECK(IsRunning());
	if (IsRunning()) {
		return CURLE_OK == curl_easy_setopt(easy_handle_,
											CURLOPT_COOKIE,
											cookie.c_str());
	}
	return false;
}

bool CurlHttpRequestBase::GetCookieList(std::list<std::string> &cookies)
{
	//	DCHECK(IsRunning());
	cookies.clear();
	if (!IsRunning())
		return false;
	curl_slist *list = NULL;
	if (CURLE_OK == curl_easy_getinfo(easy_handle_,
									  CURLINFO_COOKIELIST,
									  &list)) {
		curl_slist *next = list;
		while (next) {
			cookies.push_back(next->data);
			next = next->next;
		}
		curl_slist_free_all(list);
		return true;
	}
	return false;
}

bool CurlHttpRequestBase::SetCookieList(const std::list<std::string> &cookies)
{
	//	DCHECK(IsRunning());
	if (!IsRunning())
		return false;
	std::list<std::string>::const_iterator iter = cookies.begin();
	for (; iter != cookies.end(); iter++) {
		if (CURLE_OK != curl_easy_setopt(easy_handle_,
										 CURLOPT_COOKIELIST,
										 iter->c_str()))
			return false;
	}
	return true;
}

bool CurlHttpRequestBase::SetReferer(const char *referer)
{
	//	DCHECK(IsRunning());
	if (IsRunning())
		return CURLE_OK == curl_easy_setopt(easy_handle_,
											CURLOPT_REFERER,
											referer);
	return false;
}

bool CurlHttpRequestBase::SetReferer(const std::string &referer)
{
	//	DCHECK(IsRunning());
	if (IsRunning())
		return CURLE_OK == curl_easy_setopt(easy_handle_,
											CURLOPT_REFERER,
											referer.c_str());
	return false;
}

void CurlHttpRequestBase::SetProxy(const Proxy &proxy)
{
	proxy_ = proxy;
}

void CurlHttpRequestBase::SetTimeout(long timeout_ms)
{
	timeout_ms_ = timeout_ms;
}

//static int ConvertProxyType(ProxyType type)
//{
//	switch (type) {
//	case ProxySock5:
//		return CURLPROXY_SOCKS5;
//	case ProxySock4:
//		return CURLPROXY_SOCKS4;
//	default:
//		return ProxyHttp11;
//	}
//}

static void ConfigProxy(CURL *easy_handle, const Proxy& proxy)
{
	if (proxy.type == ProxyNone)
		return;

	// "Daniel Stenberg"<daniel@haxx.se> tells me by mail that we are using
	// Proxy Tunnel, so we should turn on this flag. This helps me a lot.
	curl_easy_setopt(easy_handle, CURLOPT_HTTPPROXYTUNNEL, 1L);
	//curl_easy_setopt(easy_handle, CURLOPT_PROXYAUTH, CURLAUTH_ANYSAFE);
	curl_easy_setopt(easy_handle,
					 CURLOPT_PROXYTYPE,
					 proxy.type);
	curl_easy_setopt(easy_handle, CURLOPT_PROXY, proxy.host.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_PROXYPORT, proxy.port);

	if (!proxy.user.empty()) {
		curl_easy_setopt(easy_handle,
						 CURLOPT_PROXYUSERNAME,
						 proxy.user.c_str());
		if (!proxy.pass.empty())
			curl_easy_setopt(easy_handle,
							 CURLOPT_PROXYPASSWORD,
							 proxy.pass.c_str());
	}
}

bool CurlHttpRequestBase::OnEasyHandleCreated()
{
	// this method is not reenterable
	//	DCHECK(IsRunning());
	response_code_ = 0;

#ifdef _DEBUG
	curl_easy_setopt(easy_handle_, CURLOPT_VERBOSE, 1);
#endif

	curl_easy_setopt(easy_handle_, CURLOPT_ERRORBUFFER, error_buffer_);
	curl_easy_setopt(easy_handle_, CURLOPT_URL, url_.c_str());
	curl_easy_setopt(easy_handle_, CURLOPT_HTTPHEADER, header_list_);
	curl_easy_setopt(easy_handle_, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(easy_handle_, CURLOPT_COOKIEFILE, ""); // enable cookie
	curl_easy_setopt(easy_handle_,
					 CURLOPT_USERAGENT,
					 "NEngine/1.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	// Timeout in milliseconds  
	if (timeout_ms_ > 0) {
		curl_easy_setopt(easy_handle_, CURLOPT_TIMEOUT_MS, timeout_ms_);
	}

	if (!post_)
		curl_easy_setopt(easy_handle_, CURLOPT_POST, 0);
	else {
		curl_easy_setopt(easy_handle_, CURLOPT_POST, 1);
		if (form_post_ != NULL)
			curl_easy_setopt(easy_handle_, CURLOPT_HTTPPOST, form_post_);
		else {
			if (!post_fields_.empty()) {
				curl_easy_setopt(easy_handle_,
								 CURLOPT_POSTFIELDS,
								 post_fields_.data());
			}
			curl_easy_setopt(easy_handle_,
							 CURLOPT_POSTFIELDSIZE,
							 post_fields_.size());
		}
		// we have files to upload
		if (!upload_file_handles_.empty())
			curl_easy_setopt(easy_handle_, CURLOPT_READFUNCTION, ReadOSFile);
	}

	if (_strnicmp(url_.c_str(), "https://", 8) == 0) {
#ifdef SKIP_HOSTNAME_VERIFICATION
		// If the site you're connecting to uses a different host name that what
		// they have mentioned in their server certificate's commonName (or
		// subjectAltName) fields, libcurl will refuse to connect. You can skip
		// this check, but this will make the connection less secure.
		curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYHOST, 0);
#else
		curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYHOST, 2);
#endif
		// we don't verify the authenticity of the peer's certificate
		curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYPEER, 0);
	}

	ConfigProxy(easy_handle_, proxy_);

	return true;
}

void CurlHttpRequestBase::OnTransferDone()
{
	if (proxy_.IsEmpty())
		curl_easy_getinfo(easy_handle_,
						  CURLINFO_RESPONSE_CODE,
						  &response_code_);
	else {
		curl_easy_getinfo(easy_handle_,
						  CURLINFO_HTTP_CONNECTCODE,
						  &response_code_);
		if (response_code_ == 0) {
			curl_easy_getinfo(easy_handle_,
							  CURLINFO_RESPONSE_CODE,
							  &response_code_);
		}
	}
}

void CurlHttpRequestBase::OnEasyHandleDestroyed()
{
	ClearHeaderFields();
	ClearForms();
	post_fields_.clear();
}

void CurlHttpRequestBase::OnError()
{

}

} // namespace net
