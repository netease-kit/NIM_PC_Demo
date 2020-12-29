
#include "net_call_helper.h"
#include <vector>
#include "tool_kits/duilib/Utils/MultiLangSupport.h"
#include "src/cpp_sdk/nim/api/nim_cpp_talk.h"
#include "src/cpp_sdk/nim/api/nim_cpp_msglog.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "module/session/session_manager.h"

const static char* kNIMNetCallType = "type";
const static char* kNIMNetCallStatus = "status";
const static char* kNIMNetCallChannelId = "channelId";
const static char* kNIMNetCallDurations = "durations";
const static char* kNIMNetCallDuration = "duration";
const static char* kNIMNetCallAccid = "accid";
/*
G2话单为新增消息类型，话单信息位于msg.attach_,json格式，样例数据:
{
	"type":2,							//通话类型，1::音频，2:视频
	"channelId":52601694881751,			
	"status":1,
	"durations":[
		{
			"duration":7,				//通话时长
			"accid":"cqu227hk_1001"		//参与的用户accid
		},
		{
			"duration":7,
			"accid":"cqu227hk_1000"
		}
	]
}
*/
namespace nim_comp
{
	const std::vector<std::wstring> StatusTipMsgs = {
		L"",
		L"STRID_SESSION_NETCALL_MSG_COMPLETE",
		L"STRID_SESSION_NETCALL_MSG_CANCELED",
		L"STRID_SESSION_NETCALL_MSG_REJECTED",
		L"STRID_SESSION_NETCALL_MSG_TIMEOUT",
		L"STRID_SESSION_NETCALL_MSG_BUSY"
	};

	static std::atomic_bool hasRegMendMsgCb = false;

	std::wstring _ParseNetCallContent(const std::string& msg_attach)
	{
		Json::Value json_values;
		Json::Reader reader;
		reader.parse(msg_attach, json_values);
		if (!json_values.isObject())
		{
			QLOG_ERR(L"ParseNetCallMsg error: wrong msg attach_: {0}") << msg_attach;
			return L"";
		}

		std::wstring ret;
		if (json_values[kNIMNetCallStatus].isInt())
		{
			int status = json_values[kNIMNetCallStatus].asInt();
			int type = 2; //通话类型，1::音频，2:视频
			assert(status < StatusTipMsgs.size(), "net call msg info error");
			if (json_values[kNIMNetCallType].isInt())
				type = json_values[kNIMNetCallType].asInt();
			ret = ui::MutiLanSupport::GetInstance()->GetStringViaID(StatusTipMsgs[status]);

			if (NIMNetCallStatus(status) == kNIMNetCallStatusComplete)
			{
				if (type == 2) {
					auto strVideo = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_VIDEO_SET_VIDEO");
					ret = strVideo + ret;
				}
				else {
					auto strVideo = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_VIDEO_SET_AUDIO");
					ret = strVideo + ret;
				}
				assert(json_values[kNIMNetCallDurations].isArray()
					&& json_values[kNIMNetCallDurations].size() > 0);

				auto duration_format = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_NETCALL_MSG_DURATION");
				std::wstring duration = nbase::StringPrintf(duration_format.c_str(),
					json_values[kNIMNetCallDurations][0][kNIMNetCallDuration].asInt()
				);
				ret += duration;
			}
		}
		return ret;
	}
	std::wstring ParseNetCallMsg(const nim::SessionData& msg)
	{
		if (msg.msg_type_ != nim::kNIMMessageTypeG2NetCall)
		{
			QLOG_ERR(L"ParseNetCallMsg error: wrong msg type: {0}") << msg.msg_type_;
			return L"";
		}

		return _ParseNetCallContent(msg.msg_attach_);
	}

	std::wstring ParseNetCallMsg(const nim::IMMessage& msg)
	{
		if (msg.type_ != nim::kNIMMessageTypeG2NetCall) 
		{
			QLOG_ERR(L"ParseNetCallMsg error: wrong msg type: {0}") << msg.type_;
			return L"";
		}
		
		return _ParseNetCallContent(msg.attach_);
	}

	void OnSendNetCallMsgCb(const nim::SendMessageArc& arc)
	{
		if (arc.rescode_ != 200)
		{
			QLOG_ERR(L"OnSendNetCallMsgCb error: {0}") << arc.rescode_;
			return;
		}

		nim::MsgLog::QueryMsgByIDAysnc(arc.msg_id_, [arc](nim::NIMResCode res_code, const std::string& msg_id, const nim::IMMessage& msg) {
			if (res_code != 200)
			{
				QLOG_ERR(L"QueryMsgByIDAysnc error: {0}") << res_code;
				return;
			}

			SessionBox* session_form = SessionManager::GetInstance()->FindSessionBox(arc.talk_id_);
			if (session_form)
				session_form->ShowMsg(msg, false, false);
		});
	}
	void SendNetCallMsg(
		const std::string& to,
		const std::string& channelId,
		int type,
		int status,
		std::vector<std::string> members,
		std::vector<int> durations)
	{
		if (!hasRegMendMsgCb)
		{
			hasRegMendMsgCb = true;
			nim::Talk::RegSendMsgCb(OnSendNetCallMsgCb);
		}
			

		Json::Value values;
		Json::FastWriter writer;
		values[kNIMNetCallType] = type;
		values[kNIMNetCallStatus] = status;
		values[kNIMNetCallChannelId] = channelId;
		assert(members.size() == durations.size());

		for (int i = 0; i < members.size(); i++)
		{
			Json::Value info;
			info[kNIMNetCallAccid] = members[i];
			info[kNIMNetCallDuration] = durations[i];

			values[kNIMNetCallDurations].append(info);
		}

		auto attach_info = writer.write(values);
		
		std::string client_msg_id = QString::GetGUID();
		nim::MessageSetting setting;

		auto json_msg = nim::Talk::CreateG2NetCallMessage(to
			, nim::kNIMSessionTypeP2P
			, client_msg_id
			, attach_info
			, setting);

		nim::Talk::SendMsg(json_msg);
	}
}

