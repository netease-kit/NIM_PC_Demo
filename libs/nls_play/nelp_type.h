/*
*	Author       biwei
*	Date         2016/10/19
*	Copyright    Hangzhou, Netease Inc.
*	Brief
*   Notice
*/

#ifndef _NELP_TYPE_H_
#define _NELP_TYPE_H_

#include "nelp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file */

typedef  NELP_MASK_HANDLE_TYPE(_HNLPSERVICE)  _HNLPSERVICE;

#define NELP_MSG_ERROR                       100   //!< 播放发生错误
#define NELP_MSG_PREPARED                    200   //!< 播放器预处理完成
#define NELP_MSG_COMPLETED                   300   //!< 播放完成
#define NELP_MSG_VIDEO_SIZE_CHANGED          400   //!< 视频分辨率发生变化
#define NELP_MSG_VIDEO_RENDERING_START       402   //!< 第一帧视频显示
#define NELP_MSG_AUDIO_RENDERING_START       403   //!< 第一帧音频播放
#define NELP_MSG_BUFFERING_START             500   //!< 开始缓冲
#define NELP_MSG_BUFFERING_END               501   //!< 缓冲结束
#define NELP_MSG_SEEK_COMPLETED              600   //!< seek完成
#define NELP_MSG_PLAYBACK_STATE_CHANGED      700   //!< 播放器状态发生变化
#define NELP_MSG_VIDEO_PARSE_ERROR           800   //!< 视频流解析失败
#define NELP_MSG_AUDIO_DEVICE_OPEN_FAILED    900   //!< 声卡打开失败


/**
 * @播放缓冲模式
 */
typedef enum enum_NELP_BUFFER_STRATEGY{
	EN_NELP_TOP_SPEED,  //!< 网络直播极速模式，仅适用于视频直播，延时最小，网络差时会有卡顿
	EN_NELP_LOW_DELAY,  //!< 网络直播低延时模式，仅适用于视频直播,延时较小，网络不好时偶尔会卡顿
	EN_NELP_FLUENT,     //!< 网络直播流畅模式，仅适用于视频直播，延时比低延时模式稍大一些，但是流畅性较好
	EN_NELP_ANTI_JITTER //!< 网络点播抗抖动模式，适用于视频点播
} EN_NELP_BUFFER_STRATEGY;


/**
 * @brief 播放器初始化参数
 */
typedef struct struct_NELP_PARAM
{
	char                    *paPlayUrl;        //!< 播放地址
	EN_NELP_BUFFER_STRATEGY  enBufferStrategy; //!< 缓冲模式
} ST_NELP_PARAM;


/**
 * @brief 消息结构体
 */
typedef struct struct_NELP_MESSAGE
{
	int iWhat;  //!< 消息类型
	int iArg1;  //!< 参数1
	int iArg2;  //!< 参数2
} ST_NELP_MESSAGE;


/**
* @brief RECT结构体
*/
typedef struct struct_NELP_RECT
{
	int iLeft;    //!< 左上角x坐标
	int iTop;     //!< 左上角y坐标
	int iWidth;   //!< 宽度
	int iHeight;  //!< 高度
} ST_NELP_RECT;


/**
* @brief 回调的视频数据结构
*/
typedef struct struct_NELP_AUDIO_FRAME
{
	int                   iChannels;      //!< 通道数
	int                   iSamplerate;     //!< 采样率
	int                   iSize;             //!<长度
	unsigned char        *puaUsrData;  //!<音频数据
} ST_NELP_AUDIO_FRAME;

/**
 * @brief 回调的视频数据格式
 */
typedef enum enum_NELP_MEDIA_FORMAT
{
	EN_YUV420,  //!< YUV420格式
	EN_ARGB8888 //!< ARGB8888格式
} EN_NELP_MEDIA_FORMAT;


/**
 * @brief 回调的视频数据结构
 */
typedef struct struct_NELP_FRAME
{
	int                   iWidth;      //!< 视频宽度
	int                   iHeight;     //!< 视频高度
	unsigned char        *puaUsrData;  //!< 视频数据
	EN_NELP_MEDIA_FORMAT  enMFormat;   //!< 视频格式
} ST_NELP_FRAME;


/**
 * @brief 截图的图片数据格式
 */
typedef enum enum_NELP_PICTURE_FORMAT
{
	EN_PIC_RGB888,   //!< RGB888格式
	EN_PIC_ARGB8888  //!< ARGB8888格式
} EN_NELP_PICTURE_FORMAT;


/**
 * @brief 截图的图片数据结构
 */
typedef struct struct_NELP_PICTURE
{
	int                     iWidth;     //!< 图片宽度
	int                     iHeight;    //!< 图片高度
	unsigned char          *puaUsrData; //!< 图片数据
	EN_NELP_PICTURE_FORMAT  enPFormat;  //!< 图片格式
} ST_NELP_PICTURE;

/**
 * @brief 播放状态
 */
typedef enum enum_NELP_PLAYBACK_STATE
{
	EN_NELP_GET_PLAYBACK_STATE_FAILED = -1,   //!< 获取失败
	EN_NELP_PLAYBACK_STATE_STOPPED,           //!< 停止状态
	EN_NELP_PLAYBACK_STATE_PLAYING,           //!< 播放状态
	EN_NELP_PLAYBACK_STATE_PAUSED,            //!< 暂停状态
	EN_NELP_PLAYBACK_STATE_SEEKING            //!< seek状态
} EN_NELP_PLAYBACK_STATE;


/**
 *  @brief 播放过程中的消息回调
 *
 *  @param frame: 回调的视频信息
 *  @param msg: 消息体
 *
 *  @return 无
 */
typedef void(*PFN_NELP_MESSAGE_CB)(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);

/**
 *  @brief 视频数据的回调
 *
 *  @param frame: 回调的视频帧结构体指针，若为NULL表示数据格式转换失败
 *
 *  @return 无
 */
typedef void(*PFN_NELP_VIDEO_FRAME_CB)(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame);

/**
*  @brief 音频数据的回调
*
*  @param frame: 回调的视频信息
*  @param frame: 回调的视频信息
*
*  @return 无
*/
typedef void(*PFN_NELP_AUDIO_FRAME_CB)(_HNLPSERVICE hNLPService, ST_NELP_AUDIO_FRAME *frame);

/**
 *  @brief 资源释放结束的回调
 */
typedef void(*PFN_NELP_RESOURCE_RELEASE_SUCCESS_CB)(_HNLPSERVICE hNLPService);


#ifdef __cplusplus
}
#endif

#endif//_NE_LIVEPLAYER_H_
