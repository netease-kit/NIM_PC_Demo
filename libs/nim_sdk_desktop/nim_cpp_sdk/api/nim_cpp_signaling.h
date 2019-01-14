/** @file nim_cpp_signaling.h
  * @brief 独立信令
  * @copyright (c) 2015-2018, NetEase Inc. All rights reserved
  * @author gq
  * @date 2018/11/5
  */

#ifndef _NIM_SDK_CPP_AVSIGNAL_H_
#define _NIM_SDK_CPP_AVSIGNAL_H_

#include "nim_signaling_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/**
* @class Signaling
* @brief namespace nim
*/
class NIM_SDK_CPPWRAPPER_DLL_API Signaling
{

public:
	typedef std::function<void(std::shared_ptr<SignalingNotityInfo>)> SignalingNotifyCallback;	/**< 单条通知回调 */
	typedef std::function<void(std::list<std::shared_ptr<SignalingNotityInfo>>)> SignalingNotifyListCallback;	/**< 多条通知回调 */
	typedef std::function<void(SignalingChannelDetailedinfo)> SignalingChannelCallback;	/**< 频道信息回调 */
	typedef std::function<void(std::list<SignalingChannelDetailedinfo>)> SignalingChannelListCallback;	/**< 频道列表通知回调 */
	typedef std::function<void(int, std::shared_ptr<SignalingResParam>)> SignalingOptCallback;				/**< 信令操作回调 */

	/** @fn static void RegOnlineNotifyCb(SignalingNotifyCallback cb)
	* @brief 注册独立信令的在线通知回调接口
	* @param[in] cb 通知回调，返回SignalingNotityInfo的子类
	* @return void 无返回值
	*/
	static void RegOnlineNotifyCb(SignalingNotifyCallback cb);

	/** @fn static void RegMutilClientSyncNotifyCb(SignalingNotifyCallback cb)
	* @brief 注册独立信令的多端同步通知回调接口，用于通知信令相关的多端同步通知。比如自己在手机端接受邀请，PC端会同步收到这个通知
	* @param[in] cb 通知回调，返回SignalingNotityInfo的子类
	* @return void 无返回值
	*/
	static void RegMutilClientSyncNotifyCb(SignalingNotifyCallback cb);

	/** @fn static void RegOfflineNotifyCb(SignalingNotifyListCallback cb)
	* @brief 注册独立信令的离线通知回调接口
	* @note 需要用户在调用相关接口时，打开存离线的开关。如果用户已经接收消息，该通知会在服务器标记已读，之后不会再收到该消息。
	* @param[in] cb 通知回调，返回SignalingNotityInfo子类的list
	* @return void 无返回值
	*/
	static void RegOfflineNotifyCb(SignalingNotifyListCallback cb);

	/** @fn static void RegChannelsSyncCb(SignalingChannelListCallback cb)
	* @brief 注册独立信令的频道列表同步回调接口
	* @note 在login或者relogin后，会通知该设备账号还未退出的频道列表，用于同步；如果没有在任何频道中，也会返回该同步通知，list为空
	* @param[in] cb 通知回调
	* @return void 无返回值
	*/
	static void RegChannelsSyncCb(SignalingChannelListCallback cb);

	/** @fn static void RegMembersSyncCb(SignalingChannelCallback cb)
	* @brief 注册独立信令的频道成员变更同步回调接口
	* @note 用于同步频道内的成员列表变更，当前该接口为定时接口，2分钟同步一次，成员有变化时才上报。\n
	* 由于一些特殊情况，导致成员在离开或掉线前没有主动调用离开频道接口，使得该成员的离开没有对应的离开通知事件，由该回调接口【频道成员变更同步通知】告知用户。
	* @param[in] cb 通知回调
	* @return void 无返回值
	*/
	static void RegMembersSyncCb(SignalingChannelCallback cb);

	/** @fn static void SignalingCreate(SignalingCreateParam param, SignalingOptCallback cb)
	* @brief 独立信令 创建频道
	* @note
	* 该接口用户创建频道，同一时刻频道名互斥，不能重复创建。但如果频道名缺省，服务器会自动分配频道id。\n
	* 对于频道在创建后如果没人加入，有效期2小时，当有成员加入后会自动延续频道有效期。当主动关闭频道或者最后一个成员退出后2小时后频道销毁。
	* @param[in] param 创建频道的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingCreateResParam
	* @return void 无返回值\n
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10405:频道已存在;
	*/
	static void SignalingCreate(SignalingCreateParam param, SignalingOptCallback cb);

	/** @fn static void SignalingClose(SignalingCloseParam param, SignalingOptCallback cb)
	* @brief 独立信令 关闭销毁频道
	* @note 整个通话结束，如果只单人退出请调用Leave接口\n
	* 该接口可以由创建者和频道内所有成员调用，无权限限制。\n
	* 调用该接口成功后，其他所有频道内的成员都回收到频道结束的通知，被动离开频道。此时其他成员不需要调用离开接口，也不会收到别人的离开通知。
	* @param[in] param 创建频道的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingCloseResParam
	* @return void 无返回值\n
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10406:不在频道内;
	*/
	static void SignalingClose(SignalingCloseParam param, SignalingOptCallback cb);

