/** @file nertc_video_device_manager.h
* @brief NERTC SDK视频设备管理接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/

#ifndef NERTC_VIDEO_DEVICE_MANAGER_H
#define NERTC_VIDEO_DEVICE_MANAGER_H

#include "nertc_base_types.h"
#include "nertc_engine_defines.h"
#include "nertc_device_collection.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** 视频设备管理方法。

 IVideoDeviceManager 接口类提供用于管理视频设备的相关接口。 可通过实例化 IVideoDeviceManager 类来获取 IVideoDeviceManager 接口。
*/
class IVideoDeviceManager
{
protected:
    virtual ~IVideoDeviceManager() {}

public:
    /** 获取系统中所有的视频采集设备列表。
     
     该方法返回一个 IDeviceCollection 对象，包含系统中所有的音频采集设备。通过IDeviceCollection 对象，App 可以枚举视频采集设备。

     @note
     在使用结束后，App 需调用 destroy 方法销毁返回的对象。

     @return
     - 方法调用成功：一个 IDeviceCollection 对象，包含所有的视频采集设备。
     - 方法调用失败：NULL 。
     */
    virtual IDeviceCollection * enumerateCaptureDevices() = 0;
    /** 指定视频采集设备。

     @param device_id 视频采集设备的设备 ID。可以通过 enumerateCaptureDevices 获取。

     @return
     - 0：方法调用成功；
     - 其他： 方法调用失败。
     */
    virtual int setDevice(const char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** 获取当前使用的视频采集设备信息。

     @param device_id 视频采集设备的设备 ID。
     @return
     - 0：方法调用成功；
     - 其他： 方法调用失败。
     */
    virtual int getDevice(char device_id[kNERtcMaxDeviceIDLength]) = 0;
};
} //namespace nertc

#endif
