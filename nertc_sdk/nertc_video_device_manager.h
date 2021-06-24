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
    /** Enumerates the video devices.

     This method returns an IDeviceCollection object including all video devices in the system. With the IDeviceCollection object, the application can enumerate the video devices. 
     The application must call the \ref IDeviceCollection::destroy "destroy" method to destroy the returned object after using it.

     @return
     - An IDeviceCollection object including all video devices in the system: Success.
     - NULL: Failure.
     */
    virtual IDeviceCollection * enumerateCaptureDevices() = 0;
    /** Sets a device with the device ID.

     @param device_id Pointer to the video-capture device ID. Call the \ref IVideoDeviceManager::enumerateCaptureDevices "enumerateCaptureDevices" method to retrieve it.

     @note Plugging or unplugging the device does not change the device ID.plus:kNERtcExternalVideoDeviceID

     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int setDevice(const char device_id[kNERtcMaxDeviceIDLength]) = 0;
    /** Retrieves the video-capture device that is in use.

     @param device_id Pointer to the video-capture device ID.
     @return
     - 0: Success.
     - other: Failure.
     */
    virtual int getDevice(char device_id[kNERtcMaxDeviceIDLength]) = 0;
};
} //namespace nertc

#endif
