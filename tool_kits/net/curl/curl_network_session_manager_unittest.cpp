// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/30
//
// Unit test cases for CurlNetworkSession, CurlNetworkSessionManager, etc.

#if defined(WITH_UNITTEST)

#include <memory>
#include "base/log/log.h"
#include "base/thread/framework_thread.h"
#include "base/util/string_util.h"
#include "base/win32/path_util.h"
#include "net/curl/curl_network_session_manager.h"
#include "net/http/curl_http_request.h"
#include "gtest/gtest.h"

namespace
{

class CurlTransferThread:
	public nbase::FrameworkThread
{
public:
	CurlTransferThread()
		: FrameworkThread("CurlTransferThread"), need_progress_(false) {}

	void Init()
	{
		static const int kUrlCount = 10;
		static const char *kUrls[kUrlCount] = {
			"http://www.163.com",
			"http://www.qq.com",
			"http://www.sina.com",
			"http://www.baidu.com",
			"http://www.sohu.com",
			"http://www.youku.com",
			"http://www.ifeng.com/",
			"http://www.taobao.com/",
			"http://www.autohome.com.cn/",
			"http://www.iqiyi.com/",
		};
		static const wchar_t *kLocal[] = {
			L"网易",
			L"腾讯",
			L"新浪",
			L"百度",
			L"搜狐",
			L"优酷",
			L"凤凰网",
			L"淘宝",
			L"汽车之家",
			L"爱奇艺",
		};
		manager_.reset(new net::CurlNetworkSessionManager(
			nbase::MessageLoop::current()->message_loop_proxy()));

		std::wstring dir = nbase::win32::GetCurrentModuleDirectory();

		int max_num_sessions = 128;
		if (need_progress_)
			max_num_sessions = 1;
		for (int i = 0; i < max_num_sessions; i++) {
			std::wstring file_path = nbase::StringPrintf(L"%s\\testing\\%s_%d.html",
				dir.c_str(),
				kLocal[i%kUrlCount],
				i);
			std::string url = std::string(kUrls[i%kUrlCount]);
			nbase::scoped_refptr<net::CurlHttpRequest>
				request1(new net::CurlHttpRequest(
					url,
					file_path.c_str(),
					std::bind(&CurlTransferThread::OnComplete,
								this,
								url,
								file_path,
								std::placeholders::_1),
					need_progress_ ?
					std::bind(&CurlTransferThread::OnProgress,
								this,
								url,
								std::placeholders::_1,
								std::placeholders::_2,
								std::placeholders::_3,
								std::placeholders::_4) :
					net::CurlHttpRequest::ProgressCallback())); 
			nbase::ThreadManager::PostTask(
				std::bind(&CurlTransferThread::AddSession,
				this,
				request1));
		}
	}

	void AddSession(const nbase::scoped_refptr<net::CurlHttpRequest>& request)
	{
		manager_->AddSession(request.get());
	}

	void OnComplete(const std::string &url,
					const std::wstring &local,
					int error)
	{
		std::wstring str;
		nbase::UTF8ToUTF16(url, str);
		LOG_APP(L"Download `%s` -> `%s` finished, error: %d", str.c_str(), local.c_str(), error);

		if (manager_->still_running() == 0)
			nbase::FrameworkThread::current()->StopSoon();
	}

	void OnProgress(const std::string &url,
		double dt, double dn, double ut, double un)
	{
		LOG_APP("%s: Download %lf/%lf, Upload %lf/%lf",
				url.c_str(), dn, dt, un, ut);
	}

	void Cleanup()
	{
		manager_.reset(nullptr);
	}

	void set_need_progress(bool set) { need_progress_ = set; }

private:
	bool need_progress_;
	std::unique_ptr<net::CurlNetworkSessionManager> manager_;
};

class CurlNetworkTest : public testing::Test
{
public:
	virtual void TearDown()
	{
		thread_.Stop();
	}

	void UseProgress()
	{
		thread_.set_need_progress(true);
	}

	void RunUtilFinished()
	{
		thread_.StartWithLoop(nbase::MessageLoop::kUVMessageLoop);
		thread_.Close();
	}

private:
	CurlTransferThread thread_;
};

} // namespace

TEST_F(CurlNetworkTest, HttpConcurrency)
{
	RunUtilFinished();
}

TEST_F(CurlNetworkTest, Progress)
{
	UseProgress();
	RunUtilFinished();
}

#endif // WITH_UNITTEST
