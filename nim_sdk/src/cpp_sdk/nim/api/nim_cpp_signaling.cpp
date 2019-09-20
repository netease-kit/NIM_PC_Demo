/** @file nim_cpp_signaling.cpp
  * @brief NIM VChat提供的音视频（包括设备）相关接口
  * @copyright (c) 2015-2018, NetEase Inc. All rights reserved
  * @author gq
  * @date 2018/11/5
  */

#include "src/cpp_sdk/nim/api//nim_cpp_signaling.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"

namespace nim
{

#ifdef NIM_SDK_DLL_IMPORT
	typedef	void (*nim_signaling_reg_online_notify_cb)(nim_signaling_notify_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_reg_mutil_client_sync_notify_cb)(nim_signaling_notify_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_reg_offline_notify_cb)(nim_signaling_notify_list_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_reg_channels_sync_cb)(nim_signaling_channels_sync_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_reg_members_sync_cb)(nim_signaling_members_sync_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_create)(struct NIMSignalingCreateParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_close)(struct NIMSignalingCloseParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_join)(struct NIMSignalingJoinParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_leave)(struct NIMSignalingLeaveParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_query_channel_info)(struct NIMSignalingQueryChannelInfoParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_call)(struct NIMSignalingCallParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_invite)(struct NIMSignalingInviteParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_cancel_invite)(struct NIMSignalingCancelInviteParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_reject)(struct NIMSignalingRejectParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_accept)(struct NIMSignalingAcceptParam* param, nim_signaling_opt_cb_func cb, const void *user_data);
	typedef	void(*nim_signaling_control)(struct NIMSignalingControlParam* param, nim_signaling_opt_cb_func cb, const void *user_data);

#else
#include "nim_signaling.h"
#endif

