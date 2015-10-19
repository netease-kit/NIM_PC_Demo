// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/9/25
//
// Making http requests using Curl on a MessageLoop based thread

#include "net/http/curl_http_request.h"

#include <shlobj.h>
#include "base/file/file_util.h"
#include "base/util/string_util.h"
#include "base/framework/message_loop_proxy.h"
//#include "base/log/log.h"



namespace net
{

size_t CurlHttpRequest::WriteHeader(void* ptr,
	size_t size,
	size_t count,
	void* param)
{
	size_t bytes_to_store = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);
	if (request->response_code_ == 0)
	{
		std::string response_code;
		response_code.append(static_cast<char *>(ptr), bytes_to_store);
		auto first_space_pos = response_code.find(" ");
		assert(first_space_pos > 0);
		if (first_space_pos > 0) {
			auto second_space_pos = response_code.find(" ", first_space_pos + 1);
			assert(second_space_pos > 0);
			if (second_space_pos > 0) {
				response_code = response_code.substr(first_space_pos + 1, second_space_pos - first_space_pos - 1);
				request->response_code_ = atoi(response_code.c_str());
			}
		}
	}
	return bytes_to_store;
}

size_t CurlHttpRequest::WriteOSFile(void* ptr,
									size_t size,
									size_t count,
									void* param)
{
	size_t bytes_to_write = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);

	// This function may be called with zero bytes data if the transferred file
	// is empty.
	if (bytes_to_write == 0) {
		//		LOG_WAR("zero byte recieved");
		return 0;
	}

	if (!request->OpenFileForWrite()) {
		return 0;
	}

	size_t bytes_written =
		fwrite(ptr, 1, bytes_to_write, request->file_handle_.get());
	if (bytes_written != bytes_to_write) {
		//		LOG_ERR("Write file error");
		return 0; // on error
	}
	return bytes_written;
}


size_t CurlHttpRequest::WriteOSFileRange( void *ptr, size_t size, size_t count, void *param )
{
	size_t bytes_to_write = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);

	// This function may be called with zero bytes data if the transferred file
	// is empty.
	if (bytes_to_write == 0) {
		//		LOG_WAR("zero byte recieved");
		return 0;
	}
	
	// to make sure the temp_file_handle and cfg_file_handle is valid
	if (!request->OpenFileForRangeWrite()) {
		return 0;
	}

	// write recived data to temp file
	size_t bytes_written = fwrite(ptr, 1, bytes_to_write, request->file_handle_.get());
	if (bytes_written != bytes_to_write) {
		//LOG_ERR("Write temp file error");
		return 0; // on error
	}
	if(fflush(request->file_handle_.get()) != 0)
	{
		//LOG_ERR("Flush temp file error");
		return 0; 
	}
	request->range_start_ += static_cast<long long>(bytes_to_write);

	// to record position
	size_t bytes_cfg_written = request->WriteCfgFile();
	if(bytes_cfg_written == 0){
		//LOG_ERR("Write cfg file error");
	}

	return bytes_written;
}

size_t CurlHttpRequest::WriteMemory(void* ptr,
									size_t size,
									size_t count,
									void* param)
{
	size_t bytes_to_store = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);
	request->content_->append(static_cast<char *>(ptr), bytes_to_store);
	return bytes_to_store;
}

int CurlHttpRequest::ProgressCB(
	void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(clientp);
	request->NotifyProgress(dltotal, dlnow, ultotal, ulnow);
	return 0;
}

CurlHttpRequest::CurlHttpRequest(const std::string &url,
								 const wchar_t *download_file_path,
								 const CompletionCallback &file_callback,
								 const ProgressCallback &progress_callback,
								 bool post) :
	CurlHttpRequestBase(url, post), memory_(false),
	range_start_(-1), response_code_(0), content_(new std::string),
	file_callback_(file_callback), progress_callback_(progress_callback)
{
	//	DCHECK(download_file_path != nullptr);
	if (download_file_path != nullptr)
		download_file_path_ = download_file_path;
	callback_message_loop_ = nbase::MessageLoopProxy::current();
}

