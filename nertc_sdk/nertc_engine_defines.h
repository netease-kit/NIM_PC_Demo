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
#define kNERtcMacSEIBufferLength    4096    /**< 直播推流中用到的SEI信息最大长度，单位：字节 */
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
    kNERtcIIDAudioDeviceManager = 1,                /**< 获取音频设备管理器的接口ID */
    kNERtcIIDVideoDeviceManager = 2,                /**< 获取视频设备管理器的接口ID */
} NERtcInterfaceIdType;

/** 参会者角色类型 */
typedef enum {
    kNERtcClientRoleBroadcaster     = 0,            /**< （默认）直播模式中的主播，可以操作摄像头等音视频设备、发布流、配置互动直播推流任务、上下线对房间内其他用户可见。 */
    kNERtcClientRoleAudience        = 1,            /**< 直播模式中的观众，观众只能接收音视频流，不支持操作音视频设备、配置互动直播推流任务、上下线不通知其他用户。 */
}NERtcClientRole;

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
    /**
     将指定uid对应用户的视频流拉入直播。如果添加多个 users，则 uid 不能重复。
     */
    uid_t uid;
    /**
     是否在直播中向观看者播放该用户的对应视频流。可设置为：
     - true：在直播中播放该用户的视频流。
     - false：在直播中不播放该用户的视频流。
     推流模式为 kNERtcLsModeAudio 时无效。
     */
    bool video_push;
    /**
     直播推流视频和画布的调节属性。详细信息请参考 NERtcLiveStreamVideoScaleMode。
     */
    NERtcLiveStreamVideoScaleMode adaption;
    /**
     x 参数用于设置用户图像的横轴坐标值。通过 x 和 y 指定画布坐标中的一个点，该点将作为用户图像的左上角。

     取值范围为 0~1920，若设置为奇数值，会自动向下取偶。

     用户图像范围如果超出超出画布，调用方法时会报错。
     */
    int  x;
    /**
     y参数用于设置用户图像的纵轴坐标值。通过 x 和 y 指定画布坐标中的一个点，该点将作为用户图像的左上角。

     取值范围为 0~1920，若设置为奇数值，会自动向下取偶。

     用户图像范围如果超出超出画布，调用方法时会报错。
     */
    int  y;
    /**
     该用户图像在画布中的宽度。

     取值范围为 0~1920，默认为0。若设置为奇数值，会自动向下取偶。

     用户图像范围如果超出超出画布，调用方法时会报错。
     */
    int  width;
    /**
     该用户图像在画布中的高度。

     取值范围为 0~1920，默认为0。若设置为奇数值，会自动向下取偶。

     用户图像范围如果超出超出画布，调用方法时会报错。
     */
    int  height;
    /**
     是否在直播中混流该用户的对应音频流。可设置为：
     - true：在直播中混流该用户的对应音频流。
     - false：在直播中将该用户设置为静音。
     */
    bool audio_push;
    /**
    图层编号，用来决定渲染层级, 取值0-100，0位于最底层，100位于最顶层。

    相同层级的渲染区域按照现有的覆盖逻辑实现，即按照数组中顺序进行渲染，index 递增依次往上叠加。
    */
    int z_order;
};

/** 图片布局 */
struct NERtcLiveStreamImageInfo {
    /**
     占位图片的URL。
     */
    char url[kNERtcMaxURILength];
    /**
     x 参数用于设置画布的横轴坐标值。

     通过 x 和 y 指定画布坐标中的一个点，该点将作为占位图片的左上角。

     取值范围为 0~1920，若设置为奇数值，会自动向下取偶。
     */
    int x;
    /**
     y 参数用于设置画布的纵轴坐标值。

     通过 x 和 y 指定画布坐标中的一个点，该点将作为占位图片的左上角。

     取值范围为 0~1920，若设置为奇数值，会自动向下取偶。
     */
    int y;
    /**
     该占位图片在画布中的宽度。

     取值范围为 0~1920，若设置为奇数值，会自动向下取偶。
     */
    int width;
    /**
     该占位图片在画布中的高度。

     取值范围为 0~1920，若设置为奇数值，会自动向下取偶。
     */
    int height;
};