	template <typename TR>
	static void OnOptCallback(int code, void* opt_res_param, const void *user_data)
	{
		CallbackProxy::DoSafeCallback<Signaling::SignalingOptCallback>(user_data, [=](const Signaling::SignalingOptCallback& cb){
			CallbackProxy::Invoke(cb, code, TR::GetOptParam(opt_res_param));
		}, true);
	}
	static void OnNotifyCallback(const struct NIMSignalingNotifyInfo* notify_info, const void *user_data)
	{
		CallbackProxy::DoSafeCallback<Signaling::SignalingNotifyCallback>(user_data, [=](const Signaling::SignalingNotifyCallback& cb){
			CallbackProxy::Invoke(cb, SignalingNotifyInfo::GetSharedPtr(notify_info));
		}, false);
	}
	static void OnNotifyListCallback(const struct NIMSignalingNotifyInfo** info_list, int32_t size, const void *user_data)
	{
		CallbackProxy::DoSafeCallback<Signaling::SignalingNotifyListCallback>(user_data, [=](const Signaling::SignalingNotifyListCallback& cb){
			std::list<std::shared_ptr<SignalingNotifyInfo>> post_info;
			if (info_list)
			{
				for (int32_t i = 0; i < size; ++i)
				{
					post_info.push_back(SignalingNotifyInfo::GetSharedPtr(info_list[i]));
				}
			}
			CallbackProxy::Invoke(cb, post_info);
		}, false);
	}
	static void OnChannelListCallback(const struct NIMSignalingChannelDetailedinfo* info_list, int32_t size, const void *user_data)
	{
		CallbackProxy::DoSafeCallback<Signaling::SignalingChannelListCallback>(user_data, [=](const Signaling::SignalingChannelListCallback& cb){
			std::list<SignalingChannelDetailedinfo> post_info;
			if (info_list)
			{
				for (int32_t i = 0; i < size; ++i)
				{
					post_info.push_back(SignalingChannelDetailedinfo(&info_list[i]));
				}
			}
			CallbackProxy::Invoke(cb, post_info);
		}, false);
	}
	static void OnChannelInfoCallback(const struct NIMSignalingChannelDetailedinfo* detailed_info, const void *user_data)
	{
		CallbackProxy::DoSafeCallback<Signaling::SignalingChannelCallback>(user_data, [=](const Signaling::SignalingChannelCallback& cb){
			CallbackProxy::Invoke(cb, SignalingChannelDetailedinfo(detailed_info));
		}, false);
	}


//dll-------------------------------
static Signaling::SignalingNotifyCallback g_cb_online_notify_pointer = nullptr;
void Signaling::RegOnlineNotifyCb(SignalingNotifyCallback cb)
{
	g_cb_online_notify_pointer = cb;
	return NIM_SDK_GET_FUNC(nim_signaling_reg_online_notify_cb)(&OnNotifyCallback, &g_cb_online_notify_pointer);
}
static Signaling::SignalingNotifyCallback g_cb_mutil_client_sync_notify_pointer = nullptr;
void Signaling::RegMutilClientSyncNotifyCb(SignalingNotifyCallback cb)
{
	g_cb_mutil_client_sync_notify_pointer = cb;
	return NIM_SDK_GET_FUNC(nim_signaling_reg_mutil_client_sync_notify_cb)(&OnNotifyCallback, &g_cb_mutil_client_sync_notify_pointer);
}
static Signaling::SignalingNotifyListCallback g_cb_offline_notify_pointer = nullptr;
void Signaling::RegOfflineNotifyCb(SignalingNotifyListCallback cb)
{
	g_cb_offline_notify_pointer = cb;
	return NIM_SDK_GET_FUNC(nim_signaling_reg_offline_notify_cb)(&OnNotifyListCallback, &g_cb_offline_notify_pointer);
}
static Signaling::SignalingChannelListCallback g_cb_channels_sync_pointer = nullptr;
void Signaling::RegChannelsSyncCb(SignalingChannelListCallback cb)
{
	g_cb_channels_sync_pointer = cb;
	return NIM_SDK_GET_FUNC(nim_signaling_reg_channels_sync_cb)(&OnChannelListCallback, &g_cb_channels_sync_pointer);
}
static Signaling::SignalingChannelCallback g_cb_members_sync_pointer = nullptr;
void Signaling::RegMembersSyncCb(SignalingChannelCallback cb)
{
	g_cb_members_sync_pointer = cb;
	return NIM_SDK_GET_FUNC(nim_signaling_reg_members_sync_cb)(&OnChannelInfoCallback, &g_cb_members_sync_pointer);
}
void Signaling::SignalingCreate(SignalingCreateParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto create_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_create)(&create_param, OnOptCallback<SignalingCreateResParam>, cb_pointer);

}
void Signaling::SignalingClose(SignalingCloseParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto close_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_close)(&close_param, OnOptCallback<SignalingCloseResParam>, cb_pointer);
}
void Signaling::Join(SignalingJoinParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto join_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_join)(&join_param, OnOptCallback<SignalingJoinResParam>, cb_pointer);
}
void Signaling::Leave(SignalingLeaveParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto leave_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_leave)(&leave_param, OnOptCallback<SignalingLeaveResParam>, cb_pointer);
}
void Signaling::QueryChannelInfo(SignalingQueryChannelInfoParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto query_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_query_channel_info)(&query_param, OnOptCallback<SignalingQueryChannelInfoResParam>, cb_pointer);
}
void Signaling::Call(SignalingCallParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto call_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_call)(&call_param, OnOptCallback<SignalingCallResParam>, cb_pointer);
}
void Signaling::Invite(SignalingInviteParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto invite_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_invite)(&invite_param, OnOptCallback<SignalingInviteResParam>, cb_pointer);
}
void Signaling::CancelInvite(SignalingCancelInviteParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto cancel_invite_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_cancel_invite)(&cancel_invite_param, OnOptCallback<SignalingCancelInviteResParam>, cb_pointer);
}
void Signaling::Reject(SignalingRejectParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto reject_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_reject)(&reject_param, OnOptCallback<SignalingRejectResParam>, cb_pointer);
}
void Signaling::Accept(SignalingAcceptParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto accept_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_accept)(&accept_param, OnOptCallback<SignalingAcceptResParam>, cb_pointer);
}
void Signaling::Control(SignalingControlParam param, SignalingOptCallback cb)
{
	SignalingOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SignalingOptCallback(cb);
	}
    auto control_param = param.GetCParam();
	NIM_SDK_GET_FUNC(nim_signaling_control)(&control_param, OnOptCallback<SignalingControlResParam>, cb_pointer);
}

}  // namespace nim

