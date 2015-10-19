#include "login_db.h"
#include "base/encrypt/encrypt_impl.h"
#include "shared/tool.h"

namespace nim_comp
{
#define LOGIN_DATA_FILE		"app_login_data.db"
static std::vector<UTF8String> kLoginDataSQLs;

LoginDB::LoginDB()
{
    static bool sqls_created = false;
    if (!sqls_created)
    {
        kLoginDataSQLs.push_back("CREATE TABLE IF NOT EXISTS logindata(uid TEXT PRIMARY KEY, \
                                  name TEXT, password TEXT, type INTEGER, status INTEGER, remember INTEGER, autologin INTEGER)");
        kLoginDataSQLs.push_back("CREATE INDEX IF NOT EXISTS logindatauidindex ON logindata(uid)");
		kLoginDataSQLs.push_back("CREATE TABLE IF NOT EXISTS proxysettingdata(proxytype INTEGER PRIMARY KEY, \
								 address TEXT, port TEXT,name TEXT, password TEXT, domain TEXT,valid INTEGER)");
		kLoginDataSQLs.push_back("CREATE INDEX IF NOT EXISTS proxydataindex ON proxysettingdata(proxytype)");
		kLoginDataSQLs.push_back("CREATE TABLE IF NOT EXISTS foreignarea(id INTEGER PRIMARY KEY, searchcode TEXT)");
		kLoginDataSQLs.push_back("CREATE TABLE IF NOT EXISTS loginforeigncode(uid TEXT PRIMARY KEY, code TEXT)");
        sqls_created = true;
    }
	aes_key_ = "12345500072bf3390c79f01004dabcde";//32位
	db_encrypt_key_ = "1234560247a0619f07548fb1b8abcedf";//注意：只支持最多32个字符的加密密钥！
}

LoginDB::~LoginDB()
{

}

ndb::SQLiteDB& LoginDB::GetSQLiteDB()
{
	return db_;
}
bool LoginDB::Load()
{
	return CreateDBFile();
}

void LoginDB::Close()
{
    db_.Close();
}

bool LoginDB::WriteLoginData(LoginData &data)
{
    nbase::NAutoLock auto_lock(&lock_);
    ndb::SQLiteStatement stmt;
    db_.Query(stmt, 
		"INSERT INTO logindata(uid, name, password, type, status, remember, autologin) VALUES (?, ?, ?, ?, ?, ?, ?);");

    stmt.BindText(1, data.user_id_.c_str(), data.user_id_.size());
    stmt.BindText(2, data.user_name_.c_str(), data.user_name_.size());
	UTF8String password_aes;
	GetAESPassword(data.user_password_, password_aes);
    stmt.BindText(3, password_aes.c_str(), password_aes.size());
    stmt.BindInt(4, data.type_);
	stmt.BindInt(5, data.status_);
	stmt.BindInt(6, data.remember_);
	stmt.BindInt(7, data.auto_login_);

    int32_t result = stmt.NextRow();
    bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
    if (false == no_error)
    {
        //DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "error: insert login data for uid : %s, \
        //       reason : %d", data.user_id_.c_str(), result);

    }
    return no_error;
}

bool LoginDB::IsNeedUpdateData(const LoginData *orgi_login_data, 
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

bool LoginDB::UpdateLoginData(UTF8String &uid, 
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
									   current_login_data->user_name_.c_str(),
									   password_aes.c_str(), 
									   current_login_data->type_, 
									   status, 
									   uid.c_str());
	}
	else
	{
		nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET name = '%s', type = %d, status = %d \
									   WHERE uid = '%s'", 
									   current_login_data->user_name_.c_str(), 
									   current_login_data->type_, 
									   status, 
									   uid.c_str());
	}
    
    int32_t result = db_.Query(query_sql.c_str());
    bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
    if (!no_error)
    {
        //DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "Error: Set LoginData password For uid: %s,  \
        //       Reason : %d", uid.c_str(), result);
    }
    
    return no_error;
}

bool    LoginDB::SetStatus(UTF8String &uid, const uint8_t status)
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET status = %d \
								   WHERE uid = '%s'", status, uid.c_str());

	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		//DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "Error: Set LoginData status For uid: %s,  \
		//														Reason : %d", uid.c_str(), result);
	}

	return no_error;
}

