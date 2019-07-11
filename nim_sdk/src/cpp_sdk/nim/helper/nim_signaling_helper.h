/** @file nim_signaling_helper.h
  * @brief 独立信令 辅助方法和数据结构定义
  * @copyright (c) 2015-2018, NetEase Inc. All rights reserved
  * @author gq
  * @date 2018/11/8
  */

#ifndef _NIM_SDK_CPP_SIGNALING_HELPER_H_
#define _NIM_SDK_CPP_SIGNALING_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include <memory>
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @struct SignalingChannelInfo
*	@brief 频道属性
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingChannelInfo
{
	NIMSignalingType channel_type_;		/**< 通话类型,1:音频;2:视频;3:其他 */
	std::string channel_name_;			/**< 创建时传入的频道名 */
	std::string channel_id_;			/**< 服务器生成的频道id */
	std::string channel_ext_;			/**< 创建时传入的扩展字段 */
	uint64_t create_timestamp_;			/**< 创建时间点 */
	uint64_t expire_timestamp_;			/**< 失效时间点 */
	std::string creator_id_;			/**< 创建者的accid */
	bool invalid_;						/**< 频道是否有效 */
	SignalingChannelInfo()
	{
		channel_type_ = kNIMSignalingTypeAudio;
		create_timestamp_ = 0;
		expire_timestamp_ = 0;
		invalid_ = true;
	}
	SignalingChannelInfo(const NIMSignalingChannelInfo* info)
	{
		if (info)
		{
			channel_type_ = info->channel_type_;
			channel_name_ = info->channel_name_;
			channel_id_ = info->channel_id_;
			channel_ext_ = info->channel_ext_;
			create_timestamp_ = info->create_timestamp_;
			expire_timestamp_ = info->expire_timestamp_;
			creator_id_ = info->creator_id_;
			invalid_ = info->invalid_;
		}
	}
};

/** @struct SignalingMemberInfo
*	@brief 成员属性
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingMemberInfo
{
	std::string account_id_;	/**< 成员的 accid */
	int64_t uid_;				/**< 成员的 uid，大于零有效，无效时服务器会分配随机频道内唯一的uid */
	uint64_t create_timestamp_;	/**< 加入时间点 */
	uint64_t expire_timestamp_;	/**< 失效时间点，失效后认为离开频道 */
	SignalingMemberInfo()
	{
		uid_ = 0;
		create_timestamp_ = 0;
		expire_timestamp_ = 0;
	}
	SignalingMemberInfo(NIMSignalingMemberInfo* info)
	{
		SignalingMemberInfo();
		if (info)
		{
			account_id_ = info->account_id_;
			uid_ = info->uid_;
			create_timestamp_ = info->create_timestamp_;
			expire_timestamp_ = info->expire_timestamp_;
		}
	}
};

/** @struct SignalingChannelDetailedinfo
*	@brief 频道的详细信息，包含频道信息及成员列表
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingChannelDetailedinfo
{
	SignalingChannelInfo channel_info_;			/**< 频道信息 */
	std::list<SignalingMemberInfo> members_;	/**< 频道内成员信息数组 */
	SignalingChannelDetailedinfo(){}
	SignalingChannelDetailedinfo(const NIMSignalingChannelDetailedinfo* info)
	{
		if (info)
		{
			channel_info_ = SignalingChannelInfo(&info->channel_info_);
			for (int32_t i = 0; i < info->member_size_; ++i)
			{
				SignalingMemberInfo member(&info->members_[i]);
				members_.push_back(member);
			}
		}
	}
};

/** @struct SignalingPushInfo
*	@brief 推送属性
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingPushInfo
{
	bool need_push_;			/**< 是否需要推送，默认false， */
	std::string push_title_;	/**< 推送标题 */
	std::string push_content_;	/**< 推送内容 */
	std::string push_payload_;	/**< 推送自定义字段 */
	bool need_badge_;			/**< 是否计入未读计数,默认true */
	SignalingPushInfo()
	{
		need_push_ = false;
		need_badge_ = true;
	}
	SignalingPushInfo(NIMSignalingPushInfo* info)
	{
		SignalingPushInfo();
		if (info)
		{
			need_push_ = info->need_push_;
			if (need_push_)
			{
				push_title_ = info->push_title_;
				push_content_ = info->push_content_;
				push_payload_ = info->push_payload_;
				need_badge_ = info->need_badge_;
			}
		}
	}
	/**
	*	@brief 将C++的结构体转成C的结构体
	*/
	NIMSignalingPushInfo GetCParam()
	{
		NIMSignalingPushInfo info;
		info.need_push_ = need_push_;
		info.push_title_ = (char*)push_title_.c_str();
		info.push_content_ = (char*)push_content_.c_str();
		info.push_payload_ = (char*)push_payload_.c_str();
		info.need_badge_ = need_badge_;
		return info;
	}
};

