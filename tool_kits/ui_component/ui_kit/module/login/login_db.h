#ifndef NIM_WIN_FM_LOGIN_LOGIN_DB_H_
#define NIM_WIN_FM_LOGIN_LOGIN_DB_H_

#include "base/synchronization/lock.h"
#include "base/memory/singleton.h"
#include "db/db_sqlite3.h"


//登录数据状态
enum LoginDataStatus
{
	kLoginDataStatusNone       = 0,    //默认,不能当查询条件,意义太多
	kLoginDataStatusValid,             //可用, 1
	kLoginDataStatusDeleted,           //已删, 2
};

struct LoginData
{
	LoginData()
	{
		user_id_		= "";     
		user_name_		= "";          
		user_password_	= ""; 	 
		type_			= 0;  
		status_			= kLoginDataStatusNone;
		remember_		= 0;
		auto_login_		= 0;
	}

	void InitLoginData()
	{
		user_id_		= "";     
		user_name_		= "";          
		user_password_	= ""; 	
		type_			= 0; 
		status_			= kLoginDataStatusNone;
		remember_		= 0;
		auto_login_		= 0;
	}

	UTF8String user_id_;            //用户ID
	UTF8String user_name_;          //用户名
	UTF8String user_password_;      //用户密码(未加密的)
	uint8_t	   type_;               //用户类型
	uint8_t	   status_;             //状态
	uint8_t	   remember_;			//是否记住密码：1=记住，0=不记住
	uint8_t	   auto_login_;			//是否自动登录：1=自动登录，0=不自动登录
};

namespace nim_comp
{
class LoginDB
{
public:
	SINGLETON_DEFINE(LoginDB);

    LoginDB();
    virtual ~LoginDB();

public:
    bool	Load();
    void	Close();
    //写入登录数据
    bool    WriteLoginData(LoginData &data);
	//是否需要更新登录帐号的信息
	bool    IsNeedUpdateData(const LoginData *orgi_login_data, 
							 const LoginData *current_login_data, 
							 bool &password_changed);
	//更新登录帐号的信息
	bool    UpdateLoginData(UTF8String &uid, 
							LoginData *current_login_data, 
							const uint8_t status, 
							bool password_changed);
	//设置登录帐号的状态
	bool    SetStatus(UTF8String &uid, const uint8_t status);
	//设置是否记住登录帐号的密码
	bool    SetRemember(UTF8String &uid, const uint8_t remember);
	//设置是否自动登录
	bool    SetAutoLogin(UTF8String &uid, const uint8_t auto_login);
	//读取登录数据
    bool    QueryLoginDataByUid(UTF8String &uid, LoginData &data);   
    // 批量获取登录数据
    uint32_t QueryAllLoginData(std::vector<LoginData> &all_data);     
	//设置所有登录数据为删除状态
	bool    SetAllLoginDataDeleted();
	void	GetAESPassword(const UTF8String &password_org, UTF8String &password_aes);
	void	GetOrgPassword(const UTF8String &password_aes, UTF8String &password_org);
	ndb::SQLiteDB& GetSQLiteDB();

private:
    bool    CreateDBFile();
    void    GetLoginDataFromStatement(ndb::SQLiteStatement &stmt, LoginData &data);
    
private:
    UTF8String		db_filepath_;
    ndb::SQLiteDB   db_;
    nbase::NLock    lock_;
	std::string		aes_key_;
	std::string		db_encrypt_key_;
};
}
#endif //NIM_WIN_FM_LOGIN_LOGIN_DB_H_