/** 直播布局 */
struct NERtcLiveStreamLayout {
    /**
     整体画布的宽度，单位为 px。取值范围为 0~1920，若设置为奇数值，会自动向下取偶。
     */
    int width;
    /**
     整体画布的高度，单位为 px。取值范围为 0~1920，若设置为奇数值，会自动向下取偶。
     */
    int height;
    /**
     画面背景颜色，格式为 256 x 256 x R + 256 x G + B的和。请将对应 RGB 的值分别带入此公式计算即可。若未设置，则默认为0。
     */
    unsigned int background_color;
    /**
     成员布局个数。
     */
    unsigned int user_count;
    /**
     成员布局数组，详细信息请参考 NERtcLiveStreamUserTranscoding。
     */
    NERtcLiveStreamUserTranscoding *users;
    /**
     详细信息请参考 NERtcLiveStreamImageInfo。
     */
    NERtcLiveStreamImageInfo* bg_image;
};

/**
 直播推流音频采样率
 */
typedef enum {
    kNERtcLiveStreamAudioSampleRate32000 = 32000, /**<采样率为 32 kHz。*/
    kNERtcLiveStreamAudioSampleRate44100 = 44100, /**<采样率为 44.1 kHz。*/
    kNERtcLiveStreamAudioSampleRate48000 = 48000, /**<（默认）采样率为 48 kHz。*/
} NERtcLiveStreamAudioSampleRate;

/**
 直播推流音频编码规格
 */
typedef enum {
    kNERtcLiveStreamAudioCodecProfileLCAAC = 0, /**<（默认）LC-AAC 规格，表示基本音频编码规格。*/
    kNERtcLiveStreamAudioCodecProfileHEAAC = 1, /**<HE-AAC 规格，表示高效音频编码规格。*/
} NERtcLiveStreamAudioCodecProfile;

/** 直播流配置 */
struct NERtcLiveConfig {
    /**
     * 单路视频透传开关，默认为关闭状态。
     * 
     * 开启后，如果房间中只有一路视频流输入， 则不对输入视频流进行转码，不遵循转码布局，直接推流 CDN。
     * 
     * 如果有多个房间成员视频流混合为一路流，则该设置失效，并在恢复为一个成员画面（单路流）时也不会恢复。
     */
    bool single_video_passthrough;
    /**
     * 音频推流码率。
     * 
     * 单位为 kbps，取值范围为 10~192。
     * 
     * 语音场景建议设置为 64 及以上码率，音乐场景建议设置为 128 及以上码率。
     */
    int audio_bitrate;

    /**
     音频推流采样率。单位为Hz。默认为 kNERtcLiveStreamAudioSampleRate48000，即采样率为 48 kHz。
     */
    NERtcLiveStreamAudioSampleRate sampleRate;

    /**
     音频推流声道数，默认值为 2 双声道。
     */
    int channels;

    /**
     音频编码规格。默认值 NERtcLiveStreamAudioCodecProfileLCAAC，普通编码规格。
     - 0: LC-AAC 规格，表示基本音频编码规格
     - 1: HE-AAC 规格，表示高效音频编码规格。
     */
    NERtcLiveStreamAudioCodecProfile audioCodecProfile;
};

/** 直播推流任务的配置项。*/
struct NERtcLiveStreamTaskInfo {
    /**
     自定义的推流任务ID。字母、数字、下划线组成的 64 位以内的字符串。请保证此ID唯一。
     */
    char task_id[kNERtcMaxTaskIDLength];
    /**
     推流地址，例如 rtmp://test.url。

     此处的推流地址可设置为网易云信直播产品中服务端 API创建房间的返回参数pushUrl。
     */
    char stream_url[kNERtcMaxURILength];
    /**
     旁路推流是否需要进行音视频录制。默认为关闭状态。
     */
    bool server_record_enabled;
    /**
     直播推流模式。详细信息请参考 NERtcLiveStreamMode。
     */
    NERtcLiveStreamMode ls_mode;
    /**
     设置互动直播的画面布局。详细信息请参考 NERtcLiveStreamLayout。
     */
    NERtcLiveStreamLayout layout;
    /**
     音视频流编码参数等设置。详细信息请参考 NERtcLiveConfig。
     */
    NERtcLiveConfig config;
    char extraInfo[kNERtcMacSEIBufferLength];   /**< SEI信息 */
};

/** 直播推流状态。*/
typedef enum {
    kNERtcLsStatePushing        = 505,  /**< 推流中 */
    kNERtcLsStatePushFail       = 506,  /**< 互动直播推流失败 */
    kNERtcLsStatePushStopped    = 511,  /**< 推流结束 */
    kNERtcLsStateImageError     = 512,  /**< 背景图片设置出错 */
}NERtcLiveStreamStateCode;

/** 系统分类。*/
typedef enum {
    kNERtcOSiOS = 1,     /**< iOS 通用设备 */
    kNERtcOSAndroid = 2, /**< Android 通用设备 */
    kNERtcOSPC = 3,      /**< PC设备 */
    kNERtcOSWebRTC = 4,  /**< WebRTC */
} NERtcOSCategory;

