#pragma once


#include <string>
#include <list>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_team_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_res_code_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_msglog_def.h"

struct TeamInfo
{
	nim::NIMTeamType		type;
	std::string		id;
	std::string		owner_id;
	std::string		name;
	std::string		intro;
	std::string		announcement;
	NIMTeamJoinMode join_mode;
	uint64_t		team_config_bits;
	bool readonly;
	int valid;
};

struct TeamMemberInfo
{
	std::string		account;
	std::string		nick;
	uint64_t		bits;
	nim::NIMTeamUserType type;
};

/** @class Team
  * @brief 群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Team
{

public:
	typedef std::function<void(nim::NIMResCode res_code, nim::NIMNotificationId notification_id, const std::string& tid, const std::string& result)>	TeamEventCallback;
	typedef std::function<void(int team_count, const std::string& result)>	QueryAllMyTeamsCallback;
	typedef std::function<void(int team_count, const std::list<nim::TeamInfo>& team_info_list)>	QueryAllMyTeamsInfoCallback;
	typedef std::function<void(const std::string& tid, int member_count, const std::list<TeamMemberInfo>& result)>	QueryTeamMembersCallback;
	typedef std::function<void(const TeamMemberInfo& team_member_info)>	QueryTeamMemberCallback;
	typedef std::function<void(const std::string& tid, const TeamInfo& result)>	QueryTeamInfoCallback;

	/** 
	* 统一注册接收群通知回调函数（创建群,收到邀请等群通知通过此接口广播，注意：服务器推送过来的群通知和APP发起请求的回调统一处理！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		群通知的回调函数
	* @return void 无返回值
	*/
	static void RegTeamEventCb(const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 创建群组，返回的结果代码：
	* 200:普通群创建成功；
	* 810:如果创建的是高级群，返回810表示邀请成功并带上tinfo；
	* 414:成员不足；
	* 801:成员数超限制；
	* 404:成员中有非法用户；
	* @param[in] team_info team信息的json string (Keys SEE MORE nim_team_def.h『群组信息 Json Keys』 as follows)
	* @param[in] jsonlist_uids		uids string array 不包括自己
	* @param[in] invitation_postscript 邀请附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		群通知的回调函数
	* @return void 无返回值
	*/
	static void CreateTeamAsync(const std::string& team_info, const std::list<std::string>& jsonlist_uids, 
		const std::string& invitation_postscript, const TeamEventCallback& cb, const std::string& json_extension = "");


	/** 
	* 获取群信息
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		获取群信息的回调函数
	* @return void 无返回值
	*/
	static void QueryTeamInfoOnlineAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 邀请，返回的结果代码：
	* 200:普通群拉人成功；
	* 810:如果是高级群，返回810表示邀请成功并带上timetag；
	* 404:非法用户；
	* 801:群人数超限；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] jsonlist_uids		uids string array
	* @param[in] invitation_postscript 邀请附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		邀请的回调函数
	* @return void 无返回值
	*/
	static void InviteAsync(const std::string& tid, const std::list<std::string>& jsonlist_uids, const std::string& invitation_postscript, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 踢人，返回的结果代码：
	* 404:非法用户；
	* 801:群人数超限；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] jsonlist_uids		uids string array
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		踢人的回调函数
	* @return void 无返回值
	*/
	static void KickAsync(const std::string& tid, const std::list<std::string>& jsonlist_uids, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 离开群，返回的结果代码：
	* 803:群不存在；
	* 804:用户不在群里；
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		离开群的回调函数
	* @return void 无返回值
	*/
	static void LeaveAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 解散群，返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		解散群的回调函数
	* @return void 无返回值
	*/
	static void DismissAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 更新群信息，返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] json_info Json string array (Keys SEE MORE `nim_team_def.h` 『群组信息 Json Keys』， 目前只支持更新群名称，群id必需填)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		更新群信息的回调函数
	* @return void 无返回值
	*/
	static void UpdateTeamInfoAsync(const std::string& tid, const std::string& json_info, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 申请入群，返回的结果代码：
	* 200:成功（直接入群）；
	* 802:群验证方式为拒绝所有人申请；
	* 808:申请成功，等待验证；
	* 809:已经在群里；
	* 801:人数限制；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] reason	附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		申请入群的回调函数
	* @return void 无返回值
	*/
	static void ApplyJoinAsync(const std::string& tid, const std::string& reason, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 同意入群申请，返回的结果代码：
	* 200:成功；
	* 509:操作已失效；
	* 809:已经在群里；
	* 801:人数限制；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] applicant_id	申请者id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		同意入群申请的回调函数
	* @return void 无返回值
	*/
	static void PassJoinApplyAsync(const std::string& tid, const std::string& applicant_id, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 拒绝入群申请，返回的结果代码：
	* 200:成功。如果用户处于申请状态则会通知申请用户被拒绝；
	* 509:操作已失效；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] applicant_id	申请者id
	* @param[in] reason		附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		拒绝入群申请的回调函数
	* @return void 无返回值
	*/
	static void RejectJoinApplyAsync(const std::string& tid, const std::string& applicant_id, const std::string& reason, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 添加管理员，返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] jsonlist_admin_ids	uids string array
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		添加管理员的回调函数
	* @return void 无返回值
	*/
	static void AddManagersAsync(const std::string& tid, const std::list<std::string>& jsonlist_admin_ids, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 删除管理员，返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] jsonlist_admin_ids	uids string array
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		删除管理员的回调函数
	* @return void 无返回值
	*/
	static void RemoveManagersAsync(const std::string& tid, const std::list<std::string>& jsonlist_admin_ids, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 移交群主，返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* 806:群数量上限；
	* @param[in] tid		群组id
	* @param[in] new_owner	移交对象uid
	* @param[in] is_leave		是否同时退出群
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		移交群主的回调函数
	* @return void 无返回值
	*/
	static void TransferTeamAsync(const std::string& tid, const std::string& new_owner, bool is_leave, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 更新自己的群属性，返回的结果代码：
	* 200:成功；
	* 803:群不存在；
	* 804:不在群里；
	* 805:群类型不对；
	* @param[in] info		json string tlist,包含tid和需要修改的字段（暂时只有nick和bits） (Keys SEE MORE `nim_team_def.h` 『群组属性 Json Keys』
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		更新自己的群属性的回调函数
	* @return void 无返回值
	*/
	static void UpdateMyPropertyAsync(const std::string& info, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 修改别人的群昵称，返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 804:不在群里；
	* 805:群类型不对；
	* @param[in] info		json string tlist 包含tid,accid,nick字段 (Keys SEE MORE `nim_team_def.h` 『群组属性 Json Keys』
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		修改别人的群昵称的回调函数
	* @return void 无返回值
	*/
	static void UpdateOtherNickAsync(const std::string& info, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 接受邀请，返回的结果代码：
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] invitor	邀请者id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		接受邀请的回调函数
	* @return void 无返回值
	*/
	static void AcceptInvitationAsync(const std::string& tid, const std::string& invitor, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 拒绝邀请，返回的结果代码：
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] invitor	邀请者id
	* @param[in] reason	附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		拒绝邀请的回调函数
	* @return void 无返回值
	*/
	static void RejectInvitationAsync(const std::string& tid, const std::string& invitor, const std::string& reason, 
		const TeamEventCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询所有群
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群的回调函数
	* @return void 无返回值
	*/
	static void QueryAllMyTeamsAsync(const QueryAllMyTeamsCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询所有群信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群信息的回调函数
	* @return void 无返回值
	*/
	static void QueryAllMyTeamsInfoAsync(const QueryAllMyTeamsInfoCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询群成员
	* @param[in] tid		群组id
	* @param[in] include_user_info 返回结果中是否包含成员信息(关系到如何解析返回的json)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群成员的回调函数
	* @return void 无返回值
	*/
	static void QueryTeamMembersAsync(const std::string& tid, const QueryTeamMembersCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询(单个)群成员信息
	* @param[in] tid		群组id
	* @param[in] user_id  群成员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群成员的回调函数
	* @return void 无返回值
	*/
	static void QueryTeamMemberAsync(const std::string& tid, const std::string& uid, const QueryTeamMemberCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询群信息
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群信息的回调函数
	* @return void 无返回值
	*/
	static void QueryTeamInfoAsync(const std::string& tid, const QueryTeamInfoCallback& cb, const std::string& json_extension = "");
};

} 