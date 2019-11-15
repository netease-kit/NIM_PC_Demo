#pragma once
#include "ui_kit/module/multi_retweet/multi_retweet_define.h"
namespace nim_comp
{

	class MultiMessageRetweetData;
	class MultiMessageRetweetRawFileInfo;
	class MultiMessageRetweetExportInfo;
	class MultiMessageRetweetImportInfo;

	class MultiMessageRetweetSession : public nbase::SupportWeakCallback
	{
	public:
		MultiMessageRetweetSession(const std::string& session_id, const std::string& show_title,
			const std::list<nim::IMMessage>& message_list,
			const MultiMessageRetweetProgressCallback& progress_cb,
			const std::function<void(bool ret, MultiMessageRetweetState state, const std::shared_ptr< MultiMessageRetweetData>& data)>& cb);
		MultiMessageRetweetSession(
			const std::shared_ptr< MultiMessageRetweetData>& data,
			const MultiMessageRetweetProgressCallback& progress_cb,
			const std::function<void(bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list)>& cb);
		void Init(const std::string& session_id, const std::string& show_title,
			const std::list<nim::IMMessage>& message_list,
			const MultiMessageRetweetProgressCallback& progress_cb,
			const std::function<void(bool ret, MultiMessageRetweetState state, const std::shared_ptr< MultiMessageRetweetData>& data)>& cb);
		void Init(
			const MultiMessageRetweetProgressCallback& progress_cb,
			const std::function<void(bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list)>& cb);
		void DoExport();
		void DoImport();
	private:
		std::string CreatePassword() const;
	private:
		std::shared_ptr< MultiMessageRetweetData> data_;
		std::shared_ptr<MultiMessageRetweetRawFileInfo> raw_file_info_;
		std::shared_ptr<MultiMessageRetweetExportInfo> exportinfo_;
		std::shared_ptr<MultiMessageRetweetImportInfo> importinfo_;
	};
}
