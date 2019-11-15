#pragma once
#include "module/thread/misc_thread.h"
#include "module/thread/db_thread.h"
#include "shared/ui/msgbox.h"

/** @class MainThread
  * @brief 主线程（UI线程）类，继承nbase::FrameworkThread
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class MainThread :
	public nbase::FrameworkThread
{
public:
	MainThread() : nbase::FrameworkThread("MainThread") {}
	virtual ~MainThread() {}
private:
	/**
	* 程序崩溃时的处理函数
	* @param[in] ret	用户的处理方式（关闭或重新运行程序）
	* @return void	无返回值
	*/
	void OnMsgBoxCallback(MsgBoxRet ret, const std::string& dmp_path);

	/**
	* 虚函数，初始化主线程
	* @return void	无返回值
	*/
	virtual void Init() override;

	/**
	* 虚函数，主线程退出时，做一些清理工作
	* @return void	无返回值
	*/
	virtual void Cleanup() override;

	/**
	* 主线程开始循环前，misc线程和db线程先开始循环
	* @return void	无返回值
	*/
	void PreMessageLoop();

	/**
	* 主线程结束循环前，misc线程和db线程先结束循环
	* @return void	无返回值
	*/
	void PostMessageLoop();
private:
	std::unique_ptr<MiscThread>	misc_thread_;
	std::unique_ptr<MiscThread>	screen_capture_thread_;
	std::unique_ptr<DBThread>	db_thread_;
	std::unique_ptr<MiscThread>	app_sdk_thread_;
};
class NimAPP : public nbase::Singleton< NimAPP,false>
{
	SingletonHideConstructor(NimAPP)
private:
	NimAPP() = default;
	~NimAPP() = default;
public:
	int Exec(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow);
	bool InitNim(const std::string& server_conf_file_path = "");
	void CleanupSDKBeforLogin();
private:
	int InitEnvironment(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow);
	void InitGlobalConfig(const std::string& server_conf_file_path,std::string& app_key, nim::SDKConfig& config);
	//int InitRedistPackages();
	int UninitEnvironment();
	int InitInstance(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow);
	int ExitInstance();
	void CheckRestartApp();
	std::unique_ptr<CComModule> com_module_;
};

