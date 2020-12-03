/** @file nertc_engine_defines.h
  * @brief NERtc SDK的一些定义
  * @copyright (c) 2015-2019, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2018/11/23
  */

#ifndef NERTC_ENGINE_DEFINES_H
#define NERTC_ENGINE_DEFINES_H

#include "nertc_base_types.h"

#define kNERtcMaxPlaneCount         4       /**< Video max plane count。*/
#define kNERtcMaxDeviceIDLength     256     /**< 设备ID最大长度。*/
#define kNERtcMaxDeviceNameLength   256     /**< 设备名最大长度。*/
#define kNERtcMaxURILength          256     /**< URI最大长度。*/
#define kNERtcMaxTaskIDLength       64      /**< 任务ID最大长度。*/
#define kNERtcMaxBuffLength         1024    /**< 字符串缓存区最大长度。*/
#define kNERtcExternalVideoDeviceID     "nertc-video-external-device"   /**< 外部视频输入源设备ID，开启外部输入之后，需要通过setDevice设置此设备ID。*/
#define kNERtcAudioDeviceAutoID         "nertc-audio-device-auto"       /**< 音频设备自动选择ID，设置该ID为设备时，SDK会根据设备插拔系统设置等自动选择合适音频设备。*/

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{

typedef uint64_t uid_t;	/**< 64位无符号整数。建议设置范围：1到 2<sup>63</sup>-1，并保证唯一性。*/
typedef uint64_t channel_id_t;	/**< 64位无符号整数。建议设置范围：1到 2<sup>63</sup>-1，并保证唯一性。*/

/** 接口ID类型。*/
typedef enum
{
    kNERtcIIDAudioDeviceManager = 1,    /**< 获取音频设备管理器的接口ID */
    kNERtcIIDVideoDeviceManager = 2,    /**< 获取视频设备管理器的接口ID */
} NERtcInterfaceIdType;

/** 场景模式 */
typedef enum 
{
    kNERtcChannelProfileCommunication       = 0,    /**< 通话场景 */
    kNERtcChannelProfileLiveBroadcasting    = 1,    /**< 直播推流场景 */
} NERtcChannelProfileType;

/** 连麦方式。*/
typedef enum
{
    kNERtcLayoutFloatingRightVertical = 0,
    kNERtcLayoutFloatingLeftVertical,
    kNERtcLayoutSplitScreen,
    kNERtcLayoutSplitScreenScaling,
    kNERtcLayoutCustom,
    kNERtcLayoutAudioOnly,
} NERtcLiveStreamLayoutMode;

/** 直播推流任务的配置项。*/
struct NERtcLiveStreamTaskOption
{
    char task_id[kNERtcMaxTaskIDLength];        /**< 推流任务ID，为推流任务的唯一标识，用于过程中增删任务操作 */
    char stream_url[kNERtcMaxURILength];        /**< 直播推流地址 */
    bool server_record_enabled;                 /**< 服务器录制功能是否开启，默认值为 false */
    NERtcLiveStreamLayoutMode layout_mode;      /**< 连麦方式，默认值为 kNERtcLayoutFloatingRightVertical */
    uid_t main_picture_account_id;              /**< 指定大画面uid（选填） */
    char layout_parameters[kNERtcMaxBuffLength];/**< 自定义布局参数（选填），JSON 字符串格式, 只有当layout_mode为 kNERtcLayoutCustom 或 kNERtcLayoutAudioOnly时才需要设置。设置详见: http://netease.im/blog/12-3 */
};

/** 直播推流模式 */
typedef enum
{
    kNERtcLsModeVideo  = 0, /**< 推流带视频 */
    kNERtcLsModeAudio  = 1, /**< 推流纯音频 */
} NERtcLiveStreamMode;

/** 直播推流视频裁剪模式 */
typedef enum
{
    kNERtcLsModeVideoScaleFit      = 0, /**< 0: 视频尺寸等比缩放。优先保证视频内容全部显示。因视频尺寸与显示视窗尺寸不一致造成的视窗未被填满的区域填充背景色。*/
    kNERtcLsModeVideoScaleCropFill = 1, /**< 1: 视频尺寸等比缩放。优先保证视窗被填满。因视频尺寸与显示视窗尺寸不一致而多出的视频将被截掉。*/
} NERtcLiveStreamVideoScaleMode;

/** 直播成员布局 */
struct NERtcLiveStreamUserTranscoding {
    uid_t uid;                      /**< 用户id */
    bool video_push;                /**< 是否推送该用户视频流，kNERtcLsModeAudio时无效 */
    NERtcLiveStreamVideoScaleMode adaption;   /**< 视频流裁剪模式 */
    int x;                          /**< 画面离主画面左边距 */
    int y;                          /**< 画面离主画面上边距 */
    int width;                      /**< 画面在主画面的显示宽度，画面右边超出主画面会失败 */
    int height;                     /**< 画面在主画面的显示高度，画面底边超出主画面会失败 */
    bool audio_push;                /**< 是否推送该用户音频流 */
};

/** 图片布局 */
struct NERtcLiveStreamImageInfo {
    char url[kNERtcMaxURILength];   /**< 图片地址 */
    int x;                          /**< 画面离主画面左边距 */
    int y;                          /**< 画面离主画面上边距 */
    int width;                      /**< 画面在主画面的显示宽度，画面右边超出主画面会失败 */
    int height;                     /**< 画面在主画面的显示高度，画面底边超出主画面会失败 */
};

/** 直播布局 */
struct NERtcLiveStreamLayout {
    int width;                              /**< 视频推流宽度 */
    int height;                             /**< 视频推流高度 */
    unsigned int background_color;          /**< 视频推流背景色，(R & 0xff) << 16 | (G & 0xff) << 8 | (B & 0xff) */
    unsigned int user_count;                /**< 成员布局个数 */
    NERtcLiveStreamUserTranscoding *users;  /**< 成员布局数组 */
    NERtcLiveStreamImageInfo* bg_image;     /**< 背景图信息 */
};

/** 直播推流任务的配置项。*/
struct NERtcLiveStreamTaskInfo
{
    char task_id[kNERtcMaxTaskIDLength];    /**< 推流任务ID，为推流任务的唯一标识，用于过程中增删任务操作 */
    char stream_url[kNERtcMaxURILength];    /**< 直播推流地址 */
    bool server_record_enabled;             /**< 服务器录制功能是否开启 */
    NERtcLiveStreamMode ls_mode;            /**< 直播推流模式，NERtcLiveStreamMode */
    NERtcLiveStreamLayout layout;           /**< 视频布局 */
};

/** 直播推流状态。*/
typedef enum {
    kNERtcLsStatePushing        = 505,  /**< 推流中 */
    kNERtcLsStatePushFail       = 506,  /**< 互动直播推流失败 */
    kNERtcLsStatePushStopped    = 511,  /**< 推流结束 */
    kNERtcLsStateImageError     = 512,  /**< 背景图片设置出错 */
}NERtcLiveStreamStateCode;

/** 系统分类。*/
typedef enum
{
    kNERtcOSiOS      = 1,    /**< iOS 通用设备 */
    kNERtcOSAndroid  = 2,		/**< Android 通用设备 */
    kNERtcOSPC       = 3,    /**< PC设备 */
    kNERtcOSWebRTC   = 4,		/**< WebRTC */
} NERtcOSCategory;

/** 音频属性。设置采样率，码率，编码模式和声道数。*/
typedef enum
{
    kNERtcAudioProfileDefault = 0,			    /**< 0: 默认设置。Speech场景下为 kNERtcAudioProfileStandard，Music场景下为 kNERtcAudioProfileHighQuality */
    kNERtcAudioProfileStandard = 1,			    /**< 1: 普通质量的音频编码，16000Hz，20Kbps */
    kNERtcAudioProfileMiddleQuality = 2,		/**< 2: 中等质量的音频编码，48000Hz，32Kbps */
    kNERtcAudioProfileMiddleQualityStereo = 3,  /**< 3: 中等质量的立体声编码，48000Hz * 2，64Kbps  */
    kNERtcAudioProfileHighQuality = 4,          /**< 4: 高质量的音频编码，48000Hz，64Kbps  */
    kNERtcAudioProfileHighQualityStereo = 5,    /**< 5: 高质量的立体声编码，48000Hz * 2，128Kbps  */
} NERtcAudioProfileType;

/** 音频应用场景。不同的场景设置对应不同的音频采集模式（移动平台）、播放模式*/
typedef enum
{
    kNERtcAudioScenarioDefault  = 0,    /** 0: 默认设置:kNERtcChannelProfileCommunication下为kNERtcAudioScenarioSpeech，kNERtcChannelProfileLiveBroadcasting下为kNERtcAudioScenarioMusic。 */
    kNERtcAudioScenarioSpeech   = 1,    /** 1: 语音场景. NERtcAudioProfileType 推荐使用 kNERtcAudioProfileMiddleQuality 及以下 */
    kNERtcAudioScenarioMusic    = 2,    /** 2: 音乐场景。NERtcAudioProfileType 推荐使用 kNERtcAudioProfileMiddleQualityStereo 及以上 */
} NERtcAudioScenarioType;

/** 视频编码配置。用于衡量编码质量。*/
typedef enum
{
    kNERtcVideoProfileLowest = 0,       /**< 160x90/120, 15fps */
    kNERtcVideoProfileLow = 1,          /**< 320x180/240, 15fps */
    kNERtcVideoProfileStandard = 2,     /**< 640x360/480, 30fps */
    kNERtcVideoProfileHD720P = 3,       /**< 1280x720, 30fps */
    kNERtcVideoProfileHD1080P = 4,      /**< 1920x1080, 30fps */
    kNERtcVideoProfileNone = 5,
    kNERtcVideoProfileMAX = kNERtcVideoProfileHD1080P,
} NERtcVideoProfileType;

/** 视频流类型。*/
typedef enum {
    kNERtcRemoteVideoStreamTypeHigh     = 0, /**< 默认大流 */
    kNERtcRemoteVideoStreamTypeLow      = 1, /**< 小流 */
    kNERtcRemoteVideoStreamTypeNone     = 2, /**< 不订阅 */
} NERtcRemoteVideoStreamType;

/** 音频设备类型。*/
typedef enum
{
    kNERtcAudioDeviceUnknown = 0,       /**< 未知音频设备 */
    kNERtcAudioDeviceRecord,            /**< 音频采集设备 */
    kNERtcAudioDevicePlayout,           /**< 音频播放设备 */
} NERtcAudioDeviceType;

/** 音频设备类型状态。*/
typedef enum
{
    kNERtcAudioDeviceActive = 0,    /**< 音频设备已激活 */
    kNERtcAudioDeviceUnactive,      /**< 音频设备未激活 */
} NERtcAudioDeviceState;

/** 音频设备链接类型。*/
typedef enum
{
    kNERtcAudioDeviceTransportTypeUnknown           = 0,    /**< 未知设备 */
    kNERtcAudioDeviceTransportTypeBluetooth         = 1,    /**< 蓝牙设备 */
    kNERtcAudioDeviceTransportTypeBluetoothA2DP     = 2,    /**< 蓝牙立体声设备 */
    kNERtcAudioDeviceTransportTypeBluetoothLE       = 3,    /**< 蓝牙低功耗设备 */
    kNERtcAudioDeviceTransportTypeUSB               = 4,    /**< USB设备 */
    kNERtcAudioDeviceTransportTypeHDMI              = 5,    /**< HDMI设备 */
    kNERtcAudioDeviceTransportTypeBuiltIn           = 6,    /**< 内置设备 */
    kNERtcAudioDeviceTransportTypeThunderbolt       = 7,    /**< 雷电接口设备 */
    kNERtcAudioDeviceTransportTypeAirPlay           = 8,    /**< AirPlay设备 */
    kNERtcAudioDeviceTransportTypeVirtual           = 9,    /**< 虚拟设备 */
    kNERtcAudioDeviceTransportTypeOther             = 10,   /**< 其他设备 */
}NERtcAudioDeviceTransportType;

/** 摄像头设备链接类型。*/
typedef enum
{
    kNERtcVideoDeviceTransportTypeUnknown   = 0,    /**< 未知设备 */
    kNERtcVideoDeviceTransportTypeUSB       = 1,    /**< USB设备 */
    kNERtcVideoDeviceTransportTypeVirtual   = 2,    /**< 虚拟设备 */
    kNERtcVideoDeviceTransportTypeOther     = 3,    /**< 其他设备 */
}NERtcVideoDeviceTransportType;

/** 设备详细信息。*/
struct NERtcDeviceInfo
{
    char device_id[kNERtcMaxDeviceIDLength];        /**< 设备ID */
    char device_name[kNERtcMaxDeviceNameLength];    /**< 设备名称 */
    int transport_type;                             /**< 设备链接类型，分别指向NERtcAudioDeviceTransportType及NERtcVideoDeviceTransportType */
    bool suspected_unavailable;                     /**< 是否是不推荐设备 */
    bool system_default_device;                     /**< 是否是系统默认设备 */
};

/** 视频设备类型。*/
typedef enum
{
    kNERtcVideoDeviceUnknown = 0,   /**< 未知视频设备 */
    kNERtcVideoDeviceCapture,       /**< 视频采集设备 */
} NERtcVideoDeviceType;

/** 视频设备类型状态。*/
typedef enum
{
    kNERtcVideoDeviceAdded = 0, /**< 视频设备已添加 */
    kNERtcVideoDeviceRemoved,   /**< 视频设备已拔除 */
} NERtcVideoDeviceState;

/** @enum NERtcVideoScalingMode 设置视频缩放模式。*/
typedef enum
{
    kNERtcVideoScaleFit      = 0,   /**< 0: 视频尺寸等比缩放。优先保证视频内容全部显示。因视频尺寸与显示视窗尺寸不一致造成的视窗未被填满的区域填充黑色。*/
    kNERtcVideoScaleFullFill = 1,   /**< 1: 视频尺寸非等比缩放。保证视频内容全部显示，且填满视窗。*/
    kNERtcVideoScaleCropFill = 2,   /**< 2: 视频尺寸等比缩放。优先保证视窗被填满。因视频尺寸与显示视窗尺寸不一致而多出的视频将被截掉。*/
} NERtcVideoScalingMode;

/** @enum NERtcVideoMirrorMode 设置镜像模式。*/
typedef enum
{
    kNERtcVideoMirrorModeAuto       = 0,    /**< 0: （默认）Windows/macOS SDK 启用镜像模式。在 iOS/Android 平台中：如果你使用前置摄像头，SDK 默认启用镜像模式；如果你使用后置摄像头，SDK 默认关闭镜像模式。*/
    kNERtcVideoMirrorModeEnabled    = 1,    /**< 1: 启用镜像模式。*/
    kNERtcVideoMirrorModeDisabled   = 2,    /**< 2: 关闭镜像模式。*/
} NERtcVideoMirrorMode;

/** 连接状态 */
typedef enum
{
    kNERtcConnectionStateDisconnected = 1,      /**< 没加入频道。*/
    kNERtcConnectionStateConnecting = 2,        /**< 正在加入频道。*/
    kNERtcConnectionStateConnected = 3,         /**< 加入频道成功。*/
    kNERtcConnectionStateReconnecting = 4,      /**< 正在尝试重新加入频道。*/
    kNERtcConnectionStateFailed = 5,            /**< 加入频道失败。*/
} NERtcConnectionStateType;

/** 声音音量信息。一个数组，包含每个说话者的用户 ID 和音量信息。*/
struct NERtcAudioVolumeInfo
{
    uid_t uid;				/**< 说话者的用户 ID。如果返回的 uid 为 0，则默认为本地用户。*/
    unsigned int volume;	/**< 说话者的音量，范围为 0（最低）- 100（最高）。*/
};

/** 通话相关的统计信息。*/
struct NERtcStats
{
    uint32_t cpu_app_usage;     /**< 当前 App 的 CPU 使用率 (%)。*/
    uint32_t cpu_idle_usage;    /**< 当前系统的 CPU 空闲率 (%)。*/
    uint32_t cpu_total_usage;	/**< 当前系统的 CPU 使用率 (%)。*/
    uint32_t memory_app_usage;	/**< 当前App的内存使用率 (%)。*/
    uint32_t memory_total_usage;/**< 当前系统的内存使用率 (%)。*/
    uint32_t memory_app_kbytes;	/**< 当前App的内存使用量 (KB)。*/
    int total_duration;         /**< 通话时长（秒）。*/
    uint64_t tx_bytes;	        /**< 发送字节数，累计值。(bytes)*/
    uint64_t rx_bytes;	        /**< 接收字节数，累计值。(bytes)*/
    uint64_t tx_audio_bytes;	/**< 音频发送字节数，累计值。(bytes)*/
    uint64_t tx_video_bytes;	/**< 视频发送字节数，累计值。(bytes)*/
    uint64_t rx_audio_bytes;	/**< 音频接收字节数，累计值。(bytes)*/
    uint64_t rx_video_bytes;	/**< 视频接收字节数，累计值。(bytes)*/
    int tx_audio_kbitrate;	    /**< 音频发送码率。(kbps)*/
    int rx_audio_kbitrate;	    /**< 音频接收码率。(kbps)*/
    int tx_video_kbitrate;	    /**< 视频发送码率。(kbps)*/
    int rx_video_kbitrate;	    /**< 视频接收码率。(kbps)*/
    int up_rtt;	                /**< 上行平均往返时延rtt(ms) */
    int tx_audio_packet_loss_rate;  /**< 本地上行音频实际丢包率。(%) */
    int tx_video_packet_loss_rate;  /**< 本地上行视频实际丢包率。(%) */
    int tx_audio_packet_loss_sum;   /**< 本地上行音频实际丢包数。 */
    int tx_video_packet_loss_sum;   /**< 本地上行视频实际丢包数。 */
    int tx_audio_jitter;            /**< 本地上行音频抖动计算。(ms) */
    int tx_video_jitter;            /**< 本地上行视频抖动计算。(ms) */
    int rx_audio_packet_loss_rate;  /**< 本地下行音频实际丢包率。(%) */
    int rx_video_packet_loss_rate;  /**< 本地下行视频实际丢包率。(%) */
    int rx_audio_packet_loss_sum;   /**< 本地下行音频实际丢包数。 */
    int rx_video_packet_loss_sum;   /**< 本地下行视频实际丢包数。 */
    int rx_audio_jitter;            /**< 本地下行音频抖动计算。(ms) */
    int rx_video_jitter;            /**< 本地下行视频抖动计算。(ms) */
};

/** 本地视频流上传统计信息。*/
struct NERtcVideoSendStats
{
    int width;      /**< 视频流宽（像素）。*/
    int height;     /**< 视频流高（像素）。*/
    int capture_frame_rate; /**< 视频采集帧率。*/
    int encoder_frame_rate;	/**< （上次统计后）编码帧率 (fps)。*/
    int sent_bitrate;		/**< （上次统计后）发送码率(Kbps)。*/
    int sent_frame_rate;	/**< （上次统计后）发送帧率 (fps)。*/
    int target_bitrate;		/**< （上次统计后）编码器目标码率(Kbps)。*/
};

/** 远端视频流的统计信息。*/
struct NERtcVideoRecvStats
{
    uid_t uid;      /**< 用户 ID，指定是哪个用户的视频流。*/
    int width;      /**< 视频流宽（像素）。*/
    int height;     /**< 视频流高（像素）。*/
    int received_bitrate;     /**< （上次统计后）接收到的码率(Kbps)。*/
    int packet_loss_rate;     /**< 下行丢包率(%)。*/
    int decoder_frame_rate;	/**< （上次统计后）解码帧率 (fps)。*/
    int render_frame_rate;	/**< （上次统计后）渲染帧率 (fps)。*/
    int received_frame_rate;  /**< （上次统计后）接收到的帧率 (fps)。*/
    int total_frozen_time;  /**< 用户的下行视频卡顿累计时长(ms)。*/
    int frozen_rate;        /**< 用户的下行视频平均卡顿率(%)。*/
};

/** 本地音频流上传统计信息。*/
struct NERtcAudioSendStats
{
    int num_channels;	    /**< 当前采集声道数。*/
    int sent_sample_rate;	/**< 本地上行音频采样率。*/
    int sent_bitrate;		/**< （上次统计后）发送码率(Kbps)。*/
    int audio_loss_rate;    /**< 特定时间内的音频丢包率 (%)。*/
    int64_t rtt;            /**< RTT。*/
    unsigned int volume;	/**< 音量，范围为 0（最低）- 100（最高）。*/
};

/** 远端用户的音频统计。*/
struct NERtcAudioRecvStats
{
    uid_t uid;              /**< 用户 ID，指定是哪个用户的音频流。*/
    int received_bitrate;   /**< （上次统计后）接收到的码率(Kbps)。*/
    int total_frozen_time;  /**< 用户的下行音频卡顿累计时长(ms)。*/
    int frozen_rate;        /**< 用户的下行音频平均卡顿率(%)。*/
    int audio_loss_rate;  /**< 特定时间内的音频丢包率 (%)。*/
    unsigned int volume;	/**< 音量，范围为 0（最低）- 100（最高）。*/
};

/** @enum NERtcNetworkQualityType 网络质量类型。*/
typedef enum
{
    kNERtcNetworkQualityUnknown = 0,    /**< 0: 网络质量未知。*/
    kNERtcNetworkQualityExcellent = 1,  /**< 1: 网络质量极好。*/
    kNERtcNetworkQualityGood = 2,       /**< 2: 用户主观感觉和 excellent 差不多，但码率可能略低于 excellent。*/
    kNERtcNetworkQualityPoor = 3,       /**< 3: 用户主观感受有瑕疵但不影响沟通。*/
    kNERtcNetworkQualityBad = 4,        /**< 4: 勉强能沟通但不顺畅。*/
    kNERtcNetworkQualityVeryBad = 5,    /**< 5: 网络质量非常差，基本不能沟通。*/
    kNERtcNetworkQualityDown = 6,       /**< 6: 完全无法沟通。*/
} NERtcNetworkQualityType;

/** 网络质量统计信息。*/
struct NERtcNetworkQualityInfo
{
    uid_t uid;                          /**< 用户 ID，指定是哪个用户的网络质量统计。*/
    NERtcNetworkQualityType tx_quality; /**< 该用户的上行网络质量，详见 #NERtcNetworkQualityType.*/
    NERtcNetworkQualityType rx_quality; /**< 该用户的下行网络质量，详见 #NERtcNetworkQualityType.*/
};

/** @enum NERtcVideoCropMode 视频画面裁剪模式。*/
typedef enum
{
    kNERtcVideoCropModeDefault = 0,     /**< Device Defalut */
    kNERtcVideoCropMode16x9    = 1,     /**< 16:9 */
    kNERtcVideoCropMode4x3     = 2,     /**< 4:3 */
    kNERtcVideoCropMode1x1     = 3,     /**< 1:1 */
}NERtcVideoCropMode;

/** @enum NERtcVideoFramerateType 视频帧率。*/
typedef enum {
    kNERtcVideoFramerateFpsDefault  = 0,    /**< 默认帧率 */
    kNERtcVideoFramerateFps_7       = 7,    /**< 7帧每秒 */
    kNERtcVideoFramerateFps_10      = 10,   /**< 10帧每秒 */
    kNERtcVideoFramerateFps_15      = 15,   /**< 15帧每秒 */
    kNERtcVideoFramerateFps_24      = 24,   /**< 24帧每秒 */
    kNERtcVideoFramerateFps_30      = 30,   /**< 30帧每秒 */
} NERtcVideoFramerateType;

/** @enum NERtcDegradationPreference 视频编码策略。*/
typedef enum {
    kNERtcDegradationDefault            = 0,  /**< 使用引擎推荐值。通话场景使用平衡模式，直播推流场景使用清晰优先 */
    kNERtcDegradationMaintainFramerate  = 1,  /**< 帧率优先 */
    kNERtcDegradationMaintainQuality    = 2,  /**< 清晰度优先 */
    kNERtcDegradationBalanced           = 3,  /**< 平衡模式 */
} NERtcDegradationPreference;

/** 视频配置的属性。*/
struct NERtcVideoConfig
{
    NERtcVideoProfileType max_profile;	/**< 视频编码的分辨率，用于衡量编码质量。*/
    NERtcVideoCropMode crop_mode_;      /**< 视频画面裁剪模式，默认kNERtcVideoCropModeDefault。*/
    NERtcVideoFramerateType framerate;  /**< 视频帧率 */
    NERtcVideoFramerateType min_framerate;  /**< 视频最小帧率 */
    uint32_t bitrate;                   /**< 视频编码码率kbps，取0时使用默认值 */
    uint32_t min_bitrate;               /**< 视频编码码率下限kbps，取0时使用默认值 */
    NERtcDegradationPreference degradation_preference;   /**< 编码策略 */
};

/** 视频帧数据回调 */
typedef void(*onFrameDataCallback)(
    uid_t uid,          /**< uid */
    void *data,         /**< 数据指针 */
    uint32_t type,      /**< NERtcVideoType */
    uint32_t width,     /**< 宽度 */
    uint32_t height,    /**< 高度 */
    uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
    uint32_t offset[4], /**< 每类数据偏移 */
    uint32_t stride[4], /**< 每类数据步进 */
    uint32_t rotation,  /**< NERtcVideoRotation */
    void *user_data     /**< 用户透传数据 */
    );

/** 屏幕共享编码参数配置。*/
typedef enum
{
    kNERtcScreenProfile480P     = 0,    /**< 640x480, 5fps */
    kNERtcScreenProfileHD720P   = 1,    /**< 1280x720, 5fps */
    kNERtcScreenProfileHD1080P  = 2,    /**< 1920x1080, 5fps。默认 */
    kNERtcScreenProfileCustom   = 3,    /**< 自定义 */
    kNERtcScreenProfileNone     = 4,
    kNERtcScreenProfileMAX = kNERtcScreenProfileHD1080P,
} NERtcScreenProfileType;

/** 待共享区域相对于整个屏幕或窗口的位置，如不填，则表示共享整个屏幕或窗口。*/
struct NERtcRectangle
{
    int x;      /**< 左上角的横向偏移。*/
    int y;      /**< 左上角的纵向偏移。*/
    int width;  /**< 待共享区域的宽。*/
    int height; /**< 待共享区域的高。*/
};

/** 视频尺寸。*/
struct NERtcVideoDimensions
{
    int width;     /**< 宽度 */
    int height;    /**< 高度 */
};

/** 屏幕共享编码参数配置。用于衡量编码质量。一期只支持profile设置。*/
struct NERtcScreenCaptureParameters
{
    NERtcScreenProfileType profile;     /**< 屏幕共享编码参数配置。*/
    NERtcVideoDimensions dimensions;    /**< 屏幕共享视频发送的最大像素值，kNERtcScreenProfileCustom下生效。*/
    int frame_rate;                     /**< 共享视频的帧率，kNERtcScreenProfileCustom下生效，单位为 fps；默认值为 5，建议不要超过 15。*/
    int bitrate;                        /**< 共享视频的码率，单位为 bps；默认值为 0，表示 SDK 根据当前共享屏幕的分辨率计算出一个合理的值。*/
    bool capture_mouse_cursor;          /**< 是否采集鼠标用于屏幕共享。*/
    bool window_focus;                  /**< 调用 startScreenCaptureByWindowId 方法共享窗口时，是否将该窗口前置。*/
    void* excluded_window_list;         /**< 待屏蔽窗口的 ID 列表。 */
    int excluded_window_count;          /**< 待屏蔽窗口的数量。*/
};

/** 视频显示设置 */
struct NERtcVideoCanvas
{
    onFrameDataCallback cb; /**< 数据回调  如果是macosx，需要设置NERtcEngineContext的video_use_exnternal_render为true才有效*/
    void *user_data;        /**< 数据回调的用户透传数据 如果是macosx，需要设置NERtcEngineContext的video_use_exnternal_render为true才有效 */
    void *window;           /**< 渲染窗口句柄 如果是macosx，需要设置NERtcEngineContext的video_use_exnternal_render为false才有效*/
    NERtcVideoScalingMode scaling_mode; /**< 视频缩放模式 */
};

/** 录制类型。*/
typedef enum
{
    kNERtcRecordTypeAll = 0,    /**< 参与混合录制且录制单人文件。*/
    kNERtcRecordTypeMix = 1,    /**< 参与混合录制。*/
    kNERtcRecordTypeSingle = 2, /**< 只录单人文件。*/
} NERtcRecordType;

/** 音频类型。*/
typedef enum
{
    kNERtcAudioTypePCM16 = 0,    /**< PCM 音频格式。*/
} NERtcAudioType;

/** 音频帧请求格式。*/
struct NERtcAudioFrameRequestFormat
{
    uint32_t     channels;      /**< 音频频道数量(如果是立体声，数据是交叉的)。单声道: 1；双声道 : 2。*/
    uint32_t     sample_rate;   /**< 采样率。*/
};

/** 音频格式。*/
struct NERtcAudioFormat
{
    NERtcAudioType type;        /**< 音频类型。*/
    uint32_t     channels;      /**< 音频频道数量(如果是立体声，数据是交叉的)。单声道: 1；双声道 : 2。*/
    uint32_t     sample_rate;    /**< 采样率。*/
    uint32_t     bytes_per_sample;    /**< 每个采样点的字节数 : 对于 PCM 来说，一般使用 16 bit，即两个字节。*/
    uint32_t     samples_per_channel;    /**< 每个频道的样本数量。*/
};

/** 音频帧。*/
struct NERtcAudioFrame
{
    NERtcAudioFormat format;    /**< 音频格式。*/
    void *data;     /**< 数据缓冲区。有效数据长度为：samples_per_channel × channels × bytes_per_sample。*/
};

/** 语音观测器对象。允许修改 frame 里 void *data 所指向的内容，不允许修改 format。如果对 format 有要求，需调用相应设置接口。*/
class INERtcAudioFrameObserver
{
public:
    virtual ~INERtcAudioFrameObserver() {}
    /** 采集音频数据回调。

     @param frame 音频帧。
     */
    virtual void onAudioFrameDidRecord(NERtcAudioFrame *frame) = 0;
    /** 播放音频数据回调。

     @param frame 音频帧。
     */
    virtual void onAudioFrameWillPlayback(NERtcAudioFrame *frame) = 0;
};

/** 视频类型。*/
typedef enum
{
    kNERtcVideoTypeI420 = 0,    /**< I420 视频格式。*/
    kNERtcVideoTypeNV12 = 1,    /**< NV12 视频格式。*/
    kNERtcVideoTypeNV21 = 2,    /**< NV21 视频格式。*/
    kNERtcVideoTypeBGRA = 3,    /**< BGRA 视频格式。*/
    kNERtcVideoTypeCVPixelBuffer = 4,    /**< oc capture native视频格式。不支持外部视频输入*/
} NERtcVideoType;

/** 视频旋转角度。*/
typedef enum
{
    kNERtcVideoRotation_0 = 0,      /**< 0 度。*/
    kNERtcVideoRotation_90 = 90,    /**< 90 度。*/
    kNERtcVideoRotation_180 = 180,  /**< 180 度。*/
    kNERtcVideoRotation_270 = 270,  /**< 270 度。*/
} NERtcVideoRotation;

/** 外部输入的视频桢。*/
struct NERtcVideoFrame {
    NERtcVideoType format;      /**< 视频类型  详见: #NERtcVideoType*/
    uint64_t timestamp;         /**< 视频桢时间戳 */
    uint32_t width;             /**< 视频桢宽度 */
    uint32_t height;            /**< 视频桢宽高 */
    NERtcVideoRotation rotation;/**<  视频旋转角度 详见: #NERtcVideoRotation */
    void* buffer;               /**<  视频桢数据 */
};

/** 用户离开原因。*/
typedef enum 
{
    kNERtcSessionLeaveNormal = 0,       /**< 正常离开。*/
    kNERtcSessionLeaveForFailOver = 1,  /**< 用户断线导致离开。*/
    kNERtcSessionLeaveUpdate = 2,       /**< 用户 Failover 过程中产生的 leave。*/
    kNERtcSessionLeaveForKick = 3,      /**< 用户被踢导致离开。*/
    kNERtcSessionLeaveTimeOut = 4,      /**< 用户超时导致离开。*/
} NERtcSessionLeaveReason;

/** 音乐文件播放状态。
*/
typedef enum 
{
    kNERtcAudioMixingStateFinished  = 0,       /**< 音乐文件播放结束。*/
    kNERtcAudioMixingStateFailed    = 1,       /**< 音乐文件报错。详见: #NERtcAudioMixingErrorCode*/
} NERtcAudioMixingState;

/** 创建混音的配置项 */
struct NERtcCreateAudioMixingOption
{
    char path[kNERtcMaxURILength];  /**< 本地文件全路径或URL */
    int loop_count;                    /**< 循环次数， <= 0, 表示无限循环，默认 1 */
    bool send_enabled;              /**< 是否可发送，默认为 true */
    uint32_t send_volume;           /**< 发送音量。最大为 100（默认）含义（0%-100%）*/
    bool playback_enabled;          /**< 是否可回放，默认为 true */
    uint32_t playback_volume;       /**< 回放音量。最大为 100（默认）*/
};

/** 创建音效的配置项 */
struct NERtcCreateAudioEffectOption
{
    char path[kNERtcMaxURILength];  /**< 本地文件全路径或URL */
    int loop_count;                    /**< 循环次数， <= 0, 表示无限循环，默认 1 */
    bool send_enabled;              /**< 是否可发送，默认为 true */
    uint32_t send_volume;           /**< 发送音量。最大为 100（默认）含义（0%-100%）*/
    bool playback_enabled;          /**< 是否可回放，默认为 true */
    uint32_t playback_volume;       /**< 回放音量。最大为 100（默认）*/
};

/** 日志级别。 */
typedef enum {
    kNERtcLogLevelFatal    = 0,        /**< Fatal 级别日志信息。 */
    kNERtcLogLevelError    = 1,        /**< Error 级别日志信息。 */
    kNERtcLogLevelWarning  = 2,        /**< Warning 级别日志信息。 */
    kNERtcLogLevelInfo     = 3,        /**< Info 级别日志信息。默认级别 */
    kNERtcLogLevelDetailInfo   = 4,    /**< Detail Info 级别日志信息。 */
    kNERtcLogLevelVerbos   = 5,        /**< Verbos 级别日志信息。 */
    kNERtcLogLevelDebug    = 6,        /**< Debug 级别日志信息。如果你想获取最完整的日志，可以将日志级别设为该等级。*/
    kNERtcLogLevelOff      = 7,        /**< 不输出日志信息。*/
} NERtcLogLevel;

/** 视频推流后发送策略。 */
typedef enum {
    kNERtcSendOnPubNone = 0, /**< 不主动发送数据流，被订阅后发送。 */
    kNERtcSendOnPubHigh = 1, /**< 主动发送大流。 */
    kNERtcSendOnPubLow = 1 << 1, /**< 主动发送小流。 */
    kNERtcSendOnPubAll = kNERtcSendOnPubLow | kNERtcSendOnPubHigh, /**< 主动发送大小流。 */
} NERtcSendOnPubType;

/** 通过 JSON 配置 SDK 提供技术预览或特别定制功能。以标准化方式公开 JSON 选项。详见API setParameters*/
#define kNERtcKeyRecordHostEnabled          "record_host_enabled"          /**< bool value. true: 录制主讲人, false: 不是录制主讲人 */
#define kNERtcKeyRecordAudioEnabled         "record_audio_enabled"         /**< bool value，启用服务器音频录制。默认值 true */
#define kNERtcKeyRecordVideoEnabled         "record_video_enabled"         /**< bool value，启用服务器视频录制。默认值 true */
#define kNERtcKeyRecordType                 "record_type"                  /**< int value, NERtcRecordType */
#define kNERtcKeyAutoSubscribeAudio         "auto_subscribe_audio"         /**< bool value，其他用户打开音频时，自动订阅。 默认值 true */
#define kNERtcKeyPublishSelfStreamEnabled   "publish_self_stream_enabled"   /**< bool value，开启旁路直播。默认值 false */
#define kNERtcKeyLogLevel                   "log_level"                     /**< int value, NERtcLogLevel，SDK 输出小于或等于该级别的log，默认为 kNERtcLogLevelInfo */
#define kNERtcKeyAudioProcessingAECEnable   "audio_processing_aec_enable"   /**< bool value. AEC开关，默认值 true */
#define kNERtcKeyAudioProcessingAGCEnable   "audio_processing_agc_enable"   /**< bool value. AGC开关，默认值 true */
#define kNERtcKeyAudioProcessingNSEnable    "audio_processing_ns_enable"   /**< bool value. NS开关，默认值 true */
#define kNERtcKeyAudioProcessingExternalAudioMixEnable  "audio_processing_external_audiomix_enable"   /**< bool value. 输入混音开关，默认值 false */
#define kNERtcKeyAudioProcessingEarphone    "audio_processing_earphone"    /**< bool value. 通知SDK是否使用耳机， true: 使用耳机, false: 不使用耳机，默认值 false */
#define kNERtcKeyVideoSendOnPubType         "video_sendonpub_type"      /**< int value. NERtcSendOnPubType；设置视频发送策略，默认发送大流 kNERtcSendOnPubHigh；通话前设置有效 */
} // namespace nertc

#endif
