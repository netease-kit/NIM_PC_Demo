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
    /** Enumerates the audio recording devices.

     This method returns an IDeviceCollection object that includes all audio recording devices in the system. With the IDeviceCollection object, the application can enumerate the audio recording devices.

     @note The application needs to call the \ref IDeviceCollection::destroy "destroy" method to destroy the returned object after using it.

     @return
     - Returns an IDeviceCollection object that includes all audio recording devices in the system: Success.
     - Returns NULL: Failure.
     */
    virtual IDeviceCollection * enumerateRecordDevices() = 0;
    /** Sets the audio recording device using the device ID.

     @param device_id Device ID of the audio recording device, retrieved by calling the \ref IAudioDeviceManager::enumerateRecordDevices "enumerateRecordDevices" method.

     @note Plugging or unplugging the audio device does not change the device ID.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setRecordDevice(const char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** Retrieves the audio recording device associated with the device ID.

     @param device_id Pointer to the device ID of the audio recording device.
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getRecordDevice(char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** Enumerates the audio playout devices.

     This method returns an IDeviceCollection object that includes all audio playout devices in the system. With the IDeviceCollection object, the application can enumerate the audio playout devices.

     @note The application must call the \ref IDeviceCollection::destroy "destroy" method to destroy the returned object after using it.

     @return
     - Returns an IDeviceCollection object that includes all audio playout devices in the system: Success.
     - Returns NULL: Failure.
     */
    virtual IDeviceCollection * enumeratePlayoutDevices() = 0;
    /** Sets the audio playout device using the device ID.

     @note Plugging or unplugging the audio device does not change the device ID.plus:kNERtcAudioDeviceAutoID

     @param device_id Device ID of the audio playout device, retrieved by calling the \ref IAudioDeviceManager::enumeratePlayoutDevices "enumeratePlayoutDevices" method.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setPlayoutDevice(const char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** Retrieves the audio playout device associated with the device ID.

     @param device_id Pointer to the ID of the audio playout device.plus:kNERtcAudioDeviceAutoID
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getPlayoutDevice(char device_id[kNERtcMaxDeviceIDLength]) = 0;
        
    /** Sets the volume of the microphone.

     @param volume Sets the volume of the microphone. The value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setRecordDeviceVolume(uint32_t volume) = 0;
    /** Retrieves the volume of the microphone.

     @param volume Pointer to the microphone volume. The volume value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getRecordDeviceVolume(uint32_t* volume) = 0;
    /** Sets the volume of the audio playout device.

     @param volume Sets the volume of the audio playout device. The value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setPlayoutDeviceVolume(uint32_t volume) = 0;
    /** Retrieves the volume of the audio playout device.

     @param volume Pointer to the audio playout device volume. The volume value ranges between 0 (lowest volume) and 255 (highest volume).
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getPlayoutDeviceVolume(uint32_t* volume) = 0;

    /** Mutes the audio playout device.

     @param mute
     - true: Mutes the device.
     - false: Unmutes the device.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setPlayoutDeviceMute(bool mute) = 0;

    /** Retrieves the mute state of the playout device.

     @param mute Whether the playout device is muted/unmuted.
     - true: The playout device is muted.
     - false: The playout device is unmuted.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getPlayoutDeviceMute(bool *mute) = 0;
    /** Mutes/Unmutes the microphone.

     @param mute Whether to mute/unmute the microphone.
     - true: Mutes the microphone.
     - false: Unmutes the microphone.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setRecordDeviceMute(bool mute) = 0;

    /** Retrieves the microphone’s mute status.

     @param mute Whether the microphone is muted/unmuted.
     - true: The microphone is muted.
     - false: The microphone is unmuted.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getRecordDeviceMute(bool *mute) = 0;

    /** 调节录音音量。

    @param[in] volume 录音信号音量可在 0 ~ 400 范围内进行调节:
    - 0: 静音；
    - 100: 原始音量；
    - 400: 最大可为原始音量的 4 倍（自带溢出保护）。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int adjustRecordingSignalVolume(uint32_t volume) = 0;

    /** 调节播放音量。

    @param[in] volume 播放音量可在 0 ~ 400 范围内进行调节:
    - 0:  静音；
    - 100: 原始音量；
    - 400: 最大可为原始音量的 4 倍（自带溢出保护）。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int adjustPlaybackSignalVolume(uint32_t volume) = 0;

    /** Starts the microphone test.

     This method tests whether the microphone works properly. Once the test starts, the SDK uses the \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication" callback function to notify the application with the volume information.

     @param indication_interval Interval period (ms) of the \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication" callback function cycle.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int startRecordDeviceTest(uint64_t indication_interval) = 0;
    /** Stops the microphone test.

     This method stops the microphone test. To stop the test, call this method after calling the IAudioDeviceManager::startRecordDeviceTest method.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int stopRecordDeviceTest() = 0;
    /** Starts the audio playout device test.

     This method tests if the playout device works properly. In the test, the SDK plays an audio file specified by the user. If the user can hear the audio, the playout device works properly.

     @param test_audio_file_path File path of the audio file for the test, which is in the UTF-8 absolute path:
     - Supported file format: wav, mp3, and aac

     @return
     - 0: Success and you can hear the sound of the file you set.
     - other: Failure.
     */
    virtual int startPlayoutDeviceTest(const char *test_audio_file_path) = 0;
    /** Stops the audio playout device test.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int stopPlayoutDeviceTest() = 0;
    /** Starts the audio device loopback test.

     This method tests whether the audio device loopback works properly. Once the test starts, the SDK uses the \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication" callback function to notify the application with the volume information.

     @param indication_interval Interval period (ms) of the \ref IRtcEngineEventHandlerEx::onLocalAudioVolumeIndication "onLocalAudioVolumeIndication" callback function cycle.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int startAudioDeviceLoopbackTest(uint64_t indication_interval) = 0;
    /** Stops the audio device loopback test.

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int stopAudioDeviceLoopbackTest() = 0;
};
} //namespace nertc

#endif
