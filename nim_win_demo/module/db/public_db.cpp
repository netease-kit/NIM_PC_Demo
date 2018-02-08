#include "public_db.h"
#include "base/encrypt/encrypt_impl.h"
#include "shared/tool.h"

namespace
{
#define LOGIN_DATA_FILE		"app_login_data.db"
static std::vector<UTF8String> kCreateDBSQLs;
}

PublicDB::PublicDB()
{
    static bool sqls_created = false;
    if (!sqls_created)
    {
		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS logindata(uid TEXT PRIMARY KEY, \
								 								                                   name TEXT, password TEXT, type INTEGER, status INTEGER, remember_user INTEGER, remember_psw INTEGER, autologin INTEGER)");
		kCreateDBSQLs.push_back("CREATE INDEX IF NOT EXISTS logindatauidindex ON logindata(uid)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS proxysettingdata(proxytype INTEGER PRIMARY KEY, \
								 address TEXT, port TEXT,name TEXT, password TEXT, domain TEXT,valid INTEGER)");
		kCreateDBSQLs.push_back("CREATE INDEX IF NOT EXISTS proxydataindex ON proxysettingdata(proxytype)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS config_info(key TEXT PRIMARY KEY, value TEXT)");

		sqls_created = true;
    }
	aes_key_ = "12345500072bf3390c79f01004dabcde";//32位
	db_encrypt_key_ = "1234560247a0619f07548fb1b8abcedf";//注意：只支持最多32个字符的加密密钥！

	this->Load();
}

PublicDB::~PublicDB()
{
	this->Close();
}

ndb::SQLiteDB& PublicDB::GetSQLiteDB()
{
	return db_;
}
bool PublicDB::Load()
{
	return CreateDBFile();
}

void PublicDB::Close()
{
    db_.Close();
}

bool PublicDB::WriteLoginData(LoginData &data)
{
    nbase::NAutoLock auto_lock(&lock_);
    ndb::SQLiteStatement stmt;
	db_.Query(stmt,
		"INSERT INTO logindata(uid, name, password, type, status, remember_user, remember_psw, autologin) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

    stmt.BindText(1, data.user_id_.c_str(), (int)data.user_id_.size());
	stmt.BindText(2, data.user_name_.c_str(), (int)data.user_name_.size());
	UTF8String password_aes;
	GetAESPassword(data.user_password_, password_aes);
	stmt.BindText(3, password_aes.c_str(), (int)password_aes.size());
    stmt.BindInt(4, data.type_);
	stmt.BindInt(5, data.status_);
	stmt.BindInt(6, data.remember_user_);
	stmt.BindInt(7, data.remember_psw_);
	stmt.BindInt(8, data.auto_login_);

    int32_t result = stmt.NextRow();
    bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
    if (false == no_error)
    {
		QLOG_APP(L"error: insert login data for uid : {0}, reason : {1}") << data.user_id_ << result;
	}
    return no_error;
}

bool PublicDB::IsNeedUpdateData(const LoginData *orgi_login_data, 
	const LoginData *current_login_data, 
	bool &password_changed)
{
	if (0 != orgi_login_data->user_password_.compare(current_login_data->user_password_))
	{//密码已更改
		password_changed = true;
		return true;
	}
	else
	{
		password_changed = false;
	}

	if ((0 != orgi_login_data->user_name_.compare(current_login_data->user_name_)) ||
		(orgi_login_data->type_ != current_login_data->type_))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool PublicDB::UpdateLoginData(UTF8String &uid, 
	LoginData *current_login_data, 
	const uint8_t status, 
	bool password_changed)
{
    nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	if (true == password_changed)
	{
		UTF8String password_aes;
		GetAESPassword(current_login_data->user_password_, password_aes);
		nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET name = '%s', password = '%s', type = %d, status = %d \
									   WHERE uid = '%s'", 
									   shared::tools::FormatSQLText(current_login_data->user_name_).c_str(),
									   password_aes.c_str(), 
									   current_login_data->type_, 
									   status, 
									   shared::tools::FormatSQLText(uid).c_str());
	}
	else
	{
		nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET name = '%s', type = %d, status = %d \
									   WHERE uid = '%s'", 
									   shared::tools::FormatSQLText(current_login_data->user_name_).c_str(),
									   current_login_data->type_, 
									   status, 
									   shared::tools::FormatSQLText(uid).c_str());
	}
    
    int32_t result = db_.Query(query_sql.c_str());
    bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
    if (!no_error)
    {
		QLOG_APP(L"Error: Set LoginData password For uid: {0}, Reason : {1}") << uid << result;
	}
    
    return no_error;
}

