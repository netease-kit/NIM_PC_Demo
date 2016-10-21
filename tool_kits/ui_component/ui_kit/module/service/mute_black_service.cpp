#include "mute_black_service.h"

namespace nim_comp
{
MuteBlackService::MuteBlackService()
{
	nim::User::RegSpecialRelationshipChangedCb(nbase::Bind(&MuteBlackService::OnMuteBlackEventCallback, this, std::placeholders::_1));
	nim::User::GetBlacklist(nbase::Bind(&MuteBlackService::OnGetBlackListCallback, this, std::placeholders::_1, std::placeholders::_2));
	nim::User::GetMutelist(nbase::Bind(&MuteBlackService::OnGetMuteListCallback, this, std::placeholders::_1, std::placeholders::_2));
}

const std::set<std::string> &MuteBlackService::GetMuteList()
{
	return mute_list_;
}

const std::set<std::string> &MuteBlackService::GetBlackList()
{
	return black_list_;
}

bool MuteBlackService::IsInMuteList(const std::string& accid)
{
	return (mute_list_.find(accid) != mute_list_.end());
}

bool MuteBlackService::IsInBlackList(const std::string& accid)
{
	return (black_list_.find(accid) != black_list_.end());
}

void MuteBlackService::OnGetMuteListCallback(nim::NIMResCode res_code, const std::list<nim::MuteListInfo>& lists)
{
	mute_list_.clear();
	if (res_code == nim::kNIMResSuccess)
	{
		for (auto& info : lists)
		{
			mute_list_.insert(info.accid_);
		}
	}
}

void MuteBlackService::OnGetBlackListCallback(nim::NIMResCode res_code, const std::list<nim::BlackListInfo>& lists)
{
	black_list_.clear();
	if (res_code == nim::kNIMResSuccess)
	{
		for (auto& info : lists)
		{
			black_list_.insert(info.accid_);
		}
	}
}

void MuteBlackService::OnMuteBlackEventCallback(const nim::SpecialRelationshipChangeEvent& change_event)
{
	switch (change_event.type_)
	{
	case nim::NIMUserSpecialRelationshipChangeType::kNIMUserSpecialRelationshipChangeTypeMarkBlack:
	{
		nim::BlackListInfo info;
		nim::User::ParseBlackListInfoChange(change_event, info);
		ModifyBlackList(info.accid_, info.set_black_);
		for (auto &cb : sync_set_black_cb_list_)
			(*cb.second)(info.accid_, info.set_black_);
		break;
	}
	case nim::NIMUserSpecialRelationshipChangeType::kNIMUserSpecialRelationshipChangeTypeMarkMute:
	{
		nim::MuteListInfo info;
		nim::User::ParseMuteListInfoChange(change_event, info);
		ModifyMuteList(info.accid_, info.set_mute_);
		for (auto &cb : sync_set_mute_cb_list_)
			(*cb.second)(info.accid_, info.set_mute_);
		break;
	}
	case nim::NIMUserSpecialRelationshipChangeType::kNIMUserSpecialRelationshipChangeTypeSyncMuteAndBlackList:
	{
		std::list<nim::BlackListInfo> black_list;
		std::list<nim::MuteListInfo> mute_list;
		nim::User::ParseSyncSpecialRelationshipChange(change_event, black_list, mute_list);
		for (auto& info : black_list)
		{
			ModifyBlackList(info.accid_, info.set_black_);
		}

		for (auto& info : mute_list)
		{
			ModifyMuteList(info.accid_, info.set_mute_);
		}
		break;
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
		mute_list_.insert(accid);
		break;
	case false:
		mute_list_.erase(accid);
		break;
	}
}

void MuteBlackService::ModifyBlackList(const std::string& accid, bool black)
{
	switch (black)
	{
	case true:
		black_list_.insert(accid);
		break;
	case false:
		black_list_.erase(accid);
		break;
	}
}

UnregisterCallback MuteBlackService::RegSyncSetMuteCallback(const SetStateCallback& cb)
{
	SetStateCallback* new_callback = new SetStateCallback(cb);
	int cb_id = (int)new_callback;
	sync_set_mute_cb_list_[cb_id].reset(new_callback);
	auto unregister = ToWeakCallback([this, cb_id]() {
		sync_set_mute_cb_list_.erase(cb_id);
	});
	return unregister;
}

UnregisterCallback MuteBlackService::RegSyncSetBlackCallback(const SetStateCallback& cb)
{
	SetStateCallback* new_callback = new SetStateCallback(cb);
	int cb_id = (int)new_callback;
	sync_set_black_cb_list_[cb_id].reset(new_callback);
	auto unregister = ToWeakCallback([this, cb_id]() {
		sync_set_black_cb_list_.erase(cb_id);
	});
	return unregister;
}
}