/** 音频属性。设置采样率，码率，编码模式和声道数。*/
typedef enum
{
    kNERtcAudioProfileDefault = 0,			    /**< 0: 默认设置。Speech场景下为 kNERtcAudioProfileStandardExtend，Music场景下为 kNERtcAudioProfileHighQuality */
    kNERtcAudioProfileStandard = 1,			    /**< 1: 普通质量的音频编码，16000Hz，20Kbps */
    kNERtcAudioProfileStandardExtend = 2,       /**< 2: 普通质量的音频编码，16000Hz，32Kbps */
    kNERtcAudioProfileMiddleQuality = 3,		/**< 3: 中等质量的音频编码，48000Hz，32Kbps */
    kNERtcAudioProfileMiddleQualityStereo = 4,  /**< 4: 中等质量的立体声编码，48000Hz * 2，64Kbps  */
    kNERtcAudioProfileHighQuality = 5,          /**< 5: 高质量的音频编码，48000Hz，64Kbps  */
    kNERtcAudioProfileHighQualityStereo = 6,    /**< 6: 高质量的立体声编码，48000Hz * 2，128Kbps  */
} NERtcAudioProfileType;

/** 音频应用场景。不同的场景设置对应不同的音频采集模式、播放模式*/
typedef enum {
    /** 0: 默认设置
       - kNERtcChannelProfileCommunication下为kNERtcAudioScenarioSpeech，
       - kNERtcChannelProfileLiveBroadcasting下为kNERtcAudioScenarioMusic。
     */
    kNERtcAudioScenarioDefault = 0,
    /** 1: 语音场景. NERtcAudioProfileType 推荐使用 kNERtcAudioProfileMiddleQuality 及以下 */
    kNERtcAudioScenarioSpeech = 1,
    /** 2: 音乐场景。NERtcAudioProfileType 推荐使用 kNERtcAudioProfileMiddleQualityStereo 及以上 */
    kNERtcAudioScenarioMusic = 2,
} NERtcAudioScenarioType;

/** 变声 预设值 */
typedef enum {
    kNERtcVoiceChangerOff           = 0,    /**< 默认关闭 */
    kNERtcVoiceChangerRobot         = 1,    /**< 机器人 */
    kNERtcVoiceChangerGaint         = 2,    /**< 巨人 */
    kNERtcVoiceChangerHorror        = 3,    /**< 恐怖 */
    kNERtcVoiceChangerMature        = 4,    /**< 成熟 */
    kNERtcVoiceChangerManToWoman    = 5,    /**< 男变女 */
    kNERtcVoiceChangerWomanToMan    = 6,    /**< 女变男 */
    kNERtcVoiceChangerManToLoli     = 7,    /**< 男变萝莉 */
    kNERtcVoiceChangerWomanToLoli   = 8,    /**< 女变萝莉 */
}NERtcVoiceChangerType;

/** 预设的美声效果 */
typedef enum {
    kNERtcVoiceBeautifierOff = 0,             /**< 默认关闭 */
    kNERtcVoiceBeautifierMuffled = 1,         /**< 低沉 */
    kNERtcVoiceBeautifierMellow = 2,          /**< 圆润 */
    kNERtcVoiceBeautifierClear = 3,           /**< 清澈 */
    kNERtcVoiceBeautifierMagnetic = 4,        /**< 磁性 */
    kNERtcVoiceBeautifierRecordingstudio = 5, /**< 录音棚 */
    kNERtcVoiceBeautifierNature = 6,          /**< 天籁 */
    kNERtcVoiceBeautifierKTV = 7,             /**< KTV */
    kNERtcVoiceBeautifierRemote = 8,          /**< 悠远 */
    kNERtcVoiceBeautifierChurch = 9,          /**< 教堂 */
    kNERtcVoiceBeautifierBedroom = 10,        /**< 卧室 */
    kNERtcVoiceBeautifierLive = 11,           /**< Live */
} NERtcVoiceBeautifierType;

