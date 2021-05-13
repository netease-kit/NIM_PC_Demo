
#ifndef DB_DB_PRETREATMENT_H_
#define DB_DB_PRETREATMENT_H_
#include <map>
#include <functional>
#include <list>
#include "db_sqlite3.h"
namespace ndb
{
	template<typename TDBVersionType>
	class DB_EXPORT DBVersionOpration
	{
	public:
		static bool HasVersionTable(ndb::SQLiteDB* db){ return false; }
		static bool UpdateVersion(ndb::SQLiteDB* db, const TDBVersionType& version){ return true; }
		static TDBVersionType CheckVersion(ndb::SQLiteDB* db){ return DBVersionType(); };
		static bool SetVersion(ndb::SQLiteDB* db, TDBVersionType version){ return true; };
		static bool CreateVersionTable(ndb::SQLiteDB* db){ return false; }
		static TDBVersionType GetInvalidVersionValue(){ return DBVersionType(); }
		static TDBVersionType GetEmptyVersionValue(){ return DBVersionType(); }
	};
	//一个版本号类型是int的特化
	template<>
	class DB_EXPORT DBVersionOpration<int>
	{
	public:
		enum DBVersionCheckRet
		{
			DBVersionCheckRet_Invalid = ~((((unsigned int)~((unsigned int)0))) >> 1),//MININT32，无法读取到版本信息数据库损坏了
			DBVersionCheckRet_Empty,//没有版本信息
		};
	protected:
		typedef int DBVersionType;
	public:
		static bool HasVersionTable(ndb::SQLiteDB* db){
			ndb::SQLiteStatement stmt;
			db->Query(stmt, "SELECT * FROM version");
			auto result = stmt.NextRow();
			return (result == SQLITE_ROW || result == SQLITE_DONE);
		}
		static bool UpdateVersion(ndb::SQLiteDB* db, const DBVersionType& version){
			char query_sql[1024];
			sprintf_s(query_sql, "UPDATE OR ROLLBACK version SET version = %d", version);
			return db->Query(query_sql) == SQLITE_OK;
		};
		static DBVersionType CheckVersion(ndb::SQLiteDB* db){
			DBVersionType ret = DBVersionCheckRet_Invalid;
			ndb::SQLiteStatement stmt;
			db->Query(stmt, "SELECT * FROM version");
			auto result = stmt.NextRow();
			if (result == SQLITE_ROW)//读取版本信息
				ret = stmt.GetIntField(0);
			else if (result == SQLITE_DONE)//全新的库，要写入版本信息
				ret = DBVersionCheckRet_Empty;
			else//数据文件可能已经损坏了
				ret = DBVersionCheckRet_Invalid;
			return ret;
		};
		static bool SetVersion(ndb::SQLiteDB* db, DBVersionType version){
			char sql[1024];
			sprintf_s(sql, "INSERT INTO version(version)VALUES(%d);", version);
			return db->Query(sql) == SQLITE_OK;
		};
		static bool CreateVersionTable(ndb::SQLiteDB* db){
			return db->Query("CREATE TABLE IF NOT EXISTS version(version INTEGER)") == SQLITE_OK;
		}
		static DBVersionType GetInvalidVersionValue(){
			return DBVersionCheckRet_Invalid;
		}
		static DBVersionType GetEmptyVersionValue(){
			return DBVersionCheckRet_Empty;
		}
	};
	template<typename TDBVersionType>
	class DB_EXPORT DBPretreatment
	{
		class DB_EXPORT IOSFileSystem
		{
		public:
			virtual bool FilePathIsExist(const std::string& file_path, bool is_directory) = 0;
			virtual bool DeleteFile(const std::string& file_path) = 0;
			virtual bool CopyFile(const std::string &from_path, const std::string &to_path, bool fail_if_exists = false) = 0;
			virtual bool CreateDir(const std::string& dir_path) = 0;
			virtual bool GetDirFromPath(const std::string& file_path, std::string& dir) = 0;//dir 含未尾的"/" or "\\"
			virtual bool GetFileNameFromPath(const std::string& file_path, std::string& name, std::string& ext) = 0;
			virtual bool MoveFile(const std::string &from_path, const std::string &to_path) = 0;
			virtual void ClearTLSLastError() = 0;
			virtual unsigned int GetTLSLastError() = 0;
			virtual void LockDBFile() = 0;
			virtual void UnLockDBFile() = 0;
		};
		class DefaultDBRestore
		{
		public:
			DefaultDBRestore() : file_system_(nullptr)
			{
			}
			virtual ~DefaultDBRestore(){}
		public:
			void SetRestoreInfo(IOSFileSystem* file_system, const std::string& db_path, const std::string& back_db_dir)
			{
				file_system_ = file_system;
				db_path_ = db_path;
				file_system_->GetDirFromPath(db_path, db_dir_);
				file_system_->GetFileNameFromPath(db_path_, db_file_name_, db_file_ext_);
				back_db_file_name_ = db_file_name_;
				back_db_file_name_.append(".bk");
				back_db_file_ext_ = db_file_ext_;
				if (back_db_dir.empty())
				{
					back_db_dir_ = db_dir_;
					back_db_dir_.append("db_file.back/");
					back_db_path_.append(back_db_dir_).append(back_db_file_name_);
				}
				else
				{
					back_db_dir_ = back_db_dir;
					back_db_path_ = back_db_dir_ + back_db_file_name_;
				}
			}
			bool DoBackup()
			{
				if (file_system_ == nullptr)
					return false;
				file_system_->ClearTLSLastError();
				if (file_system_->FilePathIsExist(db_path_, false))
				{
					if (!file_system_->FilePathIsExist(back_db_dir_, true) && !file_system_->CreateDir(back_db_dir_) && !file_system_->FilePathIsExist(back_db_dir_, true))
						return false;
					auto back_db_file_bk = back_db_path_ + ("_bk");
					bool has_back_file = file_system_->FilePathIsExist(back_db_path_, false);
					if (has_back_file)
						file_system_->MoveFile(back_db_path_, back_db_file_bk);		
					auto backup_ret = false;
					{
						file_system_->LockDBFile();
						backup_ret = file_system_->CopyFile(db_path_, back_db_path_);
						if (backup_ret)
						{
							if (has_back_file)
								file_system_->DeleteFile(back_db_file_bk);
						}
						else
						{
							if (file_system_->FilePathIsExist(back_db_file_bk, false))
							{
								file_system_->DeleteFile(back_db_path_);
								file_system_->MoveFile(back_db_file_bk, back_db_path_);
							}
						}
						file_system_->UnLockDBFile();
					}					
					return backup_ret;
				}
				return false;
			}
			bool DoRestore()
			{
				if (file_system_ == nullptr)
					return false;
				file_system_->ClearTLSLastError();
				if (file_system_->FilePathIsExist(back_db_path_, false))
				{
					if (!file_system_->FilePathIsExist(db_dir_, true) && !file_system_->CreateDir(db_dir_))
						return false;
					auto db_file_bk = db_path_ + ("_bk");		
					auto copy_ret = false;
					{
						file_system_->LockDBFile();						
						bool has_db_file = file_system_->FilePathIsExist(db_path_, false);
						if (has_db_file)
							file_system_->MoveFile(db_path_, db_file_bk);
						copy_ret = file_system_->CopyFile(back_db_path_, db_path_);
						if (copy_ret)
						{
							if (has_db_file)
								file_system_->DeleteFile(db_file_bk);
						}
						else
						{
							if (file_system_->FilePathIsExist(db_file_bk, false))
							{
								file_system_->DeleteFile(db_path_);
								file_system_->MoveFile(db_file_bk, db_path_);
							}
						}
						file_system_->UnLockDBFile();
					}
					
					return copy_ret;
				}
				return false;
			}
			void Clear()
			{
				file_system_ = nullptr;
				db_path_ =
				db_dir_ =
				db_file_name_ =
				db_file_ext_ =
				back_db_path_ =
				back_db_dir_ =
				back_db_file_name_ =
				back_db_file_ext_ = "";
			}
		private:
			IOSFileSystem* file_system_;
			std::string db_path_;
			std::string db_dir_;
			std::string db_file_name_;
			std::string db_file_ext_;
			std::string back_db_path_;
			std::string back_db_dir_;
			std::string back_db_file_name_;
			std::string back_db_file_ext_;
		};
	public:
		typedef TDBVersionType DBVersionType;
		typedef DBVersionOpration<typename DBVersionType> VersionOpration;
		typedef std::function<bool(typename DBVersionType)> DBUpdateFunc;
		typedef std::pair<typename DBVersionType, typename DBUpdateFunc> DBUpdateFunItem;
		typedef std::map<typename DBUpdateFunItem::first_type, typename DBUpdateFunItem::second_type> DBUpdateFuncList;
		typedef IOSFileSystem FileSystem;
		class PretreatmentConfig
		{
		public:
			PretreatmentConfig() :
				db_path_(""), back_db_dir_(""),
				enable_def_restore_(false), enable_backup_(true), enable_restore_(false),
				newest_version_(1), base_version_(1), db_version_now_(1)
			{
			}
		public:
			void Clear()
			{
				db_path_ = "";
				back_db_dir_ = "";
				enable_def_restore_ = false;
				enable_backup_ = true;
				enable_restore_ = false;
				newest_version_ = base_version_ = db_version_now_ = 1;
			}
		public:
			std::string db_path_;//数据文件路径
			std::string back_db_dir_;//备份目录
			bool enable_def_restore_;//是否使用缺省的恢复处理
			bool enable_backup_;//是否开启备份功能
			bool enable_restore_;//是否开启恢复功能
			DBVersionType newest_version_;	//数据库最新版本
			DBVersionType base_version_;//数据库最初版本
			DBVersionType db_version_now_;//数据库当前版本
		};
	public:
		DBPretreatment() :
			file_system_(nullptr)
		{
		}
		virtual ~DBPretreatment()
		{
			CloseDB();
		}
	public:
		virtual void Init(const PretreatmentConfig& config)
		{
			config_ = config;
			if (!config_.enable_backup_)
			{
				config_.enable_def_restore_ = false;
				config_.enable_restore_ = false;
			}
			if (!config_.enable_restore_)
			{
				config_.enable_def_restore_ = false;
			}
		}
		virtual bool CloseDB()
		{
			db_.Close();
			Clear();
			return true;
		}
		DBVersionType GetNewestDBVersion() const { return config_.newest_version_; } 
		DBVersionType GetDBVersion() const { return config_.db_version_now_; }
		DBVersionType GetDBBaseVersion() const { return config_.base_version_; }	
	protected:
		virtual bool OnDBFileBroken(){ return false; };		
		virtual bool OnDoOtheUpdate(){ return true; }
		virtual void OnOpenDB(bool new_db){};
		virtual void OnNoVersionToBaseVersion(){};		
		virtual void OnClear(){};		
	protected:
		bool DoPretreatment(FileSystem* file_system, const std::string& db_password, std::list<std::string> createdb_sqls, const DBUpdateFuncList& updatefun_list = DBUpdateFuncList())
		{
			file_system_ = file_system;
			createdb_sqls_.assign(createdb_sqls.begin(), createdb_sqls.end());
			updatefunctions_ = updatefun_list;
			db_restore_.SetRestoreInfo(file_system_, config_.db_path_, config_.back_db_dir_);
			bool new_dbfile = true;
			auto OpenDBTask = [&]()->bool{				
				//打开数据文件失败，并且处理且解决了文件损坏会再次打开数据文件
				if (!OpenDB(config_.db_path_, db_password, new_dbfile) && CatchDBFileBroken() && !OpenDB(config_.db_path_, db_password, new_dbfile))
					return false;
				if (new_dbfile)//新创建的数据文件
				{
					int db_result = SQLITE_OK;
					std::for_each(createdb_sqls_.begin(), createdb_sqls_.end(), [&](const std::string& sql){
						db_result &= db_.Query(sql.c_str());
					});
					CreateVersionTable();
					SetVersion(config_.newest_version_);
				}
				else
				{
					if (!HasVersionTable())
					{
						NoVersionToBaseVersion();
						CreateVersionTable();
						SetVersion(config_.base_version_);
					}
					int db_result = SQLITE_OK;
					std::for_each(createdb_sqls_.begin(), createdb_sqls_.end(), [&](const std::string& sql){
						db_result &= db_.Query(sql.c_str());
					});
				}
				return true;
			};
			if (!OpenDBTask())//打开数据文件失败并且没有进行修复，或者修复已后依然无法打开数据文件
				return false;
			config_.db_version_now_ = VersionOpration::CheckVersion(&db_);//如果OpenDBTask执行成功db_version_now_应该是相应的版本号
			if (config_.db_version_now_ == VersionOpration::GetInvalidVersionValue())
			{
				if (!(CatchDBFileBroken() && OpenDBTask()))
				{
					return false;
				}
				else
				{
					config_.db_version_now_ = VersionOpration::CheckVersion(&db_);
					if (config_.db_version_now_ == VersionOpration::GetInvalidVersionValue())
						return false;
				}
			}
			auto update_ret = UpdateDataBase() && (config_.newest_version_ == config_.db_version_now_);
			if (update_ret)
			{
				if (config_.enable_backup_)
					db_restore_.DoBackup();
				OnOpenDB(new_dbfile);
			}
			return update_ret;
		}	
		bool CheckFieldExist(const std::string& table, const std::string& field) const
		{
			ndb::SQLiteStatement stmt;
			char query_sql[1024];
			sprintf_s(query_sql, "PRAGMA table_info(%s)", table.c_str());
			auto ret = db_.Query(stmt, query_sql);			
			auto result = stmt.NextRow();
			while (result == SQLITE_ROW)
			{
				if (field.compare(stmt.GetTextField(1)) == 0)
					return true;
				result = stmt.NextRow();
			}
			return false;
		}
		bool AddFieldToTable(const std::string& table, const std::string& field,const std::string& field_type) const
		{
			if (!CheckFieldExist(table, field))
			{
				char sql[1024] = {0};
				sprintf_s(sql, "ALTER TABLE %s ADD COLUMN %s %s", table.c_str(), field.c_str(), field_type.c_str());
				return db_.Query(sql) == SQLITE_OK;
			}
			return true;
		}
	private:
		bool OpenDB(const std::string& db_path, const std::string& db_password,bool& new_dbfile)
		{
			bool ret = false;
			int open_db_flg = ndb::SQLiteDB::modeReadWrite | ndb::SQLiteDB::modeCreate | ndb::SQLiteDB::modeSerialized;
			new_dbfile = !file_system_->FilePathIsExist(config_.db_path_, false);
			ret = db_.Open(db_path.c_str(), db_password, open_db_flg) && db_.IsValid();
			if (!ret)
				db_.Close();
			return ret;
		}
		virtual void Clear()
		{			
			updatefunctions_.clear();
			file_system_ = nullptr;
			createdb_sqls_.clear();
			config_.Clear();
			db_restore_.Clear();
			OnClear();
		};
		bool CatchDBFileBroken()
		{
			db_.Close();
			if (config_.enable_restore_)
			{
				if (config_.enable_def_restore_ && db_restore_.DoRestore())
					return true;
			}
			return OnDBFileBroken();
		}

