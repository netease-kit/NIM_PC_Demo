#include "stdafx.h"
#include "ui_kit/module/multi_retweet/multi_retweet_manager.h"
#include "ui_kit/module/multi_retweet/multi_retweet_session.h"

namespace nim_comp
{
	MultiMessageRetweetSessionPtr MultiMessageRetweetManager::CreateMultiMessageRetweetData(const std::string& session_id, const std::string& show_title,
		const std::list<nim::IMMessage>& message_list,
		const MultiMessageRetweetProgressCallback& progress_cb,
		const std::function<void(bool ret, MultiMessageRetweetState state, const std::shared_ptr< MultiMessageRetweetData>& data)>& cb)
	{
		auto session = std::make_shared<MultiMessageRetweetSession>(session_id,show_title,message_list,progress_cb,cb);
		session_list.insert(std::make_pair((int64_t)session.get(),session));
		session->DoExport();
		return session;
	}
	MultiMessageRetweetSessionPtr MultiMessageRetweetManager::ParseMultiMessageRetweetData(
		const std::shared_ptr< MultiMessageRetweetData>& data,
		const MultiMessageRetweetProgressCallback& progress_cb,
		const std::function<void(bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list)>& cb)
	{
		auto session = std::make_shared<MultiMessageRetweetSession>(data, progress_cb, cb);
		session_list.insert(std::make_pair((int64_t)session.get(), session));
		session->DoImport();
		return session;
	}
	
}