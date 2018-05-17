#include "stdafx.h"
#include "sdk_pro.h"
#include "app_sdk_interface.h"
namespace app_sdk
{
	static const std::string cmd_register_account = "/api/createDemoUser";//注册账号
	static const std::string cmd_get_chatroomlist = "/api/chatroom/homeList";//获取聊天室列表
	static const std::string cmd_get_chatroomaddress = "/api/chatroom/requestAddress";//获取聊天室连接地址
	void SDK_PRO::ResponseBase::Parse(const std::string& response) {
		reply_content_ = response;
		pro_reply_code_ = nim::kNIMResError;
		if (GetResponseCode() == nim::kNIMResSuccess)
		{
			pro_reply_code_ = nim::kNIMResSuccess;
			OnParse(response);
		}
			
	}
	std::string SDK_PRO::ResponseBase::GetReplyContent() const 
	{ 
		return reply_content_; 
	}
	int32_t SDK_PRO::ResponseBase::GetProtocolReplyCode() const
	{
		return pro_reply_code_;
	}
	void SDK_PRO::ResponseBase::SetProtocolReplyCode(int code)
	{
		pro_reply_code_ = code;
	}
	void SDK_PRO::ResponseBase::OnParse(const std::string& response) 
	{

	};
	std::string SDK_PRO::RequestBase::GetAPIURL() const
	{
		if (app_url_.empty())
			app_url_ = OnGetAPIURL();
		return app_url_;
	}
	std::string SDK_PRO::RequestBase::OnGetAPIURL() const
	{
		return OnGetHost().append(OnGetAPI());
	}
	std::string  SDK_PRO::RequestBase::OnGetHost() const
	{
		return AppSDKInterface::GetInstance()->GetAppHost();
	}
	bool SDK_PRO::RequestBase::UsePostMethod() const
	{ 
		return IsUsePostMethod();
	}
	bool SDK_PRO::RequestBase::IsUsePostMethod() const
	{
		return true;
	}
	std::string SDK_PRO::RequestBase::OnGetAPI() const
	{
		return ""; 
	};
	void SDK_PRO::RequestBase::GetRequestContent(std::string& content) const 
	{ 
		OnGetRequestContent(content); 
	}
	void SDK_PRO::RequestBase::GetRequestHead(std::map<std::string, std::string>& heads)
	{
		heads["User-Agent"] = "nim_demo_pc";
		heads["appkey"] = AppSDKInterface::GetAppKey();
		heads["charset"] = "utf-8";
		heads["Content-Type"] = "application/json";
		OnGetRequestHead(heads);
	}
	void SDK_PRO::RequestBase::OnGetRequestContent(std::string& content) const
	{
	};	
	void SDK_PRO::RequestBase::OnGetRequestHead(std::map<std::string, std::string>& heads) const
	{
	};
	//注册账号请求/应答
	SDK_PRO::RegisterAccountRequest::RegisterAccountRequest(std::string username, std::string password, std::string nickname) :
		username_(username), password_(password), nickname_(nickname)
	{

	}
	std::string SDK_PRO::RegisterAccountRequest::OnGetHost() const
	{
		std::string host = __super::OnGetHost();
		std::string new_host = AppSDKInterface::GetInstance()->GetConfigValue("kAppServerAddress");
		if (!new_host.empty())
		{
			host = new_host;
		}
		return host;
	}
	std::string SDK_PRO::RegisterAccountRequest::OnGetAPI() const
	{
		return cmd_register_account;
	};
	void SDK_PRO::RegisterAccountRequest::OnGetRequestHead(std::map<std::string, std::string>& heads) const
	{
		heads["Content-Type"] = "application/x-www-form-urlencoded";
	}
	void SDK_PRO::RegisterAccountRequest::OnGetRequestContent(std::string& content) const
	{
		content.append("username=").append(username_)
			.append("&password=").append(password_)
			.append("&nickname=").append(nickname_);
	}
	void SDK_PRO::RegisterAccountResponse::OnParse(const std::string& response)
	{
		Json::Value json_reply;
		Json::Reader reader;
		if (reader.parse(response, json_reply) && json_reply.isObject())
		{
			int res = json_reply["res"].asInt();
			if (res != nim::kNIMResSuccess)
			{
				if(json_reply.isMember("errmsg"))
				{
					err_msg_ = json_reply["errmsg"].asString();
				}
				SetProtocolReplyCode(res);
				
				QLOG_ERR(L"Invoke  register accounterror. Json rescode: {0}.") << res;
				return;
			}
		}
	}
	//获取聊天室列表请求/应答
	std::string SDK_PRO::GetChatroomListRequest::OnGetAPI() const
	{
		return   cmd_get_chatroomlist;
	};	
	std::string SDK_PRO::GetChatroomListRequest::OnGetAPIURL() const
	{
		std::string api_addr = __super::OnGetAPIURL();
		std::string new_api_addr = AppSDKInterface::GetInstance()->GetConfigValue("kNIMChatRoomAddress");
		if (!new_api_addr.empty())
			api_addr = new_api_addr;
		return api_addr;
	}
	bool SDK_PRO::GetChatroomListRequest::IsUsePostMethod() const
	{
		return false;
	}
	void SDK_PRO::GetChatroomListResponse::OnParse(const std::string& response)
	{
		Json::Value json_reply;
		Json::Reader reader;
		if (reader.parse(response, json_reply) && json_reply.isObject())
		{
			int res = json_reply["res"].asInt();
			if (res != nim::kNIMResSuccess)
			{
				this->SetProtocolReplyCode(res);
				QLOG_ERR(L"Invoke get room list error. Json rescode: {0}.") << res;
				return;
			}
			if (!json_reply["msg"]["list"].isArray())
			{
				QLOG_ERR(L"Invoke get room list error. Reason: Not an array.");
				return;
			}
			for (auto it : json_reply["msg"]["list"])
			{
				nim_chatroom::ChatRoomInfo info;
				info.id_ = it["roomid"].asInt64();
				info.name_ = it["name"].asString();
				info.creator_id_ = it["creator"].asString();
				nbase::StringToInt(it["onlineusercount"].asString(), &info.online_count_);
				info.valid_flag_ = it["status"].asInt();
				info.ext_ = it["ext"].asString();
				info.announcement_ = it["announcement"].asString();
				info.broadcast_url_ = it["broadcasturl"].asString();
				chatroom_list_.emplace_back(info);
			}			
		}
	};
	//获取聊天室连接地址请求/应答
	SDK_PRO::GetChatroomAddressRequest::GetChatroomAddressRequest() :
		type_(2), uid_(""), room_id_(0)
	{

	}
	std::string SDK_PRO::GetChatroomAddressRequest::OnGetAPIURL() const
	{
		std::string api_addr = __super::OnGetAPIURL();
		std::string new_api_addr = AppSDKInterface::GetInstance()->GetConfigValue("kNIMChatRoomRequest");
		if (!new_api_addr.empty())
			api_addr = new_api_addr;
		return api_addr;
	}
	std::string SDK_PRO::GetChatroomAddressRequest::OnGetAPI() const
	{
		return cmd_get_chatroomaddress;
	}
	void SDK_PRO::GetChatroomAddressRequest::OnGetRequestContent(std::string& content) const
	{
		Json::Value data;
		Json::FastWriter fs;
		data["roomid"] = room_id_;
		data["uid"] = uid_;
		data["type"] = type_;
		content = fs.write(data);
	}
	void SDK_PRO::GetChatroomAddressResponse::OnParse(const std::string& response)
	{
		Json::Value json_reply;
		Json::Reader reader;
		if (reader.parse(response, json_reply) && json_reply.isObject())
		{
			int res = json_reply["res"].asInt();
			if (res != nim::kNIMResSuccess)
			{
				this->SetProtocolReplyCode(res);
				QLOG_ERR(L"Invoke get room address error. Json rescode: {0}.") << res;
				return;
			}
			if (!json_reply["msg"]["addr"].isArray())
			{
				QLOG_ERR(L"Invoke get room address error. Reason: Not an array.");
				return;
			}
			for (auto it : json_reply["msg"]["addr"])
			{
				address_.emplace_back(it.asString());
			}
		}
	}
}