/** @struct SignalingNotifyInfo
* @brief 事件通知信息基类
* @note 事件对应的通知说明\n
* kNIMSignalingEventTypeClose	返回SignalingNotifyInfoClose，支持在线、离线通知\n
* kNIMSignalingEventTypeJoin	返回SignalingNotifyInfoJoin，支持在线、离线通知\n
* kNIMSignalingEventTypeInvite	返回SignalingNotifyInfoInvite，支持在线、离线通知\n
* kNIMSignalingEventTypeCancelInvite	返回SignalingNotifyInfoCancelInvite，支持在线、离线通知\n
* kNIMSignalingEventTypeReject	返回SignalingNotifyInfoReject，支持在线、多端同步、离线通知\n
* kNIMSignalingEventTypeAccept	返回SignalingNotifyInfoAccept，支持在线、多端同步、离线通知\n
* kNIMSignalingEventTypeLeave	返回SignalingNotifyInfoLeave，支持在线、离线通知\n
* kNIMSignalingEventTypeCtrl	返回SignalingNotifyInfoControl，支持在线通知
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfo
{
	enum NIMSignalingEventType event_type_;		/**< 通知类型 */
	struct SignalingChannelInfo channel_info_;	/**< 频道信息 */
	std::string  from_account_id_;				/**< 操作者 */
	std::string  custom_info_;					/**< 操作的扩展字段 */
	uint64_t  timestamp_;						/**< 操作的时间戳 */
	/**
	*	@brief 将C的结构体转成C++的结构体智能指针
	*/
	static std::shared_ptr<SignalingNotifyInfo> GetSharedPtr(const NIMSignalingNotifyInfo* c_info);
};

/** @struct SignalingNotifyInfoClose
*	@brief 频道关闭事件通知信息，event_type_=kNIMSignalingEventTypeClose
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoClose : SignalingNotifyInfo
{
};

/** @struct SignalingNotifyInfoJoin
*	@brief 加入频道事件通知信息，event_type_=kNIMSignalingEventTypeJoin
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoJoin : SignalingNotifyInfo
{
	SignalingMemberInfo member_;	/**< 加入成员的信息，用于获得uid */
};

/** @struct SignalingNotifyInfoInvite
*	@brief 邀请事件通知信息，event_type_=kNIMSignalingEventTypeInvite
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoInvite : SignalingNotifyInfo
{
	std::string to_account_id_;		/**< 被邀请者的账号 */
	std::string request_id_;		/**< 邀请者邀请的请求id，用于被邀请者回传request_id_作对应的回应操作 */
	SignalingPushInfo push_info_;	/**< 推送信息 */
};

/** @struct SignalingNotifyInfoCancelInvite
*	@brief 取消邀请事件通知信息，event_type_=kNIMSignalingEventTypeCancelInvite
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoCancelInvite : SignalingNotifyInfo
{
	std::string to_account_id_;		/**< 被邀请者的账号 */
	std::string request_id_;		/**< 邀请者邀请的请求id */
};
/** @struct SignalingNotifyInfoReject
*	@brief 拒绝邀请事件通知信息，event_type_=kNIMSignalingEventTypeReject
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoReject : SignalingNotifyInfo
{
	std::string to_account_id_;	/**< 邀请者的账号 */
	std::string request_id_;	/**< 邀请者邀请的请求id */
};
/** @struct SignalingNotifyInfoAccept
*	@brief 接收邀请事件通知信息，event_type_=kNIMSignalingEventTypeAccept
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoAccept : SignalingNotifyInfo
{
	std::string to_account_id_;		/**< 邀请者的账号 */
	std::string request_id_;		/**< 邀请者邀请的请求id */
};

