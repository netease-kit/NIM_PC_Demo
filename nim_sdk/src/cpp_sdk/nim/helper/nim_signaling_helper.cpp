/** @file nim_signaling_helper.cpp
  * @brief 独立信令 辅助方法和数据结构定义
  * @copyright (c) 2015-2018, NetEase Inc. All rights reserved
  * @author gq
  * @date 2018/11/8
  */

#include "src/cpp_sdk/nim/helper/nim_signaling_helper.h"

namespace nim
{
	std::shared_ptr<SignalingNotifyInfo> SignalingNotifyInfo::GetSharedPtr(const NIMSignalingNotifyInfo* c_info)
	{
		std::shared_ptr<SignalingNotifyInfo> ret_notify = nullptr;
		if (c_info)
		{
			switch (c_info->event_type_)
			{
			case kNIMSignalingEventTypeClose:
			case kNIMSignalingEventTypeLeave:
			case kNIMSignalingEventTypeCtrl:
			{
				SignalingNotifyInfo* notify = new SignalingNotifyInfo;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeJoin:
			{
				auto cur_info = (NIMSignalingNotifyInfoJoin*)c_info;
				SignalingNotifyInfoJoin* notify = new SignalingNotifyInfoJoin;
				notify->member_ = SignalingMemberInfo(&cur_info->member_);
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeInvite:
			{
				auto cur_info = (NIMSignalingNotifyInfoInvite*)c_info;
				SignalingNotifyInfoInvite* notify = new SignalingNotifyInfoInvite;
				notify->push_info_ = SignalingPushInfo(&cur_info->push_info_);
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeCancelInvite:
			{
				auto cur_info = (NIMSignalingNotifyInfoCancelInvite*)c_info;
				SignalingNotifyInfoCancelInvite* notify = new SignalingNotifyInfoCancelInvite;
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeReject:
			{
				auto cur_info = (NIMSignalingNotifyInfoReject*)c_info;
				SignalingNotifyInfoReject* notify = new SignalingNotifyInfoReject;
				notify->request_id_ = cur_info->request_id_;
				notify->to_account_id_ = cur_info->to_account_id_;
				ret_notify.reset(notify);
				break;
			}
			case kNIMSignalingEventTypeAccept:
			{
				auto cur_info = (NIMSignalingNotifyInfoAccept*)c_info;
				SignalingNotifyInfoAccept* notify = new SignalingNotifyInfoAccept;
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