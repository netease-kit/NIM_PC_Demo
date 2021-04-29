#pragma once
#include "stdafx.h"

class MainThread : public nbase::FrameworkThread
{
public:
	MainThread(std::wstring install_path) : nbase::FrameworkThread("MainThread")
	{
		install_path_ = install_path;
	}
	virtual ~MainThread() {}

	void EndSession();

private:
	void StartMiscThread();
	void StopMiscThread();
	virtual void Init() override;
	virtual void Cleanup() override;
	void ShowSetupWnd();

	std::wstring install_path_;
};