/** @struct SignalingNotifyInfoLeave
*	@brief 退出频道事件通知信息，event_type_=kNIMSignalingEventTypeLeave
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoLeave : SignalingNotifyInfo
{
};

/** @struct SignalingNotifyInfoControl
*	@brief 控制事件通知信息，event_type_=kNIMSignalingEventTypeCtrl
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingNotifyInfoControl : SignalingNotifyInfo
{
};

/** @struct SignalingResParam
*	@brief 独立信令操作回调参数基类
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingResParam
{
	/**
	*	@brief 将C的返回结构体转换成C++
	*/
	static std::shared_ptr<SignalingResParam> GetOptParam(void* opt_res_param)
	{
		return nullptr;
	}
};

/** @struct SignalingCreateParam
*	@brief 创建频道接口nim_signaling_create的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCreateParam
{
	NIMSignalingType channel_type_;	/**< 通话类型,1:音频;2:视频;3:其他 */
	std::string channel_name_;		/**< 创建时传入的频道名，可缺省 */
	std::string channel_ext_;		/**< 创建时传入的扩展字段，可缺省 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingCreateParam GetCParam()
	{
		NIMSignalingCreateParam param;
		param.channel_type_ = channel_type_;
		param.channel_name_ = (char*)channel_name_.c_str();
		param.channel_ext_ = (char*)channel_ext_.c_str();
		return param;
	}
};

/** @struct SignalingCreateResParam
*	@brief 创建频道结果回调信息
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCreateResParam : SignalingResParam
{
	SignalingChannelInfo channel_info_;	/**< 频道信息 */
	/**
	*	@brief 将C的返回结构体转换成C++
	*/
	static std::shared_ptr<SignalingResParam> GetOptParam(void* opt_res_param)
	{
		if (opt_res_param == nullptr)
		{
			return nullptr;
		}
		std::shared_ptr<SignalingCreateResParam> res_param(new SignalingCreateResParam);
		res_param->channel_info_ = SignalingChannelInfo(&((NIMSignalingCreateResParam*)opt_res_param)->channel_info_);
		return res_param;
	}
};

/** @struct SignalingCloseParam
*	@brief 关闭频道接口nim_signaling_close的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCloseParam
{
	std::string channel_id_;	/**< 服务器生成的频道id */
	std::string custom_info_;	/**< 操作的扩展字段 */
	bool offline_enabled_;		/**< 是否存离线 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingCloseParam GetCParam()
	{
		NIMSignalingCloseParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		return param;
	}
};
/** @struct SignalingCloseResParam
*	@brief 关闭频道结果回调信息
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCloseResParam : SignalingResParam
{
};

/** @struct SignalingJoinParam
*	@brief 加入频道接口nim_signaling_query_channel_info的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingJoinParam
{
	std::string channel_id_;	/**< 服务器生成的频道id */
	std::string custom_info_;	/**< 操作者附加的自定义信息，透传给其他人，可缺省 */
	int64_t uid_;				/**< 自己在频道中对应的uid，大于零有效，无效时服务器会分配随机唯一的uid */
	bool offline_enabled_;		/**< 是否存离线 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingJoinParam GetCParam()
	{
		NIMSignalingJoinParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.uid_ = uid_;
		param.offline_enabled_ = offline_enabled_;
		return param;
	}
};
/** @struct SignalingJoinResParam
*	@brief 加入频道结果回调信息
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingJoinResParam : SignalingResParam
{
	SignalingChannelDetailedinfo info_;	/**< 频道详细信息 */
	/**
	*	@brief 将C的返回结构体转换成C++
	*/
	static std::shared_ptr<SignalingResParam> GetOptParam(void* opt_res_param)
	{
		if (opt_res_param == nullptr)
		{
			return nullptr;
		}
		std::shared_ptr<SignalingJoinResParam> res_param(new SignalingJoinResParam);
		res_param->info_ = SignalingChannelDetailedinfo(&((NIMSignalingJoinResParam*)opt_res_param)->info_);
		return res_param;
	}
};

