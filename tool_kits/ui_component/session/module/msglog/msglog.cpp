#include "msglog.h"
#include "gui/session/control/session_util.h"
#include "gui/session/session_form.h"

namespace nim_comp
{
void JsonToMsg( const Json::Value &json, MsgData &msg )
{
	msg.to_type = json[nim::kNIMMsgKeyToType].asString() == "0" ? nim::kNIMSessionTypeP2P : nim::kNIMSessionTypeTeam;
	msg.to_account = json[nim::kNIMMsgKeyToAccount].asString();
	msg.from_account = json[nim::kNIMMsgKeyFromAccount].asString();
	msg.msg_time = json[nim::kNIMMsgKeyTime].asInt64();
	msg.msg_type = json[nim::kNIMMsgKeyType].asInt();
	msg.msg_body = json[nim::kNIMMsgKeyBody].asString();
	msg.msg_attach = json[nim::kNIMMsgKeyAttach].asString();
	msg.client_msg_id = json[nim::kNIMMsgKeyClientMsgid].asString();
	msg.resend_flag = json[nim::kNIMMsgKeyResendFlag].asInt();

	msg.from_client_type = json[nim::kNIMMsgKeyFromClientType].asInt();
	msg.from_device_id = json[nim::kNIMMsgKeyFromDeviceId].asString();
	msg.from_nick = json[nim::kNIMMsgKeyFromNick].asString();
	msg.server_msg_id = json[nim::kNIMMsgKeyServerMsgid].asUInt64();

	msg.msg_code = json[nim::kNIMMsgKeyLocalRescode].asInt();
	msg.feature = json[nim::kNIMMsgKeyLocalMsgFeature].asInt();
	msg.local_file_path = json[nim::kNIMMsgKeyLocalFilePath].asString();
	msg.msg_status = (nim::NIMMsgLogStatus)json[nim::kNIMMsgKeyLocalLogStatus].asInt();
	msg.msg_sub_status = (nim::NIMMsgLogSubStatus)json[nim::kNIMMsgKeyLocalLogSubStatus].asInt();
}

void MsgToJson( const MsgData &msg, Json::Value &json )
{
	json[nim::kNIMMsgKeyToType] = msg.to_type;
	json[nim::kNIMMsgKeyToAccount] = msg.to_account;
	json[nim::kNIMMsgKeyFromAccount] = msg.from_account;
	json[nim::kNIMMsgKeyTime] = msg.msg_time;
	json[nim::kNIMMsgKeyType] = msg.msg_type;
	json[nim::kNIMMsgKeyBody] = msg.msg_body;
	json[nim::kNIMMsgKeyAttach] = msg.msg_attach;
	json[nim::kNIMMsgKeyClientMsgid] = msg.client_msg_id;
	json[nim::kNIMMsgKeyResendFlag] = msg.resend_flag;

	json[nim::kNIMMsgKeyLocalFilePath] = msg.local_file_path;
	json[nim::kNIMMsgKeyLocalLogStatus] = msg.msg_status;
	json[nim::kNIMMsgKeyLocalLogSubStatus] = msg.msg_sub_status;

	json[nim::kNIMMsgKeyHistorySave] = msg.history_save;
	json[nim::kNIMMsgKeyMsgRoaming] = msg.msg_roaming;
	json[nim::kNIMMsgKeyMsgSync] = msg.msg_sync;
}

void CustomSysJsonToMsg(const Json::Value &json, MsgData &msg)
{
	msg.msg_type = json[nim::kNIMSysMsgKeyType].asInt();
	msg.to_account = json[nim::kNIMSysMsgKeyToAccount].asString();
	msg.from_account = json[nim::kNIMSysMsgKeyFromAccount].asString();
	msg.msg_time = json[nim::kNIMSysMsgKeyTime].asInt64();
	msg.msg_body = json[nim::kNIMSysMsgKeyMsg].asString();
	msg.msg_attach = json[nim::kNIMSysMsgKeyAttach].asString();
	msg.server_msg_id = json[nim::kNIMSysMsgKeyMsgId].asInt64();
	msg.custom_save_flag = json[nim::kNIMSysMsgKeyCustomSaveFlag].asInt();
	msg.custom_apns_text = json[nim::kNIMSysMsgKeyCustomApnsText].asString();
}

void CustomSysMsgToJson(const MsgData &msg, Json::Value &json)
{
	json[nim::kNIMSysMsgKeyType] = msg.msg_type;
	json[nim::kNIMSysMsgKeyToAccount] = msg.to_account;
	json[nim::kNIMSysMsgKeyFromAccount] = msg.from_account;
	json[nim::kNIMSysMsgKeyTime] = msg.msg_time;
	json[nim::kNIMSysMsgKeyMsg] = msg.msg_body;
	json[nim::kNIMSysMsgKeyAttach] = msg.msg_attach; 
	json[nim::kNIMSysMsgKeyMsgId] = msg.server_msg_id;
	json[nim::kNIMSysMsgKeyCustomSaveFlag] = msg.custom_save_flag;
	json[nim::kNIMSysMsgKeyCustomApnsText] = msg.custom_apns_text;
}

bool StringToJson( const std::string &str, Json::Value &json )
{
	Json::Reader reader;
	return reader.parse(str, json);
}

void GetNotifyMsg(const std::string& msg_attach, const std::string& from_account, const std::string& to_account, std::wstring &show_text)
{
	Json::Value json;
	if (StringToJson(msg_attach, json))
	{
		nim::NIMNotificationId id = (nim::NIMNotificationId)json[nim::kNIMNotificationKeyId].asInt();

		std::vector<std::string> ids;
		Json::Value array;

		if (id == nim::kNIMNotificationIdTeamInvite)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationInviteKeyUids];
		}
		else if (id == nim::kNIMNotificationIdTeamApplyPass)
		{
			std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationTeamApplyResUid].asString();
			if (LoginManager::GetInstance()->IsEqual(uid))
			{
				show_text = L"欢迎进入";
			}
			else
			{
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				std::string team_nick = session_wnd->GetTeamMemberInfo(uid).nick;
				std::wstring name = UserService::GetInstance()->GetUserName(uid);
				if (!team_nick.empty())
				{
					name = nbase::UTF8ToUTF16(team_nick);
				}
				show_text = nbase::StringPrintf(L"欢迎 %s 进入", name.c_str());
			}
		}
		else if (id == nim::kNIMNotificationIdTeamInviteAccept)
		{
			std::wstring who = UserService::GetInstance()->GetUserName(from_account);

			std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationTeamApplyResUid].asString();
			if (LoginManager::GetInstance()->IsEqual(uid))
			{
				show_text = nbase::StringPrintf(L"%s 接受了 我的 入群邀请", who.c_str());
			}
			else
			{
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				std::string team_nick = session_wnd->GetTeamMemberInfo(uid).nick;
				std::wstring name = UserService::GetInstance()->GetUserName(uid);
				if (!team_nick.empty())
				{
					name = nbase::UTF8ToUTF16(team_nick);
				}
				show_text = nbase::StringPrintf(L"%s 接受了 %s的 入群邀请", who.c_str(), name.c_str());
			}
		}
		else if (id == nim::kNIMNotificationIdTeamKick)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKickKeyUids];
		}
		else if (id == nim::kNIMNotificationIdTeamLeave)
		{
			if (LoginManager::GetInstance()->IsEqual(from_account))
			{
				show_text = L"你已离开";
			}
			else
			{
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				std::string team_nick = session_wnd->GetTeamMemberInfo(from_account).nick;
				std::wstring name = UserService::GetInstance()->GetUserName(from_account);
				if (!team_nick.empty())
				{
					name = nbase::UTF8ToUTF16(team_nick);
				}
				show_text = nbase::StringPrintf(L"%s 离开了", name.c_str());
			}
		}
		else if (id == nim::kNIMNotificationIdTeamUpdate)
		{
			std::string str = json[nim::kNIMNotificationKeyData]["tinfo"][nim::kNIMTeamInfoKeyName].asString();
			if( !str.empty() )
			{
				std::wstring team_name = nbase::UTF8ToUTF16(str);
				show_text = nbase::StringPrintf(L"名称更新为 %s", team_name.c_str());
			}
			else
			{
				str = json[nim::kNIMNotificationKeyData]["tinfo"][nim::kNIMTeamInfoKeyAnnouncement].asString();
				if( !str.empty() )
				{
					SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
					std::string team_nick = session_wnd->GetTeamMemberInfo(from_account).nick;
					std::wstring name = UserService::GetInstance()->GetUserName(from_account);
					if (!team_nick.empty())
					{
						name = nbase::UTF8ToUTF16(team_nick);
					}
					show_text = nbase::StringPrintf(L"%s 更新了群公告", name.c_str());
				}
				else
				{
					show_text = L"信息更新";
				}
			}
		}
		else if (id == nim::kNIMNotificationIdTeamDismiss)
		{
			show_text = L"已被解散";
		}
		else if (id == nim::kNIMNotificationIdTeamOwnerTransfer)
		{
			std::string to = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationOwnerTransferUid].asString();
			std::wstring name;
			if (LoginManager::GetInstance()->IsEqual(to)) {
				name = L"你";
			}
			else {
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				std::string team_nick = session_wnd->GetTeamMemberInfo(to).nick;
				name = UserService::GetInstance()->GetUserName(to);
				if (!team_nick.empty())
				{
					name = nbase::UTF8ToUTF16(team_nick);
				}
			}
			show_text = nbase::StringPrintf(L"群主转让给 %s", name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamAddManager)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationAddManagerUids];
		}
		else if (id == nim::kNIMNotificationIdTeamRemoveManager)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationRemoveManagerUids];
		}
		else if (id == nim::kNIMNotificationIdNetcallBill || \
			id == nim::kNIMNotificationIdNetcallMiss || \
			id == nim::kNIMNotificationIdLocalNetcallReject || \
			id == nim::kNIMNotificationIdLocalNetcallNoResponse)
		{
			//未接电话,{"time":139323423424,"calltype":1,"from":"account_temp","channel":6144978055925334000}
			//话单,{"time":139323423424,"calltype":1,"duration":5,"channel":6144978055925334000}
			int type = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallTypeKey].asUInt();
			std::string creater = from_account;
			bool b_create = LoginManager::GetInstance()->IsEqual(creater);
			std::wstring tm;// = GetMessageTime(time, false);
			if (type == nim::kNIMRtsVideoChatModeAudio)
			{
				show_text = tm + L"[语音通话]";
			}
			else
			{
				show_text = tm + L"[视频通话]";
			}
			if (id == nim::kNIMNotificationIdNetcallMiss)
			{
				show_text += L" 未接";
			} 
			else if (id == nim::kNIMNotificationIdNetcallBill)
			{
				__int64 duration = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallDurationKey].asInt64();
				int h = (int)(duration / 3600), m = (int)((duration % 3600) / 60), s = (int)(duration % 60);
				std::wstring time_tip;
				if (h > 0)
					nbase::StringPrintf(time_tip, L"%d小时%d分%d秒", h, m, s);
				else if (m > 0)
				{
					nbase::StringPrintf(time_tip, L"%d分%d秒", m, s);
				}
				else
					nbase::StringPrintf(time_tip, L"%d秒", s);
				if (type == nim::kNIMRtsVideoChatModeAudio)
				{
					show_text = nbase::StringPrintf(L"%s 时长%s", show_text.c_str(), time_tip.c_str());
				}
				else
				{
					show_text = nbase::StringPrintf(L"%s 时长%s", show_text.c_str(), time_tip.c_str());
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallReject)
			{
				if (b_create)
				{
					show_text += L" 对方正忙";
				} 
				else
				{
					show_text += L" 拒绝接听";
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallNoResponse)
			{
				show_text += L" 未接通，已取消";
			}

		}

		if( !array.empty() && array.isArray() )
		{
			int len = array.size();
			for(int i = 0; i < len; i++)
			{
				ids.push_back( array[i].asString() );
			}
			if( !ids.empty() )
			{
				std::wstring obj;
				bool only_you = false;

				int n = ids.size();
				if (n == 1 && LoginManager::GetInstance()->IsEqual(ids[0]))
				{
					obj = L"你";
					only_you = true;
				}
				else
				{
					int i = 0;
					for(; i < n; i++)
					{
						if( !obj.empty() )
							obj.append(L"，");

						SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
						std::string team_nick = session_wnd->GetTeamMemberInfo(ids[i]).nick;
						std::wstring name = UserService::GetInstance()->GetUserName(ids[i]);
						if (!team_nick.empty())
						{
							name = nbase::UTF8ToUTF16(team_nick);
						}
						obj.append(name);

						if(i >= 2)
							break;
					}
					if(i < n-1)
					{
						obj.append( nbase::StringPrintf(L"等%d人", n) );
					}
				}

				if (id == nim::kNIMNotificationIdTeamInvite)
				{
					if(only_you)
						show_text = nbase::StringPrintf(L"欢迎进入");
					else
						show_text = nbase::StringPrintf(L"欢迎 %s 进入", obj.c_str());
				}
				else if (id == nim::kNIMNotificationIdTeamKick)
					show_text = nbase::StringPrintf(L"%s 已被移出", obj.c_str());
				else if (id == nim::kNIMNotificationIdTeamAddManager)
					show_text = nbase::StringPrintf(L"%s 被任命为管理员", obj.c_str());
				else if (id == nim::kNIMNotificationIdTeamRemoveManager)
					show_text = nbase::StringPrintf(L"%s 被取消管理员资格", obj.c_str());
			}
		}

	}
	if( show_text.empty() )
		show_text = L"[通知消息]";
}

