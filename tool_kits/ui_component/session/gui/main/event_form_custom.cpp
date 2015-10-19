#include "gui/main/team_event_form.h"
#include "gui/main/control/custom_msg.h"
#include "module/msglog/msg_extend_db.h"

namespace nim_comp
{
void TeamEventForm::GetMoreCustomMsg()
{
	uint32_t limit = 10;
	std::vector<MsgData> msgs = MsgExDB::GetInstance()->QueryMsgData(last_custom_msg_time_, limit);
	if (msgs.size() < limit)
	{
		has_more_custom_ = false;
	}
	for (auto it = msgs.begin(); it != msgs.end(); it++)
	{
		AddCustomMsg(*it, false);
		if (last_custom_msg_time_ == 0 || last_custom_msg_time_ > it->msg_time)
		{
			last_custom_msg_time_ = it->msg_time;
		}
	}
}

void TeamEventForm::AddCustomMsg(const MsgData& msg, bool first)
{
	CustomMsgBubble* msg_bubble = new CustomMsgBubble;
	if (first)
		custom_list_->AddAt(msg_bubble, 0);
	else
		custom_list_->Add(msg_bubble);
	msg_bubble->InitControl(msg);

	if (last_custom_msg_time_ == 0)
	{
		last_custom_msg_time_ = msg.msg_time;
	}
}
}