/** @struct SignalingLeaveParam
*	@brief 离开频道接口nim_signaling_leave的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingLeaveParam
{
	std::string channel_id_;	/**< 服务器生成的频道id */
	std::string custom_info_;	/**< 操作的扩展字段 */
	bool offline_enabled_;		/**< 是否存离线 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingLeaveParam GetCParam()
	{
		NIMSignalingLeaveParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		return param;
	}
};

/** @struct SignalingLeaveResParam
*	@brief 离开频道结果回调信息，暂时为空
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingLeaveResParam : SignalingResParam
{
};


/** @struct SignalingQueryChannelInfoParam
*	@brief 查询频道接口nim_signaling_join的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingQueryChannelInfoParam
{
	std::string channel_name_;		/**< 创建时传入的频道名 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingQueryChannelInfoParam GetCParam()
	{
		NIMSignalingQueryChannelInfoParam param;
		param.channel_name_ = (char*)channel_name_.c_str();
		return param;
	}
};
/** @struct SignalingQueryChannelInfoResParam
*	@brief 查询频道结果回调信息
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingQueryChannelInfoResParam : SignalingResParam
{
	SignalingChannelDetailedinfo info_;	/**< 频道详细信息 */
	/**
	*	@brief 将C的返回结构体转换成C++
	*/
	static std::shared_ptr<SignalingResParam> GetOptParam(void* opt_res_param)
	{
		if (opt_res_param == nullptr)
		{
			return nullptr;
		}
		std::shared_ptr<SignalingQueryChannelInfoResParam> res_param(new SignalingQueryChannelInfoResParam);
		res_param->info_ = SignalingChannelDetailedinfo(&((NIMSignalingQueryChannelInfoResParam*)opt_res_param)->info_);
		return res_param;
	}
};

