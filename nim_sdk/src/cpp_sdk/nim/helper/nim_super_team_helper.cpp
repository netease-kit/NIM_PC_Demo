/** @file nim_super_team_helper.cpp
  * @brief SuperSuperTeam 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/14
  */

#include "src/cpp_sdk/nim/helper/nim_super_team_helper.h"

namespace nim
{

void ParseSuperTeamEvent(int rescode, const std::string& team_id, const NIMNotificationId notification_id, const std::string& team_event_json, SuperTeamEvent& team_event)
{
	team_event.res_code_ = (NIMResCode)rescode;
	team_event.team_id_ = team_id;
	team_event.notification_id_ = notification_id;
	nim_cpp_wrapper_util::Json::Reader reader;
	nim_cpp_wrapper_util::Json::Value values;
	if (reader.parse(team_event_json, values) && values.isObject())
	{
		if (notification_id == kNIMNotificationIdLocalGetTeamMsgUnreadCount
			|| notification_id == kNIMNotificationIdLocalGetTeamMsgUnreadList)
		{
			team_event.src_data_ = values[kNIMNotificationKeyData];
			return;
		}
		else if (notification_id == kNIMNotificationIdSuperTeamMuteMember)
		{
			team_event.opt_ = (values[kNIMNotificationKeyData]["mute"].asInt() == 1);
		}
		//操作者和被操作者用户名片
		nim_cpp_wrapper_util::Json::Value name_cards = values[kNIMNotificationKeyData][kNIMNotificationKeyUserNameCards];
		if (!name_cards.empty() && name_cards.isArray())
			ParseNameCards(name_cards, team_event.namecards_);

		switch (notification_id)
		{
		case kNIMNotificationIdSuperTeamKick:
		case kNIMNotificationIdSuperTeamInvite:
		case kNIMNotificationIdSuperTeamAddManager:
		case kNIMNotificationIdSuperTeamRemoveManager:
		case kNIMNotificationIdSuperTeamMuteMember:
		{
			if (values[kNIMNotificationKeyData].isMember("attach"))
				team_event.attach_ = values[kNIMNotificationKeyData]["attach"].asString();
		}
		{
			//ids
			nim_cpp_wrapper_util::Json::Value ids = values[kNIMNotificationKeyData][kNIMNotificationKeyDataIds];
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
		case kNIMNotificationIdSuperTeamLeave:
		case kNIMNotificationIdLocalRejectApply:
		case kNIMNotificationIdLocalRejectInvite:
		case kNIMNotificationIdSuperTeamOwnerTransfer:
			{
				//id
				team_event.ids_.push_back(values[kNIMNotificationKeyData][kNIMNotificationKeyDataId].asString());
			}
			break;
		case kNIMNotificationIdSuperTeamUpdate:
		case kNIMNotificationIdTeamSyncCreate:
		case kNIMNotificationIdLocalGetTeamInfo:
			{
				//info
				ParseSuperTeamInfoJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamInfo], team_event.team_info_);
			}
			break;
		case kNIMNotificationIdSuperTeamDismiss:
			{
				//无
				//无内容需要解析
			}
			break;
		case kNIMNotificationIdTeamMemberChanged:
			{
				//team member property
				ParseSuperTeamMemberPropertyJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember], team_event.member_property_);
				team_event.ids_.push_back(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember][kNIMSuperTeamUserKeyAccID].asString());
			}
			break;
		case kNIMNotificationIdTeamSyncUpdateMemberProperty:
		case kNIMNotificationIdLocalUpdateMemberProperty:
			{
				ParseSuperTeamMemberPropertyJson(values[kNIMNotificationKeyData][kNIMNotificationKeyTeamMember], team_event.member_property_);
			}
			break;
		default:
			break;
		}
	}
}