/** 音效均衡波段的中心频率 */
typedef enum {
    kNERtcVoiceEqualizationBand_31  = 0, /**<  31 Hz */
    kNERtcVoiceEqualizationBand_62  = 1, /**<  62 Hz */
    kNERtcVoiceEqualizationBand_125 = 2, /**<  125 Hz */
    kNERtcVoiceEqualizationBand_250 = 3, /**<  250 Hz */
    kNERtcVoiceEqualizationBand_500 = 4, /**<  500 Hz */
    kNERtcVoiceEqualizationBand_1K  = 5, /**<  1 kHz */
    kNERtcVoiceEqualizationBand_2K  = 6, /**<  2 kHz */
    kNERtcVoiceEqualizationBand_4K  = 7, /**<  4 kHz */
    kNERtcVoiceEqualizationBand_8K  = 8, /**<  8 kHz */
    kNERtcVoiceEqualizationBand_16K = 9, /**<  16 kHz */
} NERtcVoiceEqualizationBand;

/** 视频编码配置。用于衡量编码质量。

@note 
- kNERtcVideoProfileFake:
 表示对端发送16*16的fakeVideo(纯音频下的SEI发送)，属于SDK内部行为，此时不需要显示view(收到的是黑色帧)；
 因此这个档位不主动使用，属于被动接受，如果主动使用，SDK内部会按standard处理
 */
typedef enum
{
    kNERtcVideoProfileLowest = 0,       /**< 普清（160x90/120, 15fps） */
    kNERtcVideoProfileLow = 1,          /**< 标清（320x180/240, 15fps） */
    kNERtcVideoProfileStandard = 2,     /**< 高清（640x360/480, 30fps） */
    kNERtcVideoProfileHD720P = 3,       /**< 超清（1280x720, 30fps） */
    kNERtcVideoProfileHD1080P = 4,      /**< 1080P（1920x1080, 30fps） */
    kNERtcVideoProfileNone = 5,
    kNERtcVideoProfileMAX = kNERtcVideoProfileHD1080P,
    /**
     FakeVideo标识，仅在回调中显示。请勿主动设置，否则 SDK 会按照STANDARD处理。

     当远端在纯音频状态发送 SEI 时，本端将会收到远端的onUserVideoStart回调，其中 max_profile 参数为kNERtcVideoProfileFake ， 表示对端发送 16*16 的FakeVideo，此时如果本端需要接收远端的SEI信息，只需要订阅一下远端的视频即可，无须设置远端画布。
     */
	kNERtcVideoProfileFake = 6,
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
    kNERtcVideoScaleFit      = 0,   /**< 0：适应视频，视频尺寸等比缩放。优先保证视频内容全部显示。若视频尺寸与显示视窗尺寸不一致，视窗未被填满的区域填充背景色。*/
    kNERtcVideoScaleFullFill = 1,   /**< 1：视频尺寸非等比缩放。保证视频内容全部显示，且填满视窗。*/
    kNERtcVideoScaleCropFill = 2,   /**< 2：适应区域，视频尺寸等比缩放。保证所有区域被填满，视频超出部分会被裁剪。*/
} NERtcVideoScalingMode;

/** @enum NERtcVideoMirrorMode 设置镜像模式。*/
typedef enum
{
    kNERtcVideoMirrorModeAuto       = 0,    /**< 0: Windows/macOS SDK 启用镜像模式。在 iOS/Android 平台中：如果你使用前置摄像头，SDK 默认启用镜像模式；如果你使用后置摄像头，SDK 默认关闭镜像模式。*/
    kNERtcVideoMirrorModeEnabled    = 1,    /**< 1: 启用镜像模式。*/
    kNERtcVideoMirrorModeDisabled   = 2,    /**< 2: （默认）关闭镜像模式。*/
} NERtcVideoMirrorMode;

/** 连接状态 */
typedef enum
{
    kNERtcConnectionStateDisconnected   = 1, /**< 没加入房间。*/
    kNERtcConnectionStateConnecting     = 2, /**< 正在加入房间。*/
    kNERtcConnectionStateConnected      = 3, /**< 加入房间成功。*/
    kNERtcConnectionStateReconnecting   = 4, /**< 正在尝试重新加入房间。*/
    kNERtcConnectionStateFailed         = 5, /**< 加入房间失败。*/
} NERtcConnectionStateType;


