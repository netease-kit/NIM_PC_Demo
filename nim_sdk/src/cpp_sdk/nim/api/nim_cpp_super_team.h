/** @file nim_cpp_super_team.h
  * @brief 超大群组
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_SUPERTEAM_H_
#define _NIM_SDK_CPP_SUPERTEAM_H_

#include <string>
#include <list>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_super_team_helper.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @class SuperTeam
  * @brief 超大群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能
  */
class NIM_SDK_CPPWRAPPER_DLL_API SuperTeam
{

public:
	typedef std::function<void(const SuperTeamEvent& team_event)> SuperTeamEventCallback;		/**< 群组事件通知回调模板 */
	typedef std::function<void(int team_count, const std::list<std::string>& team_id_list)> QueryAllMySuperTeamsCallback;		/**< 查询本人所有群ID回调模板 */
	typedef std::function<void(int team_count, const std::list<nim::SuperTeamInfo>& team_info_list)>	QueryAllMySuperTeamsInfoCallback;	/**< 查询本人所有群信息回调模板 */
	typedef std::function<void(int count, const std::list<nim::SuperTeamMemberProperty>& all_my_member_info_list)>	QueryMyAllMemberInfosCallback;	/**< 查询本人所有群里我的成员信息回调模板 */
	typedef std::function<void(NIMResCode error_code,const std::string& tid, int member_count, const std::list<SuperTeamMemberProperty>& props)>	QuerySuperTeamMembersCallback;	/**< 查询指定群组全部成员信息回调模板 */
	typedef std::function<void(const SuperTeamMemberProperty& team_member_property)>	QuerySuperTeamMemberCallback;	/**< 查询指定群成员信息回调模板 */
	typedef std::function<void(const std::string& tid, const SuperTeamInfo& result)>	QuerySuperTeamInfoCallback;		/**< 查询指定群信息回调模板 */
	typedef std::function<void(NIMResCode error_code, const std::string& tid, const std::list<SuperTeamMemberProperty>& team_member_propertys)>	QuerySuperTeamMembersOnlineCallback;	/**< 查询群成员信息回调模板 */

