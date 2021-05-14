/** @file nertc_audio_device_manager.h
* @brief NERTC SDK音频设备管理接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/

#ifndef NERTC_AUDIO_DEVICE_MANAGER_H
#define NERTC_AUDIO_DEVICE_MANAGER_H

#include "nertc_base_types.h"
#include "nertc_engine_defines.h"
#include "nertc_device_collection.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** 音频设备管理方法。

 IAudioDeviceManager 接口类提供用于管理音频设备的相关接口。可通过实例化 IAudioDeviceManager 类来获取 IAudioDeviceManager 接口。
 */
class IAudioDeviceManager
{
protected:
    virtual ~IAudioDeviceManager() {}

public:
    /** 获取系统中所有的音频采集设备列表。

     该方法返回一个 IDeviceCollection 对象，包含系统中所有的音频采集设备。通过IDeviceCollection 对象，App 可以枚举音频采集设备。

     @note 在使用结束后，App 需调用  \ref IDeviceCollection::destroy "destroy"  方法销毁返回的对象。
     @return
     - 方法调用成功：一个 IDeviceCollection 对象，包含所有的音频采集设备。
     - 方法调用失败：NULL 。
     */
    virtual IDeviceCollection * enumerateRecordDevices() = 0;
    /** 指定音频采集设备。

     @param device_id 音频采集设备的设备 ID。可通过 enumerateRecordDevices 获取。插拔设备不会影响 deviceId。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setRecordDevice(const char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** 获取当前使用的音频采集设备 ID。

     @param device_id 音频采集设备的设备 ID。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getRecordDevice(char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** 获取系统中所有的播放设备列表。
     
     该方法返回一个 IDeviceCollection 对象，包含系统中所有的播放设备。通过 IDeviceCollection 对象，App 可以枚举播放设备。在使用结束后，App 需调用 destroy 方法销毁返回的对象。

     @note 程序必须调用 \ref IDeviceCollection::destroy "destroy" 方法销毁返回的值。

     @return
     - 方法调用成功：一个 IDeviceCollection 对象，包含所有的音频播放设备。
     - 方法调用失败：NULL 。
     */
    virtual IDeviceCollection * enumeratePlayoutDevices() = 0;
    /** 指定播放设备。

     @param device_id 音频播放设备的设备 ID。可以通过 \ref IAudioDeviceManager::enumeratePlayoutDevices "enumeratePlayoutDevices" 获取。插拔设备不会影响 deviceId。

     @return
     - 方法调用成功：一个 IDeviceCollection 对象，包含所有的音频播放设备。
     - 方法调用失败：NULL 。
     */
    virtual int setPlayoutDevice(const char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** 获取当前使用的音频播放设备 ID。

     @param device_id 音频播放设备的设备 ID。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getPlayoutDevice(char device_id[kNERtcMaxDeviceIDLength]) = 0;
        
    /** 设置音频采集设备音量。

     @param volume 音频采集设备音量。取值范围为 0~255。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setRecordDeviceVolume(uint32_t volume) = 0;
    /** 获取音频采集设备音量。

     @param volume 音频采集设备音量。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getRecordDeviceVolume(uint32_t* volume) = 0;
    /** 设置音频播放设备音量。

     @param volume 音频播放设备音量。取值范围为 0~255。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setPlayoutDeviceVolume(uint32_t volume) = 0;
    /** 获取音频播放设备音量。

     @param volume 音频播放设备音量。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getPlayoutDeviceVolume(uint32_t* volume) = 0;

    /** 静音或取消静音音频播放设备。

     @param mute 是否静音音频播放设备。
     - true：静音音频播放设备。
     - false：取消静音音频播放设备。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setPlayoutDeviceMute(bool mute) = 0;

    /** 获取音频播放设备静音状态。

     @param mute 音频播放设备静音状态。
     - true：静音状态。
     - false：非静音状态。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getPlayoutDeviceMute(bool *mute) = 0;
    /** 静音或取消静音音频采集设备。

     @param mute 是否静音音频采集设备。
     - true：静音音频采集设备。
     - false：取消静音音频采集设备。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setRecordDeviceMute(bool mute) = 0;

    /** 获取音频采集设备静音状态。

     @param mute 音频采集设备静音状态。
     - true: 静音状态。
     - false: 非静音状态。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getRecordDeviceMute(bool *mute) = 0;

    /** 调节采集信号音量。

    @note 该方法仅调节应用程序中的采集信号音量，不修改设备音量。如果需要修改设备音量，请查看设备管理相关接口。

    @param[in] volume 采集录音音量，取值范围为 [0, 400]。其中：
    - 0: 静音；
    - 100: 原始音量；
    - 400: 最大可为原始音量的 4 倍（自带溢出保护）。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int adjustRecordingSignalVolume(uint32_t volume) = 0;

    /** 调节本地播放音量。
     @note 该方法仅调节应用程序中音量，不修改设备音量。如果需要修改设备音量，请查看设备管理相关接口。

    @param[in] volume 播放音量。取值范围为 [0, 400]。其中：
    - 0:  静音；
    - 100: 原始音量；
    - 400: 最大可为原始音量的 4 倍（自带溢出保护）。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int adjustPlaybackSignalVolume(uint32_t volume) = 0;

    /** 启动音频采集设备测试。
     
     该方法测试音频采集设备是否能正常工作。

     调用该方法后，SDK 会按设置的时间间隔触发 \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication"  回调， 报告采集设备的音量信息。

     @note
     - 该方法需在加入房间前调用。
     - 音频采集设备测试会在通话开始后自动结束，您也可以手动调用 stopRecordDeviceTest 停止音频采集设备测试。

     @param indication_interval SDK 返回 \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication"  回调的时间间隔，单位为毫秒。

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int startRecordDeviceTest(uint64_t indication_interval) = 0;
    /** 停止音频采集设备测试。
     
     该方法停止音频采集设备测试。
     @note
     - 该方法需在加入房间前调用。
     - 音频采集设备测试会在通话开始后自动结束，您也可以手动调用 stopRecordDeviceTest 停止音频采集设备测试。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int stopRecordDeviceTest() = 0;
    /** 启动音频播放设备测试。
     
     该方法测试音频播放设备是否能正常工作。启动测试后，SDK 播放指定的音频文件，测试者如果能听到声音，说明播放设备能正常工作。

     调用该方法后，SDK 会每隔 100 ms 触发一次 \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication" 回调，报告播放设备的音量信息。

     @note
     - 该方法需在加入房间前调用。
     - 音频播放设备测试会在通话开始后自动结束，您也可以手动调用 stopPlayoutDeviceTest 停止音频播放设备测试。
     - 支持文件格式包括 wav、mp3、aac。

     @param test_audio_file_path 音频文件的绝对路径，路径字符串使用 UTF-8 编码格式。

     @return
     - 0: 成功，并且可以听到所设置文件的声音。
     - 其他：失败。
     */
    virtual int startPlayoutDeviceTest(const char *test_audio_file_path) = 0;
    /** 停止播放设备测试。

     @note
     - 该方法需在加入房间前调用。
     - 播放设备测试会在通话开始后自动结束，您也可以手动调用 stopPlayoutDeviceTest 停止播放设备测试。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int stopPlayoutDeviceTest() = 0;
    /** 开始音频设备回路测试。
     
     该方法测试音频采集和播放设备是否能正常工作。一旦测试开始，音频采集设备会采集本地音频，然后使用音频播放设备播放出来。 SDK 会按设置的时间间隔触发 \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication"  回调， 报告音量信息。
     @note
     - 该方法需在加入房间前调用。
     - 音频设备回路测试会在通话开始后自动结束，您也可以手动调用 stopAudioDeviceLoopbackTest 停止音频设备回路测试。
     - 该方法仅在本地进行音频设备测试，不涉及网络连接。

     @param indication_interval SDK 返回  \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication" 回调的 时间间隔，单位为毫秒。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int startAudioDeviceLoopbackTest(uint64_t indication_interval) = 0;
    /** 停止音频设备回路测试。
     @note
     - 该方法需在加入房间前调用。
     - 音频设备回路测试会在通话开始后自动结束，您也可以手动调用 stopAudioDeviceLoopbackTest 停止音频设备回路测试。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int stopAudioDeviceLoopbackTest() = 0;
};
} //namespace nertc

#endif
