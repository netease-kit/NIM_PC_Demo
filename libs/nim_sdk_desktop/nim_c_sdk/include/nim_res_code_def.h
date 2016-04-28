/** @file nim_res_code_def.h
  * @brief NIM SDK提供给外部使用的错误号定义（包含客户端自定义和服务器返回的所有错误号）
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_RES_CODE_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_RES_CODE_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif
/** @enum NIMResCode 返回的错误号（只定义需要客户端处理的） */
enum NIMResCode
{
	//通用错误码
	kNIMResSuccess				= 200,		/**< 没有错误，一切正常 */
	kNIMResVersionError			= 201,		/**< 客户端版本不正确 */
	kNIMResUidPassError			= 302,		/**< 用户名或密码错误 */
	kNIMResForbidden			= 403,		/**< 禁止操作 */
	kNIMResNotExist				= 404,		/**< 请求的目标（用户或对象）不存在 */
	kNIMResNoModify				= 406,		/**< 数据自上次查询以来未发生变化（用于增量更新） */
	kNIMResTimeoutError			= 408,		/**< 请求过程超时 */
	kNIMResParameterError		= 414,		/**< 参数错误 */
	kNIMResConnectionError		= 415,		/**< 网络连接出现错误 */
	kNIMResFrequently			= 416,		/**< 操作太过频繁 */
	kNIMResExist				= 417,		/**< 对象已经存在 */
	kNIMResAccountBlock			= 422,		/**< 帐号被禁用 */
	kNIMResUnknownError			= 500,		/**< 未知错误，或者不方便告诉你 */
	kNIMResServerDataError		= 501,		/**< 服务器数据错误 */
	kNIMResTooBuzy				= 503,		/**< 服务器太忙 */
	kNIMResInvalid				= 509,		/**< 协议无效, 不允许访问的协议 */
	//群错误码
	kNIMResTeamECountLimit		= 801,		/**< 已达到人数限制 */
	kNIMResTeamENAccess			= 802,		/**< 没有权限 */
	kNIMResTeamENotExist		= 803,		/**< 群不存在 */
	kNIMResTeamEMemberNotExist	= 804,		/**< 用户不在兴趣组里面 */
	kNIMResTeamErrType			= 805,		/**< 群类型不对 */
	kNIMResTeamLimit			= 806,		/**< 创建群数量限制 */
	kNIMResTeamUserStatusErr	= 807,		/**< 群成员状态不对 */
	kNIMResTeamApplySuccess		= 808,		/**< 申请成功 */
	kNIMResTeamAlreadyIn		= 809,		/**< 已经在群里 */
	kNIMResTeamInviteSuccess	= 810,		/**< 邀请成功 */
	//数据解编错误代码
	kNIMResInvalidProtocol		= 997,		/**< 协议已失效 */
	kNIMResEUnpacket			= 998,		/**< 解包错误 */
	kNIMResEPacket				= 999,		/**< 打包错误 */
	//客户端自定义的错误号
	kNIMLocalRes					= 10000,		/**< 值大于该错误号的都是客户端自定义的错误号。不能随意更改其值！ */

	//客户端自定义的消息错误号
	kNIMLocalResMsgNosUploadCancel	= 10200,		/**< 发送文件消息，NOS上传暂停 */
	kNIMLocalResMsgNosDownloadCancel = 10206,		/**< 收到文件消息，NOS下载暂停 */
	kNIMLocalResMsgFileNotExist		= 10404,		/**< 本地资源不存在 */
	kNIMLocalResMsgUrlInvalid		= 10414,		/**< 收到消息，资源下载地址无效，无法下载 */
	kNIMLocalResMsgFileExist		= 10417,		/**< 收到消息，本地资源已存在，不需要重复下载 */
	kNIMLocalResParaError			= 10450,		/**< 调用api，传入的参数有误 */
	kNIMLocalResMsgSendNosError		= 10502,		/**< 发送消息，上传NOS失败 */
	kNIMLocalResCheckMsgDBFailed	= 10600,		/**< 导入消息历史时验证身份和加密密钥不通过 */
	kNIMLocalResImportMsgDBFailed	= 10601,		/**< 导入消息历史时写记录失败 */
	//客户端自定义的RTS错误号
	kNIMLocalResRtsError			= 11100,		/**< rts会话 未知错误 */
	kNIMLocalResRtsIdNotExist		= 11101,		/**< rts会话 id不存在 */
	kNIMLocalResRtsVChatExist		= 11417,		/**< rts会话 音视频已存在 */
	kNIMLocalResRtsStatusError		= 11501,		/**< rts会话 通道状态不正确 */
	kNIMLocalResRtsChannelNotExist	= 11510,		/**< rts会话 通道不存在 */

	//客户端自定义的api调用问题
	kNIMLocalResAPIErrorInitUndone	= 20000,		/**< 还未初始化或初始化未正常完成 */
	kNIMLocalResAPIErrorLoginUndone	= 20001,		/**< 还未登陆或登录未完成 */
	kNIMLocalResAPIErrorLogined		= 20002,		/**< 已经登录 */
	kNIMLocalResAPIErrorVersionError= 20003,		/**< SDK版本不对，可能会引发其他问题 */
};

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_RES_CODE_DEF_H_