void ParseSuperTeamInfoJson(const nim_cpp_wrapper_util::Json::Value& team_info_json, SuperTeamInfo& team_info)
{
	//team_info.SetType((nim::NIMSuperTeamType)(team_info_json[nim::kNIMSuperTeamInfoKeyType].asInt()));
	team_info.SetSuperTeamID(team_info_json[nim::kNIMSuperTeamInfoKeyID].asString());
	team_info.SetOwnerID(team_info_json[nim::kNIMSuperTeamInfoKeyCreator].asString());
	team_info.SetName(team_info_json[nim::kNIMSuperTeamInfoKeyName].asString());
	team_info.SetIntro(team_info_json[nim::kNIMSuperTeamInfoKeyIntro].asString());
	team_info.SetAnnouncement(team_info_json[nim::kNIMSuperTeamInfoKeyAnnouncement].asString());
	team_info.SetJoinMode((NIMSuperTeamJoinMode)(team_info_json[nim::kNIMSuperTeamInfoKeyJoinMode].asInt()));
	team_info.SetServerCustom(team_info_json[nim::kNIMSuperTeamInfoKeyServerCustom].asString());
	team_info.SetCreateTimetag(team_info_json[nim::kNIMSuperTeamInfoKeyCreateTime].asUInt64());
	team_info.SetUpdateTimetag(team_info_json[nim::kNIMSuperTeamInfoKeyUpdateTime].asUInt64());
	team_info.SetCustom(team_info_json[nim::kNIMSuperTeamInfoKeyCustom].asString());
	team_info.SetMemberCount(team_info_json[nim::kNIMSuperTeamInfoKeyMemberCount].asUInt());
	team_info.SetProperty(team_info_json[nim::kNIMSuperTeamInfoKeyProperty].asString());
	team_info.SetValid(team_info_json[nim::kNIMSuperTeamInfoKeyValidFlag].asUInt() == 0 ? false : true);
	//群属性,开发者无需关注 20161011 oleg
	//team_info.SetConfigBits(team_info_json[nim::kNIMSuperTeamInfoKeyBits].asUInt64());
	team_info.SetMemberValid(team_info_json[nim::kNIMSuperTeamInfoKeyMemberValid].asUInt() == 0 ? false : true);
	team_info.SetIcon(team_info_json[nim::kNIMSuperTeamInfoKeyIcon].asString());
}

bool ParseSuperTeamInfoJson(const std::string& team_info_json, SuperTeamInfo& team_info)
{
	nim_cpp_wrapper_util::Json::Value json;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(team_info_json, json) && json.isObject())
	{
		ParseSuperTeamInfoJson(json, team_info);
		return true;
	}
	
	return false;
}

bool ParseSuperTeamInfosJson(const std::string& team_infos_json, std::list<SuperTeamInfo>& team_infos)
{
	nim_cpp_wrapper_util::Json::Value json;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(team_infos_json, json) && json.isArray())
	{
		int len = json.size();
		for (int i = 0; i < len; i++)
		{
			SuperTeamInfo info;
			ParseSuperTeamInfoJson(json[i], info);
			team_infos.push_back(info);
		}
		return true;
	}

	return false;
}

void ParseSuperTeamMemberPropertyJson(const nim_cpp_wrapper_util::Json::Value& team_member_prop_json, SuperTeamMemberProperty& team_member_property)
{
	team_member_property.SetUserType((nim::NIMSuperTeamUserType)team_member_prop_json[nim::kNIMSuperTeamUserKeyType].asInt());
	if (team_member_property.GetUserType() != nim::kNIMSuperTeamUserTypeApply && team_member_property.GetUserType() != nim::kNIMSuperTeamUserTypeLocalWaitAccept)
	{
		team_member_property.SetAccountID(team_member_prop_json[nim::kNIMSuperTeamUserKeyAccID].asString());
		team_member_property.SetNick(team_member_prop_json[nim::kNIMSuperTeamUserKeyNick].asString());
		team_member_property.SetBits(team_member_prop_json[nim::kNIMSuperTeamUserKeyBits].asUInt64());
		team_member_property.SetCreateTimetag(team_member_prop_json[nim::kNIMSuperTeamUserKeyCreateTime].asUInt64());
		team_member_property.SetUpdateTimetag(team_member_prop_json[nim::kNIMSuperTeamUserKeyUpdateTime].asUInt64());
		team_member_property.SetSuperTeamID(team_member_prop_json[nim::kNIMSuperTeamUserKeyID].asString());
		team_member_property.SetValid(team_member_prop_json[nim::kNIMSuperTeamUserKeyValidFlag].asUInt() == 0 ? false : true);
		team_member_property.SetCustom(team_member_prop_json[nim::kNIMSuperTeamUserKeyCustom].asString());
		team_member_property.SetMute(team_member_prop_json[nim::kNIMSuperTeamUserKeyMute].asInt() == 1);
	}
}

bool ParseSuperTeamMemberPropertyJson(const std::string& team_member_prop_json, SuperTeamMemberProperty& team_member_property)
{
	nim_cpp_wrapper_util::Json::Value json;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(team_member_prop_json, json) && json.isObject())
	{
		ParseSuperTeamMemberPropertyJson(json, team_member_property);
		return true;
	}

	return false;
}

bool ParseSuperTeamMemberPropertysJson(const std::string& team_member_props_json, std::list<SuperTeamMemberProperty>& team_member_propertys)
{
	nim_cpp_wrapper_util::Json::Value json;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(team_member_props_json, json) && json.isArray())
	{
		int len = json.size();
		for (int i = 0; i < len; i++)
		{
			SuperTeamMemberProperty prop;
			ParseSuperTeamMemberPropertyJson(json[i], prop);
			team_member_propertys.push_back(prop);
		}
		return true;
	}
	return false;
}

}