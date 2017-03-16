/** @file nim_cpp_friend.h
  * @brief NIM 好友相关接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author caowei, Oleg
  * @date 2015/8/17
  */

#ifndef _NIM_SDK_CPP_FRIEND_H_
#define _NIM_SDK_CPP_FRIEND_H_

#include <string>
#include <list>
#include <functional>
#include "nim_friend_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
#include "nim_res_code_def.h"

/** @class Friend
  * @brief NIM 好友相关接口
  */
class Friend
{
public:
	typedef std::function<void(const FriendChangeEvent& )> FriendChangeCallback;	/**< 好友信息变更通知回调模板 */
	typedef std::function<void(NIMResCode res_code)> FriendOptCallback;				/**< 变更好友信息回调模板 */
	typedef std::function<void(NIMResCode res_code, const std::list<nim::FriendProfile>& user_profile_list)> GetFriendsListCallback;	/**< 获取好友回调模板 */
	typedef std::function<void(const std::string& accid, const nim::FriendProfile& user_profile)> GetFriendProfileCallback;	/**< 获取好友信息回调模板 */

public:
	/** @fn static void RegChangeCb(const FriendChangeCallback &cb, const std::string& json_extension = "")
	* (全局回调)统一注册好友变更通知回调函数（多端同步添加、删除、更新，好友列表同步）
	* @param[in] cb	好友变更通知回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	* @note 错误码	200:成功
	*				404:用户不存在
	*				500:未知错误
	*/
	static void RegChangeCb(const FriendChangeCallback &cb, const std::string& json_extension = "");

	/** @fn static bool Request(const std::string &accid, NIMVerifyType verify_type, const std::string &msg, const FriendOptCallback &cb, const std::string& json_extension = "")
	* 添加、验证好友
	* @param[in] accid	对方帐号
	* @param[in] verify_type 好友验证类型
	* @param[in] msg 接口扩展字段
	* @param[in] cb	好友请求回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				404:用户不存在
	*				500:未知错误
	*/
	static bool Request(const std::string &accid, NIMVerifyType verify_type, const std::string &msg, const FriendOptCallback &cb, const std::string& json_extension = "");
	
	/** @fn static bool Delete(const std::string &accid, const FriendOptCallback &cb, const std::string& json_extension = "")
	* 删除好友
	* @param[in] accid	对方帐号
	* @param[in] cb	删除好友回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				404:用户不存在
	*				500:未知错误
	*/
	static bool Delete(const std::string &accid, const FriendOptCallback &cb, const std::string& json_extension = "");
	
	/** @fn static bool Update(const std::string &friend_json, const FriendOptCallback &cb, const std::string& json_extension = "")
	* 更新好友资料
	* @param[in] friend_profile 好友资料
	* @param[in] cb	更新好友资料回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				404:用户不存在
	*				500:未知错误
	*/
	static bool Update(const FriendProfile &friend_profile, const FriendOptCallback &cb, const std::string& json_extension = "");
	
	/** @fn static void GetList(const GetFriendsListCallback& cb, const std::string& json_extension = "")
	* 增量获取好友列表
	* @param[in] cb	获取好友列表回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void GetList(const GetFriendsListCallback& cb, const std::string& json_extension = "");

	/** @fn static void GetFriendProfile(const std::string &accid, const GetFriendProfileCallback& cb, const std::string& json_extension = "")
	* 获取好友信息
	* @param[in] accid	对方帐号
	* @param[in] cb	获取好友信息回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void GetFriendProfile(const std::string &accid, const GetFriendProfileCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ParseFriendAddEvent(const FriendChangeEvent& change_event, FriendAddEvent& out_event)
	* 解析收到的好友添加请求通知
	* @param[in] change_event	好友添加请求通知
	* @param[out] out_event		解析后结果
	* @return bool 解析是否成功
	*/
	static bool ParseFriendAddEvent(const FriendChangeEvent& change_event, FriendAddEvent& out_event);
	
	/** @fn static bool ParseFriendDelEvent(const FriendChangeEvent& change_event, FriendDelEvent& out_event)
	* 解析收到的好友删除通知
	* @param[in] change_event	好友删除通知
	* @param[out] out_event		解析后结果
	* @return bool 解析是否成功
	*/
	static bool ParseFriendDelEvent(const FriendChangeEvent& change_event, FriendDelEvent& out_event);

	/** @fn static bool ParseFriendProfileUpdateEvent(const FriendChangeEvent& change_event, FriendProfileUpdateEvent& out_event)
	* 解析收到的好友简况更新通知
	* @param[in] change_event	好友用户信息更新通知
	* @param[out] out_event		解析后结果
	* @return bool 解析是否成功
	*/
	static bool ParseFriendProfileUpdateEvent(const FriendChangeEvent& change_event, FriendProfileUpdateEvent& out_event);

	/** @fn static bool ParseFriendProfileSyncEvent(const FriendChangeEvent& change_event, FriendProfileSyncEvent& out_event)
	* 解析收到的好友简况同步通知
	* @param[in] change_event	好友简况同步通知
	* @param[out] out_event		解析后结果
	* @return bool 解析是否成功
	*/
	static bool ParseFriendProfileSyncEvent(const FriendChangeEvent& change_event, FriendProfileSyncEvent& out_event);

	/** @fn void UnregFriendCb()
	* 反注册Friend提供的所有回调
	* @return void 无返回值
	*/
	static void UnregFriendCb();

	/** @fn bool QueryFriendshipBlock(const std::string& accid, const std::string& json_extension = "")
	* 在本地缓存数据中查询accid是否为自己的好友(同步接口，堵塞NIM内部线程，谨慎使用)
	* @param[in] accid		对方帐号
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @return bool 当正向和反向好友关系都为好友时返回true
	*/
	static bool QueryFriendshipBlock(const std::string& accid, const std::string& json_extension = "");

};

}

#endif //_NIM_SDK_CPP_FRIEND_H_