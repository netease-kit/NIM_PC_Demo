#include "gui/main/control/session_item_helper.h"
#include "duilib/Utils/MultiLangSupport.h"
namespace nim_comp
{
	void SessionItemHelper::GetMsgContent(const nim::SessionData &msg, std::wstring &show_text)
	{
		ui::MutiLanSupport* mls = ui:: MutiLanSupport::GetInstance();
		if (msg.msg_type_ == nim::kNIMMessageTypeText)
		{
			show_text = nbase::UTF8ToUTF16(msg.msg_content_);
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeImage)
		{
			Json::Value msg_attach;
			if (Json::Reader().parse(msg.msg_attach_, msg_attach) && msg_attach.isMember("description"))
				show_text = nbase::UTF8ToUTF16(msg_attach["description"].asString());
			if (show_text.empty())
				show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_IMAGE");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeAudio)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_AUDIO");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeVideo)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_VIDEO");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeFile)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_FILE");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeLocation)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_LOCATION");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeNotification)
		{
			GetNotifyMsg(msg.msg_attach_, msg.msg_sender_accid_, "", show_text, msg.id_);
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeCustom)
		{
			show_text = GetCustomMsg(msg.msg_sender_accid_, msg.msg_attach_);
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeTips)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_NOTIFY");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeUnknown)
		{
			show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_UNDEFINED");
		}
		else if (msg.msg_type_ == nim::kNIMMessageTypeRobot)
		{
			bool out_msg = false;
			Json::Value values;
			Json::Reader reader;
			if (reader.parse(msg.msg_attach_, values) && values.isObject())
				out_msg = values[nim::kNIMBotRobotReceivedMsgKeyMsgOut].asBool();
			if (!out_msg)
				show_text = nbase::UTF8ToUTF16(msg.msg_content_);
			else
				show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_ROBOT");
		}
		else
		{
			Json::Value extern_info;
			if (Json::Reader().parse(msg.msg_attach_, extern_info) && extern_info.isMember("virtual_local_message") && extern_info["virtual_local_message"].asBool())
			{
				show_text = L"";
			}
			else
			{
				show_text = mls->GetStringViaID(L"STRID_SESSION_ITEM_MSG_TYPE_UNKNOWN");
				QLOG_WAR(L"unknown msg: id_type={0}_{1} msg_type={2}") << msg.id_ << msg.type_ << msg.msg_type_;
			}
		}
	}
}