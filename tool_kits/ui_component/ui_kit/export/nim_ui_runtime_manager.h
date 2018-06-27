#pragma once
#include "module/runtime_data/runtime_data_defin.h"
namespace nim_ui
{
	using UIStyle = nim_comp::UIStyle;
	class RunTimeDataManager
	{
	public:
		RunTimeDataManager() = default;
		~RunTimeDataManager() = default;
		SINGLETON_DEFINE(RunTimeDataManager);
	public:
		void SetUIStyle(UIStyle style);
		UIStyle GetUIStyle() const;
	};
}