/** 连接状态变更原因 */
typedef enum
{
    kNERtcReasonConnectionChangedLeaveChannel           = 1, /**< kNERtcConnectionStateDisconnected 离开房间 */
    kNERtcReasonConnectionChangedChannelClosed          = 2, /**< kNERtcConnectionStateDisconnected 房间被关闭 */
    kNERtcReasonConnectionChangedBeKicked               = 3, /**< kNERtcConnectionStateDisconnected 用户被踢 */
    kNERtcReasonConnectionChangedTimeOut                = 4, /**< kNERtcConnectionStateDisconnected	服务超时 */
    kNERtcReasonConnectionChangedJoinChannel            = 5, /**< kNERtcConnectionStateConnecting 加入房间 */
    kNERtcReasonConnectionChangedJoinSucceed            = 6, /**< kNERtcConnectionStateConnected 加入房间成功 */
    kNERtcReasonConnectionChangedReJoinSucceed          = 7, /**< kNERtcConnectionStateConnected 重新加入房间成功（重连） */
    kNERtcReasonConnectionChangedMediaConnectionDisconnected = 8, /**< kNERtcConnectionStateReconnecting 媒体连接断开 */
    kNERtcReasonConnectionChangedSignalDisconnected     = 9, /**< kNERtcConnectionStateReconnecting 信令连接断开 */
    kNERtcReasonConnectionChangedRequestChannelFailed   = 10, /**< kNERtcConnectionStateFailed 请求房间失败 */
    kNERtcReasonConnectionChangedJoinChannelFailed      = 11, /**< kNERtcConnectionStateFailed 加入房间失败 */
} NERtcReasonConnectionChangedType;

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
    int down_rtt;	            /**< 下行平均往返时延rtt(ms) */
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

/** 本地视频单条流上传统计信息。*/
struct NERtcVideoLayerSendStats
{
    int layer_type;         /**< 流类型： 1、主流，2、辅流。*/
    int width;              /**< 视频流宽（像素）。*/
    int height;             /**< 视频流高（像素）。*/
    int capture_frame_rate; /**< 视频采集帧率。*/
    int render_frame_rate;  /**< 视频渲染帧率。*/
    int encoder_frame_rate;	/**< 编码帧率。*/
    int sent_frame_rate;	/**< 发送帧率。*/
    int sent_bitrate;		/**< 发送码率(Kbps)。*/
    int target_bitrate;		/**< 编码器目标码率(Kbps)。*/
    int encoder_bitrate;	/**< 编码器实际编码码率(Kbps)。*/
    char codec_name[kNERtcMaxDeviceNameLength]; /**< 视频编码器名字。*/
};
/** 本地视频流上传统计信息。*/
struct NERtcVideoSendStats
{
    NERtcVideoLayerSendStats* video_layers_list;	/**< 视频流信息数组。*/
    int video_layers_count;	                        /**< 视频流条数。*/
};

/** 远端视频单条流的统计信息。*/
struct NERtcVideoLayerRecvStats
{
    int layer_type;         /**< 流类型： 1、主流，2、辅流。*/
    int width;              /**< 视频流宽（像素）。*/
    int height;             /**< 视频流高（像素）。*/
    int received_bitrate;   /**< 接收到的码率(Kbps)。*/
    int received_frame_rate;    /**< 接收到的帧率 (fps)。*/
    int decoder_frame_rate;	/**< 解码帧率 (fps)。*/
    int render_frame_rate;	/**< 渲染帧率 (fps)。*/
    int packet_loss_rate;   /**< 下行丢包率(%)。*/
    int total_frozen_time;  /**< 用户的下行视频卡顿累计时长(ms)。*/
    int frozen_rate;        /**< 用户的下行视频平均卡顿率(%)。*/
    char codec_name[kNERtcMaxDeviceNameLength]; /**< 视频解码器名字。*/
};

/** 远端视频流的统计信息。*/
struct NERtcVideoRecvStats
{
    uid_t uid;      /**< 用户 ID，指定是哪个用户的视频流。*/
    NERtcVideoLayerRecvStats* video_layers_list;	/**< 视频流信息数组。*/
    int video_layers_count;	                        /**< 视频流条数。*/
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
    kNERtcVideoFramerateFps_60      = 60,   /**< 60帧每秒 */
} NERtcVideoFramerateType;

/** @enum NERtcDegradationPreference 视频编码策略。*/
typedef enum {
    /**
     - （默认）根据场景模式调整适应性偏好。
     - 通信场景中，选择kNERtcDegradationBalanced 模式，在编码帧率和视频质量之间保持平衡。
     - 直播场景中，选择kNERtcDegradationMaintainQuality 模式，降低编码帧率以保证视频质量。
     */
    kNERtcDegradationDefault            = 0,
    /**
     流畅优先，降低视频质量以保证编码帧率。在弱网环境下，降低视频清晰度以保证视频流畅，此时画质降低，画面会变得模糊，但可以保持视频流畅。
     */
    kNERtcDegradationMaintainFramerate  = 1,
    /**
     清晰优先，降低编码帧率以保证视频质量。在弱网环境下，降低视频帧率以保证视频清晰，此时可能会出现一定卡顿。
     */
    kNERtcDegradationMaintainQuality    = 2,
    /**
     在编码帧率和视频质量之间保持平衡。
     */
    kNERtcDegradationBalanced           = 3,
} NERtcDegradationPreference;

