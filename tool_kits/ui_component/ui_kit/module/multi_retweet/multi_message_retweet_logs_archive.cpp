#include "stdafx.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_logs_archive.h"

namespace nim_comp
{
	RawFileArchive::RawFileArchive(const std::string& file_path, const std::function<void(int current_count, int count)>& progress_cb) :
		file_path_(file_path),
		progress_cb_(progress_cb)
	{
	}
	RawFileArchive::~RawFileArchive()
	{
		if (o_stream_.is_open())
			o_stream_.close();
		if (i_stream_.is_open())
			i_stream_.close();
	}
	bool RawFileArchive::ReadFromFile(const std::shared_ptr<MultiMessageRetweetRawFileInfo>& file_info)//to msgloglist
	{
		i_stream_.open(file_path_, std::ios::in);
		if (i_stream_.bad())
			return false;
		const static int line_length = 6144;
		char* line = new char[line_length];
	
		if (i_stream_.getline(line, line_length).bad())
			return false;
		Json::Value head_json_obj;
		if (Json::Reader().parse(line, head_json_obj))
		{
			file_info->head_.version_ = head_json_obj["version"].asInt();
			file_info->head_.message_count_ = head_json_obj["message_count"].asInt();
			if (head_json_obj.isMember("terminal"))
				file_info->head_.terminal_ = head_json_obj["terminal"].asInt();
			else
				file_info->head_.terminal_ = -1;
			if (head_json_obj.isMember("sdk_version"))
				file_info->head_.sdk_version_ = head_json_obj["sdk_version"].asString();
			if (head_json_obj.isMember("app_version"))
				file_info->head_.app_version_ = head_json_obj["app_version"].asString();
		}
		int current_count = 0;
		int count = file_info->head_.message_count_;
		memset(line, 0x00, line_length * sizeof(char));
		while (!i_stream_.eof())
		{
			if (!i_stream_.getline(line, line_length).bad() && strlen(line) > 0)
			{
				file_info->body_.message_list_.emplace_back(BodyCellArchive::ToMessageLog(std::string(line)));
				progress_cb_(++current_count, count);
				memset(line, 0x00, line_length * sizeof(char));
			}
		}
		i_stream_.close();
		return true;
	}
	bool RawFileArchive::WriteToFile(const std::shared_ptr<MultiMessageRetweetRawFileInfo>& raw_file_info)//to file
	{
		o_stream_.open(file_path_, std::ios::out);
		if (o_stream_.bad())
			return false;
		o_stream_buffer_.clear();
		Json::Value head_json_obj;
		head_json_obj["version"] = raw_file_info->head_.version_;
		head_json_obj["terminal"] = raw_file_info->head_.terminal_;
		head_json_obj["sdk_version"] = raw_file_info->head_.sdk_version_;
		head_json_obj["app_version"] = raw_file_info->head_.app_version_;
		head_json_obj["message_count"] = raw_file_info->head_.message_count_;
		o_stream_buffer_.append(Json::FastWriter().write(head_json_obj)).append("\n");
		BodyCellArchive body_cell_archive;
		int current_count = 0;
		int count = raw_file_info->head_.message_count_;
		for (auto& it : raw_file_info->body_.message_list_)
		{
			o_stream_buffer_.append((BodyCellArchive::ToString(it)));
			o_stream_buffer_.append("\n");
			progress_cb_(++current_count, count);
		}
		o_stream_ << o_stream_buffer_;
		o_stream_buffer_.clear();
		o_stream_.close();
	}
}

