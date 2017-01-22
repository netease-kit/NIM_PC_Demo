#include "force_push_manager.h"
#include "module/msglog/msg_extend_db.h"

namespace nim_comp
{
void ForcePushManager::Load()
{
	std::map<std::string, std::string> data;
	MsgExDB::GetInstance()->QueryAllForcePushData(data);

	Json::Reader reader;
	Json::Value value;
	ForcePushInfo info;
	std::vector<ForcePushInfo> info_list;
	for (auto &i : data)
	{
		if (reader.parse(i.second, value) && value.isArray())
		{
			for (int j = 0; j < (int)value.size(); ++j)
			{
				info.sender_id = value[j]["id"].asString();
				info.msg_body = value[j]["msg_body"].asString();
				info.uuid = value[j]["uuid"].asString();

				info_list.push_back(info);
			}
		}
		
		session_id_atme_[i.first] = info_list;
		info_list.clear();
	}
}

void ForcePushManager::Save()
{
	MsgExDB::GetInstance()->ClearForcePushData();

	Json::FastWriter writer;
	Json::Value root;
	Json::Value value;

	std::map<std::string, std::string> data;
	std::string info;
	for (auto &i : session_id_atme_)
	{
		for (auto &j : i.second)
		{
			value["id"] = j.sender_id;
			value["msg_body"] = j.msg_body;
			value["uuid"] = j.uuid;
			root.append(value);
		}

		data[i.first] = writer.write(root);
		root.clear();
	}

	MsgExDB::GetInstance()->InsertForcePushData(data);
}

bool ForcePushManager::IsAtMeMsg(const nim::IMMessage &msg)
{
	// 是否包含atme消息，如果当前msg包含atme消息，就不显示提示条，否则显示
	if (msg.session_type_ == nim::kNIMSessionTypeTeam && msg.type_ == nim::kNIMMessageTypeText && !LoginManager::GetInstance()->IsEqual(msg.sender_accid_))
	{
		if (msg.msg_setting_.is_force_push_ == nim::BS_TRUE)
		{
			//@所有人
			if (msg.msg_setting_.force_push_ids_list_.empty())
			{
				return true;
			}
			else
			{
				for (auto &id : msg.msg_setting_.force_push_ids_list_)
				{
					if (LoginManager::GetInstance()->IsEqual(id))
						return true;
				}
			}
		}
	}

	return false;
}

void ForcePushManager::AddAtMeMsg(const std::string &session_id, const nim::IMMessage &msg)
{
	ForcePushInfo info;
	info.msg_body = msg.content_;
	info.sender_id = msg.sender_accid_;
	info.uuid = msg.client_msg_id_;

	auto i = session_id_atme_.find(session_id);
	if (i != session_id_atme_.end())
	{
		i->second.push_back(info);
	}
	else
	{
		std::vector<ForcePushInfo> list;
		list.push_back(info);
		session_id_atme_[session_id] = list;	
	}
}

bool ForcePushManager::GetAtMeMsgs(const std::string &session_id, std::vector<ForcePushInfo> &infos)
{
	auto i = session_id_atme_.find(session_id);
	if (i != session_id_atme_.end())
	{
		infos = i->second;
		return true;
	}
	else
	{
		return false;
	}
}

void ForcePushManager::ResetAtMeMsg(const std::string &session_id)
{
	// 重置对应会话中的@me消息为已读
	auto i = session_id_atme_.find(session_id);
	if (i != session_id_atme_.end())
		session_id_atme_.erase(i);
}

bool ForcePushManager::IsContainAtMeMsg(const std::string &session_id)
{
	auto i = session_id_atme_.find(session_id);
	if (i != session_id_atme_.end())
		return true;
	
	return false;
}


}