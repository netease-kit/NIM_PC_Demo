#include "stdafx.h"
#include "nim_ui_runtime_manager.h"
#include "module/runtime_data/runtime_data_manager.h"
namespace nim_ui
{
	void RunTimeDataManager::SetUIStyle(UIStyle style)
	{
		nim_comp::RunTimeDataManager::GetInstance()->SetUIStyle(style);
	}
	UIStyle RunTimeDataManager::GetUIStyle() const
	{
		return nim_comp::RunTimeDataManager::GetInstance()->GetUIStyle();
	}
}