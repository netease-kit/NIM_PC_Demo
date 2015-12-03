#ifndef _NIM_SDK_CPP_CLIENT_HELPER_H_
#define _NIM_SDK_CPP_CLIENT_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"

namespace nim
{
/** @brief Client 辅助方法和数据结构定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/07
  */

#include "nim_client_def.h"
#include "nim_res_code_def.h"

struct SDKConfig
{
	//global_config
	std::string	database_encrypt_key_;
	bool			preload_attach_;
	NIMSDKLogLevel	sdk_log_level_;

	//private_server_setting
	bool			use_private_server_;
	std::string	lbs_address_;
	std::string	nos_lbs_address_;
	std::list<std::string>	default_link_address_;
	std::list<std::string>	default_nos_upload_address_;
	std::list<std::string>	default_nos_download_address_;
	std::list<std::string>	default_nos_access_address_;
	std::string	rsa_public_key_module_;
	int			rsa_version_;

	SDKConfig() : preload_attach_(true)
				, use_private_server_(false)
				, rsa_version_(0)
				, sdk_log_level_(kNIMSDKLogLevelApp) {}
};

struct OtherClientPres
{
	std::string	app_account_;
	NIMClientType	client_type_;
	std::string	client_os_;
	std::string	mac_address_;
	std::string	device_id_;
	__int64		login_time_;

	OtherClientPres() : login_time_(0) {}
};

struct LoginRes
{
	NIMResCode res_code_;
	bool relogin_;
	NIMLoginStep	login_step_;
	std::list<OtherClientPres> other_clients_;

	LoginRes() : relogin_(false) {}
};

struct KickoutRes
{
	NIMClientType client_type_;
	NIMKickReason kick_reason_;
};

struct MultiSpotLoginRes
{
	NIMMultiSpotNotiyType	notiry_type_;
	std::list<OtherClientPres> other_clients_;
};

struct KickOtherRes
{
	NIMResCode res_code_;
	std::list<std::string> device_ids_;
};

bool ParseOtherClientsPres(const Json::Value array_objs, std::list<OtherClientPres> &outs);

}
#endif //_NIM_SDK_CPP_CLIENT_HELPER_H_