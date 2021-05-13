#include "stdafx.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_data_def.h"
#include "gui/main/control/session_item_helper.h"

namespace nim_comp
{
	const int MultiMessageRetweetData::abstract_list_length_ = 2;
	const int MultiMessageRetweetData::abstract_message_max_length_ = 32;
	MultiMessageRetweetData::MessageAbstract::MessageAbstract(const std::string& sender, const std::string& message) : 
		sender_(sender), 
		message_(message) 
	{
	}
	MultiMessageRetweetData::MessageAbstract::MessageAbstract(std::string&& sender, std::string&& message) : 
		sender_(std::move(sender)),
		message_(std::move(message)) 
	{
	}
	MultiMessageRetweetData::MultiMessageRetweetData() : 
		compressed_(false), 
		encrypted_(true) 
	{
	}
	void MultiMessageRetweetData::ToJsonOjbect(Json::Value& json_data)
	{
		json_data["url"] = url_;
		json_data["md5"] = md5_;
		json_data["compressed"] = compressed_;
		json_data["encrypted"] = encrypted_;
		json_data["sessionID"] = session_id_;
		json_data["sessionName"] = session_name_;
		if (encrypted_)
			json_data["password"] = password_;
		for (auto& it : msg_abstract_list_)
		{
			Json::Value item;
			item["sender"] = it.sender_;
			item["message"] = it.message_;
			json_data["messageAbstract"].append(item);
		}
	}
	void MultiMessageRetweetData::FromJsonObject(const Json::Value& json_data)
	{
		url_ = json_data["url"].asString();
		md5_ = json_data["md5"].asString();
		compressed_ = json_data["compressed"].asBool();
		if (json_data.isMember("encrypted"))
			encrypted_ = json_data["encrypted"].asBool();
		else
			encrypted_ = true;
		if (encrypted_)
			password_ = json_data["password"].asString();
		session_id_ = json_data["sessionID"].asString();
		session_name_ = json_data["sessionName"].asString();
		for (auto &it : json_data["messageAbstract"])
		{
			msg_abstract_list_.emplace_back(MessageAbstract(it["sender"].asString(), it["message"].asString()));
		}
	}
	void MultiMessageRetweetData::FillAbstract(const std::list<nim::IMMessage>& message_list)
	{
		const static std::wstring ellipsis(L"...");
		int count = 0;
		auto it = message_list.begin();
		while (it != message_list.end() && count < abstract_list_length_)
		{
			nim::SessionData msg;
			msg.msg_attach_ = it->attach_;
			msg.msg_content_ = it->content_;
			msg.type_ = it->session_type_;
			msg.msg_type_ = it->type_;
			std::wstring abstract_message;
			SessionItemHelper::GetMsgContent(msg, abstract_message);
			if (abstract_message.length() > abstract_message_max_length_)
			{
				abstract_message.resize(abstract_message_max_length_ - ellipsis.length());
				abstract_message.append(ellipsis);
			}
			msg_abstract_list_.emplace_back(MessageAbstract( it->readonly_sender_nickname_ ,nbase::UTF16ToUTF8(abstract_message) ));
			it++;
			count++;
		}
		
	}
}