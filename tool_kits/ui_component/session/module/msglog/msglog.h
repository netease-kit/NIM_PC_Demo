#pragma once

enum MsgResLoadStatus
{
	RS_LOADING,
	RS_RELOAD,
	RS_LOAD_OK,
	RS_LOAD_NO
};

enum CustomMsgType
{
	CustomMsgType_Jsb = 1,	//石头剪子布
	CustomMsgType_SnapChat,	//阅后即焚
	CustomMsgType_Sticker,	//贴图
	CustomMsgType_Rts,		//白板的发起结束消息
};

struct MsgData
{
	//Content
	nim::NIMSessionType	to_type;
	std::string to_account;
	std::string from_account;
	long long	msg_time; //单位是ms
	int			msg_type;
	std::string msg_body;
	std::string msg_attach;
	std::string client_msg_id;
	int			resend_flag;
	int			from_client_type;
	std::string from_device_id;
	std::string from_nick;
	long long	server_msg_id;
	//Custom
	int			custom_save_flag;
	std::string custom_apns_text;
	//Other
	int	msg_code;
	int feature;
	std::string local_file_path;
	nim::NIMMsgLogStatus	msg_status;
	nim::NIMMsgLogSubStatus msg_sub_status;
	int history_save;		/**< int, 可选，该消息是否存储云端历史，0:不支持，1:支持, 默认1 */
	int msg_roaming;		/**< int, 可选，该消息是否支持漫游，0:不支持，1:支持, 默认1 */
	int msg_sync;

	MsgData()
	{
		msg_time = 0;
		msg_type = 0;
		resend_flag = 0;
		from_client_type = 0;
		server_msg_id = 0;

		custom_save_flag = 0;

		msg_code = 0;
		feature  = 0;
		msg_status = nim::kNIMMsgLogStatusNone;
		msg_sub_status = nim::kNIMMsgLogSubStatusNone;

		history_save = 1;
		msg_roaming = 1;
		msg_sync = 1;
	}
};

struct SessionMsgData
{
	//Content
	nim::NIMSessionType	to_type;
	std::string session_id;
	std::string from_account;
	long long	msg_time; //单位是ms
	int			msg_type;
	int			msg_status;
	std::string msg_body;
	std::string msg_attach;
	int			unread_count;
};

namespace nim_comp
{
//json <-> msg
void JsonToMsg(const Json::Value &json, MsgData &msg);

void MsgToJson(const MsgData &msg, Json::Value &json);

void CustomSysJsonToMsg(const Json::Value &json, MsgData &msg);

void CustomSysMsgToJson(const MsgData &msg, Json::Value &json);

bool StringToJson(const std::string &str, Json::Value &json);

void GetNotifyMsg(const std::string& msg_attach, const std::string& from_account, const std::string& to_account, std::wstring &show_text);

//判断是否是语音通话的话单消息
bool IsNetCallMsg(nim::NIMMessageType msg_type, const std::string& msg_attach);

std::wstring GetCustomMsg(const std::string &msg_attach);
}