	/** @fn static void RegSuperTeamEventCb(const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* (全局回调)统一注册接收群通知回调函数（创建群,收到邀请等群通知通过此接口广播，注意：服务器推送过来的群通知和APP发起请求的回调统一处理！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		群通知的回调函数
	* @return void 无返回值
	*/
	static void RegSuperTeamEventCb(const SuperTeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool InviteAsync(const std::string& tid, const std::list<std::string>& ids, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 邀请
	* @param[in] tid		群组id
	* @param[in] ids		邀请对象id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:无超大群功能
	*				404:目标不存在
	*				801:群人数超限
	*				802:没有权限
	*				803:群不存在
	*				804:用户不在群里面
	*				813:部分成功
	*/
	static bool InviteAsync(const std::string& tid
		, const std::list<std::string>& ids
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool InviteAsync2(const std::string& tid, const std::list<std::string>& ids, const std::string& invitation_postscript, const TeamEventCallback& cb, const std::string& json_extension = "")
	* 邀请
	* @param[in] tid		群组id
	* @param[in] ids		邀请对象id
	* @param[in] invitation_postscript 邀请附言
	* @param[in] invitation_attachment 用户可自定义的补充邀请信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				810:如果是高级群，返回810表示邀请成功并带上timetag
	*				404:非法用户
	*				801:群人数超限
	*				802:没有权限
	*				803:群不存在
	*/
	static bool InviteAsync2(const std::string& tid
		, const std::list<std::string>& ids
		, const std::string& invitation_postscript
		, const std::string& invitation_attachment
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool KickAsync(const std::string& tid, const std::list<std::string>& ids, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 踢人
	* @param[in] tid		群组id
	* @param[in] ids		被踢对象id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		踢人的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:无超大群功能
	*				414:踢人数量超限
	*				802:群人数超限、不能踢自己
	*				803:群不存在
	*				804:用户不在群里面
	*/
	static bool KickAsync(const std::string& tid, const std::list<std::string>& ids, const SuperTeamEventCallback& cb, const std::string& json_extension = "");	

	/** @fn static bool LeaveAsync(const std::string& tid, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 离开群
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		离开群的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:无超大群功能
	*				802:没有群权限、群主不能退群
	*				803:群不存在
	*				804:用户不在群里
	*/
	static bool LeaveAsync(const std::string& tid, const SuperTeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool UpdateSuperTeamInfoAsync(const std::string& tid, const SuperTeamInfo& team_info, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 更新群信息
	* @param[in] tid		群组id
	* @param[in] team_info	群组信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		更新群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:无超大群功能
	*				802:没有权限
	*				803:群不存在
	*				804:用户不在群里
	*/
	static bool UpdateSuperTeamInfoAsync(const std::string& tid
		, const SuperTeamInfo& team_info
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool UpdateMyPropertyAsync(const SuperTeamMemberProperty& prop, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 更新自己的群属性
	* @param[in] prop	群成员属性
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		更新自己的群属性的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:无超大群功能
	*				414:参数错误
	*				802:没有权限
	*				803:群不存在
	*				804:不在群里
	*/
	static bool UpdateMyPropertyAsync(const SuperTeamMemberProperty& prop, const SuperTeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ApplyJoinAsync(const std::string& tid, const std::string& reason, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 申请入群
	* @param[in] tid		群组id
	* @param[in] reason		附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		申请入群的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:群验证方式为拒绝所有人申请
	*				808:申请成功，等待验证
	*				809:已经在群里
	*				801:人数限制
	*				803:群不存在
	*				806:群数量上限
	*/
	static bool ApplyJoinAsync(const std::string& tid
		, const std::string& reason
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool PassJoinApplyAsync(const std::string& tid, const std::string& applicant_id, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 同意入群申请
	* @param[in] tid			群组id
	* @param[in] applicant_id	申请者id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb				同意入群申请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				809:已经在群里
	*				801:人数限制
	*				802:没有权限
	*				803:群不存在
	*/
	static bool PassJoinApplyAsync(const std::string& tid
		, const std::string& applicant_id
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool RejectJoinApplyAsync(const std::string& tid, const std::string& applicant_id, const std::string& reason, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 拒绝入群申请
	* @param[in] tid			群组id
	* @param[in] applicant_id	申请者id
	* @param[in] reason			附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb				拒绝入群申请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功，如果用户处于申请状态则会通知申请用户被拒绝
	*				802:没有权限
	*				803:群不存在
	*				809:已经在群里
	*/
	static bool RejectJoinApplyAsync(const std::string& tid
		, const std::string& applicant_id
		, const std::string& reason
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool AddManagersAsync(const std::string& tid, const std::list<std::string>& ids, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 添加管理员
	* @param[in] tid	群组id
	* @param[in] ids	管理员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		添加管理员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:没有权限
	*				803:群不存在
	*				815:管理员数量上限
	*/
	static bool AddManagersAsync(const std::string& tid
		, const std::list<std::string>& ids
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool RemoveManagersAsync(const std::string& tid, const std::list<std::string>& ids, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 删除管理员
	* @param[in] tid	群组id
	* @param[in] ids	管理员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		删除管理员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:没有权限
	*				803:群不存在
	*/
	static bool RemoveManagersAsync(const std::string& tid
		, const std::list<std::string>& ids
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool TransferTeamAsync(const std::string& tid, const std::string& new_owner_id, bool is_leave, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 移交群主
	* @param[in] tid			群组id
	* @param[in] new_owner_id	移交对象id
	* @param[in] is_leave		是否同时退出群
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		移交群主的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:没有权限
	*				803:群不存在
	*				806:群数量上限
	*/
	static bool TransferTeamAsync(const std::string& tid
		, const std::string& new_owner_id
		, bool is_leave
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool UpdateOtherNickAsync(const TeamMemberProperty& prop, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 修改别人的群昵称
	* @param[in] prop	群成员属性
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		修改别人的群昵称的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:没有权限
	*				803:群不存在
	*				804:不在群里
	*/
	static bool UpdateOtherNickAsync(const SuperTeamMemberProperty& prop, const SuperTeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool AcceptInvitationAsync(const std::string& tid, const std::string& invitor_id, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 接受邀请
	* @param[in] tid		群组id
	* @param[in] invitor_id	邀请者id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		接受邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:没有权限
	*				803:群不存在
	*				809:已经在群里
	*/
	static bool AcceptInvitationAsync(const std::string& tid
		, const std::string& invitor_id
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool RejectInvitationAsync(const std::string& tid, const std::string& invitor_id, const std::string& reason, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 拒绝邀请
	* @param[in] tid		群组id
	* @param[in] invitor_id	邀请者id
	* @param[in] reason	附言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		拒绝邀请的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				802:没有权限
	*				803:群不存在
	*				809:已经在群里
	*/
	static bool RejectInvitationAsync(const std::string& tid
		, const std::string& invitor_id
		, const std::string& reason
		, const SuperTeamEventCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool MuteMemberAsync(const std::string& tid, const std::string& member_id, bool set_mute, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 禁言/解除禁言
	* @param[in] tid		群组id
	* @param[in] member_id	操作对象id
	* @param[in] set_mute	禁言/解除禁言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		踢人的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				404:禁言对象不存在
	*				414:参数错误
	*				803:群不存在
	*				802:没有权限
	*/
	static bool MuteMemberAsync(const std::string& tid, const std::string& member_id, bool set_mute, const SuperTeamEventCallback& cb, const std::string& json_extension = "");

    /** @fn bool QueryMuteListAsync(const std::string& tid, const QuerySuperTeamMembersCallback& cb, const std::string& json_extension = "")
     * 获取群禁言成员列表
     * @param[in] tid	群组id
     * @param[in] cb		回调函数
     * @param[in] json_extension json扩展参数（备用，目前不需要）
     * @return bool 检查参数如果不符合要求则返回失败
     * @note 错误码	200:成功
     *				803:群不存在
     *				802:没有权限
     */
    static bool QueryMuteListAsync(const std::string& tid, const QuerySuperTeamMembersCallback& cb, const std::string& json_extension = "");

	/** @fn static bool MuteAsync(const std::string& tid, bool set_mute, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 群禁言/解除群禁言
	* @param[in] tid		群组id
	* @param[in] set_mute	禁言/解除禁言
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		踢人的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				414:参数错误
	*/
	static bool MuteAsync(const std::string& tid, bool set_mute, const SuperTeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool QueryAllMySuperTeamsAsync(const QueryAllMySuperTeamsCallback& cb, const std::string& json_extension = "")
	* 查询所有群
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群的回调函数
	* @return void 无返回值
	*/
	static void QueryAllMySuperTeamsAsync(const QueryAllMySuperTeamsCallback& cb, const std::string& json_extension = "");

	/** @fn static void QueryAllMySuperTeamsInfoAsync(const QueryAllMySuperTeamsInfoCallback& cb, const std::string& json_extension = "")
	* 查询所有群信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群信息的回调函数
	* @return void 无返回值
	*/
	static void QueryAllMySuperTeamsInfoAsync(const QueryAllMySuperTeamsInfoCallback& cb, const std::string& json_extension = "");

	/** @fn static QuerySuperTeamsInfoByKeywordAsync(const std::string& keyword, NIMSuperTeamQueryType query_type, QueryAllMySuperTeamsInfoCallback& cb, const std::string& json_extension = "");
	* 查询所有群信息
	* @param[in] keyword 要查询的关键字
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QuerySuperTeamsInfoByKeywordAsync(const std::string& keyword, const QueryAllMySuperTeamsInfoCallback& cb, const std::string& json_extension = "");

	/** @fn static void QueryMyAllMemberInfosAsync(const QueryMyAllMemberInfosCallback& cb, const std::string& json_extension = "")
	* 查询所有群里我的成员信息（使用场景：获取了所有群列表后，需要查询自己在每个群里自己的成员信息，使用成员信息里的bits字段，可以判断当某个群发来消息后，是否做消息通知）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询所有群里我的成员信息的回调函数
	* @return void 无返回值
	*/
	static void QueryMyAllMemberInfosAsync(const QueryMyAllMemberInfosCallback& cb, const std::string& json_extension = "");

	/** @fn static void QuerySuperTeamMembersAsync(const std::string& tid, const QuerySuperTeamMembersCallback& cb, const std::string& json_extension = "")
	* 查询群成员
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群成员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码 200:成功
	*				403:无超大群功能
	*				406:没有变化
	*				802:没有权限
	*/
	static bool QuerySuperTeamMembersAsync(const std::string& tid, const QuerySuperTeamMembersCallback& cb, const std::string& json_extension = "");

	/** @fn static bool QuerySuperTeamMemberAsync(const std::string& tid, const std::string& id, const QuerySuperTeamMemberCallback& cb, const std::string& json_extension = "")
	* 查询(单个)群成员信息
	* @param[in] tid		群组id
	* @param[in] id		    群成员id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群成员的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QuerySuperTeamMemberAsync(const std::string& tid
		, const std::string& id
		, const QuerySuperTeamMemberCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool QuerySuperTeamMemberBlock(const std::string& tid, const std::string& id)
	* 查询(单个)群成员信息(同步接口，堵塞NIM内部线程，谨慎使用)
	* @param[in] tid		群组id
	* @param[in] id		    群成员id
	* @return 群成员信息
	*/
	static SuperTeamMemberProperty QuerySuperTeamMemberBlock(const std::string& tid, const std::string& id);

	/** @fn static bool QuerySuperTeamInfoAsync(const std::string& tid, const QuerySuperTeamInfoCallback& cb, const std::string& json_extension = "")
	* 查询群信息
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		查询群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QuerySuperTeamInfoAsync(const std::string& tid, const QuerySuperTeamInfoCallback& cb, const std::string& json_extension = "");

	/** @fn static SuperTeamInfo QuerySuperTeamInfoBlock(const std::string& tid)
	* 查询群信息(同步接口，堵塞NIM内部线程，谨慎使用)
	* @param[in] tid		群组id
	* @return 群信息
	*/
	static SuperTeamInfo QuerySuperTeamInfoBlock(const std::string& tid);

	/** @fn static bool QuerySuperTeamInfoOnlineAsync(const std::string& tid, const SuperTeamEventCallback& cb, const std::string& json_extension = "")
	* 获取群信息（从服务器获取）
	* @param[in] tid		群组id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		获取群信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:无超大群功能
	*				802:没有群权限
	*				803:群不存在
	*/
	static bool QuerySuperTeamInfoOnlineAsync(const std::string& tid, const SuperTeamEventCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ParseSuperTeamInfo(const std::string& json_team_info, SuperTeamInfo& team_info)
	* 解析群组信息
	* @param[in] json_team_info  json格式的群组信息
	* @param[out] team_info		群组信息
	* @return bool 解析成功失败
	*/
	static bool ParseSuperTeamInfo(const std::string& json_team_info, SuperTeamInfo& team_info);

	/** @fn void UnregSuperTeamCb()
	* 反注册SuperTeam提供的所有回调
	* @return void 无返回值
	*/
	static void UnregSuperTeamCb();
};

} 

#endif //_NIM_SDK_CPP_SUPERTEAM_H_