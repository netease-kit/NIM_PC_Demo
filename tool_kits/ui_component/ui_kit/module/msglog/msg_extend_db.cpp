
#include "msg_extend_db.h"
#include "module/login/login_manager.h"

namespace nim_comp
{
#define MSG_EX_FILE		"msg_extend.db"
static std::vector<UTF8String> kMsgLogSQLs;

MsgExDB::MsgExDB()
{
	static bool sqls_created = false;
	if (!sqls_created)
	{
		kMsgLogSQLs.push_back("CREATE TABLE IF NOT EXISTS msg_local_file(msg_id TEXT PRIMARY KEY, path TEXT, extend TEXT)");

		kMsgLogSQLs.push_back("CREATE TABLE IF NOT EXISTS custom_msglog(serial INTEGER PRIMARY KEY, \
							  to_account TEXT, from_account TEXT, msg_type INTEGER, msg_time INTEGER, msg_id INTEGER, save_flag INTEGER, \
							  msg_body TEXT, msg_attach TEXT, apns_text TEXT, msg_status INTEGER, msg_param TEXT)");

		kMsgLogSQLs.push_back("CREATE TABLE IF NOT EXISTS force_push_data(session_id TEXT PRIMARY KEY, data TEXT)");
		sqls_created = true;
	}
	Load();
}

MsgExDB::~MsgExDB()
{

}

bool MsgExDB::Load()
{
	return CreateDBFile();
}

void MsgExDB::Close()
{
	db_.Close();
}

bool MsgExDB::CreateDBFile()
{
	bool result = false;
	std::string acc = LoginManager::GetInstance()->GetAccount();
	std::wstring dirctory = QPath::GetUserAppDataDir(acc);
	UTF8String dbfile = nbase::UTF16ToUTF8(dirctory) + MSG_EX_FILE;
	db_filepath_ = dbfile;
	std::string key = nim::Tool::GetMd5(LoginManager::GetInstance()->GetAccount());
	result = db_.Open(dbfile.c_str(),
		key,
		ndb::SQLiteDB::modeReadWrite|ndb::SQLiteDB::modeCreate|ndb::SQLiteDB::modeSerialized
		);
	if (result)
	{
		int dbresult = SQLITE_OK;
		for (size_t i = 0; i < kMsgLogSQLs.size(); i++)
		{
			dbresult |= db_.Query(kMsgLogSQLs[i].c_str());
		}
		result = dbresult == SQLITE_OK;
	}

	return result;
}

bool MsgExDB::InsertData(const std::string& msg_id, const std::string& path, const std::string& extend)
{
	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "INSERT OR REPLACE into msg_local_file (msg_id, path, extend) values (?, ?, ?);");

	stmt.BindText(1, msg_id.c_str(), msg_id.size());
	stmt.BindText(2, path.c_str(), path.size());
	stmt.BindText(3, extend.c_str(), extend.size());
	int32_t result = stmt.NextRow();
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (no_error)
	{
		return true;
	}
	else
	{
		QLOG_ERR(L"error: insert MsgExDB for id: {0}, path: {1}, reason: {2}") << msg_id << path << result;
	}
	return false;
}

bool MsgExDB::QueryDataWithMsgId(const std::string& msg_id, std::string& path, std::string& extend)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "SELECT * FROM msg_local_file WHERE msg_id=?");
	stmt.BindText(1, msg_id.c_str(), msg_id.size());
	int32_t result = stmt.NextRow();
	bool find = false;
	if (result == SQLITE_ROW)
	{
		path = stmt.GetTextField(1);
		extend = stmt.GetTextField(2);
		find = true;
	}
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_ERR(L"error: QueryDataWithMsgId for id: {0}, reason: {1}") << msg_id << result;
	}
	return find;
}