/** @struct SignalingCallParam
*	@brief 呼叫接口nim_signaling_call的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCallParam
{
	NIMSignalingType channel_type_;	/**< 通话类型,1:音频;2:视频;3:其他 */
	std::string channel_name_;		/**< 创建时传入的频道名，可缺省 */
	std::string channel_ext_;		/**< 创建时传入的扩展字段，可缺省 */
	int64_t uid_;					/**< 自己在频道中对应的uid，大于零有效，无效时服务器会分配随机唯一的uid */
	std::string account_id_;		/**< 被邀请者的账号 */
	std::string request_id_;		/**< 邀请者邀请的请求id，需要邀请者填写，之后取消邀请、拒绝、接受需要复用该request_id_ */
	std::string custom_info_;		/**< 操作的扩展字段，透传给被邀请者，可缺省 */
	bool offline_enabled_;			/**< 是否存离线 */
	SignalingPushInfo push_info_;	/**< 推送属性 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingCallParam GetCParam()
	{
		NIMSignalingCallParam param;
		param.channel_type_ = channel_type_;
		param.channel_name_ = (char*)channel_name_.c_str();
		param.channel_ext_ = (char*)channel_ext_.c_str();
		param.uid_ = uid_;
		param.account_id_ = (char*)account_id_.c_str();
		param.request_id_ = (char*)request_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		param.push_info_ = push_info_.GetCParam();
		return param;
	}
};

/** @struct SignalingCallResParam
*	@brief 呼叫结果回调信息
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCallResParam : SignalingJoinResParam
{

};

/** @struct SignalingInviteParam
*	@brief 邀请接口nim_signaling_invite的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingInviteParam
{
	std::string channel_id_;				/**< 服务器生成的频道id */
	std::string account_id_;				/**< 被邀请者的账号 */
	std::string request_id_;				/**< 邀请者邀请的请求id，需要邀请者填写，之后取消邀请、拒绝、接受需要复用该request_id_ */
	std::string custom_info_;				/**< 操作的扩展字段，透传给被邀请者，可缺省 */
	bool offline_enabled_;			/**< 是否存离线 */
	struct SignalingPushInfo push_info_;	/**< 推送属性 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingInviteParam GetCParam()
	{
		NIMSignalingInviteParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.account_id_ = (char*)account_id_.c_str();
		param.request_id_ = (char*)request_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		param.push_info_ = push_info_.GetCParam();
		return param;
	}
};
/** @struct SignalingInviteResParam
*	@brief 邀请结果回调信息，暂时为空
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingInviteResParam : SignalingResParam
{
};

/** @struct SignalingCancelInviteParam
*	@brief 取消邀请接口nim_signaling_cancel_invite的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCancelInviteParam
{
	std::string channel_id_;		/**< 服务器生成的频道id */
	std::string account_id_;		/**< 被邀请者的账号 */
	std::string request_id_;		/**< 邀请者邀请的请求id */
	std::string custom_info_;		/**< 操作的扩展字段，可缺省 */
	bool offline_enabled_;	/**< 是否存离线 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingCancelInviteParam GetCParam()
	{
		NIMSignalingCancelInviteParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.account_id_ = (char*)account_id_.c_str();
		param.request_id_ = (char*)request_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		return param;
	}
};

/** @struct SignalingCancelInviteResParam
*	@brief 取消邀请结果回调信息，暂时为空
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingCancelInviteResParam : SignalingResParam
{
};

/** @struct SignalingRejectParam
*	@brief 拒绝接口nim_signaling_reject的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingRejectParam
{
	std::string channel_id_;		/**< 服务器生成的频道id */
	std::string account_id_;		/**< 邀请者的账号 */
	std::string request_id_;		/**< 邀请者邀请的请求id */
	std::string custom_info_;		/**< 操作的扩展字段 */
	bool offline_enabled_;	/**< 是否存离线 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingRejectParam GetCParam()
	{
		NIMSignalingRejectParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.account_id_ = (char*)account_id_.c_str();
		param.request_id_ = (char*)request_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		return param;
	}
};
/** @struct SignalingRejectResParam
*	@brief 拒绝邀请结果回调信息，暂时为空
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingRejectResParam : SignalingResParam
{
};
/** @struct SignalingAcceptParam
*	@brief 接受邀请接口nim_signaling_accept的传入参数
*/
struct SignalingAcceptParam
{
	std::string channel_id_;			/**< 服务器生成的频道id */
	std::string account_id_;			/**< 邀请者的账号 */
	std::string request_id_;			/**< 邀请者邀请的请求id */
	std::string accept_custom_info_;	/**< 操作的扩展字段 */
	bool offline_enabled_;				/**< 是否存离线 */
	bool auto_join_;					/**< 是否加入，打开后后续参数uid_、join_custom_info_有效 */
	int64_t uid_;						/**< 自己在频道中对应的uid，大于零有效，无效时服务器会分配随机唯一的uid，可缺省填0 */
	std::string join_custom_info_;		/**< 加入频道的自定义扩展信息，将在加入频道通知中带给其他频道成员，可缺省 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingAcceptParam GetCParam()
	{
		NIMSignalingAcceptParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.account_id_ = (char*)account_id_.c_str();
		param.request_id_ = (char*)request_id_.c_str();
		param.accept_custom_info_ = (char*)accept_custom_info_.c_str();
		param.offline_enabled_ = offline_enabled_;
		param.auto_join_ = auto_join_;
		param.uid_ = uid_;
		param.join_custom_info_ = (char*)join_custom_info_.c_str();
		return param;
	}
};

/** @struct SignalingAcceptResParam
*	@brief 接受邀请结果回调信息，在自动加入成功后返回有效
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingAcceptResParam : SignalingJoinResParam
{

};

/** @struct SignalingControlParam
*	@brief 控制通知接口nim_signaling_control的传入参数
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingControlParam
{
	std::string channel_id_;		/**< 服务器生成的频道id */
	std::string account_id_;		/**< 对方accid，如果为空，则通知所有人 */
	std::string custom_info_;		/**< 操作的扩展字段 */
	/**
	*	@brief 将C++的传入参数转成C的结构体
	*/
	NIMSignalingControlParam GetCParam()
	{
		NIMSignalingControlParam param;
		param.channel_id_ = (char*)channel_id_.c_str();
		param.account_id_ = (char*)account_id_.c_str();
		param.custom_info_ = (char*)custom_info_.c_str();
		return param;
	}
};
/** @struct SignalingControlResParam
*	@brief 控制通知结果回调信息，暂时为空
*/
struct NIM_SDK_CPPWRAPPER_DLL_API SignalingControlResParam : SignalingResParam
{
};

} //namespace nim

#endif //_NIM_SDK_CPP_SIGNALING_HELPER_H_