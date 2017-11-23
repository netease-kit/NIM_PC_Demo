/** @file nim_team_helper.cpp
  * @brief Team 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/14
  */

#include "nim_team_helper.h"

namespace nim
{

void ParseTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, TeamEvent& team_event)
{
	team_event.res_code_ = (NIMResCode)rescode;
	team_event.team_id_ = team_id;
	team_event.notification_id_ = notification_id;
	Json::Reader reader;
	Json::Value values;
	if (reader.parse(team_event_json, values) && values.isObject())
	{
		//操作者和被操作者用户名片
		Json::Value name_cards = values[kNIMNotificationKeyData][kNIMNotificationKeyUserNameCards];
		if (!name_cards.empty() && name_cards.isArray())
			ParseNameCards(name_cards, team_event.namecards_);

		switch (notification_id)
		{
		case kNIMNotificationIdTeamKick:
		case kNIMNotificationIdTeamInvite:
			{
				if (values[kNIMNotificationKeyData].isMember("attach"))
					team_event.attach_ = values[kNIMNotificationKeyData]["attach"].asString();
			}
		case kNIMNotificationIdTeamAddManager:
		case kNIMNotificationIdTeamRemoveManager:
		case kNIMNotificationIdLocalCreateTeam:
		{
			//ids
			Json::Value ids = values[kNIMNotificationKeyData][kNIMNotificationKeyDataIds];
			int len = ids.size();
			for (int i = 0; i < len; i++)
			{
				team_event.ids_.push_back(ids[i].asString());
			}

			if (values[kNIMNotificationKeyData].isMember(kNIMNotificationKeyDataInvalidIds))
			{
				ids = values[kNIMNotificationKeyData][kNIMNotificationKeyDataInvalidIds];
				int len = ids.size();
				for (int i = 0; i < len; i++)
				{
					team_event.invalid_ids_.push_back(ids[i].asString());
				}
			}
		}
		break;
		case kNIMNotificationIdTeamLeave:
		case kNIMNotificationIdTeamApplyPass:
		case kNIMNotificationIdTeamInviteAccept:
		case kNIMNotificationIdLocalRejectApply:
		case kNIMNotificationIdLocalRejectInvite:
			{
				//id
				team_event.ids_.push_back(values[kNIMNotificationKeyData][kNIMNotificationKeyDataId].asString());
			}
			break;
		case kNIMNotificationIdTeamUpdate:
		case kNIMNotificationIdTeamSyncCreate:
		case kNIMNotificationIdLocalGetTeamInfo:
			{
				//info
				ParseTeamInfoJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamInfo], team_event.team_info_);
			}
			break;
		case kNIMNotificationIdTeamDismiss:
		case kNIMNotificationIdLocalApplyTeam:
		case kNIMNotificationIdLocalGetTeamList:
			{
				//无
				//无内容需要解析
			}
			break;
		case kNIMNotificationIdTeamOwnerTransfer:
			{
				team_event.opt_ = values[kNIMNotificationKeyData]["leave"].asBool();
				team_event.ids_.push_back(values[kNIMNotificationKeyData][kNIMNotificationKeyDataId].asString());
			}
			break;
		case kNIMNotificationIdTeamMemberChanged:
			{
				//team member property
				ParseTeamMemberPropertyJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember], team_event.member_property_);
				team_event.ids_.push_back(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember][kNIMTeamUserKeyAccID].asString());
			}
			break;
		case kNIMNotificationIdLocalUpdateOtherNick:
			{
				//与kNIMNotificationIdLocalUpdateMemberProperty事件重复，上层需要做去重
				ParseTeamMemberPropertyJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember], team_event.member_property_);
			}
			break;
		case kNIMNotificationIdTeamSyncUpdateMemberProperty:
		case kNIMNotificationIdLocalUpdateMemberProperty:
			{
				ParseTeamMemberPropertyJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember], team_event.member_property_);
			}
			break;
		case kNIMNotificationIdTeamMuteMember:
			{
				ParseTeamInfoJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamInfo], team_event.team_info_);
			}
		case kNIMNotificationIdLocalMuteMember:
			{
				team_event.opt_ = values[kNIMNotificationKeyData]["mute"].asInt() == 1;
				team_event.ids_.push_back(values[kNIMNotificationKeyData][kNIMNotificationKeyDataId].asString());
			}
			break;
		default:
			break;
		}
	}
}


