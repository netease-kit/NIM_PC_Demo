/** @file nim_cpp_team.h
  * @brief 群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_TEAM_H_
#define _NIM_SDK_CPP_TEAM_H_

#include <string>
#include <list>
#include <functional>
#include "nim_team_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#include "nim_res_code_def.h"
#include "nim_msglog_def.h"

/** @class Team
  * @brief 群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能
  */
class Team
{

public:
	typedef std::function<void(const TeamEvent& team_event)> TeamEventCallback;		/**< 群组事件通知回调模板 */
	typedef std::function<void(int team_count, const std::list<std::string>& team_id_list)> QueryAllMyTeamsCallback;		/**< 查询本人所有群ID回调模板 */
	typedef std::function<void(int team_count, const std::list<nim::TeamInfo>& team_info_list)>	QueryAllMyTeamsInfoCallback;	/**< 查询本人所有群信息回调模板 */
	typedef std::function<void(const std::string& tid, int member_count, const std::list<TeamMemberProperty>& props)>	QueryTeamMembersCallback;	/**< 查询指定群组全部成员信息回调模板 */
	typedef std::function<void(const TeamMemberProperty& team_member_property)>	QueryTeamMemberCallback;	/**< 查询指定群成员信息回调模板 */
	typedef std::function<void(const std::string& tid, const TeamInfo& result)>	QueryTeamInfoCallback;		/**< 查询指定群信息回调模板 */

