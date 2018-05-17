#include "stdafx.h"
#include "app_sdk_interface.h"
#include "shared/xml_util.h"
#include "protocol/sdk_pro.h"
#include "base/http/sdk_http_manager.h"
namespace app_sdk
{
	const std::string AppSDKInterface::kAppKey = "45c6af3c98409b18a84451215d0bdd6e";	
	const std::string AppSDKInterface::kAppHost = "http://app.netease.im";
	std::string AppSDKInterface::GetConfigValue(const std::string& key)
	{
		std::string value;
		std::wstring server_conf_path = QPath::GetAppPath();
		server_conf_path.append(L"global_conf.txt");
		TiXmlDocument document;
		if (shared::LoadXmlFromFile(document, server_conf_path))
		{
			TiXmlElement* root = document.RootElement();
			if (root)
			{
				Json::Value srv_config;
				if (auto pchar = root->Attribute(key.c_str()))
				{
					value = pchar;
				}
			}
		}
		return value;
	}	
	std::string AppSDKInterface::GetAppKey()
	{
		const static std::string config_key_AppKey = "kAppKey";
		std::string app_key = kAppKey;
		std::string new_app_key = GetConfigValue(config_key_AppKey);
		if (!new_app_key.empty())
		{
			app_key = new_app_key;
		}
		return app_key;
	}
	bool AppSDKInterface::IsNimDemoAppKey(const std::string& appkey)
	{
		static const std::string kNimTestAppKey = "fe416640c8e8a72734219e1847ad2547";
		if (appkey.compare(kNimTestAppKey) == 0 || appkey.compare(kAppKey) == 0)
			return true;
		return false;
	}
	std::string AppSDKInterface::GetAppHost()
	{
		return kAppHost;
	}
	void AppSDKInterface::InvokeFormatAccountAndPassword(const std::string &username, const std::string &password, const OnFormatAccountAndPasswordCallback& cb)
	{
		//如果是云信的demo password会进行md5编码，如果是开发者自己的应用可以引入其它方式
		//甚至可以转到开发者自己的应用服务器，拿到真正的accid 与password,可以异步操作，但此时 user相关数据（user目录、image目录以及日志等）并未创建
		cb(!(username.empty() || password.empty()),username, (IsNimDemoAppKey(GetAppKey()) ? QString::GetMd5(password) : password));		
	}
	void AppSDKInterface::InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb)
	{
		//在构造函数中传入请求参数
		auto&& req = app_sdk::CreateHttpRequest<app_sdk_pro::RegisterAccountReq>(username,password,nickname);

		SDKManager::GetInstance()->Invoke_Request<app_sdk_pro::RegisterAccountReq, app_sdk_pro::RegisterAccountRsp>(req, 
			ToWeakCallback([cb](const app_sdk_pro::RegisterAccountReq& req, const app_sdk_pro::RegisterAccountRsp& rsp){
			if (cb != nullptr)
			{
				cb((rsp->GetResponseCode() == nim::kNIMResSuccess ? rsp->GetProtocolReplyCode() : rsp->GetResponseCode()), rsp->err_msg_);
			}
		}));
	}
	void AppSDKInterface::InvokeGetChatroomList(const OnGetChatroomListCallback& cb)
	{
		auto&& req = app_sdk::CreateHttpRequest<app_sdk_pro::GetChatroomListReq>();
		SDKManager::GetInstance()->Invoke_Request<app_sdk_pro::GetChatroomListReq, app_sdk_pro::GetChatroomListRsp>(req,
			ToWeakCallback([cb](const app_sdk_pro::GetChatroomListReq& req, const app_sdk_pro::GetChatroomListRsp& rsp){
			if (cb != nullptr)
			{				
				cb((rsp->GetResponseCode() == nim::kNIMResSuccess ? rsp->GetProtocolReplyCode() : rsp->GetResponseCode()), rsp->chatroom_list_);
			}				
		}));
	}
	void AppSDKInterface::InvokeGetChatroomAddress(__int64 room_id, const std::string& uid, int type, const OnGetChatroomAddressCallback& cb)
	{
		auto&& req = app_sdk::CreateHttpRequest<app_sdk_pro::GetChatroomAddressReq>();
		req->room_id_ = room_id;
		req->type_ = type;
		req->uid_ = uid;
		SDKManager::GetInstance()->Invoke_Request<app_sdk_pro::GetChatroomAddressReq, app_sdk_pro::GetChatroomAddressRsp>(req,
			ToWeakCallback([cb](const app_sdk_pro::GetChatroomAddressReq& req, const app_sdk_pro::GetChatroomAddressRsp& rsp){
			if (cb != nullptr)
			{
				cb((rsp->GetResponseCode() == nim::kNIMResSuccess ? rsp->GetProtocolReplyCode() : rsp->GetResponseCode()), rsp->address_);
			}
		}));
	}
}