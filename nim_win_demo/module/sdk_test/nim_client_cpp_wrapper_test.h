#pragma once

#include "nim_all_cpp_wrapper.h"

namespace nim_test
{

	class Client
	{
	public:

		//SDK初始化
		static bool Init()
		{
			std::string app_data_dir = "Netease";	//程序数据目录
			std::string app_install_dir = "";		//程序安装目录
			Json::Value config_root;				//扩展信息
			Json::Value config_values;
			config_values[nim::kNIMDataBaseEncryptKey] = "123456789123456789"; //SDK使用的数据库的加密密钥，建议使用32个字符
			config_root[nim::kNIMGlobalConfig] = config_values;
			bool ret = nim::Client::Init(app_data_dir, app_install_dir, config_root.toStyledString().c_str());
			assert(ret);

			return ret;
		}


		//登录回调处理
		static void OnLoginCallback(const char *json_params, const void* user_data)
		{
			Json::Value json;
			Json::Reader reader;
			if (reader.parse(json_params, json))	//解析返回的json字符串
			{
				int login_step = json[nim::kNIMLoginStep].asInt();	//登录所处的阶段
				int code = json[nim::kNIMErrorCode].asInt();		//错误码
				if (login_step == nim::kNIMLoginStepLogin && code == nim::kNIMResSuccess)
				{
					::MessageBox(NULL, L"hello word ", L"login success", NULL);
				}
			}
		}

// 		//登录
// 		static void Login(const std::string& user, const std::string& password)
// 		{
// 			std::string app_key = "45c6af3c98409b18a84451215d0bdd6e";	//app key 申请获得
// 			//std::string pass_md5 = QString::GetMd5(password);	//MD5加密redrain
// 			const char* md5 = nim::Tool::GetMd5(password);
// 			std::string pass_md5(md5);
// 			nim::Global::NIMFreeBuf((char*)md5);
// 
// 			nim::Client::Login(app_key, user, pass_md5, OnLoginCallback, nullptr);
// 		}



		//退出回调处理
		static void OnLogoutCallback(const std::string& json_params)
		{
			::MessageBox(NULL, L"success", L"Logout", NULL);
		}

		//退出
		static void Logout()
		{
			nim::NIMLogoutType logout_type = nim::kNIMLogoutAppExit;	//logout类型
			nim::Client::Logout(logout_type, &OnLogoutCallback);
		}

	};
























}

