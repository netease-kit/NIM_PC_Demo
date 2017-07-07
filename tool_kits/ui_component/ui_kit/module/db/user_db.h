#include "base/synchronization/lock.h"
#include "db/db_sqlite3.h"


namespace nim_comp
{
/** @class UserDB
  * @brief 用户数据库，存储某个用户的数据
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class UserDB
{
public:
	SINGLETON_DEFINE(UserDB);
	UserDB();
	virtual ~UserDB();

public:
	/**
	* 加载数据库
	* @return bool true 成功，false 失败
	*/
	bool Load();

	/**
	* 关闭数据库
	* @return void	无返回值
	*/
	void Close();

public:
	/**
	* 保存文件传输中的文件保存路径
	* @param[in] msg_id 消息id
	* @param[in] path 文件路径
	* @param[in] extend 扩展信息
	* @return bool true 成功，false 失败
	*/
	bool InsertData(const std::string& msg_id, const std::string& path, const std::string& extend);

	/**
	* 获取文件传输中的文件保存路径
	* @param[in] msg_id 消息id
	* @param[out] path 文件路径
	* @param[out] extend 扩展信息
	* @return bool true 成功，false 失败
	*/
	bool QueryDataWithMsgId(const std::string& msg_id, std::string& path, std::string& extend);

public:
	/**
	* 保存自定义通知消息
	* @param[in] msg 自定义通知消息
	* @return bool true 成功，false 失败
	*/
	bool InsertMsgData(const nim::SysMessage& msg);

	/**
	* 获取指定时间之后的自定义通知消息
	* @param[in] time 时间
	* @param[in] time 获取的数量上限
	* @return std::vector<nim::SysMessage> 自定义消息列表
	*/
	std::vector<nim::SysMessage> QueryMsgData(int64_t time, int limit = 20);

public:
	/**
	* 保存强推信息
	* @param[in] data 强推信息
	* @return bool true 成功，false 失败
	*/
	bool InsertForcePushData(std::map<std::string, std::string> &data);

	/**
	* 获取所有本地保存的强推信息
	* @param[out] data 强推信息
	* @return void 无返回值
	*/
	void QueryAllForcePushData(std::map<std::string, std::string> &data);

	/**
	* 清除所有本地保存的强推信息
	* @return void 无返回值
	*/
	void ClearForcePushData();

public:
	enum TimeTagType
	{
		kTimeTagTypeSubscribeEvent	= 0
	};

	/**
	* 插入某种类型的时间戳
	* @param[in] type 时间戳类型
	* @param[in] timetag 时间戳
	* @return bool true 成功，false 失败
	*/
	bool InsertTimetag(TimeTagType type, uint64_t timetag);

	/**
	* 获取某种类型的时间戳
	* @param[in] type 时间戳类型
	* @param[out] timetag 时间戳
	* @return bool true 成功，false 失败
	*/
	bool QueryTimetag(TimeTagType type, uint64_t &timetag);

private:
	/**
	* 创建数据库
	* @return bool true 成功，false 失败
	*/
    bool CreateDBFile();

private:
    UTF8String		db_filepath_;
    ndb::SQLiteDB   db_;
    nbase::NLock    lock_;
};
}