	/** @fn static void Join(SignalingJoinParam param, SignalingOptCallback cb)
	* @brief 独立信令 加入频道接口
	* @param[in] param 加入频道的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingJoinResParam
	* @return void 无返回值\n
	* 错误码：		200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10407:已经在频道内;
	*			  10417:uid冲突;
	*			  10419:频道人数超限;
	*		      10420:已经在频道内（自己的其他端）;
	*/
	static void Join(SignalingJoinParam param, SignalingOptCallback cb);

	/** @fn static void Leave(SignalingLeaveParam param, SignalingOptCallback cb)
	* @brief 独立信令 离开频道接口
	* @param[in] param 离开频道的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingLeaveResParam
	* @return void 无返回值\n
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10406:不在频道内;
	*/
	static void Leave(SignalingLeaveParam param, SignalingOptCallback cb);

	/** @fn static void Call(SignalingCallParam param, SignalingOptCallback cb)
	* @brief 独立信令 呼叫接口
	* @note 用于用户新开一个频道并邀请对方加入频道，如果返回码不是200、10201、10202时，sdk会主动关闭频道，标记接口调用失败\n
	* 该接口为组合接口，等同于用户先创建频道，成功后加入频道并邀请对方
	* @param[in] param 呼叫的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingCallResParam
	* @return void 无返回值
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10405:频道已存在;
	*			  10201:对方云信不在线，如果打开存离线开关，可认为发送成功，对方可收到离线消息;
	*			  10202:对方推送亦不可达，如果打开存离线开关，可认为发送成功，对方可收到离线消息;
	*/
	static void Call(SignalingCallParam param, SignalingOptCallback cb);

	/** @fn static void Invite(SignalingInviteParam param, SignalingOptCallback cb)
	* @brief 独立信令 邀请接口
	* @note 该接口用于邀请对方加入频道，邀请者必须是创建者或者是频道中成员。\n
	* 如果需要对离线成员邀请，可以打开离线邀请开关并填写推送信息。被邀请者在线后通过离线通知接收到该邀请，并通过频道信息中的invalid_字段判断频道的有效性，也可以对所有离线消息处理后判断该邀请是否被取消。
	* @param[in] param 邀请的传入参数，其中邀请标识由用户生成，在取消邀请和邀请通知及接受拒绝时做对应
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingInviteResParam
	* @return void 无返回值
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10406:不在频道内（自己）;
	*			  10407:已经在频道内（对方）;
	*			  10419:频道人数超限;
	*			  10201:对方云信不在线，如果打开存离线开关，可认为发送成功，对方可收到离线消息;
	*			  10202:对方推送亦不可达，如果打开存离线开关，可认为发送成功，对方可收到离线消息;
	*/
	static void Invite(SignalingInviteParam param, SignalingOptCallback cb);

	/** @fn static void CancelInvite(SignalingCancelInviteParam param, SignalingOptCallback cb)
	* @brief 独立信令 取消邀请接口
	* @param[in] param 取消邀请的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingCancelInviteResParam
	* @return void 无返回值
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10408:邀请不存在或已过期;
	*			  10409:邀请已经拒绝;
	*			  10410:邀请已经接受了;
	*/
	static void CancelInvite(SignalingCancelInviteParam param, SignalingOptCallback cb);

	/** @fn static void Reject(SignalingRejectParam param, SignalingOptCallback cb)
	* @brief 独立信令 拒绝邀请接口
	* @note 拒绝邀请后用户也可以通过加入频道接口加入频道，接口的使用由用户的业务决定
	* @param[in] param 拒绝邀请的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingRejectResParam
	* @return void 无返回值
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10408:邀请不存在或已过期;
	*			  10409:邀请已经拒绝;
	*			  10410:邀请已经接受了;
	*			  10201:对方不在线;
	*/
	static void Reject(SignalingRejectParam param, SignalingOptCallback cb);

	/** @fn static void Accept(SignalingAcceptParam param, SignalingOptCallback cb)
	* @brief 独立信令 接受邀请接口
	* @note 不开自动加入开关：该接口只接受邀请并告知邀请者，并同步通知自己的其他在线设备，但不会主动加入频道，需要单独调用加入接口\n
	* 打开自动加入开关：该接口为组合接口，等同于先调用接受邀请，成功后再加入频道。\n
	* 该接口在打开自动加入开关后是组合接口，对应的通知会有2个，接收邀请通知和加入通知
	* @param[in] param 接受邀请的传入参数
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingAcceptResParam，在不自动加入时SignalingAcceptResParam无效
	* @return void 无返回值
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10408:邀请不存在或已过期;
	*			  10409:邀请已经拒绝;
	*			  10410:邀请已经接受了;
	*			  10201:对方不在线;
	*/
	static void Accept(SignalingAcceptParam param, SignalingOptCallback cb);

	/** @fn static void Control(SignalingControlParam param, SignalingOptCallback cb)
	* @brief 独立信令 用户自定义控制指令接口
	* @note 该接口用于在频道中透传一些自定义指令，协助频道管理。该接口允许非频道内成员调用，但接收者必须是频道内成员或创建者
	* @param[in] param 控制通知的传入参数，接收方to_填空为群发，只支持在线通知
	* @param[in] cb 结果回调见nim_signaling_helper.h，返回SignalingControlResParam
	* @return void 无返回值
	* 错误码：	    200:成功;
	*				  0:接口服务异常;
	*				408:超时;
	*			  10404:频道不存在;
	*			  10406:不在频道内;
	*			  10201:对方不在线;
	*/
	static void Control(SignalingControlParam param, SignalingOptCallback cb);


};
}

#endif //_NIM_SDK_CPP_AVSIGNAL_H_