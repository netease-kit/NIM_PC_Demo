#include "nim_ui_init_manager.h"
#include "callback/login/login_callback.h"
#include "callback/session/session_callback.h"
#include "callback/http/http_callback.h"
#include "callback/audio/audio_callback.h"
#include "callback/team/team_callback.h"
#include "callback/vchat/vchat_callback.h"
#include "callback/rts/rts_callback.h"
#include "module/service/user_service.h"

namespace nim_ui
{

InitManager::InitManager()
{

}

InitManager::~InitManager()
{

}

void InitManager::InitSdk(const std::string& app_data_dir, const std::string& app_install_dir, const std::string& json_extension)
{
// 	Json::Value config_root;
// 
// 	std::wstring server_conf_path = QPath::GetAppPath();
// 	server_conf_path.append(L"server_conf.txt");
// 	TiXmlDocument document;
// 	if (shared::LoadXmlFromFile(document, server_conf_path))
// 	{
// 		TiXmlElement* root = document.RootElement();
// 		if (root)
// 		{
// 			bool use_test = false;
// 			Json::Value srv_config;
// 			if (auto pchar = root->Attribute("kNIMLbsAddress")) {
// 				srv_config[nim::kNIMLbsAddress] = pchar;
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMNosLbsAddress")) {
// 				srv_config[nim::kNIMNosLbsAddress] = pchar;
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMDefaultLinkAddress")) {
// 				srv_config[nim::kNIMDefaultLinkAddress].append(pchar);
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMDefaultNosUploadAddress")) {
// 				srv_config[nim::kNIMDefaultNosUploadAddress].append(pchar);
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMDefaultNosDownloadAddress")) {
// 				srv_config[nim::kNIMDefaultNosDownloadAddress].append(pchar);
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMDefaultNosAccessAddress")) {
// 				srv_config[nim::kNIMDefaultNosAccessAddress].append(pchar);
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMRsaPublicKeyModule")) {
// 				srv_config[nim::kNIMRsaPublicKeyModule] = pchar;
// 				use_test = true;
// 			}
// 			if (auto pchar = root->Attribute("kNIMRsaVersion")) {
// 				srv_config[nim::kNIMRsaVersion] = atoi(pchar);
// 				use_test = true;
// 			}
// 			if (use_test)
// 			{
// 				config_root[nim::kNIMPrivateServerSetting] = srv_config;
// 			}
// 		}
// 	}
// 
// 	//sdk能力参数（必填）
// 	Json::Value config_values;
// 	config_values[nim::kNIMDataBaseEncryptKey] = "Netease"; //string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）
// 	config_values[nim::kNIMPreloadAttach] = true;        //bool （选填，是否需要预下载附件缩略图， sdk默认预下载）
// 	config_root[nim::kNIMGlobalConfig] = config_values;
// 
// 	bool ret = nim::Client::Init("Netease", "", config_root.toStyledString().c_str()); // 载入云信sdk，初始化安装目录和用户目录

 	bool ret = nim::Client::Init(app_data_dir.c_str(), "", json_extension.c_str()); // 载入云信sdk，初始化安装目录和用户目录
	assert(ret);
	ret = nim::VChat::Init(""); // 初始化云信音视频
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
	nim::Talk::RegSendCustomSysmsgCb(nbase::Bind(&nim_comp::TalkCallback::OnSendCustomSysmsgCallback, std::placeholders::_1));

	//注册群事件的回调
	nim::Team::RegTeamEventCb(nbase::Bind(&nim_comp::TeamCallback::OnTeamEventCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//注册收到系统消息的回调
	nim::SystemMsg::RegSysmsgCb(nbase::Bind(&nim_comp::TeamCallback::OnReceiveSysmsgCallback, std::placeholders::_1));

	//注册http下载回调
	nim::Http::RegDownloadCb(nbase::Bind(&nim_comp::HttpCallback::OnHttpDownloadCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

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
}

void InitManager::CleanupSdk()
{
	nim::Audio::Cleanup();
	nim::VChat::Cleanup();
	nim::Client::Cleanup();
}

}