		/****************升级相关接口********************/
		bool UpdateDataBase()
		{
			//增量升级DB。例如1->2,2->3, 3->4
			bool ret = true;
			std::for_each(std::find_if(updatefunctions_.begin(), updatefunctions_.end(), [&](const DBUpdateFunItem& item){return item.first > config_.db_version_now_ ; }),
									 std::find_if(updatefunctions_.begin(), updatefunctions_.end(), [&](const DBUpdateFunItem& item){return item.first > config_.newest_version_; }),
				[&](const DBUpdateFunItem& item)->bool{
				if (!ret) return false;
				if (item.second(item.first))
					UpdateVersion(item.first);
				else
					ret = false;
				return ret;
			});
			//全量升级DB。例如1->4, 2->4, 3->4
			return DoOtherUpdate();
		}
		
	private:
		void NoVersionToBaseVersion()
		{
			OnNoVersionToBaseVersion();
		};
		bool DoOtherUpdate()
		{
			return OnDoOtheUpdate();
		}
		//DBVersionOpration
		bool HasVersionTable()
		{
			return VersionOpration::HasVersionTable(&db_);
		}
		bool UpdateVersion(DBVersionType version)
		{
			auto ret = VersionOpration::UpdateVersion(&db_, version);
			if (ret)
				config_.db_version_now_ = version;
			return ret;
		}
		DBVersionType CheckVersion()
		{
			return VersionOpration::CheckVersion(&db_);
		}
		bool SetVersion(DBVersionType version)
		{
			auto ret = VersionOpration::SetVersion(&db_, version);
			if (ret)
				config_.db_version_now_ = version;
			return ret;			
		}
		bool CreateVersionTable()
		{
			return VersionOpration::CreateVersionTable(&db_);
		}

	protected:
		ndb::SQLiteDB db_;		
	private:
		PretreatmentConfig config_;
		typename FileSystem* file_system_;
		DefaultDBRestore db_restore_;
		std::list<std::string> createdb_sqls_;//只包含建表SQL
		typename DBUpdateFuncList updatefunctions_;
	};
}
#endif//DB_DB_PRETREATMENT_H_