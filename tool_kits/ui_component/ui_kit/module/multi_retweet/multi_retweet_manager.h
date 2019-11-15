#pragma once
#include "ui_kit/module/multi_retweet/multi_retweet_define.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_data_def.h"
namespace nim_comp
{
	class MultiMessageRetweetSession;
	using MultiMessageRetweetSessionPtr = std::shared_ptr<MultiMessageRetweetSession>;
	class  MultiMessageRetweetManager : public nbase::Singleton<MultiMessageRetweetManager>
	{
	public:
		MultiMessageRetweetSessionPtr CreateMultiMessageRetweetData(const std::string& session_id,const std::string& show_title,
			const std::list<nim::IMMessage>& message_list, 
			const MultiMessageRetweetProgressCallback& progress_cb,
			const std::function<void(bool ret, MultiMessageRetweetState state,const std::shared_ptr< MultiMessageRetweetData>& data)>& cb);
		MultiMessageRetweetSessionPtr ParseMultiMessageRetweetData(
			const std::shared_ptr< MultiMessageRetweetData>& data,
			const MultiMessageRetweetProgressCallback& progress_cb, 
			const std::function<void(bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list)>& cb);	
	private:
		std::map<int64_t, MultiMessageRetweetSessionPtr> session_list;
	};
}