bool PublicDB::SetStatus(UTF8String &uid, const uint8_t status)
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET status = %d \
								   WHERE uid = '%s'", status, shared::tools::FormatSQLText(uid).c_str());

	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_APP(L"Error: Set LoginData status For uid: {0},  Reason : {1}") << uid << result;
	}

	return no_error;
}

bool PublicDB::SetRemember(UTF8String &uid, const uint8_t remember_user, const uint8_t remember_psw)
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET remember_user = %d, remember_psw = %d \
								   WHERE uid = '%s'", remember_user, remember_psw, uid.c_str());

	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_APP(L"Error: Set LoginData remember For uid: {0}, Reason : {1}") << uid << result;
	}

	return no_error;
}

bool PublicDB::SetAutoLogin(UTF8String &uid, const uint8_t auto_login)
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET autologin = %d \
								   WHERE uid = '%s'", auto_login, shared::tools::FormatSQLText(uid).c_str());

	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_APP(L"Error: Set LoginData autologin For uid: {0}, Reason : {1}") << uid << result;
	}

	return no_error;
}

bool PublicDB::QueryLoginDataByUid(UTF8String &uid, LoginData &data)
{
    nbase::NAutoLock auto_lock(&lock_);
    bool result = false;
    ndb::SQLiteStatement stmt;
    db_.Query(stmt, "SELECT * FROM logindata WHERE uid=?");
	stmt.BindText(1, uid.c_str(), (int)uid.size());
    uint32_t db_reslut = stmt.NextRow();
    
    if (db_reslut == SQLITE_OK || db_reslut == SQLITE_ROW)
    {
        GetLoginDataFromStatement(stmt, data);
        result = true;
    }
    return result;
}


uint32_t PublicDB::QueryAllLoginData(std::vector<LoginData> &all_data)
{
    all_data.clear();
    nbase::NAutoLock auto_lock(&lock_);
    ndb::SQLiteStatement stmt;
    db_.Query(stmt, "SELECT * FROM logindata");
  
    int32_t result = stmt.NextRow();
    while (result == SQLITE_ROW)
    {
        LoginData login_data;
        GetLoginDataFromStatement(stmt, login_data);
        all_data.push_back(login_data);
        result = stmt.NextRow();
    }
    return (uint32_t)all_data.size();
}

bool PublicDB::CreateDBFile()
{
	if (db_.IsValid())
		return true;

    bool result = false;
	UTF8String dirctory = nbase::UTF16ToUTF8(QPath::GetNimAppDataDir(L""));
	UTF8String dbfile = dirctory + LOGIN_DATA_FILE;
	db_filepath_ = dbfile;
	result = db_.Open(dbfile.c_str(),
		db_encrypt_key_,
		ndb::SQLiteDB::modeReadWrite|ndb::SQLiteDB::modeCreate|ndb::SQLiteDB::modeSerialized
		);
	if (result)
	{
		int dbresult = SQLITE_OK;
		for (size_t i = 0; i < kCreateDBSQLs.size(); i++)
		{
			dbresult |= db_.Query(kCreateDBSQLs[i].c_str());
		}
		result = dbresult == SQLITE_OK;
	}

    return result;
}

