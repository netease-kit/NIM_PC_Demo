
#include "base/synchronization/lock.h"
#include "db/db_sqlite3.h"
#include "msglog.h"

namespace nim_comp
{
class MsgExDB
{
public:
	SINGLETON_DEFINE(MsgExDB);
	MsgExDB();
	virtual ~MsgExDB();

public:
    bool Load();
    void Close();
	//用于保存文件传输中的文件保存路径
	bool InsertData(const std::string& msg_id, const std::string& path, const std::string& extend);
	bool QueryDataWithMsgId(const std::string& msg_id, std::string& path, std::string& extend);

	//用于保存一些自定义通知消息
	bool InsertMsgData(const nim::SysMessage& msg);
	std::vector<nim::SysMessage> QueryMsgData(int64_t time, int limit = 20);

private:
    bool CreateDBFile();
    
private:
    UTF8String		db_filepath_;
    ndb::SQLiteDB   db_;
    nbase::NLock    lock_;
};
}