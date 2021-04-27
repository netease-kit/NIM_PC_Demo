#pragma once
#include <string>
/*
 *G2 话单解析模块
 *
 */
namespace nim_comp
{

	enum NIMNetCallStatus
	{
		kNIMNetCallStatusComplete = 1,	/**< 1:通话完成*/
		kNIMNetCallStatusCanceled,			/**< 2:通话取消*/
		kNIMNetCallStatusRejected,			/**< 3:通话拒绝*/
		kNIMNetCallStatusTimeout,			/**< 4:超时未接听*/
		kNIMNetCallStatusBusy,					/**< 5:对方忙线*/
	};

	enum NIMNetCallType
	{
		kNIMNetCallTypeAudio = 1,
		kNIMNetCallTypeVideo = 2,
	};

	std::wstring ParseNetCallMsg(const nim::SessionData& msg);
	std::wstring ParseNetCallMsg(const nim::IMMessage& msg);

	void SendNetCallMsg(
		const std::string& to,
		const std::string& channelId,
		int type,
		int status,
		std::vector<std::string> members,
		std::vector<int> durations);
}

