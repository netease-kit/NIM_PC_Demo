#pragma once
#include <functional>

namespace app_sdk
{

// 格式化账号密码回调
using OnFormatAccountAndPasswordCallback =
    std::function<void(bool ret, /*是否进行登录*/ const std::string &account, const std::string &password)>;
// 注册新账号回调
using OnRegisterAccountCallback = std::function<void(int code, const std::string &err_msg)>;
// 获取聊天室列表回调
using OnGetChatroomListCallback =
    std::function<void(int code, const std::vector<nim_chatroom::ChatRoomInfo> &chatroom_list)>;
// 获取聊天室地址回调
using OnGetChatroomAddressCallback = std::function<void(int code, const std::list<std::string> &address_list)>;

// 发送手机验证码回调
using OnSendSmsCodeCallback =
    std::function<void(int code, bool is_first_register, const std::string &request_id, const std::string &err_msg)>;

// 手机验证码登录回调 - 内部使用的完整结构体
struct LoginByCodeResult
{
    std::string user;
    std::string access_token;
    std::string im_accid;
    std::string im_token;
    std::string dynamic_token;
    std::string avatar;
    std::string av_room_uid;
    std::string nickname;
    std::string account_id;
    std::string user_uuid;
    std::string user_name;
    std::string icon;
    std::string auth_type;
};

// 对外回调只返回登录所需的账号密码
using OnLoginByCodeCallback = std::function<void(int code, const std::string &im_accid, const std::string &im_token,
                                                 const std::string &request_id, const std::string &err_msg)>;

} // namespace app_sdk
