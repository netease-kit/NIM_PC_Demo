#pragma once

#include "base/memory/singleton.h"
#include "login_db.h"

enum LoginStatus
{
	LoginStatus_NONE,		//初始状态
	LoginStatus_LOGIN,		//正在登录
	LoginStatus_CANCEL,		//取消登录
	LoginStatus_SUCCESS,	//已经登录
	LoginStatus_EXIT		//退出登录
};

namespace nim_comp
{
/** @class LoginManager
  * @brief 登陆管理器
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class LoginManager
{
public:
	SINGLETON_DEFINE(LoginManager);	
	LoginManager();

	/**
	* 判断帐号是否为已登录帐号
	* @param[in] account 用户id
	* @return bool true 是，false 否
	*/
	bool IsSelf(const std::string &account);

	/**
	* 设置登陆账号
	* @param[in] account
	* @return void	无返回值
	*/
	void SetAccount(const std::string &account);

	/**
	* 获取登陆账号
	* @return string 用户id
	*/
	std::string GetAccount();

	/**
	* 判断帐号是否为已登录帐号(自动转换为小写来判断)
	* @param[in] account 用户id
	* @return bool true 是，false 否
	*/
	bool IsEqual(const std::string& account);

	/**
	* 设置登陆密码
	* @param[in] password 登录密码
	* @return void	无返回值
	*/
	void SetPassword(const std::string &password);

	/**
	* 获取登陆密码
	* @return string 用户密码
	*/
	std::string GetPassword();

	/**
	* 设置登陆状态
	* @param[in] status 登录状态
	* @return void	无返回值
	*/
	void SetLoginStatus(LoginStatus status);

	/**
	* 获取登陆状态
	* @return LoginStatus 登录状态
	*/
	LoginStatus GetLoginStatus();

	/**
	* 设置断网重连标记
	* @param[in] active 是否处于连接状态
	* @return void	无返回值
	*/
	void SetLinkActive(bool active);

	/**
	* 判断是否处于连接状态
	* @return bool true 连接，false 断开
	*/
	bool IsLinkActive();

	/**
	* 保存登录数据
	* @return void	无返回值
	*/
	void SaveLoginData();

	/**
	* 获取登陆数据
	* @param[in] 
	* @return LoginData* 登陆数据
	*/
	LoginData* GetLoginData(){ return current_login_data_.get(); }

	/**
	* 设置日记记录等级
	* @param[in] level 记录等级
	* @return void	无返回值
	*/
	void SetDemoLogLevel(int level) { demo_log_level_ = (LOG_LEVEL)level; }

	/**
	* 获取日志记录等级
	* @return LOG_LEVEL 记录等级
	*/
	LOG_LEVEL GetDemoLogLevel() { return demo_log_level_; }

	/**
	* 设置收发文件限制的大小(单位:M)
	* @param[in] file_size 文件大小
	* @return void	无返回值
	*/
	void SetFileSizeLimit(int file_size) { limit_file_size_ = file_size; }

	/**
	* 获取收发文件限制的大小(单位:M)
	* @return int 文件大小
	*/
	int GetFileSizeLimit() { return limit_file_size_; }

private:
	/**
	* 从数据库读取登陆数据
	* @return void	无返回值
	*/
	void ReadLoginDataFromFile();

	/**
	* 从配置文件读取日志记录等级
	* @return void	无返回值
	*/
	void ReadDemoLogLevel();

private:
	std::string account_;
	std::string password_;
	LoginStatus status_;
	bool active_;
	std::unique_ptr<LoginData>	current_login_data_;
	UTF8String			default_login_account_;     // 注销跳转到登录view的时候用到

	LOG_LEVEL demo_log_level_ = LV_APP;
	int limit_file_size_ = 15;
};
}