/** 视频配置的属性。*/
struct NERtcVideoConfig
{
    /**
     视频编码的分辨率，用于衡量编码质量。详细信息请参考 NERtcVideoProfileType。
     */
    NERtcVideoProfileType max_profile;
    /**
     视频编码分辨率，衡量编码质量，以宽x高表示。与maxProfile属性二选一。

     width表示视频帧在横轴上的像素，即自定义宽。
     - 设置为负数时表示采用 max_profile 档位。
     - 如果需要自定义分辨率场景，则设置此属性，maxProfile属性失效。

     自定义视频输入width和height无效，会自动根据 maxProfile 缩放。
     */
    uint32_t width;
    /**
     视频编码分辨率，衡量编码质量，以宽x高表示。与maxProfile属性二选一。

     height表示视频帧在纵轴上的像素，即自定义高。
     - 设置为负数时表示采用 max_profile 档位。
     - 如果需要自定义分辨率场景，则设置此属性，maxProfile属性失效。

     自定义视频输入width和height无效，会自动根据 maxProfile 缩放。
     */
    uint32_t height;
    /**
     视频裁剪模式，宽高比。默认为 kNERtcVideoCropModeDefault。详细信息请参考 NERtcVideoCropMode。
     */
    NERtcVideoCropMode crop_mode_;
    /**
     主流的视频编码的帧率。详细信息请参考 NERtcVideoFramerateType。默认根据设置的maxProfile决定帧率。
     - max_profile >= STANDARD，frameRate = FRAME_RATE_FPS_30 。
     - max_profile < STANDARD，frameRate = FRAME_RATE_FPS_15 。
     */
    NERtcVideoFramerateType framerate;
    /**
     视频编码的最小帧率。默认为 0，表示使用默认最小帧率
     */
    NERtcVideoFramerateType min_framerate;
    uint32_t bitrate;                   /**< 视频编码码率kbps，取0时使用默认值 */
    /**
     视频编码的最小码率，单位为 Kbps。您可以根据场景需要，手动设置想要的最小码率，若设置为0，SDK 将会自行计算处理。
     */
    uint32_t min_bitrate;
    /**
     带宽受限时的视频编码降级偏好。详细信息请参考 NERtcDegradationPreference。
     */
    NERtcDegradationPreference degradation_preference;
};

/** 视频帧数据回调

    @param  uid  用户id
    @param  data 数据指针
    @param  type 数据类型NERtcVideoType
    @param  width 宽度
    @param  height 高度
    @param  count 数据类型个数，即offset及stride的数目
    @param  offset 每类数据偏移
    @param  stride 每类数据步进
    @param  rotation 画面旋转角度NERtcVideoRotation
    @param  user_data 用户透传数据

    @return
*/
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
/** 屏幕共享功能的编码策略倾向

- kNERtcSubStreamContentPreferMotion: 内容类型为动画。当共享的内容是视频、电影或游戏时，推荐选择该内容类型
当用户设置内容类型为动画时，按用户设置的帧率处理
    
- kNERtcSubStreamContentPreferDetails: 内容类型为细节。当共享的内容是图片或文字时，推荐选择该内容类型
当用户设置内容类型为细节时，最高允许用户设置到10帧，设置超过10帧时，不生效，按10帧处理

 */

typedef enum
{
    kNERtcSubStreamContentPreferMotion  = 0,    /**< 动画模式。*/
    kNERtcSubStreamContentPreferDetails = 1,    /**< 细节模式。*/
}NERtcSubStreamContentPrefer;

/** 屏幕共享编码参数配置。用于衡量编码质量。*/
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
    NERtcSubStreamContentPrefer prefer; /**< 编码策略倾向。*/
};

/** 视频显示设置 */
struct NERtcVideoCanvas
{
    /**
     数据回调。详细信息请参考 onFrameDataCallback。

     在 macosx中，需要设置 NERtcEngineContex t的 video_use_exnternal_render 为 true 才有效。
     */
    onFrameDataCallback cb;
    /**
     数据回调的用户透传数据。

     在 macosx中，需要设置 NERtcEngineContex t的 video_use_exnternal_render 为 true 才有效。
     */
    void *user_data;
    /**
     渲染窗口句柄。

     在 macosx中，需要设置 NERtcEngineContex t的 video_use_exnternal_render 为 false 才有效。
     */
    void *window;
    /**
     视频显示模式，详细信息请参考 NERtcVideoScalingMode。
     */
    NERtcVideoScalingMode scaling_mode;
};

