#pragma once
#include "pch.h"
#include "avchat_business_wrapper.h"
#include "g2_kit/component/avchat_component.h"
#include "g2_kit/component/avchat_component_def.h"
#include "shared/business_action_gateway/business_manager/business_manager.h"

namespace nim_comp
{
    AvChatComponent AvChatBusinessWrapper::component;
    std::shared_ptr<AvChatComponentEventHandler> AvChatBusinessWrapper::eventHandler_;

    void AvChatComponentEventHandler::onInvited(const std::string& invitor, std::vector<std::string> userIDs, bool isFromGroup, const std::string& groupID, int type, const std::string& attachment)
    {
        QLOG_APP(L"AvChatComponentEventHandler onInvited, invitor: {0}") << invitor;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnInvite;
        AvChatParams params;

        params.invitorAccid = invitor;
        params.members = userIDs;
        params.isFromGroup = isFromGroup;
        params.callType = type;

        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }

    void AvChatComponentEventHandler::onUserAccept(const std::string& userId)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserAccept, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserAccept;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserReject(const std::string& userId) 
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserReject, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserReject;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserEnter(const std::string& userId)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserEnter, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserEnter;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserLeave(const std::string& userId)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserLeave, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserLeave;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserDisconnect(const std::string& userId)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserDisconnect, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserDisconnect;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserBusy(const std::string& userId)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserBusy, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserBusy;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserCancel(const std::string& userId)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserCanale, userId: {0}") << userId;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserCancel;
        AvChatParams params;