void ParseTeamInfoJson(const Json::Value& team_info_json, TeamInfo& team_info)
{
	team_info.SetType((nim::NIMTeamType)(team_info_json[nim::kNIMTeamInfoKeyType].asInt()));
	team_info.SetTeamID(team_info_json[nim::kNIMTeamInfoKeyID].asString());
	team_info.SetOwnerID(team_info_json[nim::kNIMTeamInfoKeyCreator].asString());
	team_info.SetName(team_info_json[nim::kNIMTeamInfoKeyName].asString());
	team_info.SetIntro(team_info_json[nim::kNIMTeamInfoKeyIntro].asString());
	team_info.SetAnnouncement(team_info_json[nim::kNIMTeamInfoKeyAnnouncement].asString());
	team_info.SetJoinMode((NIMTeamJoinMode)(team_info_json[nim::kNIMTeamInfoKeyJoinMode].asInt()));
	team_info.SetServerCustom(team_info_json[nim::kNIMTeamInfoKeyServerCustom].asString());
	team_info.SetCreateTimetag(team_info_json[nim::kNIMTeamInfoKeyCreateTime].asUInt64());
	team_info.SetUpdateTimetag(team_info_json[nim::kNIMTeamInfoKeyUpdateTime].asUInt64());
	team_info.SetCustom(team_info_json[nim::kNIMTeamInfoKeyCustom].asString());
	team_info.SetMemberCount(team_info_json[nim::kNIMTeamInfoKeyMemberCount].asUInt());
	team_info.SetProperty(team_info_json[nim::kNIMTeamInfoKeyProperty].asString());
	team_info.SetValid(team_info_json[nim::kNIMTeamInfoKeyValidFlag].asUInt() == 0 ? false : true);
	//群属性,开发者无需关注 20161011 oleg
	//team_info.SetConfigBits(team_info_json[nim::kNIMTeamInfoKeyBits].asUInt64());
	team_info.SetMemberValid(team_info_json[nim::kNIMTeamInfoKeyMemberValid].asUInt() == 0 ? false : true);
	team_info.SetIcon(team_info_json[nim::kNIMTeamInfoKeyIcon].asString());
	team_info.SetBeInviteMode(team_info_json[nim::kNIMTeamInfoKeyBeInviteMode].asInt() == 0 ? kNIMTeamBeInviteModeNeedAgree : kNIMTeamBeInviteModeNotNeedAgree);
	team_info.SetInviteMode(team_info_json[nim::kNIMTeamInfoKeyInviteMode].asInt() == 0 ? kNIMTeamInviteModeManager : kNIMTeamInviteModeEveryone);
	team_info.SetUpdateInfoMode(team_info_json[nim::kNIMTeamInfoKeyUpdateInfoMode].asInt() == 0 ? kNIMTeamUpdateInfoModeManager : kNIMTeamUpdateInfoModeEveryone);
	team_info.SetUpdateCustomMode(team_info_json[nim::kNIMTeamInfoKeyUpdateCustomMode].asInt() == 0 ? kNIMTeamUpdateCustomModeManager : kNIMTeamUpdateCustomModeEveryone);
#if NIMAPI_UNDER_WIN_DESKTOP_ONLY
	team_info.SetAllMemberMute(team_info_json[nim::kNIMTeamInfoKeyMuteAll].asInt() == 0 ? false : true);
#endif
}

bool ParseTeamInfoJson(const std::string& team_info_json, TeamInfo& team_info)
{
	Json::Value json;
	Json::Reader reader;
	if (reader.parse(team_info_json, json) && json.isObject())
	{
		ParseTeamInfoJson(json, team_info);
		return true;
	}
	
	return false;
}

bool ParseTeamInfosJson(const std::string& team_infos_json, std::list<TeamInfo>& team_infos)
{
	Json::Value json;
	Json::Reader reader;
	if (reader.parse(team_infos_json, json) && json.isArray())
	{
		int len = json.size();
		for (int i = 0; i < len; i++)
		{
			TeamInfo info;
			ParseTeamInfoJson(json[i], info);
			team_infos.push_back(info);
		}
		return true;
	}

	return false;
}

void ParseTeamMemberPropertyJson(const Json::Value& team_member_prop_json, TeamMemberProperty& team_member_property)
{
	team_member_property.SetUserType((nim::NIMTeamUserType)team_member_prop_json[nim::kNIMTeamUserKeyType].asInt());
	if (team_member_property.GetUserType() != nim::kNIMTeamUserTypeApply && team_member_property.GetUserType() != nim::kNIMTeamUserTypeLocalWaitAccept)
	{
		team_member_property.SetAccountID(team_member_prop_json[nim::kNIMTeamUserKeyAccID].asString());
		team_member_property.SetNick(team_member_prop_json[nim::kNIMTeamUserKeyNick].asString());
		team_member_property.SetBits(team_member_prop_json[nim::kNIMTeamUserKeyBits].asUInt64());
		team_member_property.SetCreateTimetag(team_member_prop_json[nim::kNIMTeamUserKeyCreateTime].asUInt64());
		team_member_property.SetUpdateTimetag(team_member_prop_json[nim::kNIMTeamUserKeyUpdateTime].asUInt64());
		team_member_property.SetTeamID(team_member_prop_json[nim::kNIMTeamUserKeyID].asString());
		team_member_property.SetValid(team_member_prop_json[nim::kNIMTeamUserKeyValidFlag].asUInt() == 0 ? false : true);
		team_member_property.SetCustom(team_member_prop_json[nim::kNIMTeamUserKeyCustom].asString());
		team_member_property.SetMute(team_member_prop_json[nim::kNIMTeamUserKeyMute].asInt() == 1);
	}
}

bool ParseTeamMemberPropertyJson(const std::string& team_member_prop_json, TeamMemberProperty& team_member_property)
{
	Json::Value json;
	Json::Reader reader;
	if (reader.parse(team_member_prop_json, json) && json.isObject())
	{
		ParseTeamMemberPropertyJson(json, team_member_property);
		return true;
	}

	return false;
}

bool ParseTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<TeamMemberProperty>& team_member_propertys)
{
	Json::Value json;
	Json::Reader reader;
	if (reader.parse(team_member_props_json, json) && json.isArray())
	{
		int len = json.size();
		for (int i = 0; i < len; i++)
		{
			TeamMemberProperty prop;
			ParseTeamMemberPropertyJson(json[i], prop);
			team_member_propertys.push_back(prop);
		}
		return true;
	}
	return false;
}

}