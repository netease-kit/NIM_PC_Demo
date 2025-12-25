#pragma once
#include "app_sdk_define.h"
namespace app_sdk
{
class AppSDKInterface : public nbase::SupportWeakCallback
{
  public:
    AppSDKInterface() = default;
    ~AppSDKInterface() = default;
    SINGLETON_DEFINE(AppSDKInterface);

    /**
     * 判断是否存在指定的配置项
     * @param[in] key 需要判断的信息关键字
     * @param[in] from_private_setting 公共配置中没有配置时，是否查找私有化配置项
     * @return bool
     */
    static bool HasconfigValue(const std::string &key, bool from_private_setting = true);

    /**
     * 获取连接服务器的某一个配置信息
     * @param[in] key 需要获取的信息关键字
     * @return string 配置信息
     */
    static std::string GetConfigValue(const std::string &key);

    /**
     * 获取配置文件的版本信息
     * @return string 版本
     */
    static int GetConfigFileVersion();

    /**
     * 获取应用的app key
     * @return string 配置信息app key
     */
    static std::string GetAppKey();

    /**
     * 应用的app key是否为网易云信Demo本身的app key
     * @return bool true:是,false:否
     */
    static bool IsNimDemoAppKey(const std::string &appkey);

    /**
     * 判断 URL 是否是安全链接
     * @param[in] safe_url 要检测的 URL
     * @return 返回 true 表示是一个安全短链接，否则为 false
     */
    static bool IsSafeUrl(const std::string &safe_url);

    /**
     * 获取安全连接中需要传递的 Referrer 白名单 URL
     * @return 返回白名单允许的 Referrer URL 地址
     */
    static std::string GetReferrer();

  public:
    /**
     * 获取应用发送http请求的host,host与api拼接成一个完整的http请求地址
     * 开发者可以修改为具体应用的host
     * @return string http请求的host
     */
    std::string GetAppHost();

    /**
     * 注册一个新账号
     * @param[in] username 要用户名
     * @param[in] password 密码
     * @param[in] cb 格式化完毕的回调通知函数
     * @return void	无返回值
     */
    void InvokeFormatAccountAndPassword(const std::string &username, const std::string &password,
                                        const OnFormatAccountAndPasswordCallback &cb);

    /**
     * 注册一个新账号
     * @param[in] username 要注册的用户名
     * @param[in] password 用户密码
     * @param[in] nickname 用户昵称
     * @param[in] cb 注册完毕的回调通知函数
     * @return void	无返回值
     */
    void InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname,
                               const OnRegisterAccountCallback &cb);

    /**
     * 获取聊天室列表
     * @param[in] cb 回调通知函数
     * @return void	无返回值
     */
    void InvokeGetChatroomList(const OnGetChatroomListCallback &cb);

    /**
     * 获取聊天室连接地址,多用于匿名登录时
     * @param[in] room_id 聊天室ID
     * @param[in] uid 用户UID
     * @param[in] type 进入聊天室的模式 Logined = 1,Anonymous = 2,
     * @param[in] cb 回调通知函数
     * @return void	无返回值
     */
    void InvokeGetChatroomAddress(__int64 room_id, const std::string &uid, int type,
                                  const OnGetChatroomAddressCallback &cb);

    /**
     * 发送手机验证码
     * @param[in] mobile 手机号码
     * @param[in] cb 发送验证码完毕的回调通知函数
     * @return void	无返回值
     */
    void InvokeSendSmsCode(const std::string &mobile, const OnSendSmsCodeCallback &cb);

    /**
     * 通过验证码获取登录信息
     * @param[in] mobile 手机号码
     * @param[in] sms_code 短信验证码
     * @param[in] cb 登录完毕的回调通知函数
     * @return void	无返回值
     */
    void InvokeLoginByCode(const std::string &mobile, const std::string &sms_code, const OnLoginByCodeCallback &cb);

  private:
    static void LoadConfig();
    /**
     * 获取连接服务器的某一个配置信息
     * @param[in] key 需要获取的信息关键字
     * @return string 配置信息
     */
    static std::string GetConfigValueFromMap(const std::string &key);

  private:
    static const std::string kAppKey;
    static const std::string kAppHost;
    static std::map<std::string, std::string> config_map_;
};

} // namespace app_sdk