/** @file nertc_device_collection.h
* @brief NERTC SDK device collection接口头文件。
* NERtc SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/

#ifndef NERTC_DEVICE_COLLECTION_H
#define NERTC_DEVICE_COLLECTION_H

#include "nertc_base_types.h"
#include "nertc_engine_defines.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** device collection methods.

The IDeviceCollection interface class retrieves device-related information.
*/
class IDeviceCollection
{
protected:
    virtual ~IDeviceCollection(){}

public:
    /** Retrieves the total number of  playback or  recording devices.

     @note You must first call the \ref IAudioDeviceManager::enumeratePlayoutDevices "enumeratePlayoutDevices" or \ref IAudioDeviceManager::enumerateRecordDevices "enumerateRecordDevices" method before calling this method to return the number of   playback or  recording devices.

     @return Number of  playback or  recording devices.
     */
    virtual uint16_t getCount() = 0;

    /** Retrieves a specified piece of information about an indexed  device.

     @param index The specified index that must be less than the return value of \ref IDeviceCollection::getCount "getCount".
     @param device_name Pointer to the  device name.
     @param device_id Pointer to the  device ID.
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getDevice(uint16_t index, char device_name[kNERtcMaxDeviceNameLength], char device_id[kNERtcMaxDeviceIDLength]) = 0;

    /** 检索有关索引设备的指定信息
     @note 可返回设备的链接方式，和SDK判定的疑似不可用状态。
     @param index The specified index that must be less than the return value of \ref IDeviceCollection::getCount "getCount".
     @param device_info Pointer to the NERtcDeviceInfo.
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getDeviceInfo(uint16_t index, NERtcDeviceInfo* device_info) = 0;
   
    /** Releases all IDeviceCollection resources.
    */
    virtual void destroy() = 0;
};
} //namespace nertc

#endif
