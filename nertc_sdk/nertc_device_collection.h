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
/** 设备相关方法。

 此接口类获取设备相关的信息。
*/
class IDeviceCollection
{
protected:
    virtual ~IDeviceCollection(){}

public:
    /** 获取设备数量。

     @note 调用此方法之前，必须调用 \ref IAudioDeviceManager::enumeratePlayoutDevices "enumeratePlayoutDevices" 或 \ref IAudioDeviceManager::enumerateRecordDevices "enumerateRecordDevices" 方法获取播放或采集设备数量。

     @return 采集或播放设备数量。
     */
    virtual uint16_t getCount() = 0;

    /** 获取指定 index 的设备信息。

     @param index  指定想查询的设备信息。必须小于 \ref IDeviceCollection::getCount "getCount"返回的值。
     @param device_name  设备名称。
     @param device_id  设备 ID。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getDevice(uint16_t index, char device_name[kNERtcMaxDeviceNameLength], char device_id[kNERtcMaxDeviceIDLength]) = 0;

    /** 检索有关索引设备的指定信息。
     @note 可返回设备的链接方式，和SDK判定的疑似不可用状态。
     @param index  指定想查询的设备信息。
     @param device_info 设备信息，详细信息请参考 \ref NERtcDeviceInfo "NERtcDeviceInfo"。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int getDeviceInfo(uint16_t index, NERtcDeviceInfo* device_info) = 0;
   
    /** 释放所有 IDeviceCollection 资源。
    */
    virtual void destroy() = 0;
};
} //namespace nertc

#endif
