/** @file nertc_engine_main.h
* @brief NERTC SDK接口概览。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2021, NetEase Inc. All rights reserved
*/


/**
 @mainpage Introduction
 @brief <p>网易云信 NERTC SDK
 提供完善的音视频通话开发框架，提供基于网络的点对点视频通话和语音通话功能，还提供多人视频和音频会议功能，支持通话中音视频设备控制和实时音视频模式切换，支持视频采集数据回调以实现美颜等自定义功能。</p>
 <p><ul>
 <li>\ref nertc::IRtcEngine "IRtcEngine" 接口类包含应用程序调用的主要方法。</li>
 <li>\ref nertc::IRtcEngineEx "IRtcEngineEx" 接口类包含应用程序调用的扩展方法。</li>
 <li>\ref nertc::IRtcEngineEventHandler "IRtcEngineEventHandler" 接口类用于向应用程序发送回调通知。</li>
 <li>\ref nertc::IRtcEngineEventHandlerEx "IRtcEngineEventHandlerEx" 接口类用于向应用程序发送扩展回调通知。</li>
 <li>\ref nertc::IRtcMediaStatsObserver "IRtcMediaStatsObserver" 接口类用于向应用程序发送扩展的媒体回调通知。</li>
 <li>\ref nertc::INERtcAudioFrameObserver "INERtcAudioFrameObserver" 接口类用于向应用程序发送音频数据帧回调通知。</li>
 <li>\ref nertc::IAudioDeviceManager "IAudioDeviceManager" 接口类用于向应用程序使用音频设备相关功能方法。</li>
 <li>\ref nertc::IVideoDeviceManager "IVideoDeviceManager" 接口类用于向应用程序使用视频设备相关功能方法。</li>
 </ul></p>
 
 <h2 id="房间管理">房间管理</h2>

 <table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::createNERtcEngine "createNERtcEngine"</td>
    <td>创建 RTC 引擎对象。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::destroyNERtcEngine "destroyNERtcEngine"</td>
    <td>销毁 RTC 引擎对象。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::initialize "initialize"</td>
    <td>初始化 NERTC SDK 服务。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::release "release"</td>
    <td>销毁 IRtcEngine 对象。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getVersion "getVersion"</td>
    <td>查询 SDK 版本号。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::setChannelProfile "setChannelProfile"</td>
    <td>设置房间场景。</td>
    <td>V3.6.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::setClientRole "setClientRole"</td>
    <td>设置用户角色。</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::joinChannel "joinChannel"</td>
    <td>加入房间。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::leaveChannel "leaveChannel"</td>
    <td>离开房间。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::switchChannel "switchChannel"</td>
    <td>快速切换房间。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getConnectionState "getConnectionState"</td>
    <td>获取网络连接状态。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::queryInterface "queryInterface"</td>
    <td>获取设备管理员对象的指针。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setParameters "setParameters"</td>
    <td>设置音视频通话的相关参数。</td>
    <td>V3.5.0</td>
  </tr>
 </table>

 ## 房间事件
 
 <table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onClientRoleChanged "onClientRoleChanged"</td>
    <td>用户角色已切换回调。</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onJoinChannel "onJoinChannel"</td>
    <td>加入房间回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onRejoinChannel "onRejoinChannel"</td>
    <td>重新加入房间回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel"</td>
    <td>离开房间回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onUserJoined "onUserJoined"</td>
    <td>远端用户加入当前房间回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onUserLeft "onUserLeft"</td>
    <td>远端用户离开当前房间回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onDisconnect "onDisconnect"</td>
    <td>服务器连接断开回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onReconnectingStart "onReconnectingStart"</td>
    <td>开始重连回调。</td>
    <td>V3.7.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onConnectionStateChange "onConnectionStateChange"</td>
    <td>网络连接状态已改变回调。</td>
    <td>V3.8.1</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onReleasedHwResources "onReleasedHwResources"</td>
    <td>通话结束设备资源释放回调。</td>
    <td>V3.5.0</td>
  </tr>
</table>

 ## 音频管理

 <table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setAudioProfile "setAudioProfile"</td>
    <td>设置音频编码配置</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::adjustRecordingSignalVolume "adjustRecordingSignalVolume"</td>
    <td>调节录音音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::adjustPlaybackSignalVolume "adjustPlaybackSignalVolume"</td>
    <td>设置音频编码配置</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::enableLocalAudio "enableLocalAudio"</td>
    <td>开关本地音频采集</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::muteLocalAudioStream "muteLocalAudioStream"</td>
    <td>开关本地音频发送</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::subscribeRemoteAudioStream "subscribeRemoteAudioStream"</td>
    <td>订阅／取消订阅指定音频流。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startSystemAudioLoopbackCapture "startSystemAudioLoopbackCapture"</td>
    <td>开启本地声卡采集。仅支持 Windows 平台。</td>
    <td>V4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopSystemAudioLoopbackCapture "stopSystemAudioLoopbackCapture"</td>
    <td>关闭本地声卡采集。仅支持 Windows 平台。</td>
    <td>V4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setSystemAudioLoopbackCaptureVolume "setSystemAudioLoopbackCaptureVolume"</td>
    <td>设置本地声卡采集音量。仅支持 Windows 平台。</td>
    <td>V4.0.0</td>
  </tr>
</table>
 

 ## 视频管理
 
 <table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::enableLocalVideo "enableLocalVideo"</td>
    <td>开关本地视频</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setVideoConfig "setVideoConfig"</td>
    <td>设置视频发送配置</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::setupLocalVideoCanvas "setupLocalVideoCanvas"</td>
    <td>设置本地用户视图</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::setupRemoteVideoCanvas "setupRemoteVideoCanvas"</td>
    <td>设置远端用户视图</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setLocalRenderMode "setLocalRenderMode"</td>
    <td>设置本地视图显示模式</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setRemoteRenderMode "setRemoteRenderMode"</td>
    <td>设置远端视图显示模式</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startVideoPreview "startVideoPreview"</td>
    <td>开启视频预览</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopVideoPreview "stopVideoPreview"</td>
    <td>停止视频预览</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::muteLocalVideoStream "muteLocalVideoStream"</td>
    <td>开关本地视频发送</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngine::subscribeRemoteVideoStream "subscribeRemoteVideoStream"</td>
    <td>订阅 / 取消订阅指定远端用户的视频流</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setLocalVideoMirrorMode "setLocalVideoMirrorMode"</td>
    <td>设置本地视频镜像模式</td>
    <td>V3.5.0</td>
  </tr>
</table>
 
<h2 id="本地媒体事件">本地媒体事件</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onFirstVideoDataReceived "onFirstVideoDataReceived"</td>
    <td>已显示首帧远端视频回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onFirstAudioDataReceived "onFirstAudioDataReceived"</td>
    <td>已接收到远端音频首帧回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onFirstAudioFrameDecoded "onFirstAudioFrameDecoded"</td>
    <td>已解码远端音频首帧的回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onFirstVideoFrameDecoded "onFirstVideoFrameDecoded"</td>
    <td>已接收到远端视频并完成解码回调</td>
    <td>V3.5.0</td>
  </tr>
</table>

<h2 id="远端媒体事件">远端媒体事件</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onUserAudioStart "onUserAudioStart"</td>
    <td>远端用户开启音频回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onUserAudioStop "onUserAudioStop"</td>
    <td>远端用户停用音频回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onUserVideoStart "onUserVideoStart"</td>
    <td>远端用户开启视频回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onUserVideoStop "onUserVideoStop"</td>
    <td>远端用户停用视频回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onUserVideoProfileUpdate "onUserVideoProfileUpdate"</td>
    <td>远端用户视频配置更新回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onUserAudioMute "onUserAudioMute"</td>
    <td>远端用户是否静音回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onUserVideoMute "onUserVideoMute"</td>
    <td>远端用户是否禁视频流回调</td>
    <td>V3.5.0</td>
  </tr>
</table>

<h2 id="数据统计事件">数据统计事件</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcMediaStatsObserver::onRemoteAudioStats "onRemoteAudioStats"</td>
    <td>通话中远端音频流的统计信息回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcMediaStatsObserver::onRtcStats "onRtcStats"</td>
    <td>当前通话统计回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcMediaStatsObserver::onNetworkQuality "onNetworkQuality"</td>
    <td>通话中每个用户的网络上下行质量报告回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcMediaStatsObserver::onLocalAudioStats "onLocalAudioStats"</td>
    <td>本地音频流统计信息回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcMediaStatsObserver::onLocalVideoStats "onLocalVideoStats"</td>
    <td>本地视频流统计信息回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcMediaStatsObserver::onRemoteVideoStats "onRemoteVideoStats"</td>
    <td>通话中远端视频流的统计信息回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setStatsObserver "setStatsObserver"</td>
    <td>注册统计信息观测器</td>
    <td>V3.5.0</td>
  </tr>
</table>

<h2 id="屏幕共享">屏幕共享</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startScreenCaptureByDisplayId "startScreenCaptureByDisplayId"</td>
    <td>通过屏幕 ID 共享屏幕，该方法仅适用于 macOS</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startScreenCaptureByWindowId "startScreenCaptureByWindowId"</td>
    <td>通过窗口 ID 共享窗口</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::updateScreenCaptureRegion "updateScreenCaptureRegion"</td>
    <td>更新屏幕共享区域</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopScreenCapture "stopScreenCapture"</td>
    <td>停止屏幕共享</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startScreenCaptureByScreenRect "startScreenCaptureByScreenRect"</td>
    <td>通过指定区域共享屏幕</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pauseScreenCapture "pauseScreenCapture"</td>
    <td>暂停屏幕共享。</td>
    <td>V3.7.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::resumeScreenCapture "resumeScreenCapture"</td>
    <td>恢复屏幕共享。</td>
    <td>V3.7.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setupLocalSubStreamVideoCanvas "setupLocalSubStreamVideoCanvas"</td>
    <td>设置本端的辅流视频回放画布</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setupRemoteSubStreamVideoCanvas "setupRemoteSubStreamVideoCanvas"</td>
    <td>设置远端的辅流视频回放画布</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setLocalSubStreamRenderMode "setLocalSubStreamRenderMode"</td>
    <td>设置本端的辅流渲染缩放模式</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setRemoteSubSteamRenderMode "setRemoteSubSteamRenderMode"</td>
    <td>设置远端的屏幕共享辅流视频渲染缩放模式</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::subscribeRemoteVideoSubStream "subscribeRemoteVideoSubStream"</td>
    <td>订阅或取消订阅远端的屏幕共享辅流视频，订阅之后才能接收远端的辅流视频数据</td>
    <td>V3.9.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onUserSubStreamVideoStart "onUserSubStreamVideoStart"</td>
    <td>远端用户开启屏幕共享辅流通道的回调</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onUserSubStreamVideoStop "onUserSubStreamVideoStop"</td>
    <td>远端用户停止屏幕共享辅流通道的回调</td>
    <td>V3.9.0</td>
  </tr>
</table>

<h2 id="音乐文件播放及混音">音乐文件播放及混音</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startAudioMixing "startAudioMixing"</td>
    <td>开始播放音乐文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopAudioMixing "stopAudioMixing"</td>
    <td>停止播放音乐文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pauseAudioMixing "pauseAudioMixing"</td>
    <td>暂停播放音乐文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::resumeAudioMixing "resumeAudioMixing"</td>
    <td>恢复播放音乐文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setAudioMixingPlaybackVolume "setAudioMixingPlaybackVolume"</td>
    <td>设置音乐文件播放音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setAudioMixingSendVolume "setAudioMixingSendVolume"</td>
    <td>设置音乐文件的发送音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getAudioMixingPlaybackVolume "getAudioMixingPlaybackVolume"</td>
    <td>获取音乐文件的播放音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getAudioMixingSendVolume "getAudioMixingSendVolume"</td>
    <td>获取音乐文件的发送音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getAudioMixingDuration "getAudioMixingDuration"</td>
    <td>获取音乐文件的总长度</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getAudioMixingCurrentPosition "getAudioMixingCurrentPosition"</td>
    <td>获取音乐文件的播放进度</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setAudioMixingPosition "setAudioMixingPosition"</td>
    <td>设置音乐文件的播放进度</td>
    <td>V3.5.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAudioMixingStateChanged "onAudioMixingStateChanged"</td>
    <td>本地用户的音乐文件播放状态改变回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAudioMixingTimestampUpdate "onAudioMixingTimestampUpdate"</td>
    <td>本地用户的音乐文件播放进度回调</td>
    <td>V3.5.0</td>
  </tr>
</table>


<h2 id="音效文件播放管理">音效文件播放管理</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getEffectPlaybackVolume "getEffectPlaybackVolume"</td>
    <td>获取音效文件播放音量。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setEffectPlaybackVolume "setEffectPlaybackVolume"</td>
    <td>设置音效文件播放音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::playEffect "playEffect"</td>
    <td>播放指定音效文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopEffect "stopEffect"</td>
    <td>停止播放指定音效文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopAllEffects "stopAllEffects"</td>
    <td>停止播放所有音效文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pauseEffect "pauseEffect"</td>
    <td>暂停音效文件播放</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pauseAllEffects "pauseAllEffects"</td>
    <td>暂停所有音效文件播放</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::resumeEffect "resumeEffect"</td>
    <td>恢复播放指定音效文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::resumeAllEffects "resumeAllEffects"</td>
    <td>恢复播放所有音效文件</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setEffectSendVolume "setEffectSendVolume"</td>
    <td>调节音效文件发送音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getEffectSendVolume "getEffectSendVolume"</td>
    <td>获取音效文件发送音量</td>
    <td>V3.5.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAudioEffectFinished "onAudioEffectFinished"</td>
    <td>本地音效文件播放已结束回调</td>
    <td>V3.5.0</td>
  </tr>
</table>


<h2 id="变声与混响">变声与混响</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setAudioEffectPreset "setAudioEffectPreset"</td>
    <td>设置 SDK 预设的人声的变声音效。</td>
    <td>4.1.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setVoiceBeautifierPreset "setVoiceBeautifierPreset"</td>
    <td>设置 SDK 预设的美声效果。</td>
    <td>4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setLocalVoiceEqualization "setLocalVoiceEqualization"</td>
    <td>设置本地语音音效均衡，即自定义设置本地人声均衡波段的中心频率。</td>
    <td>4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setLocalVoicePitch "setLocalVoicePitch"</td>
    <td>设置本地语音音调。</td>
    <td>4.1.0</td>
  </tr>
</table>

<h2 id="媒体增强信息">媒体增强信息</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::sendSEIMsg "sendSEIMsg" </td>
    <td>通过主流通道发送媒体补充增强信息。	</td>
    <td>V4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::sendSEIMsg "sendSEIMsg" </td>
    <td>发送媒体补充增强信息。通过本接口可指定发送 SEI 时使用主流或辅流通道。	</td>
    <td>V4.0.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onRecvSEIMsg "onRecvSEIMsg" </td>
    <td>收到远端流的 SEI 内容回调。</td>
    <td>V4.0.0</td>
  </tr>
</table>

<h2 id="旁路推流">旁路推流</h2>

注意：该组方法仅适用于互动直播 2.0。

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::addLiveStreamTask "addLiveStreamTask"</td>
    <td>添加房间推流任务</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::updateLiveStreamTask "updateLiveStreamTask"</td>
    <td>更新修改房间推流任务</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::removeLiveStreamTask "removeLiveStreamTask"</td>
    <td>删除房间推流任务</td>
    <td>V3.5.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAddLiveStreamTask "onAddLiveStreamTask"</td>
    <td>通知添加直播任务结果</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onUpdateLiveStreamTask "onUpdateLiveStreamTask"</td>
    <td>通知更新直播任务结果</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onRemoveLiveStreamTask "onRemoveLiveStreamTask"</td>
    <td>通知删除直播任务结果</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onLiveStreamState "onLiveStreamState"</td>
    <td>通知直播推流状态</td>
    <td>V3.5.0</td>
  </tr>
 </table>

<h2 id="媒体补充增强信息">媒体补充增强信息</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::sendSEIMsg "sendSEIMsg"</td>
    <td>通过主流通道发送媒体补充增强信息。</td>
    <td>V4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::sendSEIMsg "sendSEIMsg"</td>
    <td>发送媒体补充增强信息。<p>通过本接口可指定发送 SEI 时使用主流或辅流通道。</td>
    <td>V4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onRecvSEIMsg "onRecvSEIMsg"</td>
    <td>收到远端流的媒体补充增强信息回调。</td>
    <td>V4.0.0</td>
  </tr>
</table>

 <h2 id="音量提示">音量提示</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::enableAudioVolumeIndication "enableAudioVolumeIndication"</td>
    <td>启用说话者音量提示</td>
    <td>V3.5.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onRemoteAudioVolumeIndication "onRemoteAudioVolumeIndication"</td>
    <td>提示房间内谁正在说话及说话者音量的回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication"</td>
    <td>提示房间内本地用户瞬时音量的回调</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="耳返">耳返</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::enableEarback "enableEarback"</td>
    <td>开启耳返功能</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setEarbackVolume "setEarbackVolume"</td>
    <td>设置耳返音量</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="视频大小流">视频大小流</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::enableDualStreamMode "enableDualStreamMode"</td>
    <td>设置是否开启视频大小流模式。</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="自定义音频采集与渲染">自定义音频采集与渲染</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setExternalAudioSource "setExternalAudioSource"</td>
    <td>启用外部自定义音频数据输入功能，并设置采集参数。</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pushExternalAudioFrame "pushExternalAudioFrame"</td>
    <td>将外部音频数据帧推送给内部引擎</td>
    <td>V3.9.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setExternalAudioRender "setExternalAudioRender"</td>
    <td>设置外部音频渲染</td>
    <td>V4.0.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pullExternalAudioFrame "pullExternalAudioFrame"</td>
    <td>拉取外部音频数据</td>
    <td>V4.0.0</td>
  </tr>
</table>

 <h2 id="自定义视频采集">自定义视频采集</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setExternalVideoSource "setExternalVideoSource"</td>
    <td>配置外部视频源</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::pushExternalVideoFrame "pushExternalVideoFrame"</td>
    <td>推送外部视频帧</td>
    <td>V3.5.0</td>
  </tr>
</table>

<h2 id="原始音频数据">原始音频数据</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setRecordingAudioFrameParameters "setRecordingAudioFrameParameters"</td>
    <td>设置录制的声音格式</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setPlaybackAudioFrameParameters "setPlaybackAudioFrameParameters"</td>
    <td>设置播放的声音格式</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setAudioFrameObserver "setAudioFrameObserver"</td>
    <td>注册语音观测器对象</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::setMixedAudioFrameParameters "setMixedAudioFrameParameters"</td>
    <td>设置采集和播放后的混合后的采样率。需要在加入房间之前调用该接口</td>
    <td>V3.5.0</td>
  </tr>
</table>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::INERtcAudioFrameObserver::onAudioFrameDidRecord "onAudioFrameDidRecord"</td>
    <td>采集音频数据回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::INERtcAudioFrameObserver::onAudioFrameWillPlayback "onAudioFrameWillPlayback"</td>
    <td>播放音频数据回调。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::INERtcAudioFrameObserver::onMixedAudioFrame "onMixedAudioFrame"</td>
    <td>音频采集与播放混合后数据帧回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::INERtcAudioFrameObserver::onPlaybackAudioFrameBeforeMixing "onPlaybackAudioFrameBeforeMixing"</td>
    <td>某一远端用户的原始音频帧回调</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="原始视频数据">原始视频数据</h2>

<table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onCaptureVideoFrame "onCaptureVideoFrame"</td>
    <td>采集视频数据回调。</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="音频设备管理">音频设备管理</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::enumerateRecordDevices "enumerateRecordDevices"</td>
    <td>枚举音频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::setRecordDevice "setRecordDevice"</td>
    <td>设置音频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::getRecordDevice "getRecordDevice"</td>
    <td>获取当前音频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::enumeratePlayoutDevices "enumeratePlayoutDevices"</td>
    <td>枚举音频播放设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::setPlayoutDevice "setPlayoutDevice"</td>
    <td>设备音频播放设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::getPlayoutDevice "getPlayoutDevice"</td>
    <td>获取当前音频播放设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::setRecordDeviceVolume "setRecordDeviceVolume"</td>
    <td>设置当前音频采集设备音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::getRecordDeviceVolume "getRecordDeviceVolume"</td>
    <td>获取当前音频采集设备音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::setPlayoutDeviceVolume "setPlayoutDeviceVolume"</td>
    <td>设置当前音频播放设备音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::getPlayoutDeviceVolume "getPlayoutDeviceVolume"</td>
    <td>获取当前音频播放设别音量</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::setPlayoutDeviceMute "setPlayoutDeviceMute"</td>
    <td>设置当前播放设备静音状态</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::getPlayoutDeviceMute "getPlayoutDeviceMute"</td>
    <td>获取当前播放设备静音状态</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::setRecordDeviceMute "setRecordDeviceMute"</td>
    <td>设置当前采集设备静音状态</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::getRecordDeviceMute "getRecordDeviceMute"</td>
    <td>获取当前采集设备静音状态</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::startRecordDeviceTest "startRecordDeviceTest"</td>
    <td>开始测试音频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::stopRecordDeviceTest "stopRecordDeviceTest"</td>
    <td>停止测试音频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::startPlayoutDeviceTest "startPlayoutDeviceTest"</td>
    <td>开始测试音频播放设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::stopPlayoutDeviceTest "stopPlayoutDeviceTest"</td>
    <td>停止测试音频播放设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::startAudioDeviceLoopbackTest "startAudioDeviceLoopbackTest"</td>
    <td>开始音频采集播放设备回路测试</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IAudioDeviceManager::stopAudioDeviceLoopbackTest "stopAudioDeviceLoopbackTest"</td>
    <td>停止音频采集播放设备回路测试</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAudioHowling "onAudioHowling"</td>
    <td>检测到啸叫回调。</td>
    <td>V3.9.0</td>
  </tr>
</table>

 <h2 id="视频设备管理">视频设备管理</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IVideoDeviceManager::enumerateCaptureDevices "enumerateCaptureDevices"</td>
    <td>枚举视频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IVideoDeviceManager::setDevice "setDevice"</td>
    <td>设置视频采集设备</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IVideoDeviceManager::getDevice "getDevice"</td>
    <td>获取当前视频采集设备</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="设备管理事件">设备管理事件</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAudioDeviceStateChanged "onAudioDeviceStateChanged"</td>
    <td>音频设备状态更改回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onAudioDefaultDeviceChanged "onAudioDefaultDeviceChanged"</td>
    <td>音频默认设备更改回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandlerEx::onVideoDeviceStateChanged "onVideoDeviceStateChanged"</td>
    <td>视频设备状态更改回调</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <h2 id="故障排查">故障排查</h2>

<table>
  <tr>
    <th width=400><b>方法</b></th>
    <th width=600><b>功能</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::startAudioDump "startAudioDump"</td>
    <td>开始记录音频 dump 音频 dump 可用于分析音频问题</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::stopAudioDump "stopAudioDump"</td>
    <td>结束记录音频 dump</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::getErrorDescription "getErrorDescription"</td>
    <td>获取错误描述。</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEx::uploadSdkInfo "uploadSdkInfo"</td>
    <td>上传SDK日志信息</td>
    <td>V3.5.0</td>
  </tr>
</table>

 <table>
  <tr>
    <th width=400><b>事件</b></th>
    <th width=600><b>描述</b></th>
    <th width=200><b>起始版本</b></th>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onError "onError"</td>
    <td>发生错误回调</td>
    <td>V3.5.0</td>
  </tr>
  <tr>
    <td> \ref nertc::IRtcEngineEventHandler::onWarning "onWarning"</td>
    <td>发生警告回调</td>
    <td>V3.5.0</td>
  </tr>
</table>

*/

/**
 @defgroup createNERtcEngine Create an NERTC Engine
 */
