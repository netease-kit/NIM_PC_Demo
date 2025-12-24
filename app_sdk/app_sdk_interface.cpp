#include "stdafx.h"

#include "app_sdk_config_helper.h"
#include "app_sdk_interface.h"
#include "base/http/sdk_http_manager.h"
#include "protocol/sdk_pro.h"
#include "shared/xml_util.h"

namespace app_sdk {
const std::map<std::string, std::tuple<std::string, NimServerConfType>> key_use_nim_server_conf = {
    {nim::kNIMAppKey, std::make_tuple("appkey", NimServerConfType::NimServerConfType_String)},
    {nim::kNIMUseHttps, std::make_tuple("https_enabled", NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMDedicatedClusteFlag, std::make_tuple("dedicated_cluste_flag", NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMNegoKeyNECA, std::make_tuple("nego_key_neca", NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMCommNECA, std::make_tuple("comm_enca", NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMIPProtVersion, std::make_tuple("ip_protocol_version", NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMHandShakeType, std::make_tuple("hand_shake_type", NimServerConfType::NimServerConfType_Int)},
    {"NRTCStreamENCType", std::make_tuple("nrtc_stream_enc_type", NimServerConfType::NimServerConfType_Int)},
    {"NRTCStreamENCKey", std::make_tuple("nrtc_stream_enc_key", NimServerConfType::NimServerConfType_String)},
    {"kNIMChatRoomAddress", std::make_tuple("chatroomDemoListUrl", NimServerConfType::NimServerConfType_String)},
    {nim::kNIMFcsAuthType, std::make_tuple(nim::kNIMFcsAuthType, NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMFcsCustomEnable, std::make_tuple(nim::kNIMFcsCustomEnable, NimServerConfType::NimServerConfType_Int)},
    {nim::kNIMMockUA, std::make_tuple(nim::kNIMMockUA, NimServerConfType::NimServerConfType_String)},
    {nim::kNIMMockRefer, std::make_tuple(nim::kNIMMockRefer, NimServerConfType::NimServerConfType_String)},
};
const std::string AppSDKInterface::kAppKey = "3e215d27b6a6a9e27dad7ef36dd5b65c";
const std::string AppSDKInterface::kAppHost = "http://app.netease.im";

std::map<std::string, std::string> AppSDKInterface::config_map_;
bool AppSDKInterface::HasconfigValue(const std::string& key, bool from_private_setting) {
    bool ret = false;
    if (config_map_.empty()) {
        LoadConfig();
    }
    ret = (config_map_.find(key) != config_map_.end());
    if (!ret && from_private_setting) {
        Json::Value json_value;
        if (key_use_nim_server_conf.find(key) != key_use_nim_server_conf.end() &&
            nbase::FilePathIsExist(nbase::UTF8ToUTF16(app_sdk::AppSDKConfig::GetInstance()->GetAppConfigPath()), false) &&
            GetNimServerConfJsonObject(std::get<0>(key_use_nim_server_conf.find(key)->second), json_value)) {
            ret = true;
        }
    }
    return ret;
}
std::string AppSDKInterface::GetConfigValue(const std::string& key) {
    Json::Value json_value;
    if (key_use_nim_server_conf.find(key) != key_use_nim_server_conf.end() &&
        nbase::FilePathIsExist(nbase::UTF8ToUTF16(app_sdk::AppSDKConfig::GetInstance()->GetAppConfigPath()), false) &&
        app_sdk::GetNimServerConfJsonObject(std::get<0>(key_use_nim_server_conf.find(key)->second), json_value)) {
        auto it = key_use_nim_server_conf.find(key);
        return GetConfigStringValueFromNimServerConf(std::get<0>(it->second), std::get<1>(it->second));
    }
    return GetConfigValueFromMap(key);
}
std::string AppSDKInterface::GetConfigValueFromMap(const std::string& key) {
    if (config_map_.empty()) {
        LoadConfig();
    }
    if (config_map_.find(key) != config_map_.end())
        return config_map_[key];
    return "";
}
int AppSDKInterface::GetConfigFileVersion() {
    int version = 0;
    std::string value;
    std::wstring server_conf_path = QPath::GetAppPath();
    server_conf_path.append(L"global_conf.txt");
    TiXmlDocument document;
    if (shared::LoadXmlFromFile(document, server_conf_path)) {
        TiXmlElement* root = document.RootElement();
        if (root != nullptr) {
            if (auto pchar = root->Attribute("kConfVersion")) {
                value = pchar;
            }
        }
    }
    if (!value.empty())
        nbase::StringToInt(value, &version);
    return version;
}
std::string AppSDKInterface::GetAppKey() {
    std::string app_key = kAppKey;
    std::string new_app_key = GetConfigValue(nim::kNIMAppKey);
    if (!new_app_key.empty()) {
        app_key = new_app_key;
    }
    return app_key;
}
bool AppSDKInterface::IsNimDemoAppKey(const std::string& appkey) {
    static const std::string kNimTestAppKey = "fe416640c8e8a72734219e1847ad2547";
    static const std::string kNimSGAppKey = "b88e33e57c0f2a427e19db2b8461cb55";
    if (appkey.compare(kNimTestAppKey) == 0 || appkey.compare(kAppKey) == 0 || appkey.compare(kNimSGAppKey) == 0)
        return true;
    return false;
}

bool AppSDKInterface::IsSafeUrl(const std::string& safe_url) {
    std::string temp(safe_url);
    std::string params;
    std::vector<std::string> param_list;
    std::map<std::string, std::string> param_pair_list;
    auto param_pos = ((temp.find("?") == std::string::npos) ? (0) : (temp.find("?") + strlen("?")));
    params = temp.substr(param_pos, temp.length());
    nbase::LowerString(params);
    shared::tools::SplitString(params, "&", param_list);
    for (auto& it : param_list) {
        std::vector<std::string> param_pair;
        shared::tools::SplitString(it, "=", param_pair);
        if (param_pair.empty())
            continue;
        param_pair_list.insert(std::make_pair(*param_pair.begin(), *param_pair.rbegin()));
    }
    static const std::string safe_url_param_key = "_im_url";
    static const int safe_url_param_value = 0x00000001;
    if (param_pair_list.find(safe_url_param_key) != param_pair_list.end()) {
        return (std::atoi(param_pair_list[safe_url_param_key].c_str()) & safe_url_param_value) == safe_url_param_value;
    }
    return false;
}

std::string AppSDKInterface::GetReferrer() {
    // 开发者请根据自己实际情况来获取该域名地址
    return "http://yunxin_referer";
}

std::string AppSDKInterface::GetAppHost() {
    return kAppHost;
}
void AppSDKInterface::InvokeFormatAccountAndPassword(const std::string& username,
                                                     const std::string& password,
                                                     const OnFormatAccountAndPasswordCallback& cb) {
    //如果是云信的demo password会进行md5编码，如果是开发者自己的应用可以引入其它方式
    //甚至可以转到开发者自己的应用服务器，拿到真正的accid 与password,可以异步操作，但此时 user相关数据（user目录、image目录以及日志等）并未创建
    bool password_use_md5 = IsNimDemoAppKey(GetAppKey());
    if (!password_use_md5)
        password_use_md5 = (HasconfigValue("kNIMPasswordMD5") && (std::atoi(GetConfigValue("kNIMPasswordMD5").c_str()) != 0));
    cb(!(username.empty() || password.empty()), username, (password_use_md5 ? nim::Tool::GetMd5(password) : password));
}
void AppSDKInterface::InvokeRegisterAccount(const std::string& username,
                                            const std::string& password,
                                            const std::string& nickname,
                                            const OnRegisterAccountCallback& cb) {
    //在构造函数中传入请求参数
    auto&& req = app_sdk::CreateHttpRequest<app_sdk_pro::RegisterAccountReq>(username, password, nickname);

    SDKManager::GetInstance()->Invoke_Request<app_sdk_pro::RegisterAccountReq, app_sdk_pro::RegisterAccountRsp>(
        req, ToWeakCallback([cb](const app_sdk_pro::RegisterAccountReq& req, const app_sdk_pro::RegisterAccountRsp& rsp) {
            if (cb != nullptr) {
                cb((rsp->GetResponseCode() == nim::kNIMResSuccess ? rsp->GetProtocolReplyCode() : rsp->GetResponseCode()), rsp->err_msg_);
            }
        }));
}
void AppSDKInterface::InvokeGetChatroomList(const OnGetChatroomListCallback& cb) {
    auto&& req = app_sdk::CreateHttpRequest<app_sdk_pro::GetChatroomListReq>();
    SDKManager::GetInstance()->Invoke_Request<app_sdk_pro::GetChatroomListReq, app_sdk_pro::GetChatroomListRsp>(
        req, ToWeakCallback([cb](const app_sdk_pro::GetChatroomListReq& req, const app_sdk_pro::GetChatroomListRsp& rsp) {
            if (cb != nullptr) {
                cb((rsp->GetResponseCode() == nim::kNIMResSuccess ? rsp->GetProtocolReplyCode() : rsp->GetResponseCode()), rsp->chatroom_list_);
            }
        }));
}
void AppSDKInterface::InvokeGetChatroomAddress(__int64 room_id, const std::string& uid, int type, const OnGetChatroomAddressCallback& cb) {
    auto&& req = app_sdk::CreateHttpRequest<app_sdk_pro::GetChatroomAddressReq>();
    req->room_id_ = room_id;
    req->type_ = type;
    req->uid_ = uid;
    SDKManager::GetInstance()->Invoke_Request<app_sdk_pro::GetChatroomAddressReq, app_sdk_pro::GetChatroomAddressRsp>(
        req, ToWeakCallback([cb](const app_sdk_pro::GetChatroomAddressReq& req, const app_sdk_pro::GetChatroomAddressRsp& rsp) {
            if (cb != nullptr) {
                cb((rsp->GetResponseCode() == nim::kNIMResSuccess ? rsp->GetProtocolReplyCode() : rsp->GetResponseCode()), rsp->address_);
            }
        }));
}
void AppSDKInterface::LoadConfig() {
    config_map_.clear();
    std::wstring server_conf_path = QPath::GetAppPath();
    server_conf_path.append(L"global_conf.txt");
    TiXmlDocument document;
    if (shared::LoadXmlFromFile(document, server_conf_path)) {
        TiXmlElement* root = document.RootElement();
        auto attribute = root->FirstAttribute();
        while (attribute != nullptr) {
            config_map_[attribute->Name()] = attribute->Value();
            attribute = attribute->Next();
        }
    }
    if (config_map_.empty()) {
        config_map_["nim_global_conf_error"] = "nim_global_conf_error";
    }
}
}  // namespace app_sdk