#ifndef _SDK_SUPPORT_PLATFORM_BUSINESS_ACTION_GATEWAY_BATP_BATP_H_
#define _SDK_SUPPORT_PLATFORM_BUSINESS_ACTION_GATEWAY_BATP_BATP_H_

#include <string>
#include <atomic>
#include "batp_param.h"

namespace nbase
{
	enum class BatpPackType
	{
		REQUEST,//请求
		RESPONSE,//应答
		NOTIFY,//推送
	};
	enum class BatpCode
	{
		ERR = 0,
		OK = 200,
		FORBIDDEN = 403,
		NOTFOUND = 404,
		METHODNOTALLOWED = 405,
		PARAMETERERROR = 414,
	};
	enum class BatpRequestMeth
	{
		POST = 0,//异步
		SEND = 1,//同步
		UNIDIRECTIONAL = 2,//只发送，不等应答

	};
	class  BatpTrailInfo
	{
	public:
		BatpTrailInfo() = default;
		//BatpTrailInfo(const std::string file_name, int line, const std::string function_name) :
		//	file_name_(file_name), line_(line), function_name_(function_name) {}
	public:
		std::string file_name_;
		int line_;
		std::string function_name_;
	};
#ifdef OS_WIN
#define __FILENAME__ strrchr(__FILE__,'\\')?strrchr(__FILE__,'\\')+1:__FILE__
#else
#define __FILENAME__ strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__
#endif
#define BatpTrailInfoFromHere {__FILENAME__,__LINE__,__FUNCTION__}
#define BATP_NOTIFY_SN (0x00)
	uint64_t CalcBatpSN();
	class BatpHead
	{
	public:
		BatpPackType type_;
		BatpCode code_;
		std::string code_message_;
		uint64_t sn_;
		std::string action_name_;
		BatpRequestMeth meth_;
		std::string extern_message_;
	};
	class BatpBody
	{
	public:
		BatpParam param_;
	};
	class BatpPack
	{
	public:
		BatpPack() = default;
		virtual ~BatpPack() = default;
	public:
		BatpHead head_;
		BatpBody body_;
		BatpTrailInfo trail_info_;
	};
	class BatpPackResponse : public BatpPack
	{
	public:
		BatpPackResponse() = default;
	public:
		BatpPackResponse(const BatpHead& request_head, const BatpParam& param, const BatpTrailInfo trail_info)
		{
			head_.action_name_ = request_head.action_name_;
			head_.meth_ = request_head.meth_;
			head_.type_ = BatpPackType::RESPONSE;
			head_.code_ = BatpCode::OK;
			head_.code_message_ = "SUCCESS";
			head_.sn_ = request_head.sn_;
			body_.param_ = param;
			trail_info_ = trail_info;
		}
		BatpPackResponse(const BatpHead& request_head, const BatpTrailInfo trail_info)
		{
			head_.action_name_ = request_head.action_name_;
			head_.meth_ = request_head.meth_;
			head_.type_ = BatpPackType::RESPONSE;
			head_.code_ = BatpCode::OK;
			head_.code_message_ = "SUCCESS";
			head_.sn_ = request_head.sn_;
			trail_info_ = trail_info;
		}
	};
	class BatpRequest : public BatpPack
	{
	public:
		BatpRequest() = default;
		BatpRequest(BatpRequestMeth meth, const std::string& action_name, const BatpParam& param, const BatpTrailInfo trail_info)
		{
			head_.action_name_ = action_name;
			head_.meth_ = meth;
			head_.type_ = BatpPackType::REQUEST;
			head_.code_ = BatpCode::OK;
			head_.code_message_ = "SUCCESS";
			head_.sn_ = CalcBatpSN();
			body_.param_ = param;
			trail_info_ = trail_info;
		}
		BatpRequest(BatpRequestMeth meth, const std::string& action_name, const BatpTrailInfo trail_info)
		{
			head_.action_name_ = action_name;
			head_.meth_ = meth;
			head_.type_ = BatpPackType::REQUEST;
			head_.code_ = BatpCode::OK;
			head_.code_message_ = "SUCCESS";
			head_.sn_ = CalcBatpSN();
			trail_info_ = trail_info;
		}
	};
	class BatpPackPostRequest : public BatpRequest
	{
	public:
		BatpPackPostRequest() = default;
	public:
		BatpPackPostRequest(const std::string& action_name, const BatpParam& param, const BatpTrailInfo trail_info) :
			BatpRequest(BatpRequestMeth::POST, action_name, param, trail_info)
		{
		}
		BatpPackPostRequest(const std::string& action_name, const BatpTrailInfo trail_info) :
			BatpRequest(BatpRequestMeth::POST, action_name, trail_info)
		{
		}
	};
	class BatpPackSendRequest : public BatpRequest
	{
	public:
		BatpPackSendRequest() = default;
	public:
		BatpPackSendRequest(const std::string& action_name, const BatpParam& param, const BatpTrailInfo trail_info)
			: BatpRequest(BatpRequestMeth::SEND, action_name, param, trail_info)
		{
		}
		BatpPackSendRequest(const std::string& action_name, const BatpTrailInfo trail_info)
			: BatpRequest(BatpRequestMeth::SEND, action_name, trail_info)
		{
		}
	};
	class BatpPackUnidirectionalRequest : public BatpRequest
	{
	public:
		BatpPackUnidirectionalRequest() = default;
	public:
		BatpPackUnidirectionalRequest(const std::string& action_name, const BatpParam& param, const BatpTrailInfo trail_info)
			: BatpRequest(BatpRequestMeth::UNIDIRECTIONAL, action_name, param, trail_info)
		{
		}
		BatpPackUnidirectionalRequest(const std::string& action_name, const BatpTrailInfo trail_info)
			: BatpRequest(BatpRequestMeth::UNIDIRECTIONAL, action_name, trail_info)
		{
		}
	};
	class BatpPackNotify : public BatpPack
	{
	public:
		BatpPackNotify() = default;
	public:
		BatpPackNotify(const std::string& action_name, const BatpParam& param, const BatpTrailInfo trail_info)
		{
			head_.action_name_ = action_name;
			head_.meth_ = BatpRequestMeth::POST;
			head_.type_ = BatpPackType::NOTIFY;
			head_.code_ = BatpCode::OK;
			head_.code_message_ = "Notify";
			head_.sn_ = BATP_NOTIFY_SN;
			body_.param_ = param;
			trail_info_ = trail_info;
		}
		BatpPackNotify(const std::string& action_name, const BatpTrailInfo trail_info)
		{
			head_.action_name_ = action_name;
			head_.meth_ = BatpRequestMeth::POST;
			head_.type_ = BatpPackType::NOTIFY;
			head_.code_ = BatpCode::OK;
			head_.code_message_ = "Notify";
			head_.sn_ = BATP_NOTIFY_SN;
			trail_info_ = trail_info;
		}
	};
	template<typename ValueType>
	inline ValueType BatpParamCast(const BatpPack& pack)
	{
		typedef typename std::remove_reference<ValueType>::type nonref;
		static_assert(!std::is_reference<nonref>::value, "still right reference");
		return BatpParamCast<const nonref&>(const_cast<BatpParam&>(pack.body_.param_));
	}
	template<typename ValueType>
	inline ValueType BatpParamCast(const BatpBody& body)
	{
		typedef typename std::remove_reference<ValueType>::type nonref;
		static_assert(!std::is_reference<nonref>::value, "still right reference");
		return BatpParamCast<const nonref&>(const_cast<BatpParam&>(body.param_));
	}
}

#endif //_SDK_SUPPORT_PLATFORM_BUSINESS_ACTION_GATEWAY_BATP_BATP_H_