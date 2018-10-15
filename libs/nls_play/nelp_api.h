/*
*	Author       biwei
*	Date         2016/10/19
*	Copyright    Hangzhou, Netease Inc.
*	Brief
*   Notice
*/

#ifndef _NELP_API_H_
#define _NELP_API_H_

#include "nelp_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file */

/**
* @brief 获取SDK版本号
*
* @return 无
*/
EXPORTS_API const char* Nelp_GetSDKVersion();

/**
 * @brief 创建播放器实例
 * @param paLogpath:    日志文件路径(如: "D:\\log"; 若设置为NULL则会默认保存到当前的工作目录下)
 * @param phNLPService: 播放器实例，出参
 *
 * @return NELP_RET: NELP_OK 成功; NELP_ERR 失败
 */
EXPORTS_API NELP_RET Nelp_Create(const char *paLogpath, NELP_OUT _HNLPSERVICE *phNLPService);

/**
 * @brief 初始化参数，包括播放地址、缓冲模式等
 * @param hNLPService: 播放器实例
 * @param pstParam:    相关参数
 *
 * @return NELP_RET: NELP_OK 成功; NELP_ERR 失败
 */
EXPORTS_API NELP_RET Nelp_InitParam(_HNLPSERVICE hNLPService, ST_NELP_PARAM *pstParam);

/**
* @brief 播放器已经初始化且尚未关闭的情况下，立刻停止之前的播放并切换到新的url开始播放。注意：调用此接口成功后，仍需要上层在收到NELP_MSG_PREPARED后主动调用Nelp_Start。
* @param hNLPService: 播放器实例
* @param pstNewParam: 新的播放参数
*
* @return NELP_RET: NELP_OK 成功; NELP_ERR 失败
*/
EXPORTS_API NELP_RET Nelp_SwitchContentUrl(_HNLPSERVICE hNLPService, ST_NELP_PARAM *pstNewParam);

/**
* @brief 设置拉流超时时间，在prepareToPlay之前调用
* @param hNLPService: 播放器实例
* @param timeout: 超时时间（单位：秒）
*
* @return 无
*/
EXPORTS_API void Nelp_SetPlaybackTimeout(_HNLPSERVICE hNLPService, int timeout);

/**
 * @brief 初始化播放器，包括拉流，解析等。注意：为防止卡UI线程，此接口为异步接口，收到NELP_MSG_PREPARED消息才表示初始化完成。
 * @param hNLPService: 播放器实例
 *
 * @return NELP_RET: NELP_OK 成功; NELP_ERR 失败
 */
EXPORTS_API NELP_RET Nelp_PrepareToPlay(_HNLPSERVICE hNLPService);

/**
 * @brief 开始播放。注意：此接口涉及视频渲染，请在UI线程调用。建议播放器初始化完成（收到NELP_MSG_PREPARED）后再调用此接口，若调用Nelp_PrepareToPlay之后立刻调用此接口，会阻塞直到播放器初始化完成。
 * @param hNLPService: 播放器实例
 *
 * @return NELP_RET: NELP_OK 成功; NELP_ERR 失败
 */
EXPORTS_API NELP_RET Nelp_Start(_HNLPSERVICE hNLPService);

/**
 * @brief 暂停播放，用Nelp_Resume来继续播放（仅适用于点播）
 * @param hNLPService: 播放器实例
 *
 * @return 无
 */
EXPORTS_API void Nelp_Pause(_HNLPSERVICE hNLPService);

/**
* @brief 继续播放（仅适用于点播）
* @param hNLPService: 播放器实例
*
* @return 无
*/
EXPORTS_API void Nelp_Resume(_HNLPSERVICE hNLPService);

/**
* @brief 停止播放，Nelp_Start的逆操作
* @param hNLPService: 播放器实例
*
* @return 无
*/
EXPORTS_API void Nelp_Stop(_HNLPSERVICE hNLPService);

/**
 * @brief 关闭播放器并释放相关资源，Nelp_PrepareToPlay的逆操作。此接口同时会销毁_HNLPSERVICE实例，调调用完后请将句柄置空。此接口是一个同步接口。
 * @param hNLPService: 播放器实例
 *
 * @return 无
 */
EXPORTS_API void Nelp_Shutdown(_HNLPSERVICE hNLPService);

/**
* @brief 设置显示窗口句柄以及显示区域，SDK将直接在窗口上绘制视频画面，播放过程中也可多次调用此接口来更新绘制区域。注：此接口涉及视频渲染，请在UI线程调用。
* @param hNLPService:       播放器实例
* @param hWnd:              显示窗口句柄
* @param pstDisplayRect:    绘制区域
*
* @return NELP_RET: NELP_OK 成功; NELP_ERR 失败
*/
EXPORTS_API NELP_RET Nelp_SetDisplay(_HNLPSERVICE hNLPService, void* hWnd, ST_NELP_RECT* pstDisplayRect);