bool IsNetCallMsg(nim::NIMMessageType msg_type, const std::string& msg_attach)
{
	if (msg_type != nim::kNIMMessageTypeNotification)
	{
		return false;
	}
	Json::Value valus;
	Json::Reader reader;
	if (reader.parse(msg_attach, valus) && valus.isObject() && valus[nim::kNIMNotificationKeyData].isObject())
	{
		nim::NIMNotificationId id = (nim::NIMNotificationId)valus[nim::kNIMNotificationKeyId].asInt();
		if (id == nim::kNIMNotificationIdNetcallBill || \
			id == nim::kNIMNotificationIdNetcallMiss || \
			id == nim::kNIMNotificationIdLocalNetcallReject || \
			id == nim::kNIMNotificationIdLocalNetcallNoResponse)
		{
			return true;
		}
	}

	return false;
}
std::wstring GetCustomMsg(const std::string &msg_attach)
{
	std::wstring show_text = L"[自定义消息]";
	Json::Value json;
	if (StringToJson(msg_attach, json))
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_Jsb) //finger
		{
			show_text = L"[猜拳]";
		}
		else if (sub_type == CustomMsgType_SnapChat)
		{
			show_text = L"[阅后即焚]";
		}
		else if (sub_type == CustomMsgType_Sticker)
		{
			show_text = L"[贴图]";
		}
		else if (sub_type == CustomMsgType_Rts)
		{
			show_text = L"[白板]";
			if (json["data"].isObject())
			{
				int flag = json["data"]["flag"].asInt();
				if (flag == 0)
				{
					show_text = L"我已发起了[白板演示]";
				}
				else if (flag == 1)
				{
					show_text = L"白板演示已结束";
				}
			}
		}
	}
	return show_text;
}
}