#ifndef UI_ANIMATION_ANIMATIONPLAYER_H_
#define UI_ANIMATION_ANIMATIONPLAYER_H_

#pragma once

#include "base/base_export.h"

namespace ui 
{

typedef std::function<void (int)> PlayCallback;
typedef std::function<void (void)> CompleteCallback;

class UILIB_API AnimationPlayerBase : public nbase::SupportWeakCallback
{
public:
	AnimationPlayerBase();

	virtual void Init()
	{
		m_startValue = 0; 
		m_endValue = 0;
		m_totalMillSeconds = DUI_NOSET_VALUE;
		m_palyedMillSeconds = 0;
		m_elapseMillSeconds = 0;
		m_currentTime.QuadPart = 0;
		m_currentValue = 0;
		m_reverseStart = false;
		m_bPlaying = false;
		m_liPerfFreq.QuadPart = 0;
	}

	~AnimationPlayerBase()
	{
		
	}

	void SetStartValue(int startValue)
	{
		m_startValue = startValue;
	}
	int GetStartValue()
	{
		return m_startValue;
	}

	void SetEndValue(int endValue)
	{
		m_endValue = endValue;
	}
	int GetEndValue()
	{
		return m_endValue;
	}

	void SetTotalMillSeconds(int totalMillSeconds)
	{
		m_totalMillSeconds = totalMillSeconds;
	}

	void SetCallback(const PlayCallback& callback)
	{
		m_playcallback = callback;
	}

	void SetCompleteCallback(const CompleteCallback& callback);

	virtual void Start();
	virtual void Continue();
	virtual void ReverseContinue();
	virtual void StartTimer();
	
	bool IsPlaying() 
	{
		return m_bPlaying;
	}

	void Reset() 
	{
		weakFlagOwner.Cancel();
		Init();
	}

	virtual int GetCurrentValue() = 0;

	bool IsFirstRun()
	{
		return m_bFirstRun;
	}

private:
	void Play();
	void ReverseAllValue();
	void Complete();

protected:
	bool m_bFirstRun;
	PlayCallback m_playcallback;
	CompleteCallback m_completeCallback;
	int m_startValue;
	int m_endValue;
	int m_totalMillSeconds;
	double m_palyedMillSeconds;
	LARGE_INTEGER m_currentTime;
	int m_currentValue;
	int m_elapseMillSeconds;
	bool m_reverseStart;
	bool m_bPlaying;
	LARGE_INTEGER m_liPerfFreq;
	nbase::WeakCallbackFlag weakFlagOwner;
};


class UILIB_API AnimationPlayer : public AnimationPlayerBase
{
public:
	AnimationPlayer();

	virtual void Init() override
	{
		__super::Init();
		m_speedUpRatio = 0;
		m_speedDownRatio = 0;
		m_linerMillSeconds = 0;
		m_speedDownMillSeconds = 0;
		m_linearSpeed = 0;
		m_speedUpfactorA = 0;
		m_speedDownfactorA = 0;
		m_speedDownfactorB = 0;
		m_maxTotalMillSeconds = 1000000;
	}

	virtual void StartTimer() override;
	virtual int GetCurrentValue() override;
	void InitFactor();

	virtual void SetSpeedUpfactorA(double factorA)
	{
		m_speedUpfactorA = factorA;
	}

	virtual void SetSpeedDownfactorA(double factorA)
	{
		m_speedDownfactorA = factorA;
	}

	void SetMaxTotalMillSeconds(int maxTotalMillSeconds)
	{
		m_maxTotalMillSeconds = maxTotalMillSeconds;
	}

	void SetSpeedUpRatio(double speedUpRatio)
	{
		m_speedUpRatio = speedUpRatio;
	}

	void SetSpeedDownRatio(double speedDownRatio)
	{
		m_speedDownRatio = speedDownRatio;
	}

	void SetLinearSpeed(double linearSpeed)
	{
		m_linearSpeed = linearSpeed;
	}

private:
	double m_speedUpRatio;
	double m_speedDownRatio;
	double m_speedUpMillSeconds;
	double m_linerMillSeconds;
	double m_speedDownMillSeconds;
	double m_linearSpeed;
	double m_speedUpfactorA;
	double m_speedDownfactorA;
	double m_speedDownfactorB;
	int m_maxTotalMillSeconds;
};

} // namespace ui

#endif // UI_ANIMATION_ANIMATIONPLAYER_H_
