/** @file nim_nos_helper.h
  * @brief NOS数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/12/16
  */

#include "nim_nos_helper.h"

namespace nim
{

bool ParseUploadResult( const std::string& url, const std::string& json, UploadMediaResult& res )
{
	res.url_ = url;

	Json::Value values;
	Json::Reader reader;
	if (reader.parse(json, values) && values.isObject())
	{
#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
		if (values.isMember(kNIMNosCallId))
			res.call_id_ = values[kNIMNosCallId].asString();
#endif
		res.res_id_ = values[kNIMNosResId].asString();
	}

	return true;	
}

bool ParseDownloadResult( const std::string& file_path, const std::string& call_id, const std::string& res_id, DownloadMediaResult& res )
{
	res.file_path_ = file_path;
	res.call_id_ = call_id;
	res.res_id_ = res_id;
	return true;
}

bool ParseProgressData( const std::string& json, ProgressData& res )
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(json, values) && values.isObject())
	{
		res.res_id_ = values[kNIMNosResId].asString();
	}

	return true;
}

}
