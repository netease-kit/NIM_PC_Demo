#include "multiport_push_callback.h"
#include "gui/profile_form/profile_form.h"

namespace nim_comp
{
	void MultiportPushCallback::OnMultiportPushConfigChange(int rescode, bool switch_on)
	{
		if (rescode == nim::kNIMResSuccess)
		{
			ProfileForm *form = (ProfileForm*)WindowsManager::GetInstance()->GetWindow(ProfileForm::kClassName, ProfileForm::kClassName);
			if (form != NULL)
				form->OnMultiportPushConfigChange(switch_on);
		}
	}
}