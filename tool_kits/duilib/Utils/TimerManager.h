#ifndef UI_ANIMATION_ANIMATIONTIMER_H_
#define UI_ANIMATION_ANIMATIONTIMER_H_

#pragma once

#include "base/base_export.h"
#include "base/callback/callback.h"

namespace ui 
{

struct TIMERINFO
{
	typedef std::function<void ()> TimerCallback;

	bool operator < (const TIMERINFO& timerInfo) const {
		return dwTimeEnd.QuadPart > timerInfo.dwTimeEnd.QuadPart;
	}

	TIMERINFO() 
		: uPause(0), iRepeatTime(0)
	{
		dwTimeEnd.QuadPart = 0;
	}

	TimerCallback timerCallback;
	LONGLONG uPause;//定时器间隔
	LARGE_INTEGER dwTimeEnd;//定时器结束时间（单位1ms）
	std::weak_ptr<nbase::WeakFlag> weakFlag; //重复次数
	int iRepeatTime; //重复次数
};


class TimerManager
{
public:
	static TimerManager* GetInstance() {
		static TimerManager timerManager;
		return &timerManager;
	}

	const static int REPEAT_FOREVER = -1;
	bool AddCancelableTimer(const std::weak_ptr<nbase::WeakFlag>& weakFlag, const TIMERINFO::TimerCallback& callback, UINT uElapse, int iRepeatTime);

private:
	TimerManager();
	static LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static void CALLBACK TimeCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

	void Poll();

private:
	static HWND m_hideWnd;
	std::priority_queue<TIMERINFO> m_aTimers;
	LARGE_INTEGER m_liPerfFreq;
	bool m_isMinPause;
	UINT m_oldTimerId;
};

} // namespace ui

#endif // UI_ANIMATION_ANIMATIONTIMER_H_
