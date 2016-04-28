#include "nim_ui_init_manager.h"
#include "module/emoji/emoji_info.h"
#include "callback/login/login_callback.h"
#include "callback/session/session_callback.h"
#include "callback/http/http_callback.h"
#include "callback/audio/audio_callback.h"
#include "callback/team/team_callback.h"
#include "callback/vchat/vchat_callback.h"
#include "callback/rts/rts_callback.h"
#include "module/service/user_service.h"
#include "shared/modal_wnd/async_do_modal.h"

namespace nim_ui
{

InitManager::InitManager()
{

}

InitManager::~InitManager()
{

}

void InitManager::InitUiKit()
{
	bool ret = nim::VChat::Init(""); // 初始化云信音视频
	assert(ret);

	nim_http::Init(); // 初始化云信http

	/* 以下注册的回调函数，都是在收到服务器推送的消息或事件时执行的。因此需要在程序开始时就注册好。 */
	//注册重连、被踢、掉线、多点登录、把移动端踢下线的回调
	nim::Client::RegReloginCb(&nim_comp::LoginCallback::OnReLoginCallback);
	nim::Client::RegKickoutCb(&nim_comp::LoginCallback::OnKickoutCallback);
	nim::Client::RegDisconnectCb(&nim_comp::LoginCallback::OnDisconnectCallback);
	nim::Client::RegMultispotLoginCb(&nim_comp::LoginCallback::OnMultispotLoginCallback);
	nim::Client::RegKickOtherClientCb(&nim_comp::LoginCallback::OnKickoutOtherClientCallback);

	//注册返回发送消息结果的回调，和收到消息的回调。
	nim::Talk::RegSendMsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendMsgCallback, std::placeholders::_1));
	nim::Talk::RegReceiveCb(nbase::Bind(&nim_comp::TalkCallback::OnReceiveMsgCallback, std::placeholders::_1));
	nim::Talk::RegReceiveMessagesCb(nbase::Bind(&nim_comp::TalkCallback::OnReceiveMsgsCallback, std::placeholders::_1));
	//nim::SystemMsg::RegSendCustomSysmsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendCustomSysmsgCallback, std::placeholders::_1));
	nim::MsgLog::RegMessageStatusChangedCb(nbase::Bind(&nim_comp::TalkCallback::OnMsgStatusChangedCallback, std::placeholders::_1));

	//注册群事件的回调
	nim::Team::RegTeamEventCb(nbase::Bind(&nim_comp::TeamCallback::OnTeamEventCallback, std::placeholders::_1));

	//注册返回发送自定义消息的结果的回调，和收到系统消息（包括自定义消息）的回调
	nim::SystemMsg::RegSendCustomSysmsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendCustomSysmsgCallback, std::placeholders::_1));
	nim::SystemMsg::RegSysmsgCb(nbase::Bind(&nim_comp::TeamCallback::OnReceiveSysmsgCallback, std::placeholders::_1));

	//注册NOS下载回调
	nim::NOS::RegDownloadCb(nbase::Bind(&nim_comp::HttpCallback::OnHttpDownloadCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//注册音视频回调
	nim::VChat::SetVideoDataCb(true, nim_comp::VChatCallback::VideoCaptureData);
	nim::VChat::SetVideoDataCb(false, nim_comp::VChatCallback::VideoRecData);
	nim::VChat::SetCbFunc(nim_comp::VChatCallback::VChatCb);

	//注册白板消息回调
	nim::Rts::SetStartNotifyCb(nbase::Bind(&nim_comp::RtsCallback::StartNotifyCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	nim::Rts::SetAckNotifyCb(nbase::Bind(&nim_comp::RtsCallback::AckNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	nim::Rts::SetSyncAckNotifyCb(nbase::Bind(&nim_comp::RtsCallback::SyncAckNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	nim::Rts::SetConnectNotifyCb(nbase::Bind(&nim_comp::RtsCallback::ConnectNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	nim::Rts::SetMemberChangeCb(nbase::Bind(&nim_comp::RtsCallback::MemberNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	nim::Rts::SetHangupNotifyCb(nbase::Bind(&nim_comp::RtsCallback::HangupNotifyCallback, std::placeholders::_1, std::placeholders::_2));
	nim::Rts::SetControlNotifyCb(nbase::Bind(&nim_comp::RtsCallback::ControlNotifyCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	nim::Rts::SetRecDataCb(nbase::Bind(&nim_comp::RtsCallback::RecDataCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	nim_comp::emoji::LoadEmoji();
}

void InitManager::CleanupUiKit()
{
	nim_audio::Audio::Cleanup();
	nim::VChat::Cleanup();
	nim::Client::Cleanup();

	CancelAllAsyncModalDialogs();
}

}

