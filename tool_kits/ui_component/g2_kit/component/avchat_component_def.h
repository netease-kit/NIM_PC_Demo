#pragma once
//G2易用性组件
//2020.11.03
//Martin
#ifndef AVCHAT_COMPONENT_DEF_
#define AVCHAT_COMPONENT_DEF_

#ifdef __cplusplus
extern"C"
{
#endif
	static const char* kAvChatRelease			= "release";
	static const char* kAvChatSetupAppKey		= "setupAppKey";
	static const char* kAvChatLogin				= "login";
	static const char* kAvChatLogout			= "logout";
	static const char* kAvChatCall				= "call";
	static const char* kAvChatAccept			= "accept";
	static const char* kAvChatReject			= "reject";
	static const char* kAvChatHangup			= "hangup";
	static const char* kAvChatCancel			= "cancel";
	static const char* kAvChatLeave				= "leave";

	static const char* kAvChatSetupLocalView	= "setupLocalView";
	static const char* kAvChatSetupRemoteView	= "setupRemoteView";
	static const char* kAvChatEnableLocalVideo	= "enableLocalVideo";
	static const char* kAvChatMuteLocalAudio	= "muteLocalAudio";
	static const char* kAvChatEnableAudioPlayout= "enableAudioPlayout";
	static const char* kAvChatSwitchCamera		= "switchCamera";
	static const char* kAvChatSetRecordVolume	= "setRecordDeviceVolume";
	static const char* kAvChatSetPlayoutVolume  = "setPlayoutDeviceVolume";
	static const char* kAvChatSetAudioDevice    = "setAudioDevice";
	static const char* kAvChatSetVideoDevice    = "setVideoDevice";
	static const char* kAvChatStartVideoPreview = "startVideoPreview";
	static const char* kAvChatSwitchCallType = "switchCallType";
	static const char* kAvChatSetAudioMute = "setAudioMute";
	static const char* kAvChatGetTockenService = "getTockenService";
	static const char* kAvChatStartAudioDeviceLoopbackTest = "startAudioDeviceLoopbackTest";
	static const char* kAvChatStopAudioDeviceLoopbackTest = "stopAudioDeviceLoopbackTest";
	static const char* kAvChatSetVideoQuality   = "setVideoQuality";
		   
	static const char* kAvChatOnCalling			= "onCalling";
	static const char* kAvChatOnInvite			= "onInvite";
	static const char* kAvChatOnUserAccept		= "onUserAccept";
	static const char* kAvChatOnUserReject		= "onUserReject";
	static const char* kAvChatOnUserEnter		= "onUserEnter";
	static const char* kAvChatOnUserLeave		= "onUserLeave";
	static const char* kAvChatOnUserDisconnect	= "onUserDisconnect";
	static const char* kAvChatOnUserBusy		= "onUserBusy";
	static const char* kAvChatOnUserCancel		= "onUserCancel";
	static const char* kAvChatOnCallingTimeOut	= "onCallingTimeOut";
	static const char* kAvChatOnDisconnect		= "onDisconnect";
	static const char* kAvChatOnVideoToAudio	= "onVideoToAudio";
	static const char* kAvChatOnCallEnd			= "onCallEnd";
	static const char* kAvChatOnError			= "onError";

	static const char* kAvChatOnOtherClientAccept = "onOtherClientAccept";
	static const char* kAvChatOnOtherClientReject = "onOtherClientReject";
	static const char* kAvChatOnUserNetworkQuality = "onUserNetworkQuality";

	static const char* kAvChatOnCameraAvailable = "onCameraAvailable";
	static const char* kAvChatOnAudioAvailable = "onAudioAvailable";
	static const char* kAvChatOnLocalCameraStateChanged = "onLocalCameraStateChanged";
	static const char* kAvChatOnAudioVolumChanged = "onAudioVolumeChanged";
	//自定义json信息中用到的key
	static const char* kAvChatChannelMembers	= "callUserList";
	static const char* kAvChatCallType			= "callType";
		  
	//nertc日志目录
	#define  AVCHAT_LOG_DIR	 L"Netease\\G2"
#ifdef __cplusplus
};
#endif //__cplusplus

#endif	   