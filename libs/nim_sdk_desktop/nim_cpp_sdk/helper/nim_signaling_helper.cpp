/** @file nim_signaling_helper.cpp
  * @brief 独立信令 辅助方法和数据结构定义
  * @copyright (c) 2015-2018, NetEase Inc. All rights reserved
  * @author gq
  * @date 2018/11/8
  */

#include "nim_signaling_helper.h"

namespace nim
{
	std::shared_ptr<SignalingNotityInfo> SignalingNotityInfo::GetSharedPtr(const NIMSignalingNotityInfo* c_info)
	{
		std::shared_ptr<SignalingNotityInfo> ret_notify = nullptr;
		if (c_info)
		{
			switch (c_info->event_type_)
			{
			case kNIMSignalingEventTypeClose:
			case kNIMSignalingEventTypeLeave:
			case kNIMSignalingEventTypeCtrl:
			{
				SignalingNotityInfo* notify = new SignalingNotityInfo;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeJoin:
			{
				auto cur_info = (NIMSignalingNotityInfoJoin*)c_info;
				SignalingNotityInfoJoin* notify = new SignalingNotityInfoJoin;
				notify->member_ = SignalingMemberInfo(&cur_info->member_);
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeInvite:
			{
				auto cur_info = (NIMSignalingNotityInfoInvite*)c_info;
				SignalingNotityInfoInvite* notify = new SignalingNotityInfoInvite;
				notify->push_info_ = SignalingPushInfo(&cur_info->push_info_);
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeCancelInvite:
			{
				auto cur_info = (NIMSignalingNotityInfoCancelInvite*)c_info;
				SignalingNotityInfoCancelInvite* notify = new SignalingNotityInfoCancelInvite;
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeReject:
			{
				auto cur_info = (NIMSignalingNotityInfoReject*)c_info;
				SignalingNotityInfoReject* notify = new SignalingNotityInfoReject;
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeAccept:
			{
				auto cur_info = (NIMSignalingNotityInfoAccept*)c_info;
				SignalingNotityInfoAccept* notify = new SignalingNotityInfoAccept;
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			default:
				break;
			}
		}
		if (ret_notify)
		{
			ret_notify->event_type_ = c_info->event_type_;
			ret_notify->channel_info_ = SignalingChannelInfo(&c_info->channel_info_);
			ret_notify->from_account_id_ = c_info->from_account_id_;
			ret_notify->custom_info_ = c_info->custom_info_;
			ret_notify->timestamp_ = c_info->timestamp_;
		}
		return ret_notify;
	}

}