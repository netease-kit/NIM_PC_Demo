#pragma once
#include "stdafx.h"

class MainThread : public nbase::FrameworkThread
{
public:
	MainThread(bool setup_running, bool yixin_running, bool update) : nbase::FrameworkThread("MainThread")
	{
		setup_running_ = setup_running;
		yixin_running_ = yixin_running;
		update_yixin_ = update;
	}
	virtual ~MainThread() {}

	void EndSession();

private:
	void StartMiscThread();
	void StopMiscThread();
	virtual void Init() override;
	virtual void Cleanup() override;
	void ShowSetupWnd();

	bool setup_running_;	//安装包正在运行
	bool yixin_running_;	//易信正在运行
	bool update_yixin_;		//易信更新正在运行
};
