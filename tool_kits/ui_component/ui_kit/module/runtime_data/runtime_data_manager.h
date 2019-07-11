#pragma once
#include "runtime_data_defin.h"
namespace nim_comp
{	
	class RunTimeDataManager : public nbase::Singleton<RunTimeDataManager>
	{
		SingletonHideConstructor(RunTimeDataManager);
	private:
		RunTimeDataManager() : 
			uistyle_(UIStyle::join), is_searching_friend_(false), is_atting_someone_(false), is_sdk_inited_(false)
		{}
		~RunTimeDataManager() = default;
	public:
		void SetUIStyle(UIStyle style){ uistyle_ = style; };
		UIStyle GetUIStyle() const { return uistyle_; }
		bool IsSearchingFriend() const { return is_searching_friend_; }
		void SetSearchingFriendState(bool searching) { is_searching_friend_ = searching; };
		bool IsAttingSomeOne() const { return is_atting_someone_; }
		void SetAttingSomeOne(bool atting) { is_atting_someone_ = atting; }
		void SetSDKInited(bool value) { is_sdk_inited_ = value; };
		bool IsSDKInited() const { return is_sdk_inited_; }
	private:
		UIStyle uistyle_ = UIStyle::join;
		bool is_searching_friend_;
		bool is_atting_someone_;
		bool is_sdk_inited_;
	};
}