bool    LoginDB::SetRemember(UTF8String &uid, const uint8_t remember)
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET remember = %d \
								   WHERE uid = '%s'", remember, uid.c_str());

	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		//DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "Error: Set LoginData remember For uid: %s,  \
		//														Reason : %d", uid.c_str(), result);
	}

	return no_error;
}

bool    LoginDB::SetAutoLogin(UTF8String &uid, const uint8_t auto_login)
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET autologin = %d \
								   WHERE uid = '%s'", auto_login, uid.c_str());

	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		//DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "Error: Set LoginData autologin For uid: %s,  \
		//														Reason : %d", uid.c_str(), result);
	}

	return no_error;
}

bool    LoginDB::QueryLoginDataByUid(UTF8String &uid, LoginData &data)
{
    nbase::NAutoLock auto_lock(&lock_);
    bool result = false;
    ndb::SQLiteStatement stmt;
    db_.Query(stmt, "SELECT * FROM logindata WHERE uid=?");
    stmt.BindText(1, uid.c_str(), uid.size());
    uint32_t db_reslut = stmt.NextRow();
    
    if (db_reslut == SQLITE_OK || db_reslut == SQLITE_ROW)
    {
        GetLoginDataFromStatement(stmt, data);
        result = true;
    }
    return result;
}

uint32_t  LoginDB::QueryAllLoginData(std::vector<LoginData> &all_data)
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
    return all_data.size();
}

bool    LoginDB::CreateDBFile()
{
    bool result = false;
	UTF8String dirctory = nbase::UTF16ToUTF8(QPath::GetNimAppDataDir());
	UTF8String dbfile = dirctory + LOGIN_DATA_FILE;
	db_filepath_ = dbfile;
	result = db_.Open(dbfile.c_str(),
		db_encrypt_key_,
		ndb::SQLiteDB::modeReadWrite|ndb::SQLiteDB::modeCreate|ndb::SQLiteDB::modeSerialized
		);
	if (result)
	{
		int dbresult = SQLITE_OK;
		for (size_t i = 0; i < kLoginDataSQLs.size(); i++)
		{
			dbresult |= db_.Query(kLoginDataSQLs[i].c_str());
		}
		result = dbresult == SQLITE_OK;
	}

    return result;
}

void    LoginDB::GetLoginDataFromStatement(ndb::SQLiteStatement &stmt, LoginData &data)
{
    data.user_id_       = stmt.GetTextField(0);
    data.user_name_     = stmt.GetTextField(1);
	UTF8String password_org;
	UTF8String password_aes = stmt.GetTextField(2);
	GetOrgPassword(password_aes, password_org);
    data.user_password_	= password_org;
    data.type_          = stmt.GetIntField(3); 
	data.status_        = stmt.GetIntField(4); 
	data.remember_      = stmt.GetIntField(5); 
	data.auto_login_    = stmt.GetIntField(6); 
}

void	LoginDB::GetAESPassword(const UTF8String &password_org, UTF8String &password_aes)
{
	nbase::EncryptInterface_var encrypt_enc(new nbase::Encrypt_Impl());
	encrypt_enc->SetMethod(nbase::ENC_AES128);
	encrypt_enc->SetEncryptKey(aes_key_);
	encrypt_enc->Encrypt(password_org, password_aes);
	password_aes = nbase::BinaryToHexString(password_aes);
}

void	LoginDB::GetOrgPassword(const UTF8String &password_aes, UTF8String &password_org)
{
	nbase::EncryptInterface_var encrypt_dec(new nbase::Encrypt_Impl());
	encrypt_dec->SetMethod(nbase::ENC_AES128);
	encrypt_dec->SetDecryptKey(aes_key_);
	std::string password_enc = nbase::HexStringToBinary(password_aes);
	encrypt_dec->Decrypt(password_enc, password_org);
}

bool    LoginDB::SetAllLoginDataDeleted()
{
	nbase::NAutoLock auto_lock(&lock_);
	UTF8String query_sql;
	nbase::StringPrintf(query_sql, "UPDATE OR ROLLBACK logindata SET status = %d",
		kLoginDataStatusDeleted);
	int32_t result = db_.Query(query_sql.c_str());
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		//DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "Error: Set All Login Data Deleted");
	}
	return no_error;
}
}