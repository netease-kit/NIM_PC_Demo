#include "nim_ui_notify_center.h"

namespace nim_ui
{

UnregisterCallback NotifyCenter::RegNotify(NotifyType type, OnNotify notify)
{
	return nim_comp::NotifyCenter::GetInstance()->RegNotify(type, notify);
}

}