CurlHttpRequest::CurlHttpRequest( const std::string &url,
								  const wchar_t *download_file_path,
								  long long range_start,
								  const CompletionCallback &file_callback,
								  const ProgressCallback &progress_callback /*= ProgressCallback()*/,
								  bool post /*= false*/ ) :
	CurlHttpRequestBase(url, post), memory_(false),
	range_start_(range_start), response_code_(0), content_(new std::string),
	file_callback_(file_callback), progress_callback_(progress_callback)
{
	if(range_start_ < 0) { 
		range_start_ = 0;
	}

	//	DCHECK(download_file_path != nullptr);
	if (download_file_path != nullptr)
		download_file_path_ = download_file_path;
	callback_message_loop_ = nbase::MessageLoopProxy::current();
}

CurlHttpRequest::CurlHttpRequest(const std::string &url,
								 const ContentCallback &content_callback,
								 const ProgressCallback &progress_callback,
								 bool post) :
	CurlHttpRequestBase(url, post), memory_(true),
	range_start_(-1), response_code_(0), content_(new std::string),
	content_callback_(content_callback), progress_callback_(progress_callback)
{
	callback_message_loop_ = nbase::MessageLoopProxy::current();
}

const void* CurlHttpRequest::content() const
{
	if (!memory_ || content_ == nullptr)
		return nullptr;
	return content_->c_str();
}

size_t CurlHttpRequest::content_length() const
{
	if (!memory_ || content_ == nullptr)
		return 0;
	return content_->length();
}

