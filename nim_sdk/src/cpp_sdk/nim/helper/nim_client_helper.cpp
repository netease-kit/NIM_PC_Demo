/** @file nim_client_helper.cpp
  * @brief Client 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/07
  */

#include "src/cpp_sdk/nim/helper/nim_client_helper.h"

namespace nim
{
	const std::string kDEFAULT_PRELOAD_IMAGE_NAME_TEMPLATE = "thumb_";
	SDKConfig::SDKConfig() : preload_attach_(true)
		, preload_image_quality_(-1)
		, preload_image_name_template_(kDEFAULT_PRELOAD_IMAGE_NAME_TEMPLATE)
		, sdk_log_level_(kNIMSDKLogLevelApp)
		, use_private_server_(false)
		, rsa_version_(0)
		, sync_session_ack_(true)
		, need_update_lbs_befor_relogin_(false)
		, login_max_retry_times_(0)
		, custom_timeout_(30)
		, use_https_(true)
		, team_notification_unread_count_(false)
		, vchat_miss_unread_count_(false)
		, reset_unread_count_when_recall_(false)
		, upload_sdk_events_after_login_(false)
		, animated_image_thumbnail_enabled_(false)
		, client_antispam_(false)
		, team_msg_ack_(false)
		, caching_markread_(false)
		, caching_markread_time_(1000)
		, caching_markread_count_(10)
		, enable_user_datafile_backup_(true)
		, enable_user_datafile_restore_(false)
		, enable_user_datafile_defrestoreproc_(false)
		, user_datafile_localbackup_folder_("")
		, nego_key_neca_(0x0001)
		, nego_key_neca_key_parta_("")
		, nego_key_neca_key_partb_("")
		, nego_key_neca_key_version_(0)
		, comm_neca_(0x0001)
		, dedicated_cluste_flag_(false)
		, ip_protocol_version_(0)
		, hand_shake_type_(1){}
 bool ParseOtherClientsPres(const nim_cpp_wrapper_util::Json::Value array_objs, std::list<OtherClientPres> &outs)
{
	if (array_objs.isArray())
	{
		int size = array_objs.size();
		for (int index = 0; index < size; index++)
		{
			OtherClientPres pres;
			pres.app_account_ = array_objs[index][kNIMPresAppAccount].asString();
			pres.client_type_ = (NIMClientType)array_objs[index][kNIMPresClientType].asUInt();
			pres.client_os_ = array_objs[index][kNIMPresOS].asString();
			pres.mac_address_ = array_objs[index][kNIMPresMac].asString();
			pres.device_id_ = array_objs[index][kNIMPresDeviceID].asString();
			pres.login_time_ = array_objs[index][kNIMPresLoginTime].asInt64();
			pres.custom_data_ = array_objs[index][kNIMPresCustomTag].asString();
			outs.push_back(pres);
		}
		return true;
	}
	return false;
}
}