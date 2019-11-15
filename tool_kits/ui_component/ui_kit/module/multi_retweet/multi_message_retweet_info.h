#pragma once
#include "ui_kit/module/multi_retweet/multi_retweet_define.h"
namespace nim_comp
{
	class MultiMessageRetweetInfo
	{
	public:
		MultiMessageRetweetInfo();
		virtual ~MultiMessageRetweetInfo() = default;
	public:
		std::wstring decrypt_path_;
		std::wstring encrypt_path_;
		std::wstring migrate_temp_directory_;
		std::string url_;
		std::string tag_;
		MultiMessageRetweetProgressCallback progreess_callback_;
		MultiMessageRetweetCompleteCallback complete_callback_;
	};
	using MultiMessageRetweetInfoPtr = std::shared_ptr<MultiMessageRetweetInfo>;

	// ------------------------------------------------------------------- ExportInfo

	class MultiMessageRetweetExportInfo : public MultiMessageRetweetInfo
	{
	public:
		MultiMessageRetweetExportInfo();
		~MultiMessageRetweetExportInfo() = default;
	public:
		MultiMessageRetweetCustomEncryptExportFileCallback encrypt_callback_;
	};
	using ExportInfoPtr = std::shared_ptr<MultiMessageRetweetExportInfo>;

	// ------------------------------------------------------------------- ImportInfo

	class MultiMessageRetweetImportInfo : public MultiMessageRetweetInfo
	{
	public:
		MultiMessageRetweetImportInfo();
		~MultiMessageRetweetImportInfo() = default;
	public:
		MultiMessageRetweetCustomDecryptImportFileCallback decrypt_callback_;
	};
	using ImportInfoPtr = std::shared_ptr<MultiMessageRetweetImportInfo>;

	class MultiMessageRetweetRawFileInfo
	{
	public:
		class Head
		{
		public:
			Head() :version_(0), terminal_(4)
			{
			}
		public:
			int version_;//必填,文件格式版本
			int terminal_;//选填 终端类型 PC客户端可固定填4
			std::string sdk_version_;//选填 云信SDK版本号
			std::string app_version_;//选填 应用版本号（这里是demo的版本号）
			int message_count_;//必填,最终写入文件中的消息数
		};
		class Body
		{
		public:
			std::list<nim::IMMessage> message_list_;
		};
	public:
		Head head_;
		Body body_;
	};
}