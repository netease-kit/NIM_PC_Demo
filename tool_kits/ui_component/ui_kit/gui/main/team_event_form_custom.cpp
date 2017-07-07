#include "gui/main/team_event_form.h"
#include "gui/main/control/custom_msg.h"
#include "module/db/user_db.h"

namespace nim_comp
{
void TeamEventForm::GetMoreCustomMsg()
{
	uint32_t limit = 10;
	std::vector<nim::SysMessage> msgs = UserDB::GetInstance()->QueryMsgData(last_custom_msg_time_, limit);
	if (msgs.size() < limit)
	{
		has_more_custom_ = false;
	}
	for (auto it = msgs.begin(); it != msgs.end(); it++)
	{
		AddCustomMsg(*it, false);
		if (last_custom_msg_time_ == 0 || last_custom_msg_time_ > it->timetag_)
		{
			last_custom_msg_time_ = it->timetag_;
		}
	}
}

void TeamEventForm::AddCustomMsg(const nim::SysMessage& msg, bool first)
{
	CustomMsgBubble* msg_bubble = new CustomMsgBubble;
	if (first)
		custom_list_->AddAt(msg_bubble, 0);
	else
		custom_list_->Add(msg_bubble);
	nim::IMMessage message;
	CustomSysMessageToIMMessage(msg, message);
	msg_bubble->InitControl(message);

	if (last_custom_msg_time_ == 0)
	{
		last_custom_msg_time_ = msg.timetag_;
	}
}
}