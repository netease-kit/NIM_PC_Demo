#pragma once
#include "runtime_data_defin.h"
namespace nim_comp
{	
	class RunTimeDataManager
	{
	public:
		RunTimeDataManager() = default;
		~RunTimeDataManager() = default;
		SINGLETON_DEFINE(RunTimeDataManager);
	public:
		void SetUIStyle(UIStyle style){ uistyle_ = style; };
		UIStyle GetUIStyle() const { return uistyle_; }
	private:
		UIStyle uistyle_ = UIStyle::join;
	};
}