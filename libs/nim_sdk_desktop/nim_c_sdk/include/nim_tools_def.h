/** @file nim_tools_def.h
  * @brief NIM SDK提供的一些工具相关定义，主要包括获取SDK里app account对应的app data目录，计算md5等
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_TOOLS_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_TOOLS_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif
/** @enum NIMAppDataType AppData类型 */
enum NIMAppDataType
{
	kNIMAppDataTypeUnknownOtherRes	= 0,	/**< 其他资源文件（除了消息历史文件和已知类型之外的消息数据缓存文件） */
	kNIMAppDataTypeImage			= 1,	/**< 图片消息文件 */
	kNIMAppDataTypeAudio			= 2,	/**< 语音消息文件 */
	kNIMAppDataTypeVideo			= 3,	/**< 视频消息文件 */
};

/** @name Trans Audio Msg Info Keys
  * @{
  */
static const char *kNIMTransAudioKeyMime			= "mime";		/**< string 语音类型 */
static const char *kNIMTransAudioKeySample			= "samp";		/**< string 语音采样率 */
static const char *kNIMTransAudioKeyAudioUrl		= "url";		/**< string 语音url*/
static const char *kNIMTransAudioKeyDuration		= "dur";		/**< long 语音时长，毫秒*/
/** @}*/ //Trans Audio Msg Info Keys

/** @typedef void (*nim_tool_get_audio_text_cb_func)(const char *text, const char *json_extension, const void *user_data)
  * nim callback function for get audio text
  * @param[out] rescode 		转换结果，成功200
  * @param[out] text 			语音文字
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_tool_get_audio_text_cb_func)(int rescode, const char *text, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_TOOLS_DEF_H_