	/** @fn static void RegTeamEventCb(const TeamEventCallback& cb, const std::string& json_extension = "")
	* 统一注册接收群通知回调函数（创建群,收到邀请等群通知通过此接口广播，注意：服务器推送过来的群通知和APP发起请求的回调统一处理！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		群通知的回调函数
	* @return void 无返回值
	*/
	static void RegTeamEventCb(const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool CreateTeamAsync(const TeamInfo& team_info, const std::list<std::string>& ids, const std::string& invitation_postscript, const TeamEventCallback& cb, const std::string& json_extension = "");
	* 创建群组，回调函数中返回的结果代码：
	* 200:普通群创建成功；
	* 810:如果创建的是高级群，返回810表示邀请成功并带上tinfo；
	* 414:成员不足；
	* 801:成员数超限制；
	* 404:成员中有非法用户；
	* @param[in] team_info 群组信息
	* @param[in] ids		邀请对象id
	* @param[in] invitation_postscript 邀请附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		群通知的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool CreateTeamAsync(const TeamInfo& team_info
		, const std::list<std::string>& ids
		, const std::string& invitation_postscript
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool InviteAsync(const std::string& tid, const std::list<std::string>& ids, const std::string& invitation_postscript, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 邀请，回调函数中返回的结果代码：
	* 200:普通群拉人成功；
	* 810:如果是高级群，返回810表示邀请成功并带上timetag；
	* 404:非法用户；
	* 801:群人数超限；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] ids		邀请对象id
	* @param[in] invitation_postscript 邀请附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool InviteAsync(const std::string& tid
		, const std::list<std::string>& ids
		, const std::string& invitation_postscript
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool KickAsync(const std::string& tid, const std::list<std::string>& ids, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 踢人，回调函数中返回的结果代码：
	* 404:非法用户；
	* 801:群人数超限；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] ids		被踢对象id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		踢人的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool KickAsync(const std::string& tid, const std::list<std::string>& ids, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool LeaveAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 离开群，回调函数中返回的结果代码：
	* 803:群不存在；
	* 804:用户不在群里；
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		离开群的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool LeaveAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool DismissAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 解散群，回调函数中返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		解散群的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool DismissAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool UpdateTeamInfoAsync(const std::string& tid, const TeamInfo& team_info, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 更新群信息，回调函数中返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* @param[in] tid		群组id
	* @param[in] team_info	群组信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		更新群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool UpdateTeamInfoAsync(const std::string& tid
		, const TeamInfo& team_info
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool ApplyJoinAsync(const std::string& tid, const std::string& reason, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 申请入群，回调函数中返回的结果代码：
	* 200:成功（直接入群）；
	* 802:群验证方式为拒绝所有人申请；
	* 808:申请成功，等待验证；
	* 809:已经在群里；
	* 801:人数限制；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] reason		附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		申请入群的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool ApplyJoinAsync(const std::string& tid
		, const std::string& reason
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool PassJoinApplyAsync(const std::string& tid, const std::string& applicant_id, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 同意入群申请，回调函数中返回的结果代码：
	* 200:成功；
	* 509:操作已失效；
	* 809:已经在群里；
	* 801:人数限制；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid			群组id
	* @param[in] applicant_id	申请者id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb				同意入群申请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool PassJoinApplyAsync(const std::string& tid
		, const std::string& applicant_id
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool RejectJoinApplyAsync(const std::string& tid, const std::string& applicant_id, const std::string& reason, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 拒绝入群申请，回调函数中返回的结果代码：
	* 200:成功。如果用户处于申请状态则会通知申请用户被拒绝；
	* 509:操作已失效；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid			群组id
	* @param[in] applicant_id	申请者id
	* @param[in] reason			附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb				拒绝入群申请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool RejectJoinApplyAsync(const std::string& tid
		, const std::string& applicant_id
		, const std::string& reason
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool AddManagersAsync(const std::string& tid, const std::list<std::string>& ids, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 添加管理员，回调函数中返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid	群组id
	* @param[in] ids	管理员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		添加管理员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool AddManagersAsync(const std::string& tid
		, const std::list<std::string>& ids
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool RemoveManagersAsync(const std::string& tid, const std::list<std::string>& ids, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 删除管理员，回调函数中返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid	群组id
	* @param[in] ids	管理员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		删除管理员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool RemoveManagersAsync(const std::string& tid
		, const std::list<std::string>& ids
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool TransferTeamAsync(const std::string& tid, const std::string& new_owner_id, bool is_leave, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 移交群主，回调函数中返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* 806:群数量上限；
	* @param[in] tid			群组id
	* @param[in] new_owner_id	移交对象id
	* @param[in] is_leave		是否同时退出群
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		移交群主的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool TransferTeamAsync(const std::string& tid
		, const std::string& new_owner_id
		, bool is_leave
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool UpdateMyPropertyAsync(const TeamMemberProperty& prop, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 更新自己的群属性，回调函数中返回的结果代码：
	* 200:成功；
	* 803:群不存在；
	* 804:不在群里；
	* 805:群类型不对；
	* @param[in] prop	群成员属性
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		更新自己的群属性的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool UpdateMyPropertyAsync(const TeamMemberProperty& prop, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool UpdateOtherNickAsync(const TeamMemberProperty& prop, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 修改别人的群昵称，回调函数中返回的结果代码：
	* 200:成功；
	* 802:没有权限；
	* 803:群不存在；
	* 804:不在群里；
	* 805:群类型不对；
	* @param[in] prop	群成员属性
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		修改别人的群昵称的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool UpdateOtherNickAsync(const TeamMemberProperty& prop, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool AcceptInvitationAsync(const std::string& tid, const std::string& invitor_id, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 接受邀请，回调函数中返回的结果代码：
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] invitor_id	邀请者id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		接受邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool AcceptInvitationAsync(const std::string& tid
		, const std::string& invitor_id
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool RejectInvitationAsync(const std::string& tid, const std::string& invitor_id, const std::string& reason, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 拒绝邀请，回调函数中返回的结果代码：
	* 802:没有权限；
	* 803:群不存在；
	* 805:群类型不对；
	* @param[in] tid		群组id
	* @param[in] invitor_id	邀请者id
	* @param[in] reason	附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		拒绝邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool RejectInvitationAsync(const std::string& tid
		, const std::string& invitor_id
		, const std::string& reason
		, const TeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool QueryAllMyTeamsAsync(const QueryAllMyTeamsCallback& cb, const std::string& json_extension = "")
	* 查询所有群
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群的回调函数
	* @return void 无返回值
	*/
	static void QueryAllMyTeamsAsync(const QueryAllMyTeamsCallback& cb, const std::string& json_extension = "");

	/** @fn static void QueryAllMyTeamsInfoAsync(const QueryAllMyTeamsInfoCallback& cb, const std::string& json_extension = "")
	* 查询所有群信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群信息的回调函数
	* @return void 无返回值
	*/
	static void QueryAllMyTeamsInfoAsync(const QueryAllMyTeamsInfoCallback& cb, const std::string& json_extension = "");

	/** @fn static void QueryTeamMembersAsync(const std::string& tid, const QueryTeamMembersCallback& cb, const std::string& json_extension = "")
	* 查询群成员
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群成员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryTeamMembersAsync(const std::string& tid, const QueryTeamMembersCallback& cb, const std::string& json_extension = "");

	/** @fn static bool QueryTeamMemberAsync(const std::string& tid, const std::string& id, const QueryTeamMemberCallback& cb, const std::string& json_extension = "")
	* 查询(单个)群成员信息
	* @param[in] tid		群组id
	* @param[in] id		    群成员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群成员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryTeamMemberAsync(const std::string& tid
		, const std::string& id
		, const QueryTeamMemberCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool QueryTeamInfoAsync(const std::string& tid, const QueryTeamInfoCallback& cb, const std::string& json_extension = "")
	* 查询群信息
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryTeamInfoAsync(const std::string& tid, const QueryTeamInfoCallback& cb, const std::string& json_extension = "");

	/** @fn static bool QueryTeamInfoOnlineAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 获取群信息
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		获取群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryTeamInfoOnlineAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ParseTeamInfo(const std::string& json_team_info, TeamInfo& team_info)
	* 解析群组信息
	* @param[in] json_team_info  json格式的群组信息
	* @param[out] team_info		群组信息
	* @return bool 解析成功失败
	*/
	static bool ParseTeamInfo(const std::string& json_team_info, TeamInfo& team_info);

	/** @fn void UnregTeamCb()
	* 反注册Team提供的所有回调
	* @return void 无返回值
	*/
	static void UnregTeamCb();

};

} 

#endif //_NIM_SDK_CPP_TEAM_H_