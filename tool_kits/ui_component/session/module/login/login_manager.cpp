#include "login_manager.h"

namespace nim_comp
{
LoginManager::LoginManager()
{
	account_ = "";
	password_ = "";
	status_ = LoginStatus_NONE;
	active_ = true;
	current_login_data_.reset(new LoginData);
	ReadLoginDataFromFile();
}

bool LoginManager::IsSelf( const std::string &account )
{
	return (account == account_);
}

void LoginManager::SetAccount( const std::string &account )
{
	account_ = account;
	nbase::LowerString(account_);;
}

std::string LoginManager::GetAccount()
{
	return account_;
}

bool LoginManager::IsEqual(const std::string& account)
{
	std::string new_account = account;
	nbase::LowerString(new_account);
	return account_ == new_account;
}

void LoginManager::SetPassword( const std::string &password )
{
	password_ = password;
}

std::string LoginManager::GetPassword()
{
	return password_;
}

void LoginManager::SetLoginStatus( LoginStatus status )
{
	status_ = status;
}

LoginStatus LoginManager::GetLoginStatus()
{
	return status_;
}

void LoginManager::SetLinkActive( bool active )
{
	active_ = active;
	
	Json::Value json;
	json["link"] = active;
	NotifyCenter::GetInstance()->InvokeNotify(NT_LINK, json);
}

bool LoginManager::IsLinkActive()
{
	return active_;
}


void LoginManager::ReadLoginDataFromFile()
{
	LoginDB* login_db = LoginDB::GetInstance();
	if (login_db)
	{
		try
		{
			login_db->Load();
			std::vector<LoginData> all_data;
			login_db->QueryAllLoginData(all_data);  
			if (all_data.size() > 0)
			{
				std::vector<LoginData>::iterator it = all_data.begin();
				for (; it != all_data.end(); it++)
				{
					if (kLoginDataStatusValid == it->status_)
					{
						*current_login_data_ = *it;
						break;
					}
				}				
			}	
			default_login_account_ = current_login_data_->user_name_;
		}
		catch(...)
		{
			//DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "decrypt user info error - kernel error");
		}  
	}
}


void LoginManager::SaveLoginData()
{
	// 可能是第一次注册，因此需要更新defaultloginaccount
	if (current_login_data_->user_name_.length() > 0) 
	{
		default_login_account_ = current_login_data_->user_name_;
	}
	LoginDB* login_db = LoginDB::GetInstance();
	if (login_db)
	{
		LoginData login_data;
		bool ret = login_db->QueryLoginDataByUid(current_login_data_->user_id_, login_data);
		if (false == ret)
		{//不存在，则直接写入，并将原来的登录帐号全部设为已删除
			login_db->SetAllLoginDataDeleted();
			login_db->WriteLoginData(*current_login_data_);
		}
		else
		{
			login_db->SetAllLoginDataDeleted();
			bool password_changed = false;
			if (true == login_db->IsNeedUpdateData(&login_data, current_login_data_.get(), password_changed))
			{			
				login_db->UpdateLoginData(current_login_data_->user_id_, 
					current_login_data_.get(), 
					kLoginDataStatusValid,
					password_changed);
			}
			else
			{//只更改状态
				login_db->SetStatus(current_login_data_->user_id_, kLoginDataStatusValid);
			}
			//是否记住帐号密码
			login_db->SetRemember(current_login_data_->user_id_, current_login_data_->remember_);
			//是否自动登录
			login_db->SetAutoLogin(current_login_data_->user_id_, current_login_data_->auto_login_);
		}

	}  
}
}