/**
* @brief 音量调节
* @param hNLPService: 播放器实例
* @param fVolume: 音量大小 (范围: 0.0 ~ 1.0  0.0:静音   1.0:最大)
*
* @return 无
*/
EXPORTS_API void Nelp_SetVolume(_HNLPSERVICE hNLPService, float fVolume);

/**
* @brief 静音/取消静音
* @param hNLPService: 播放器实例
* @param bIsMute: true 开启静音; false 取消静音
*
* @return 无
*/
EXPORTS_API void Nelp_SetMute(_HNLPSERVICE hNLPService, bool bIsMute);

/**
* @brief 获取播放器的当前状态
* @param hNLPService: 播放器实例
*
* @return EN_NELP_PLAYBACK_STATE: 播放器状态   返回	EN_NELP_GET_PLAYBACK_STATE_FAILED 则表示获取失败
*/
EXPORTS_API EN_NELP_PLAYBACK_STATE Nelp_GetPlaybackState(_HNLPSERVICE hNLPService);

/**
 * @brief 获取当前播放位置
 * @param hNLPService: 播放器实例
 *
 * @return 当前播放位置(单位: 秒) < 0: 获取失败
 */
EXPORTS_API long long Nelp_GetCurrentPlaybackTime(_HNLPSERVICE hNLPService);

/**
 * @brief 获取文件总时长（仅适用于点播）
 * @param hNLPService: 播放器实例
 *
 * @return 文件总时长(单位: 秒)   < 0: 获取失败
 */
EXPORTS_API long long Nelp_GetDuration(_HNLPSERVICE hNLPService);

/**
 * @brief 获取当前可播放的位置，即已缓存的位置
 * @param hNLPService: 播放器实例
 *
 * @return 当前可播放位置(单位: 秒)   < 0: 获取失败
 */
EXPORTS_API long long Nelp_GetPlayableDuration(_HNLPSERVICE hNLPService);

/**
 * @brief 指定到某一时间点播放（仅适用于点播）
 * @param hNLPService: 播放器实例
 * @param llTime: 指定的播放时间点(单位: 秒 s)
 *
 * @return NELP_RET: NELP_OK  成功    NELP_ERR  失败
 */
EXPORTS_API NELP_RET Nelp_SeekTo(_HNLPSERVICE hNLPService, long long llTime);

/**
* @brief 获取当前视频画面的ARGB格式截图，此接口不支持多线程调用
* @param hNLPService: 播放器实例
* @param ePFormat: 图片格式
*
* @return ST_NELP_PICTURE*: 包含所截图片的原始数据和信息
*/
EXPORTS_API ST_NELP_PICTURE* Nelp_GetSnapshot(_HNLPSERVICE hNLPService, EN_NELP_PICTURE_FORMAT ePFormat);

/**
 * @brief 注册获取消息的回调, 用户需要实现回调函数cb来接收相关消息
 * @param hNLPService:  播放器实例
 * @param pMessageCB:   获取消息的回调

 * @return 无
 */
EXPORTS_API void Nelp_RegisterMessageCB(_HNLPSERVICE hNLPService, PFN_NELP_MESSAGE_CB pMessageCB);

/**
 * @brief 注册获取音频帧数据的回调，用户需要实现回调函数cb来接收视频帧
 * @param hNLPService:   播放器实例
 * @param pAudioFrameCB:  获取音频帧的回调
 *
 * @return 无
 */
EXPORTS_API void Nelp_RegisterGetAudioFrameCB(_HNLPSERVICE hNLPService, PFN_NELP_AUDIO_FRAME_CB pAudioFrameCB);

/**
* @brief 注册获取视频帧数据的回调，用户需要实现回调函数cb来接收视频帧
* @param hNLPService:   播放器实例
* @param eMFormat:      回调的视频格式
* @param pVideoFrameCB:  获取视频帧的回调
*
* @return 无
*/
EXPORTS_API void Nelp_RegisterGetVideoFrameCB(_HNLPSERVICE hNLPService, EN_NELP_MEDIA_FORMAT eMFormat, PFN_NELP_VIDEO_FRAME_CB pVideoFrameCB);

/**
* @brief 注册获取资源释放结束消息的回调(资源释放是异步的), 用户需要实现回调函数cb来接收相关消息
* @param hNLPService:  播放器实例
* @param pReleaseCB:   获取资源释放结束消息的回调
*
* @return 无
*/
EXPORTS_API void Nelp_RegisterResourceReleaseSuccessCB(_HNLPSERVICE hNLPService, PFN_NELP_RESOURCE_RELEASE_SUCCESS_CB pReleaseCB);

#ifdef __cplusplus
}
#endif

#endif