bool CurlHttpRequest::OnEasyHandleCreated()
{
	if (!__super::OnEasyHandleCreated())
		return false;

	curl_easy_setopt(easy_handle_, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(easy_handle_, CURLOPT_HEADERFUNCTION, WriteHeader);

	if (memory_) {
		curl_easy_setopt(easy_handle_, CURLOPT_WRITEDATA, this);
		curl_easy_setopt(easy_handle_, CURLOPT_WRITEFUNCTION, WriteMemory);
	} else {
		if(range_start_ >= 0) {
			std::string range;
			nbase::StringPrintf(range, "%I64d-", range_start_);
			curl_easy_setopt(easy_handle(), CURLOPT_RANGE, range.c_str());
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEDATA, this);
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEFUNCTION, WriteOSFileRange);
		}
		else
		{
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEDATA, this);
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEFUNCTION, WriteOSFile);
		}
	}

	if (progress_callback_) {
		curl_easy_setopt(easy_handle(), CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(easy_handle(), CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(easy_handle(), CURLOPT_PROGRESSFUNCTION, ProgressCB);
	}

	return true;
}

bool CurlHttpRequest::OpenFileForWrite()
{
	if (!file_handle_) {
		// create the directory first
		std::wstring directory;
		nbase::FilePathApartDirectory(download_file_path_, directory);
		if( !nbase::CreateDirectory(directory) )
		{
			//LOG_ERR(L"Create Directory Failed %s %d", directory.c_str(), GetLastError());
		}

		file_handle_.reset(nbase::OpenFile(download_file_path_, L"wb"));
		if (!file_handle_) {
#if defined(COMPILER_MSVC)
			//LOG_ERR(L"Open local file failed, %s %d", download_file_path_.c_str(), errno);
#else
			//LOG_ERR(L"Open local file failed, %s %d", download_file_path_.c_str(), errno);
#endif
			return false;
		}
	}
	return true;
}

bool CurlHttpRequest::OpenFileForRangeWrite()
{
	// open temp file for downloading
	if (!file_handle_) {
		std::wstring temp_path = download_file_path_;
		temp_path.append(L".tmp");
		// create the directory first
		std::wstring directory;
		nbase::FilePathApartDirectory(temp_path, directory);
		nbase::CreateDirectory(directory);

		file_handle_.reset(nbase::OpenFile(temp_path, L"rb+"));
		if (!file_handle_) {
#if defined(COMPILER_MSVC)
			//LOG_ERR(L"Open local file failed, %s", temp_path.c_str());
#else
			//LOG_ERR(L"Open local file failed, %s", temp_path.c_str());
#endif
			return false;
		}

		if( range_start_ >= 0)
		{
			if(_fseeki64(file_handle_.get(), range_start_, SEEK_SET) != 0) {
#if defined(COMPILER_MSVC)
				//LOG_ERR(L"Seek local file failed, %s, seek position: %I64d", temp_path.c_str(), range_start_);
#else
				//LOG_ERR(L"Seek local file failed, %s, seek position: %I64d", temp_path.c_str(), range_start_);
#endif
			}
		}
	}

	// create or open cfg file for recoding downloaded size 
	// in order to support continuing transferring from breakpoint
	if(!cfg_file_handle_){
		std::wstring cfg_path = download_file_path_;
		cfg_path.append(L".cfg");
		// create the directory first
		std::wstring directory;
		nbase::FilePathApartDirectory(cfg_path, directory);
		nbase::CreateDirectory(directory);

		cfg_file_handle_.reset(nbase::OpenFile(cfg_path, L"rb+"));
		if (!cfg_file_handle_) {
#if defined(COMPILER_MSVC)
			//LOG_ERR(L"Open cfg file failed, %s", cfg_path.c_str());
#else
			//LOG_ERR(L"Open cfg file failed, %s", cfg_path.c_str());
#endif
			return false;
		}
	}

	return true;
}

size_t CurlHttpRequest::WriteCfgFile()
{
	if(!cfg_file_handle_)
		return 0;

	if(fseek(cfg_file_handle_.get(), 0, SEEK_SET) != 0)
	{
		//LOG_ERR("Seek to cfg file begin error");
		return 0;
	}

	size_t bytes_written = fwrite(&range_start_, 1, sizeof(long long), cfg_file_handle_.get());
	if (bytes_written != sizeof(long long)) {
		//LOG_ERR("Write cfg file error");
		return 0;
	}

	if(fflush(cfg_file_handle_.get()) != 0)
	{
		//LOG_ERR("Flush cfg file error");
		return 0; 
	}

	return bytes_written;
}

void CurlHttpRequest::OnTransferDone()
{
	__super::OnTransferDone();

	if (!memory_ && file_handle_) {
		file_handle_.reset(nullptr);
		if(cfg_file_handle_){
			cfg_file_handle_.reset(nullptr);
		}
	}
}

void CurlHttpRequest::OnEasyHandleDestroyed()
{
	__super::OnEasyHandleDestroyed();
	if (file_handle_)
		file_handle_.reset(nullptr);
	if (cfg_file_handle_)
		cfg_file_handle_.reset(nullptr);

	NotifyCompletion();
}

void CurlHttpRequest::OnError()
{
	__super::OnError();

	if (file_handle_) {
		file_handle_.reset(nullptr);
		// break point transfer should not
		// delete temp file while http error occurred.
		if(!cfg_file_handle_){
			nbase::DeleteFile(download_file_path_);
		}
	}
	if(cfg_file_handle_) {
		cfg_file_handle_.reset(nullptr);
	}
}

void CurlHttpRequest::NotifyCompletion()
{
	if (memory_) {
		if (!content_callback_)
			return;
		ContentCallback cb = content_callback_;
		content_callback_ = nullptr;
		if (callback_message_loop_ != nullptr) {
			callback_message_loop_->PostTask(nbase::Bind(cb, content_, download_ok(), response_code_));
		} else {
			// Run the callback on current thread
			cb(content_, download_ok(), response_code_);
		}
	} else {
		if (!file_callback_)
			return;
		CompletionCallback cb = file_callback_;
		file_callback_ = nullptr;
		if (callback_message_loop_ != nullptr) {
			callback_message_loop_->PostTask(nbase::Bind(cb, download_ok(), response_code_));
		} else {
			cb(download_ok(), response_code_);
		}
	}
	progress_callback_ = nullptr;
}

void CurlHttpRequest::NotifyProgress(
	double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (!progress_callback_)
	{
		return;
	}

	if (callback_message_loop_ != nullptr) {
		callback_message_loop_->PostTask(
			nbase::Bind(progress_callback_, ultotal, ulnow, dltotal, dlnow));
	} else {
		progress_callback_(ultotal, ulnow, dltotal, dlnow);
	}
}


}
