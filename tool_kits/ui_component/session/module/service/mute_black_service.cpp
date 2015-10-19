#include "mute_black_service.h"

namespace nim_comp
{
MuteBlackService::MuteBlackService()
{
	nim::User::RegSpecialRelationshipChangedCb(nbase::Bind(&MuteBlackService::OnMuteBlackEventCallback, this, std::placeholders::_1, std::placeholders::_2));
	nim::User::GetMuteAndBlacklist(nbase::Bind(&MuteBlackService::OnGetMuteAndBlackListCallback, this, std::placeholders::_1, std::placeholders::_2));
}

const std::set<std::string> &MuteBlackService::GetMuteList()
{
	return m_mute_list;
}

const std::set<std::string> &MuteBlackService::GetBlackList()
{
	return m_black_list;
}

bool MuteBlackService::IsInMuteList(const std::string& accid)
{
	return (m_mute_list.find(accid) != m_mute_list.end());
}

bool MuteBlackService::IsInBlackList(const std::string& accid)
{
	return (m_black_list.find(accid) != m_black_list.end());
}

void MuteBlackService::OnGetMuteAndBlackListCallback(int res_code, const std::string& mute_black_list_json)
{
	m_mute_list.clear();
	m_black_list.clear();
	if (res_code == 200)
	{
		Json::Value j_mute_black_list;
		Json::Reader reader;
		if (reader.parse(mute_black_list_json, j_mute_black_list))
		{
			int size = j_mute_black_list.size();
			for (int i = 0; i < size; i++)
			{
				if (j_mute_black_list[i][nim::kNIMSpecialRelationKeyIsMute].asBool())
					m_mute_list.insert(j_mute_black_list[i][nim::kNIMSpecialRelationKeyAccid].asString());
				if (j_mute_black_list[i][nim::kNIMSpecialRelationKeyIsBlackList].asBool())
					m_black_list.insert(j_mute_black_list[i][nim::kNIMSpecialRelationKeyAccid].asString());
			}
		}
		else
			QLOG_ERR(L"Get mute list error: Parse json string error: {0}") << nbase::UTF8ToUTF16(mute_black_list_json);
	}
	else
		QLOG_ERR(L"Get mute list error: Res Error: {0}") << res_code;
}

void MuteBlackService::OnMuteBlackEventCallback(nim::NIMUserSpecialRelationshipChangeType type, const std::string& result_json)
{
	Json::Value ret;
	Json::Reader reader;
	if (reader.parse(result_json, ret))
	{
		switch (type)
		{
		case nim::NIMUserSpecialRelationshipChangeType::kNIMUserSpecialRelationshipChangeTypeMarkBlack:
		{
			std::string accid = ret[nim::kNIMSpecialRelationKeyAccid].asString();
			bool black = ret["black"].asBool();
			ModifyBlackList(accid, black);
			for (auto &cb : sync_set_black_cb_list)
				(*cb.second)(accid, black);
			break;
		}
		case nim::NIMUserSpecialRelationshipChangeType::kNIMUserSpecialRelationshipChangeTypeMarkMute:
		{
			std::string accid = ret[nim::kNIMSpecialRelationKeyAccid].asString();
			bool mute = ret["mute"].asBool();
			ModifyMuteList(accid, mute);
			for (auto &cb : sync_set_mute_cb_list)
				(*cb.second)(accid, mute);
			break;
		}
		case nim::NIMUserSpecialRelationshipChangeType::kNIMUserSpecialRelationshipChangeTypeSyncMuteAndBlackList:
		{
			int size = ret.size();
			for (int i = 0; i < size; i++)
			{
				std::string accid = ret[i][nim::kNIMSpecialRelationKeyAccid].asString();
				bool mute = ret[i][nim::kNIMSpecialRelationKeyIsMute].asBool();
				bool black = ret[i][nim::kNIMSpecialRelationKeyIsBlackList].asBool();
				ModifyMuteList(accid, mute);
				ModifyBlackList(accid, black);
				for (auto &cb : sync_set_mute_cb_list)
					(*cb.second)(accid, mute);
				for (auto &cb : sync_set_black_cb_list)
					(*cb.second)(accid, black);
			}
			break;
		}
		}
	}
}

void MuteBlackService::InvokeSetMute(const std::string& accid, bool mute)
{
	nim::User::SetMuteCallback cb = [this](int res_code, const std::string& accid, bool opt) {
		if (res_code != 200)
			QLOG_ERR(L"Set mute failed. Error code: {0}") << res_code;
	};
	nim::User::SetMute(accid, mute, cb);
}

void MuteBlackService::InvokeSetBlack(const std::string& accid, bool black)
{
	nim::User::SetBlackCallback cb = [this](int res_code, const std::string& accid, bool opt) {
		if (res_code != 200)
			QLOG_ERR(L"Set black failed. Error code: {0}") << res_code;
	};
	nim::User::SetBlack(accid, black, cb);
}

void MuteBlackService::ModifyMuteList(const std::string& accid, bool mute)
{
	switch (mute)
	{
	case true:
		m_mute_list.insert(accid);
		break;
	case false:
		m_mute_list.erase(accid);
		break;
	}
}

void MuteBlackService::ModifyBlackList(const std::string& accid, bool black)
{
	switch (black)
	{
	case true:
		m_black_list.insert(accid);
		break;
	case false:
		m_black_list.erase(accid);
		break;
	}
}

UnregisterCallback MuteBlackService::RegSyncSetMuteCallback(const SetStateCallback& cb)
{
	SetStateCallback* new_callback = new SetStateCallback(cb);
	int cb_id = (int)new_callback;
	sync_set_mute_cb_list[cb_id].reset(new_callback);
	auto unregister = ToWeakCallback([this, cb_id]() {
		sync_set_mute_cb_list.erase(cb_id);
	});
	return unregister;
}

UnregisterCallback MuteBlackService::RegSyncSetBlackCallback(const SetStateCallback& cb)
{
	SetStateCallback* new_callback = new SetStateCallback(cb);
	int cb_id = (int)new_callback;
	sync_set_black_cb_list[cb_id].reset(new_callback);
	auto unregister = ToWeakCallback([this, cb_id]() {
		sync_set_black_cb_list.erase(cb_id);
	});
	return unregister;
}
}