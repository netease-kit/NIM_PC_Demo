#include "stdafx.h"
#include "session_dock_def.h"
#include "session_form.h"
#include "module/plugins/main_plugins_manager.h"
#include "gui/plugins/session/session_plugin.h"

namespace nim_comp
{
	ISessionDock* ISessionDock::InstantDock()
	{
		ISessionDock* ret = nullptr;
		UIStyle type = RunTimeDataManager::GetInstance()->GetUIStyle();
		switch (type)
		{
		case nim_comp::UIStyle::conventional:
		{
			ret = new SessionForm;
		}
			break;
		case nim_comp::UIStyle::join:
		{
			auto&& plugin = MainPluginsManager::GetInstance()->GetPlugin(SessionPlugin::kPLUGIN_NAME);
			if (plugin != nullptr)
			{
				ret = std::dynamic_pointer_cast<SessionPlugin>(plugin)->GetSessionDock();
			}
		}
			break;
		default:
			break;
		}
		return ret;
	}
}