//用于保存一些自定义通知消息
bool MsgExDB::InsertMsgData(const nim::SysMessage& msg)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;

	db_.Query(stmt, "insert into custom_msglog(serial, to_account, from_account, \
					msg_type, msg_time, msg_id, save_flag, msg_body, msg_attach, apns_text, \
					msg_status, msg_param) values(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

	stmt.BindText(1, msg.receiver_accid_.c_str(), msg.receiver_accid_.size());
	stmt.BindText(2, msg.sender_accid_.c_str(), msg.sender_accid_.size());
	stmt.BindInt(3, msg.type_);
	stmt.BindInt64(4, msg.timetag_);
	stmt.BindInt64(5, msg.id_);
	stmt.BindInt(6, msg.msg_setting_.need_offline_);
	stmt.BindText(7, msg.content_.c_str(), msg.content_.size());
	stmt.BindText(8, msg.attach_.c_str(), msg.attach_.size());
	stmt.BindText(9, msg.msg_setting_.push_content_.c_str(), msg.msg_setting_.push_content_.size());
	stmt.BindInt(10, msg.status_);
	stmt.BindText(11, "");

	int32_t result = stmt.NextRow();
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_ERR(L"error: InsertMsgData, reason: {0}") << result;
	}
	return no_error;
}

std::vector<nim::SysMessage> MsgExDB::QueryMsgData(int64_t time, int limit)
{
	nbase::NAutoLock auto_lock(&lock_);
	std::vector<nim::SysMessage> ret_msgs;
	ndb::SQLiteStatement stmt;
	if (time <= 0)
	{
		db_.Query(stmt, "SELECT * FROM custom_msglog ORDER BY msg_time DESC LIMIT ? ");
		stmt.BindInt(1, limit);
	} 
	else
	{
		db_.Query(stmt, "SELECT * FROM custom_msglog WHERE msg_time < ? ORDER BY msg_time DESC LIMIT ? ");
		stmt.BindInt64(1, time);
		stmt.BindInt(2, limit);
	}
	int32_t result = stmt.NextRow();
	while (result == SQLITE_ROW)
	{
		nim::SysMessage msg;
		msg.receiver_accid_ = stmt.GetTextField(1);
		msg.sender_accid_ = stmt.GetTextField(2);
		msg.type_ = (nim::NIMSysMsgType)stmt.GetIntField(3);
		msg.timetag_ = stmt.GetInt64Field(4);
		msg.id_ = stmt.GetInt64Field(5);
		msg.msg_setting_.need_offline_ = stmt.GetIntField(6) > 0 ? nim::BS_TRUE : nim::BS_FALSE;
		msg.content_ = stmt.GetTextField(7);
		msg.attach_ = stmt.GetTextField(8);
		msg.msg_setting_.push_content_ = stmt.GetTextField(9);
		msg.status_ = (nim::NIMSysMsgStatus)stmt.GetIntField(10);
		ret_msgs.push_back(msg);
		result = stmt.NextRow();
	}
	return ret_msgs;
}

bool MsgExDB::InsertForcePushData(std::map<std::string, std::string> &data)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteAutoTransaction transaction(&db_);
	transaction.Begin();

	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "INSERT OR REPLACE into force_push_data (session_id, data) values (?, ?);");

	bool no_error = true;
	for (auto &i : data)
	{
		stmt.BindText(1, i.first.c_str(), i.first.size());
		stmt.BindText(2, i.second.c_str(), i.second.size());
		int32_t result = stmt.NextRow();
		no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
		if (!no_error)
		{
		 	QLOG_ERR(L"error: InsertForcePushData for session_id: {0}, reason: {1}") << i.first << result;
		 	break;
		}
		stmt.Rewind();
	}
	stmt.Finalize();

	if (no_error)
		return transaction.Commit();
	else
		return transaction.Rollback();
}

void MsgExDB::QueryAllForcePushData(std::map<std::string, std::string> &data)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;

	db_.Query(stmt, "SELECT * FROM force_push_data");
	int32_t result = stmt.NextRow();

	std::string session_id;
	std::string session_data;
	while (result == SQLITE_ROW)
	{
		session_id = stmt.GetTextField(0);
		session_data = stmt.GetTextField(1);
	
		data[session_id] = session_data;
		result = stmt.NextRow();
	}
}

void MsgExDB::ClearForcePushData()
{
	nbase::NAutoLock auto_lock(&lock_);

	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "delete from force_push_data;");
	stmt.NextRow();
}

}