void PublicDB::GetLoginDataFromStatement(ndb::SQLiteStatement &stmt, LoginData &data)
{
	data.user_id_ = stmt.GetTextField(0);
	data.user_name_ = stmt.GetTextField(1);
	UTF8String password_org;
	UTF8String password_aes = stmt.GetTextField(2);
	GetOrgPassword(password_aes, password_org);
	data.user_password_ = password_org;
	data.type_ = stmt.GetIntField(3);
	data.status_ = stmt.GetIntField(4);
	data.remember_user_ = stmt.GetIntField(5);
	data.remember_psw_ = stmt.GetIntField(6);
	data.auto_login_ = stmt.GetIntField(7);
}

void PublicDB::GetAESPassword(const UTF8String &password_org, UTF8String &password_aes)
{
	nbase::EncryptInterface_var encrypt_enc(new nbase::Encrypt_Impl());
	encrypt_enc->SetMethod(nbase::ENC_AES128);
	encrypt_enc->SetEncryptKey(aes_key_);
	encrypt_enc->Encrypt(password_org, password_aes);
	password_aes = nbase::BinaryToHexString(password_aes);
}

void PublicDB::GetOrgPassword(const UTF8String &password_aes, UTF8String &password_org)
{
	nbase::EncryptInterface_var encrypt_dec(new nbase::Encrypt_Impl());
	encrypt_dec->SetMethod(nbase::ENC_AES128);
	encrypt_dec->SetDecryptKey(aes_key_);
	std::string password_enc = nbase::HexStringToBinary(password_aes);
	encrypt_dec->Decrypt(password_enc, password_org);
}

bool PublicDB::SetAllLoginDataDeleted()
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET status = %d",
		kLoginDataStatusDeleted);
	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_APP(L"Error: Set All Login Data Deleted");
	}
	return no_error;
}

void PublicDB::ReadLoginData()
{
	try
	{
		std::vector<LoginData> all_data;
		this->QueryAllLoginData(all_data);
		if (all_data.size() > 0)
		{
			std::vector<LoginData>::iterator it = all_data.begin();
			for (; it != all_data.end(); it++)
			{
				if (kLoginDataStatusValid == it->status_)
				{
					current_login_data_ = *it;
					break;
				}
			}
		}
	}
	catch (...)
	{

	}
}

void PublicDB::SaveLoginData()
{
	LoginData login_data;
	bool ret = this->QueryLoginDataByUid(current_login_data_.user_id_, login_data);
	if (false == ret)
	{
		//不存在，则直接写入，并将原来的登录帐号全部设为已删除
		this->SetAllLoginDataDeleted();
		this->WriteLoginData(current_login_data_);
	}
	else
	{
		this->SetAllLoginDataDeleted();
		bool password_changed = false;
		if (true == this->IsNeedUpdateData(&login_data, &current_login_data_, password_changed))
		{
			this->UpdateLoginData(current_login_data_.user_id_,
				&current_login_data_,
				kLoginDataStatusValid,
				password_changed);
		}
		else
		{
			//只更改状态
			this->SetStatus(current_login_data_.user_id_, kLoginDataStatusValid);
		}
		//是否记住帐号密码
		this->SetRemember(current_login_data_.user_id_, current_login_data_.remember_user_, current_login_data_.remember_psw_);
	}
}

bool PublicDB::InsertConfigData(const std::string& key, const std::string& value)
{
	nbase::NAutoLock auto_lock(&lock_);

	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "INSERT OR REPLACE into config_info (key, value) values (?, ?);");
	stmt.BindText(1, key.c_str(), (int)key.size());
	stmt.BindText(2, value.c_str(), (int)value.size());
	int32_t result = stmt.NextRow();
	stmt.Finalize();

	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_ERR(L"error: InsertConfigData for key: {0}, reason: {1}") << key << result;
	}

	return no_error;
}

void PublicDB::QueryConfigData(const std::string& key, std::string& value)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;

	db_.Query(stmt, "SELECT value FROM config_info WHERE key=?");
	stmt.BindText(1, key.c_str(), (int)key.size());
	int32_t result = stmt.NextRow();

	if (result == SQLITE_ROW)
		value = stmt.GetTextField(0);
}

void PublicDB::ClearConfigData()
{
	nbase::NAutoLock auto_lock(&lock_);

	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "delete from config_info;");
	stmt.NextRow();
}
