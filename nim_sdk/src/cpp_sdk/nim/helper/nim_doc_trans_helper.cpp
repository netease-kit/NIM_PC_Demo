/** @file nim_doc_trans_helper.cpp
  * @brief 文档传输辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2016/12/15
  */

#include "src/cpp_sdk/nim/helper/nim_doc_trans_helper.h"
#include "public_define/defines/nim_define/nim_doc_trans_def.h"

namespace nim
{


 bool ParseDocTransInfo( const std::string& json, DocTransInfo& info )
{
	nim_cpp_wrapper_util::Json::Value value;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(json, value) && value.isObject())
	{
		info.id_ = value[kNIMDocTransId].asString();
		info.name_ = value[kNIMDocTransName].asString();
		info.source_type_ = (NIMDocTranscodingFileType)value[kNIMDocTransSourceType].asInt();
		info.size_ = value[kNIMDocTransSize].asInt64();
		info.pic_type_ = (NIMDocTranscodingImageType)value[kNIMDocTransPicType].asInt();
		info.state_ = (NIMDocTranscodingState)value[kNIMDocTransState].asInt();
		info.url_prefix_ = value[kNIMDocTransUrlPrefix].asString();
		info.page_num_ = value[kNIMDocTransPageNum].asInt();
		info.ext_ = value[kNIMDocTransExt].asString();

		if (value.isMember(kNIMDocTransFilePath))
			info.upload_file_path = value[kNIMDocTransFilePath].asString();
		if (value.isMember(kNIMDocTransUploadStatus))
			info.continue_upload_state_ = (NIMDocContinueUploadState)value[kNIMDocTransUploadStatus].asInt();
		if (value.isMember(kNIMDocTransPicInfo))
		{
			auto size = value[kNIMDocTransPicInfo].size();
			for (int i = 0; i < (int)size; i++)
			{
				PictureInfo pic_info;
				pic_info.quality_ = (NIMDocTranscodingQuality)value[kNIMDocTransPicInfo][i][kNIMDocTransQuality].asInt();
				pic_info.width_ = value[kNIMDocTransPicInfo][i][kNIMDocTransWidth].asInt();
				pic_info.height_ = value[kNIMDocTransPicInfo][i][kNIMDocTransHeight].asInt();
				pic_info.size_ = value[kNIMDocTransPicInfo][i][kNIMDocTransSize].asInt64();
				info.pic_info_.push_back(pic_info);
			}
		}
		return true;
	}

	return false;
}

 int32_t ParseDocTransInfos( const std::string& json, std::list<DocTransInfo>& info_list )
{
	int count = 0;
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(json, values) && values.isObject())
	{
		count = values[kNIMDocTransCount].asInt();
		nim_cpp_wrapper_util::Json::Value infos = values[kNIMDocTransInfos];
		if (infos.isArray() /*&& infos.size() == count*/)
		{
			auto size = infos.size();
			nim_cpp_wrapper_util::Json::Value value;
			for (int i = 0; i < (int)size; i++)
			{
				value = infos[i];
				DocTransInfo info;
				info.id_ = value[kNIMDocTransId].asString();
				info.name_ = value[kNIMDocTransName].asString();
				info.source_type_ = (NIMDocTranscodingFileType)value[kNIMDocTransSourceType].asInt();
				info.size_ = value[kNIMDocTransSize].asInt64();
				info.pic_type_ = (NIMDocTranscodingImageType)value[kNIMDocTransPicType].asInt();
				info.state_ = (NIMDocTranscodingState)value[kNIMDocTransState].asInt();
				info.url_prefix_ = value[kNIMDocTransUrlPrefix].asString();
				info.page_num_ = value[kNIMDocTransPageNum].asInt();
				info.ext_ = value[kNIMDocTransExt].asString();

				if (value.isMember(kNIMDocTransFilePath))
					info.upload_file_path = value[kNIMDocTransFilePath].asString();
				if (value.isMember(kNIMDocTransUploadStatus))
					info.continue_upload_state_ = (NIMDocContinueUploadState)value[kNIMDocTransUploadStatus].asInt();
				if (value.isMember(kNIMDocTransPicInfo))
				{
					auto size = value[kNIMDocTransPicInfo].size();
					for (int i = 0; i < (int)size; i++)
					{
						PictureInfo pic_info;
						pic_info.quality_ = (NIMDocTranscodingQuality)value[kNIMDocTransPicInfo][i][kNIMDocTransQuality].asInt();
						pic_info.width_ = value[kNIMDocTransPicInfo][i][kNIMDocTransWidth].asInt();
						pic_info.height_ = value[kNIMDocTransPicInfo][i][kNIMDocTransHeight].asInt();
						pic_info.size_ = value[kNIMDocTransPicInfo][i][kNIMDocTransSize].asInt64();
						info.pic_info_.push_back(pic_info);
					}
				}
				info_list.push_back(info);
			}			
		}
	}

	return count;
}

}
