/** @file nim_user_helper.cpp
  * @brief SDK用户信息辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/20
  */

#include "nim_user_helper.h"

namespace nim
{

bool ParseNameCards(const std::string& namecards_json, std::list<UserNameCard>& namecards)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(namecards_json, values) && values.isArray())
	{
		return ParseNameCards(values, namecards);
	}
	return false;
}

bool ParseNameCards(const Json::Value& namecards_json, std::list<UserNameCard>& namecards)
{
	if (!namecards_json.isArray())
		return false;

	int len = namecards_json.size();
	for (int i = 0; i < len; i++)
	{
		UserNameCard info;
		ParseNameCard(namecards_json[i], info);
		namecards.push_back(info);
	}

	return true;
}

bool ParseNameCard(const std::string& namecard_json, UserNameCard& namecard)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(namecard_json, values) && values.isObject())
	{
		ParseNameCard(values, namecard);
		return true;
	}
	return false;
}

void ParseNameCard(const Json::Value& namecard_json, UserNameCard& namecard)
{
	namecard.SetAccId(namecard_json[kNIMNameCardKeyAccid].asString());
	if(namecard_json.isMember(kNIMNameCardKeyName))
		namecard.SetName(namecard_json[kNIMNameCardKeyName].asString());
	if (namecard_json.isMember(kNIMNameCardKeyIcon))
		namecard.SetIconUrl(namecard_json[kNIMNameCardKeyIcon].asString());
	if (namecard_json.isMember(kNIMNameCardKeySign))
		namecard.SetSignature(namecard_json[kNIMNameCardKeySign].asString());
	if (namecard_json.isMember(kNIMNameCardKeyGender))
		namecard.SetGender(namecard_json[kNIMNameCardKeyGender].asUInt());
	if (namecard_json.isMember(kNIMNameCardKeyEmail))
		namecard.SetEmail(namecard_json[kNIMNameCardKeyEmail].asString());
	if (namecard_json.isMember(kNIMNameCardKeyBirth))
		namecard.SetBirth(namecard_json[kNIMNameCardKeyBirth].asString());
	if (namecard_json.isMember(kNIMNameCardKeyMobile))
		namecard.SetMobile(namecard_json[kNIMNameCardKeyMobile].asString());
	if (namecard_json.isMember(kNIMNameCardKeyEx))
		namecard.SetExpand(namecard_json[kNIMNameCardKeyEx]);
	if (namecard_json.isMember(kNIMNameCardKeyCreateTime))
		namecard.SetCreateTimetag(namecard_json[kNIMNameCardKeyCreateTime].asUInt64());
	if (namecard_json.isMember(kNIMNameCardKeyUpdateTime))
		namecard.SetUpdateTimetag(namecard_json[kNIMNameCardKeyUpdateTime].asUInt64());
}

bool ParseSpecialListInfo(const std::string& list_json, std::list<BlackMuteListInfo>& mute_black_list)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(list_json, values) && values.isArray())
	{
		int len = values.size();
		for (int i = 0; i < len; i++)
		{
			Json::Value &value = values[i];
			BlackMuteListInfo info;
			info.accid_ = value[kNIMSpecialRelationKeyAccid].asString();
			info.set_black_ = value[kNIMSpecialRelationKeyIsBlackList].asBool();
			info.set_mute_ = value[kNIMSpecialRelationKeyIsMute].asBool();
			info.create_timetag_ = value[kNIMSpecialRelationKeyCreateTime].asUInt64();
			info.update_timetag_ = value[kNIMSpecialRelationKeyUpdateTime].asUInt64();
			mute_black_list.push_back(info);
		}

		return true;
	}
	return false;
}

}