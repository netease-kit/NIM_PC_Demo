/** @file nertc_engine_media_stats_observer.h
* @brief NERTC SDK回调扩展接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/

#ifndef NERTC_ENGINE_MEDIA_STATS_OBSERVERA_H
#define NERTC_ENGINE_MEDIA_STATS_OBSERVERA_H

#include "nertc_base_types.h"
#include "nertc_engine_defines.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** IRtcMediaStatsObserver 回调扩展接口类用于 SDK 向 App 上报统计信息。

 接口类的所有方法都有缺省（空）实现，App 可以根据需要只继承关心的事件。在回调方法中，App 不应该做耗时或者调用可能会引起阻塞的 API（如开启音频或视频等），否则可能影响 SDK 的运行。
 */
class IRtcMediaStatsObserver
{
public:
    virtual ~IRtcMediaStatsObserver() {}

    /** 当前通话统计回调。

    SDK 定期向 App 报告当前通话的统计信息，每 2 秒触发一次。

     @param stats NERTC 引擎统计数据: NERtcStats
     */
    virtual void onRtcStats(const NERtcStats &stats) {
        (void)stats;
    }    

    /** 本地音频流统计信息回调。

    该回调描述本地设备发送音频流的统计信息，每 2 秒触发一次。

     @param stats 本地音频流统计信息。详见 NERtcAudioSendStats.
     */
    virtual void onLocalAudioStats(const NERtcAudioSendStats &stats) {
        (void)stats;
    }

    /** 通话中远端音频流的统计信息回调。

     该回调描述远端用户在通话中端到端的音频流统计信息，每 2 秒触发一次。

     @param stats 每个远端用户音频统计信息的数组。详见 NERtcAudioRecvStats.
     @param user_count stats 数组的大小。
     */
    virtual void onRemoteAudioStats(const NERtcAudioRecvStats *stats, unsigned int user_count) {
        (void)stats;
        (void)user_count;
    }

    /** 本地视频流统计信息回调。

    该回调描述本地设备发送视频流的统计信息，每 2 秒触发一次。

     @param stats 本地视频流统计信息。详见 NERtcVideoSendStats.
     */
    virtual void onLocalVideoStats(const NERtcVideoSendStats &stats) {
        (void)stats;
    }

    /** 通话中远端视频流的统计信息回调。

     该回调描述远端用户在通话中端到端的视频流统计信息，每 2 秒触发一次。

     @param stats 每个远端用户视频统计信息的数组。详见 NERtcVideoRecvStats.
     @param user_count stats 数组的大小。
     */
    virtual void onRemoteVideoStats(const NERtcVideoRecvStats *stats, unsigned int user_count) {
        (void)stats;
        (void)user_count;
    }

    /** 通话中每个用户的网络上下行质量报告回调。

     该回调描述每个用户在通话中的网络状态，每 2 秒触发一次，只上报状态有变更的成员。

     @param infos 每个用户 ID 和网络上下行质量信息的数组: NERtcNetworkQualityInfo
     @param user_count infos 数组的大小，即用户数。
     */
    virtual void onNetworkQuality(const NERtcNetworkQualityInfo *infos, unsigned int user_count) {
        (void)infos;
        (void)user_count;
    }
};
} //namespace nertc

#endif
