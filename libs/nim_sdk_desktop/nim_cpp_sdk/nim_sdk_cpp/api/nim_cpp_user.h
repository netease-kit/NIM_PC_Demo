/** @file nim_cpp_user.h
  * @brief NIM SDK提供的用户相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/8/17
  */

#ifndef _NIM_SDK_CPP_USER_H_
#define _NIM_SDK_CPP_USER_H_

#include <string>
#include <list>
#include <functional>
#include "nim_user_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
#include "nim_user_def.h"
#include "nim_res_code_def.h"

/** @class User
  * @brief NIM SDK提供的用户相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/8/17
  */
class User
{

public:
	typedef std::function<void(const SpecialRelationshipChangeEvent&)> SpecialRelationshipChangedCallback;		/**< 黑名单\静音列表变更事件通知回调模板 */

	typedef std::function<void(NIMResCode res_code, const std::string& accid, bool set_opt)> SetRelationCallback;	/**< 设置黑名单\静音列表回调模板 */
	typedef SetRelationCallback SetBlackCallback;																/**< 设置黑名单回调模板 */
	typedef SetRelationCallback SetMuteCallback;																/**< 设置静音列表回调模板 */

	typedef std::function<void(NIMResCode res_code, const std::list<MuteListInfo>&)> GetMuteListCallback;		/**< 获取静音列表回调模板 */
	typedef std::function<void(NIMResCode res_code, const std::list<BlackListInfo>&)> GetBlackListCallback;		/**< 设置黑名单列表回调模板 */

	typedef std::function<void(const std::list<UserNameCard>&)> UserNameCardChangedCallback;					/**< 用户名片变更通知回调模板 */
	typedef std::function<void(const std::list<UserNameCard>&)> GetUserNameCardCallback;						/**< 获取用户名片回调模板 */
	typedef std::function<void(NIMResCode res_code)> UpdateMyUserNameCardCallback;								/**< 更新自己的名片回调模板 */

	/** @fn static void RegSpecialRelationshipChangedCb(const SpecialRelationshipChangedCallback& cb, const std::string& json_extension = "")
	* (全局回调)统一注册用户属性变更通知回调函数（多端同步黑名单、静音名单变更）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void RegSpecialRelationshipChangedCb(const SpecialRelationshipChangedCallback& cb, const std::string& json_extension = "");

	/** @fn static bool SetBlack(const std::string& accid, bool set_black, const SetBlackCallback& cb, const std::string& json_extension = "")
	* 设置、取消设置黑名单
	* @param[in] accid 好友id
	* @param[in] set_black 取消或设置
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				419:黑名单数量超过上限 
	*/
	static bool SetBlack(const std::string& accid, bool set_black, const SetBlackCallback& cb, const std::string& json_extension = "");

	/** @fn static bool SetMute(const std::string& accid, bool set_mute, const SetMuteCallback& cb, const std::string& json_extension = "")
	* 设置、取消设置静音名单
	* @param[in] accid 好友id
	* @param[in] set_mute 取消或设置
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				419:静音列表数量超过上限 
	*/
	static bool SetMute(const std::string& accid, bool set_mute, const SetMuteCallback& cb, const std::string& json_extension = "");

	/** @fn static void GetMutelist(const GetMuteListCallback& cb, const std::string& json_extension = "")
	* 获得静音名单列表 
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*/
	static void GetMutelist(const GetMuteListCallback& cb, const std::string& json_extension = "");

	/** @fn static void GetBlacklist(const GetBlackListCallback& cb, const std::string& json_extension = "")
	* 获得黑名单名单
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void GetBlacklist(const GetBlackListCallback& cb, const std::string& json_extension = "");

	/** @fn static void RegUserNameCardChangedCb(const UserNameCardChangedCallback& cb, const std::string& json_extension = "")
	* (全局回调)统一注册用户名片变更通知回调函数
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegUserNameCardChangedCb(const UserNameCardChangedCallback& cb, const std::string& json_extension = "");

	/** @fn static bool GetUserNameCard(const std::list<std::string>& accids, const GetUserNameCardCallback& cb, const std::string& json_extension = "")
	* 获取本地的指定帐号的用户名片
	* @param[in] accids 
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	* @note 错误码	200:成功
	*				414:参数错误
	*				20000:还未初始化或初始化未正常完成
	*				20001:还未登陆或登录未完成
	*/
	static bool GetUserNameCard(const std::list<std::string>& accids, const GetUserNameCardCallback& cb, const std::string& json_extension = "");

	/** @fn static bool GetUserNameCardOnline(const std::list<std::string>& accids, const GetUserNameCardCallback& cb, const std::string& json_extension = "")
	* 在线查询指定帐号的用户名片
	* @param[in] accids 
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				414:参数错误
	*				419:超限
	*				20000:还未初始化或初始化未正常完成
	*				20001:还未登陆或登录未完成
	*/
	static bool GetUserNameCardOnline(const std::list<std::string>& accids, const GetUserNameCardCallback& cb, const std::string& json_extension = "");

	/** @fn static bool UpdateMyUserNameCard(const UserNameCard& namecard, const UpdateMyUserNameCardCallback& cb, const std::string& json_extension = "")
	* 更新自己的用户名片
	* @param[in] namecard 用户名片内容
	* @param[in] cb 操作结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				400:非法参数
	*/
	static bool UpdateMyUserNameCard(const UserNameCard& namecard, const UpdateMyUserNameCardCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ParseBlackListInfoChange(const SpecialRelationshipChangeEvent& change_event, BlackListUserNameCard& info)
	* 解析黑名单变更通知
	* @param[in] change_event 特殊关系变更通知
	* @param[out] info   解析结果
	* @return bool 解析是否成功
	*/
	static bool ParseBlackListInfoChange(const SpecialRelationshipChangeEvent& change_event, BlackListInfo& info);

	/** @fn static bool ParseMuteListInfoChange(const SpecialRelationshipChangeEvent& change_event, MuteListInfo& info)
	* 解析静音名单变更通知
	* @param[in] change_event 特殊关系变更通知
	* @param[out] info   解析结果
	* @return bool 解析是否成功
	*/
	static bool ParseMuteListInfoChange(const SpecialRelationshipChangeEvent& change_event, MuteListInfo& info);

	/** @fn static bool ParseSyncSpecialRelationshipChange(const SpecialRelationshipChangeEvent& change_event, std::list<BlackListInfo>& black_list, std::list<MuteListInfo>& mute_list)
	* 解析多端同步特殊关系通知
	* @param[in] change_event 特殊关系变更通知
	* @param[out] black_list  黑名单列表
	* @param[out] mute_list   静音列表
	* @return bool 解析是否成功
	*/
	static bool ParseSyncSpecialRelationshipChange(const SpecialRelationshipChangeEvent& change_event, std::list<BlackListInfo>& black_list, std::list<MuteListInfo>& mute_list);

	/** @fn void UnregUserCb()
	* 反注册User提供的所有回调
	* @return void 无返回值
	*/
	static void UnregUserCb();

};

}

#endif //_NIM_SDK_CPP_USER_H_