        params.userId = userId;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }

    void AvChatComponentEventHandler::onCallingTimeOut()
    {
        QLOG_APP(L"AvChatComponentEventHandler onCallingTimeOut");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnCallingTimeOut;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }

    void AvChatComponentEventHandler::onDisconnect(int reason)
    {
        QLOG_APP(L"AvChatComponentEventHandler onDisconnect");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnDisconnect;
        AvChatParams params;

        params.reason = reason;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }

    void AvChatComponentEventHandler::OnVideoToAudio()
    {
        QLOG_APP(L"AvChatComponentEventHandler OnVideoToAudio");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnVideoToAudio;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onCallEnd()
    {
        QLOG_APP(L"AvChatComponentEventHandler onCallEnd");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnCallEnd;

        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onError(int errCode)
    {
        QLOG_APP(L"AvChatComponentEventHandler onError: {0}")<<errCode;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnError;
        AvChatParams params;
        params.errCode = errCode;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onOtherClientAccept()
    {
        QLOG_APP(L"AvChatComponentEventHandler onOtherClientAccept: ");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnOtherClientAccept;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onOtherClientReject()
    {
        QLOG_APP(L"AvChatComponentEventHandler onOtherClientReject: ");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnOtherClientReject;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onUserNetworkQuality(std::map<uint64_t, nertc::NERtcNetworkQualityType> network_quality)
    {
        QLOG_APP(L"AvChatComponentEventHandler onUserNetworkQuality: ");

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnUserNetworkQuality;
        AvChatParams params;
        params.network_quality = network_quality;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onCameraAvailable(const std::string& userId, bool available)
    {
        QLOG_APP(L"AvChatComponentEventHandler onCameraAvailable, userId: {0}, available: {1}") << userId << available;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnCameraAvailable;
        AvChatParams params;
        params.userId = userId;
        params.cameraAvailable = available;
        bp.body_.param_ = params;

        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onAudioAvailable(const std::string& userId, bool available)
    {
        QLOG_APP(L"AvChatComponentEventHandler onAudioAvailable, userId: {0}, available: {1}") << userId << available;

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnAudioAvailable;
        AvChatParams params;
        params.userId = userId;
        params.audioAvailable = available;
        bp.body_.param_ = params;

        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onLocalCameraStateChanged(bool available)
    {
        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnLocalCameraStateChanged;
        AvChatParams params;
        params.cameraAvailable = available;
        bp.body_.param_ = params;

        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onAudioVolumeChanged(unsigned char value, bool isRecord)
    {
        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnAudioVolumChanged;
        AvChatParams params;
        params.volume = value;
        params.isRecordDevice = isRecord;
        bp.body_.param_ = params;

        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatComponentEventHandler::onJoinChannel(const std::string& accid, nertc::uid_t uid, nertc::channel_id_t cid, const std::string& cname)
    {
        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnJoinChannel;
        AvChatParams params;
        params.userId = accid;
        params.uid = uid;
        params.cid = cid;
        params.cname = cname;
        bp.body_.param_ = params;

        nbase::BusinessManager::GetInstance()->Notify(bp);
    }
    void AvChatBusinessWrapper::release(const nbase::BatpPack& request)
    {
        QLOG_APP(L"Release component resource");
        component.release();

        nbase::BusinessManager::GetInstance()->Response(request);
    }
    void AvChatBusinessWrapper::setupAppKey(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        std::string key = params.appKey;
        bool bUseRtcSafeMode = params.useRtcSafeMode;
        component.setupAppKey(key, bUseRtcSafeMode);

        eventHandler_.reset();
        eventHandler_ = std::make_shared<AvChatComponentEventHandler>();
        component.regEventHandler(eventHandler_);
    }

    //avchat 组件并未定义onCalling事件，为了通知界面更新状态，在AvChatBusinessWrapper::call中发一个onCalling事件
    void AvChatBusinessWrapper::call(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        std::string userId = params.userId;
        AVCHAT_CALL_TYPE callType = AVCHAT_CALL_TYPE(params.callType);

        Json::Value values;
        Json::FastWriter writer;
        values["key"] = "call";
        values["value"] = "testvalue";
        component.call(userId, callType, writer.write(values), params.optCb);

        nbase::BatpPack bp;
        bp.head_.action_name_ = kAvChatOnCalling;
        bp.body_.param_ = params;
        nbase::BusinessManager::GetInstance()->Notify(bp);
    }

    void AvChatBusinessWrapper::login(const nbase::BatpPack& request)
    {
        assert(false);
    }
    void AvChatBusinessWrapper::logout(const nbase::BatpPack& request)
    {
        assert(false);
    }

    void AvChatBusinessWrapper::accept(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.accept(params.optCb);
    }
    void AvChatBusinessWrapper::reject(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.reject(params.optCb);
    }
    void AvChatBusinessWrapper::hangup(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.hangup(params.optCb);
    }
    void AvChatBusinessWrapper::switchCallType(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.switchCallType(params.sessionId, params.callType);
    }
    void AvChatBusinessWrapper::setAudioMute(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setAudioMute(params.sessionId, params.muteAudio);
    }
    void AvChatBusinessWrapper::cancel(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.cancel(params.optCb);
    }
    void AvChatBusinessWrapper::leave(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.leave(params.optCb);
    }
    void AvChatBusinessWrapper::setupLocalView(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        nertc::NERtcVideoCanvas canvas;
        canvas.window = params.window;
        canvas.cb = params.dataCb;
        canvas.user_data = nullptr;
        canvas.scaling_mode = nertc::kNERtcVideoScaleFit;
        component.setupLocalView(&canvas);
        nbase::BatpPackResponse response(request.head_, BatpTrailInfoFromHere);
        nbase::BusinessManager::GetInstance()->Response(response);
    }

    void AvChatBusinessWrapper::setupRemoteView(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        nertc::NERtcVideoCanvas canvas;
        canvas.window = params.window;
        canvas.cb = params.dataCb;
        canvas.user_data = nullptr;
        canvas.scaling_mode = nertc::kNERtcVideoScaleFit;
        component.setupRemoteView(&canvas, params.userId);
    }
    void AvChatBusinessWrapper::switchCamera(const nbase::BatpPack& request)
    {
        component.switchCamera();
    }
    void AvChatBusinessWrapper::getLocalDeviceList(
        std::vector<std::wstring>* recordDevicesNames,
        std::vector<std::wstring>* recordDevicesIds,
        std::vector<std::wstring>* playoutDevicesNames,
        std::vector<std::wstring>* playoutDevicesIds,
        std::vector<std::wstring>* videoDeviceNames,
        std::vector<std::wstring>* videoDeviceIds)
    {
        component.getLocalDeviceList(
            recordDevicesNames, recordDevicesIds,
            playoutDevicesNames, playoutDevicesIds,
            videoDeviceNames, videoDeviceIds);
    }
    unsigned char AvChatBusinessWrapper::getAudioVolumn(bool isRecord)
    {
        auto value = component.getAudioVolumn(isRecord);
        return value <= 255 ? value : value % 255;
    }
    std::wstring AvChatBusinessWrapper::getVideoDevice()
    {
        return component.getVideoDevice();
    }
    std::wstring AvChatBusinessWrapper::getAudioDevice(bool isRecord)
    {
        return component.getAudioDevice(isRecord);
    }
//     bool AvChatBusinessWrapper::isAudioEnable(bool isRecord)
//     {
//         return component.isAudioEnable(isRecord);
//     }
//     bool AvChatBusinessWrapper::isVideoEnable()
//     {
//         return component.isVideoEnable();
//     }
    void AvChatBusinessWrapper::setRecordDeviceVolume(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setRecordDeviceVolume(params.volume);
    }
    void AvChatBusinessWrapper::setPlayoutDeviceVolume(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setPlayoutDeviceVolume(params.volume);
    }

    void AvChatBusinessWrapper::setVideoDevice(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setVideoDevice(params.deviceId);
    }
    void AvChatBusinessWrapper::setAudioDevice(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setAudioDevice(params.deviceId, params.isRecordDevice);
    }
    void AvChatBusinessWrapper::enableLocalVideo(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.enableLocalVideo(params.cameraAvailable);
    }
    void AvChatBusinessWrapper::muteLocalAudio(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.muteLocalAudio(params.muteAudio);
    }
    void AvChatBusinessWrapper::enableAudioPlayout(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.enableAudioPlayout(params.audioAvailable);
    }
    void AvChatBusinessWrapper::startVideoPreview(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.startVideoPreview(params.startPreview);
    }

    void AvChatBusinessWrapper::startAudioDeviceLoopbackTest(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.startAudioDeviceLoopbackTest(params.interval);
    }
    void AvChatBusinessWrapper::stopAudioDeviceLoopbackTest(const nbase::BatpPack& request)
    {
        component.stopAudioDeviceLoopbackTest();
    }
    void AvChatBusinessWrapper::setVideoQuality(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setVideoQuality((nertc::NERtcVideoProfileType)params.videoQuality);
    }
    void AvChatBusinessWrapper::getTokenService(const nbase::BatpPack& request)
    {
        AvChatParams params = nbase::BatpParamCast<AvChatParams>(request.body_.param_);
        component.setTokenService((GetTokenServiceFunc)params.tockenServiceFunc);
    }
    std::string AvChatBusinessWrapper::getChannelId()
    {
        auto info = component.getCreatedChannelInfo();
        return info.channel_info_.channel_id_;
    }
}