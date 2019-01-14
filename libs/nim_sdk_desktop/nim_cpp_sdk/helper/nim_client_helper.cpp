/** @file nim_client_helper.cpp
  * @brief Client 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/07
  */

#include "nim_client_helper.h"

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
		, login_max_retry_times_(0)
		, custom_timeout_(30)
		, use_https_(false)
		, team_notification_unread_count_(false)
		, animated_image_thumbnail_enabled_(false)
		, upload_statistics_data_(true)
		, client_antispam_(false)
		, team_msg_ack_(false)
		, caching_markread_(false)
		, caching_markread_time_(1000)
		, caching_markread_count_(10)
		, enable_user_datafile_backup_(true)
		, enable_user_datafile_restore_(false)
		, enable_user_datafile_defrestoreproc_(false)
		, user_datafile_localbackup_folder_("") {}
 bool ParseOtherClientsPres(const Json::Value array_objs, std::list<OtherClientPres> &outs)
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