/** @file nertc_warn_code.h
  * @brief NERtc SDK的警告码定义
  * @copyright (c) 2015-2019, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2018/11/23
  */

#ifndef NERTC_WARN_CODE_H
#define NERTC_WARN_CODE_H

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** 警告代码。

警告代码意味着 SDK 遇到问题，但有可能恢复，警告代码仅起告知作用，一般情况下应用程序可以忽略警告代码。
*/
typedef enum
{
    kNERtcWarnInvalidView               = 100,      /**< 指定的 view 无效，使用视频功能时需要指定 view，如果 view 尚未指定，则返回该警告。*/
    kNERtcWarnInitVideo                 = 101,      /**< 初始化视频功能失败。有可能是因视频资源被占用导致的。用户无法看到视频画面，但不影响语音通信。*/
    kNERtcWarnPending                   = 102,      /**< 请求处于待定状态。一般是由于某个模块还没准备好，请求被延迟处理。*/
    //ADM 模块
    kNERtcWarnADMRuntimePlayoutWarning  = 1001,     /**< 音频设备模块：运行时播放设备出现警告。*/
    kNERtcWarnADMRuntimeRecordingWarning = 1002,    /**< 音频设备模块：运行时录音设备出现警告。*/
    kNERtcWarnADMRecordAudioSilence     = 1003,     /**< 音频设备模块：没有采集到有效的声音数据。*/
    kNERtcWarnADMPlayoutMalfunction     = 1004,     /**< 音频设备模块：播放设备故障。*/
    kNERtcWarnADMRecordMalfunction      = 1005,     /**< 音频设备模块：录音设备故障。*/
    kNERtcWarnADMRecordAudioLowLevel    = 1006,     /**< 音频设备模块：录到的声音太低。*/
    kNERtcWarnADMPlayoutAudioLowLevel   = 1007,     /**< 音频设备模块：播放的声音太低。*/
    kNERtcWarnAPMHowling                = 1008,     /**< 音频设备模块：录音声音监测到啸叫。*/
    kNERtcWarnADMGlitchState            = 1009,     /**< 音频设备模块：音频播放会卡顿。*/
    kNERtcWarnADMImproperSettings       = 1010,     /**< 音频设备模块：音频底层设置被修改。*/
    //Windows 平台下的警告码定义
    kNERtcWarnADMWinCoreNoDataReadyEvent        = 2000,     /**< 音频设备模块：音频驱动异常。解决方案：禁用并重新使能音频设备，或者重启机器，或者更新声卡驱动。*/
    kNERtcWarnADMWinCoreNoRecordingDevice       = 2001,     /**< 音频设备模块：无可用音频采集设备。*/
    kNERtcWarnADMWinCoreNoPlayoutDevice         = 2002,     /**< 音频设备模块：无可用音频播放设备。解决方案：插入音频设备。*/
    kNERtcWarnADMWinCoreImproperCaptureRelease  = 2003,     /**< 音频设备模块：音频采集释放有误。解决方案：禁用并重新使能音频设备，或者重启机器，或者更新声卡驱动。*/
    //macOS 平台下的警告码定义。30000 开始

    //iOS 平台下的警告码定义。40000 开始

    //Android 平台下的警告码定义。50000 开始

} NERtcWarnCode;

} // namespace nertc

#endif