/** 录制类型。*/
typedef enum
{
    kNERtcRecordTypeAll = 0,    /**< 参与合流+单流录制。*/
    kNERtcRecordTypeMix = 1,    /**< 参与合流录制模式。*/
    kNERtcRecordTypeSingle = 2, /**< 参与单流录制模式。*/
} NERtcRecordType;

/** 音频类型。*/
typedef enum
{
    kNERtcAudioTypePCM16 = 0,    /**< PCM 音频格式。*/
} NERtcAudioType;

/** 音频帧请求数据的读写模式。*/
typedef enum {
	kNERtcRawAudioFrameOpModeReadOnly = 0,    /**< 返回数据只读模式 */
	kNERtcRawAudioFrameOpModeReadWrite,       /**< 返回数据可读写 */
} NERtcRawAudioFrameOpModeType;

/** 音频帧请求格式。*/
struct NERtcAudioFrameRequestFormat
{
    uint32_t     channels;      /**< 音频声道数量。如果是立体声，数据是交叉的。单声道: 1；双声道 : 2。*/
    uint32_t     sample_rate;   /**< 采样率。*/
	NERtcRawAudioFrameOpModeType mode = kNERtcRawAudioFrameOpModeReadWrite; /**<读写模式 */
};

/** 音频格式。*/
struct NERtcAudioFormat {
    NERtcAudioType type; /**< 音频类型。*/
    uint32_t channels; /**< 音频声道数量。如果是立体声，数据是交叉的。单声道: 1；双声道 : 2。*/
    uint32_t sample_rate; /**< 采样率。*/
    uint32_t bytes_per_sample; /**< 每个采样点的字节数。对于 PCM 来说，一般使用 16 bit，即两个字节。*/
    uint32_t samples_per_channel; /**< 每个房间的样本数量。*/
};

/** 音频帧。*/
struct NERtcAudioFrame {
    NERtcAudioFormat format;    /**< 音频格式。*/
    void *data;     /**< 数据缓冲区。有效数据长度为：samples_per_channel * channels * bytes_per_sample。*/
};

/** 语音观测器对象。
 
 部分接口允许修改 frame 里 void *data 所指向的内容，但不允许修改 format。如果对 format 有要求，需调用相应设置接口。*/
class INERtcAudioFrameObserver
{
public:
    virtual ~INERtcAudioFrameObserver() {}
    /** 采集音频数据回调，用于声音处理等操作。

     @note
     - 返回音频数据支持读/写。
     - 有本地音频数据驱动就会回调。

     @param frame 音频帧。
     */
    virtual void onAudioFrameDidRecord(NERtcAudioFrame *frame) = 0;
    /** 播放音频数据回调，用于声音处理等操作。
    
     @note
     - 返回音频数据支持读/写。
     - 有本地音频数据驱动就会回调。

     @param frame 音频帧。
     */
    virtual void onAudioFrameWillPlayback(NERtcAudioFrame *frame) = 0;
    /** 获取本地用户和所有远端用户混音后的原始音频数据。

     @note
     - 返回音频数据只读。
     - 有本地音频数据驱动就会回调。

     @param frame 音频帧。
     */
    virtual void onMixedAudioFrame(NERtcAudioFrame * frame) = 0;
    /** 
     * 获取单个远端用户混音前的音频数据。
     * 
     * 成功注册音频观测器后，如果订阅了远端音频（默认订阅）且远端用户开启音频后，SDK 会在捕捉到混音前的音频数据时，触发该回调，将音频数据回调给用户。
    
     @note
     - 返回音频数据只读。

     @param userID 用户ID。
     @param frame  音频帧。
     */
    virtual void onPlaybackAudioFrameBeforeMixing(uint64_t userID, NERtcAudioFrame * frame) = 0;
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
    NERtcVideoType format;      /**< 视频帧格式，详细信息请参考 NERtcVideoType。*/
    uint64_t timestamp;         /**< 视频时间戳，单位为毫秒。 */
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
    /**
     待播放的音乐文件的绝对路径或 URL 地址。
     */
    char path[kNERtcMaxURILength];
    /**
     伴音循环播放的次数：
     - 1：（默认）播放音效一次。
     - ≤ 0：无限循环播放音效，直至调用 stopEffect 或 stopAllEffects 后停止。
     */
    int loop_count;
    /**
     是否将伴音发送远端，默认为 true，即远端用户订阅本端音频流后可听到该伴音。
     */
    bool send_enabled;
    /**
     音乐文件的发送音量，取值范围为 0~100。默认为 100，表示使用文件的原始音量。
     */
    uint32_t send_volume;
    /**
     是否可播放。默认为 true，即可在本地播放该伴音
     */
    bool playback_enabled;
    /**
     音乐文件的播放音量，取值范围为 0~100。默认为 100，表示使用文件的原始音量。
     */
    uint32_t playback_volume;
};

