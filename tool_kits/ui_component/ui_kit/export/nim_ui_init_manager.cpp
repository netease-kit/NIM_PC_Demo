#include "nim_ui_init_manager.h"
#include "module/emoji/emoji_info.h"
#include "module/service/user_service.h"
#include "module/subscribe_event/subscribe_event_manager.h"
#include "callback/login/login_callback.h"
#include "callback/session/session_callback.h"
#include "callback/http/http_callback.h"
#include "callback/audio/audio_callback.h"
#include "callback/team/team_callback.h"
#include "callback/vchat/vchat_callback.h"
#include "callback/rts/rts_callback.h"
#include "callback/login/data_sync_callback.h"
#include "callback/multiport/multiport_push_callback.h"
#include "callback/subscribe_event/subscribe_event_callback.h"
#include "shared/modal_wnd/async_do_modal.h"

namespace nim_ui
{

void InitManager::InitUiKit(bool enable_subscribe_event, InitMode mode)
{
	if (init_)
		return;

	/* 以下注册的回调函数，都是在收到服务器推送的消息或事件时执行的。因此需要在程序开始时就注册好。 */
	//注册重连、被踢、掉线、多点登录、把移动端踢下线的回调
	nim::Client::RegReloginCb(&nim_comp::LoginCallback::OnReLoginCallback);
	nim::Client::RegKickoutCb(&nim_comp::LoginCallback::OnKickoutCallback);
	nim::Client::RegDisconnectCb(&nim_comp::LoginCallback::OnDisconnectCallback);
	nim::Client::RegMultispotLoginCb(&nim_comp::LoginCallback::OnMultispotLoginCallback);
	nim::Client::RegKickOtherClientCb(&nim_comp::LoginCallback::OnKickoutOtherClientCallback);
	nim::Client::RegSyncMultiportPushConfigCb(&nim_comp::MultiportPushCallback::OnMultiportPushConfigChange);

	if (mode == kIM)
	{
		//注册数据同步结果的回调
		nim::DataSync::RegCompleteCb(nbase::Bind(&nim_comp::DataSyncCallback::SyncCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		//注册返回发送消息结果的回调，和收到消息的回调。
		nim::Talk::RegSendMsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendMsgCallback, std::placeholders::_1));
		nim::Talk::RegReceiveCb(nbase::Bind(&nim_comp::TalkCallback::OnReceiveMsgCallback, std::placeholders::_1));
		nim::Talk::RegRecallMsgsCallback(nbase::Bind(&nim_comp::TalkCallback::OnReceiveRecallMsgCallback, std::placeholders::_1, std::placeholders::_2));
		nim::Talk::RegReceiveMessagesCb(nbase::Bind(&nim_comp::TalkCallback::OnReceiveMsgsCallback, std::placeholders::_1));
		//nim::SystemMsg::RegSendCustomSysmsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendCustomSysmsgCallback, std::placeholders::_1));
		nim::MsgLog::RegMessageStatusChangedCb(nbase::Bind(&nim_comp::TalkCallback::OnMsgStatusChangedCallback, std::placeholders::_1));

		nim::Talk::RegReceiveBroadcastMsgCb(nbase::Bind(&nim_comp::TalkCallback::OnReceiveBroadcastMsgCallback, std::placeholders::_1));
		nim::Talk::RegReceiveBroadcastMsgsCb(nbase::Bind(&nim_comp::TalkCallback::OnReceiveBroadcastMsgsCallback, std::placeholders::_1));

		//注册群事件的回调
		nim::Team::RegTeamEventCb(nbase::Bind(&nim_comp::TeamCallback::OnTeamEventCallback, std::placeholders::_1));

		//注册返回发送自定义消息的结果的回调，和收到系统消息（包括自定义消息）的回调
		nim::SystemMsg::RegSendCustomSysmsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendCustomSysmsgCallback, std::placeholders::_1));
		nim::SystemMsg::RegSysmsgCb(nbase::Bind(&nim_comp::TeamCallback::OnReceiveSysmsgCallback, std::placeholders::_1));

		//注册NOS下载上传回调
		nim::NOS::RegDownloadCb(nbase::Bind(&nim_comp::HttpCallback::OnHttpDownloadCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		nim::NOS::RegUploadCb(nbase::Bind(&nim_comp::HttpCallback::OnHttpUploadCallback, std::placeholders::_1, std::placeholders::_2));

		//注册事件订阅回调
		nim::SubscribeEvent::RegPushEventCb(nbase::Bind(&nim_comp::SubscribeEventCallback::OnPushEventCallback, std::placeholders::_1, std::placeholders::_2));
		nim::SubscribeEvent::RegBatchPushEventCb(nbase::Bind(&nim_comp::SubscribeEventCallback::OnBatchPushEventCallback, std::placeholders::_1, std::placeholders::_2));

		//注册音视频回调
		nim::VChat::SetVideoDataCb(true, nim_comp::VChatCallback::VideoCaptureData);
		nim::VChat::SetVideoDataCb(false, nim_comp::VChatCallback::VideoRecData);
		nim::VChat::SetCbFunc(nim_comp::VChatCallback::VChatCb);

		//注册白板消息回调
		nim::Rts::SetStartNotifyCb(nbase::Bind(&nim_comp::RtsCallback::StartNotifyCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		nim::Rts::SetAckNotifyCb(nbase::Bind(&nim_comp::RtsCallback::AckNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		nim::Rts::SetSyncAckNotifyCb(nbase::Bind(&nim_comp::RtsCallback::SyncAckNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		nim::Rts::SetConnectNotifyCb(nbase::Bind(&nim_comp::RtsCallback::ConnectNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		nim::Rts::SetMemberChangeCb(nbase::Bind(&nim_comp::RtsCallback::MemberNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
		nim::Rts::SetHangupNotifyCb(nbase::Bind(&nim_comp::RtsCallback::HangupNotifyCallback, std::placeholders::_1, std::placeholders::_2));
		nim::Rts::SetControlNotifyCb(nbase::Bind(&nim_comp::RtsCallback::ControlNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		nim::Rts::SetRecDataCb(nbase::Bind(&nim_comp::RtsCallback::RecDataCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		//启用事件订阅模块
		nim_comp::SubscribeEventManager::GetInstance()->SetEnabled(enable_subscribe_event);
	}

	//加载聊天表情
	nim_comp::emoji::LoadEmoji();

	init_ = true;
}

void InitManager::CleanupUiKit()
{
	if (!init_)
		return;

	nim_audio::Audio::Cleanup();
	nim::VChat::Cleanup();
	nim::Client::Cleanup();

	CancelAllAsyncModalDialogs();

	init_ = false;
}

}