/** 创建音效的配置项 */
struct NERtcCreateAudioEffectOption
{
    /**
     待播放的音乐文件的绝对路径或 URL 地址。
     */
    char path[kNERtcMaxURILength];
    /**
     音效循环播放的次数：
     - 1：（默认）播放音效一次。
     - ≤ 0：无限循环播放音效，直至调用 stopEffect 或 stopAllEffects 后停止。
     */
    int loop_count;
    /**
     是否将伴音发送远端，默认为 true，即远端用户订阅本端音频流后可听到该伴音。
     */
    bool send_enabled;
    /**
     音乐文件的发送音量，取值范围为 0~100。默认为 100，表示使用文件的原始音量。
     */
    uint32_t send_volume;
    /**
     是否可播放。默认为 true，即可在本地播放该音效。
     */
    bool playback_enabled;
    /**
     音乐文件的播放音量，取值范围为 0~100。默认为 100，表示使用文件的原始音量。
     */
    uint32_t playback_volume;
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

/**
 SEI发送的流通道类型
 */
typedef enum {
    kNERtcStreamChannelTypeMainStream   = 0, /**< 主流通道 */
    kNERtcStreamChannelTypeSubStream    = 1, /**< 辅流通道 */
} NERtcStreamChannelType;

/** 通过 JSON 配置 SDK 提供技术预览或特别定制功能。以标准化方式公开 JSON 选项。详见API setParameters*/
#define kNERtcKeyRecordHostEnabled          "record_host_enabled"          /**< bool value. true: 录制主讲人, false: 不是录制主讲人。通话前设置有效 */
#define kNERtcKeyRecordAudioEnabled         "record_audio_enabled"         /**< bool value，启用服务器音频录制。默认值 false。通话前设置有效 */
#define kNERtcKeyRecordVideoEnabled         "record_video_enabled"         /**< bool value，启用服务器视频录制。默认值 false。通话前设置有效 */
#define kNERtcKeyRecordType                 "record_type"                  /**< int value, NERtcRecordType。通话前设置有效 */
#define kNERtcKeyAutoSubscribeAudio         "auto_subscribe_audio"         /**< bool value，其他用户打开音频时，自动订阅。默认值 true。通话前设置有效 */
#define kNERtcKeyPublishSelfStreamEnabled   "publish_self_stream_enabled"   /**< bool value，开启旁路直播。默认值 false。通话前设置有效 */
#define kNERtcKeyLogLevel                   "log_level"                     /**< int value, NERtcLogLevel，SDK 输出小于或等于该级别的log，默认为 kNERtcLogLevelInfo */
#define kNERtcKeyAudioProcessingAECEnable   "audio_processing_aec_enable"   /**< bool value. AEC开关，默认值 true */
#define kNERtcKeyAudioAECLowLevelEnable     "audio_aec_low_level_enable" /**< bool value. low level AEC开关，默认值 false,需要kNERtcKeyAudioProcessingAECEnable打开才生效 */
#define kNERtcKeyAudioProcessingAGCEnable   "audio_processing_agc_enable"   /**< bool value. AGC开关，默认值 true */
#define kNERtcKeyAudioProcessingNSEnable    "audio_processing_ns_enable"   /**< bool value. NS开关，默认值 true */
#define kNERtcKeyAudioProcessingAINSEnable  "audio_processing_ai_ns_enable"   /**< bool value.AI  NS开关，建议通话前修改，默认值 false */
#define kNERtcKeyAudioProcessingExternalAudioMixEnable  "audio_processing_external_audiomix_enable"   /**< bool value. 输入混音开关，默认值 false */
#define kNERtcKeyAudioProcessingEarphone    "audio_processing_earphone"    /**< bool value. 通知SDK是否使用耳机， true: 使用耳机, false: 不使用耳机，默认值 false */
#define kNERtcKeyVideoSendOnPubType         "video_sendonpub_type"      /**< int value. NERtcSendOnPubType；设置视频发送策略，默认发送大流 kNERtcSendOnPubHigh。通话前设置有效 */
#define kNERtcKeyTest1v1                    "test_1v1"                  /**< bool value. 1v1模式开关，默认关闭。通话前设置有效 */
} // namespace nertc

#endif
