// SQLite Database

#include "db/db_sqlite3.h"
#include <assert.h>

static const char kNULL[] = "\0\0\0";

namespace ndb
{
	
//////////////////////////////////////////////////////////////////////////////
// SQLiteDB
SQLiteDB::SQLiteDB()
{
	sqlite3_ = NULL;
}
	
SQLiteDB::SQLiteDB(SQLiteDB& src)
{
	sqlite3_     = src.sqlite3_;
	src.sqlite3_ = NULL;
}
	
SQLiteDB::~SQLiteDB()
{
	Interrupt();
	Close();
}
	
SQLiteDB& SQLiteDB::operator=(SQLiteDB& src)
{
	Interrupt();
	Close();
		
	sqlite3_     = src.sqlite3_;
	src.sqlite3_ = NULL;
	return *this;
}
	
bool SQLiteDB::Open(int flags)
{
    return Open(":memory:", "", flags);
}

bool SQLiteDB::Open(const char* filename, 
	const std::string &key, 
	int flags/* = modeReadWrite|modeCreate|modeSerialized*/)
{
	Interrupt();
	Close();
		
	if (filename == NULL)
		return false;
		
	int r = sqlite3_open_v2(filename, &sqlite3_, flags, NULL);
	if (r != SQLITE_OK)
	{
		Close();
	}
	else
	{
		if (key.length()) 
		{
			if( SQLITE_OK != sqlite3_key(sqlite3_,key.c_str(),key.length()))
				r = sqlite3_rekey(sqlite3_,key.c_str(),key.length());
		}
	}
		
		
	return r == SQLITE_OK;
}

bool SQLiteDB::Close()
{
	if (sqlite3_ == NULL)
		return true;
		
	/*
		* If sqlite3_close() is called on a database connection that still has
		* outstanding prepared statements or BLOB handles, then it returns SQLITE_BUSY
		*/
	if (SQLITE_OK != sqlite3_close(sqlite3_))
		return false;
		
	sqlite3_ = NULL;
	return true;
}
	
bool SQLiteDB::Compact()
{
	if (SQLITE_OK != Query("VACUUM"))
		return false;
		
	return true;
}
	
bool SQLiteDB::IsValid() const
{
	return sqlite3_ != NULL;
}
	
int SQLiteDB::GetLastErrorCode() const
{
	if (sqlite3_ == NULL)
		return SQLITE_OK;
	return sqlite3_errcode(sqlite3_);
}
	
int SQLiteDB::GetLastExtendedErrorCode() const
{
	if (sqlite3_ == NULL)
		return SQLITE_OK;
	return sqlite3_extended_errcode(sqlite3_);
}
	
const char* SQLiteDB::GetLastErrorMessage() const
{
	assert(sqlite3_);
	if (sqlite3_ == NULL)
		return kNULL;
	return sqlite3_errmsg(sqlite3_);
}
	
int SQLiteDB::GetChanges() const
{
	assert(sqlite3_);
	if (sqlite3_ == NULL)
		return -1;
	return sqlite3_changes(sqlite3_);
}
	
int SQLiteDB::GetTotalChanges() const
{
	assert(sqlite3_);
	if (sqlite3_ == NULL)
		return -1;
	return sqlite3_total_changes(sqlite3_);
}

bool SQLiteDB::DoesTableExist(const char* table_name) const
{
	return DoesTableOrIndexExist(table_name, "table");
}

bool SQLiteDB::DoesIndexExist(const char* index_name) const
{
	return DoesTableOrIndexExist(index_name, "index");
}

bool SQLiteDB::DoesTableOrIndexExist(
	const char* name, const char* type) const
{
	SQLiteStatement stat;
	Query(stat, "SELECT name FROM sqlite_master WHERE type=? AND name=?");

	if (!stat.IsValid())
		return false;

	// Bind is 1 based
	stat.BindText(1, type);
	stat.BindText(2, name);

	return stat.NextRow() == SQLITE_ROW;
}

bool SQLiteDB::DoesColumnExist(
	const char* table_name, const char* column_name) const
{
	std::string sql("PRAGMA TABLE_INFO(");
	sql.append(table_name);
	sql.append(")");

	SQLiteStatement stat;
	Query(stat, sql.c_str(), (int)sql.size());
	if (!stat.IsValid())
		return false;
	while (stat.NextRow() == SQLITE_ROW) {
		if (!strcmp(stat.GetTextField(1), column_name))
			return true;
	}
	return false;
}
	
sqlite3_int64 SQLiteDB::GetLastInsertRowId() const
{
	assert(sqlite3_);
	if (sqlite3_ == NULL)
		return -1;
	return sqlite3_last_insert_rowid(sqlite3_);
}
	
int SQLiteDB::Query(SQLiteStatement& statement, const char* sql_text, int length) const
{
	statement.Finalize();
		
	if (!sqlite3_ || !sql_text)
		return SQLITE_MISUSE;
		
	/*
		* SQLite documents say:
		* If the caller knows that the supplied string is nul-terminated,
		* then there is a small performance advantage to be gained by passing an nByte parameter
		* that is equal to the number of bytes in the input string including the nul-terminator bytes.
		*/
	if (length < 0)
		length = (int)strlen(sql_text) + 1;
		
	return sqlite3_prepare_v2(sqlite3_, sql_text, length, &statement.stmt_, NULL);
}
	
int SQLiteDB::Query(const char* sql_text, SQLiteQueryDelegate delegate, void* param) const
{
	if (!sqlite3_ || !sql_text)
		return SQLITE_MISUSE;
	return sqlite3_exec(sqlite3_, sql_text, delegate, param, NULL);
}
	
int SQLiteDB::Query(const char* sql_text, SQLiteResultTable& table) const
{
	if (!sqlite3_ || !sql_text)
		return SQLITE_MISUSE;
	// 先清空结果表
	table.Free();
	int r = sqlite3_get_table(sqlite3_, sql_text,
								&table.result_column_, &table.row_, &table.col_, NULL);
	if (r == SQLITE_OK)
		table.result_ = table.result_column_ + table.col_;
	return r;
}
	
void SQLiteDB::Interrupt()
{
	if (!sqlite3_)
		return;
	sqlite3_interrupt(sqlite3_);
}
	
int SQLiteDB::SetBusyTimeout(int ms)
{
	if (!sqlite3_)
		return SQLITE_MISUSE;
	return sqlite3_busy_timeout(sqlite3_, ms);
}
	
int SQLiteDB::GetVersion()
{
	return sqlite3_libversion_number();
}
	
	
//////////////////////////////////////////////////////////////////////////////
// SQLiteResultTable
SQLiteResultTable::SQLiteResultTable()
{
	Reset();
}

SQLiteResultTable::SQLiteResultTable(SQLiteResultTable& src)
{
	row_              = src.row_;
	col_              = src.col_;
	result_                 = src.result_;
	result_column_    = src.result_column_;
		
	src.Reset();
}

SQLiteResultTable::~SQLiteResultTable()
{
	Free();
}

SQLiteResultTable& SQLiteResultTable::operator=(SQLiteResultTable& src)
{
	row_              = src.row_;
	col_              = src.col_;
	result_                 = src.result_;
	result_column_    = src.result_column_;
		
	src.Reset();
		
	return *this;
}

void SQLiteResultTable::Free()
{
	if (result_column_ != NULL)
	{
		sqlite3_free_table(result_column_);
		Reset();
	}
}

void SQLiteResultTable::Reset()
{
	row_			= 0;
	col_			= 0;
	result_			= NULL;
	result_column_	= NULL;
}
	
bool SQLiteResultTable::IsEmpty() const
{
	return result_column_ == 0;
}

int SQLiteResultTable::GetRowCount() const
{
	return row_;
}

int SQLiteResultTable::GetColumnCount() const
{
	return col_;
}
	
const char* SQLiteResultTable::GetValue(int row, int col) const
{
	if (!result_ || row >= row_ ||
		col >= col_ || row < 0 || col < 0)
	{
		return 0;
	}
		
	return result_[row * col_ + col];
}

const char** const SQLiteResultTable::operator[](int row) const
{
	// 为了效率，这里不检查越界，就像STL那样
	return (const char**)result_ + row * col_;
}
	
const char* SQLiteResultTable::GetColumnName(int col) const
{
	if (!result_column_ || col < 0 || col >= col_)
		return 0;
		
	return result_column_[col];
}
	
	
//////////////////////////////////////////////////////////////////////////////
// SQLiteStatement
SQLiteStatement::SQLiteStatement()
{
	stmt_ = NULL;
	eof_  = true;
}
	
SQLiteStatement::SQLiteStatement(SQLiteStatement& src)
{
	stmt_     = src.stmt_;
	eof_      = src.eof_;
	src.stmt_ = NULL;
	src.eof_  = true;
}
	
SQLiteStatement::~SQLiteStatement()
{
	Finalize();
}
	
SQLiteStatement& SQLiteStatement::operator=(SQLiteStatement& src)
{
	Finalize();
		
	stmt_     = src.stmt_;
	eof_      = src.eof_;
	src.stmt_ = NULL;
	src.eof_  = true;
		
	return *this;
}
	
int SQLiteStatement::Finalize()
{
	int r = SQLITE_OK;
	if (stmt_ != NULL)
	{
		r = sqlite3_finalize(stmt_);
		if (r == SQLITE_OK)
		{
			stmt_ = NULL;
			eof_  = true;
		}
	}
	return r;
}
	
int SQLiteStatement::Rewind()
{
	int r = SQLITE_MISUSE;
	if (stmt_ != NULL)
	{
		r = sqlite3_reset(stmt_);
		if (SQLITE_OK == r)
		{
			eof_ = false;
			return SQLITE_OK;
		}
	}
	return r;
}
	
int SQLiteStatement::NextRow()
{
	if (stmt_ == NULL)
		return SQLITE_MISUSE;
		
	int r = sqlite3_step(stmt_);
		
	if (r == SQLITE_DONE)
		eof_ = true;
		
	return r;
}
	
bool SQLiteStatement::IsEof()
{
	return eof_;
}
	
bool SQLiteStatement::IsNullField(int col)
{
	return GetFieldValue(col) != NULL;
}
	
const void* SQLiteStatement::GetBlobField(int col)
{
	if (stmt_ == NULL)
		return NULL;
	return sqlite3_column_blob(stmt_, col);
}
	
int   SQLiteStatement::GetBytesField(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_bytes(stmt_, col);
}
	
int   SQLiteStatement::GetBytes16Field(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_bytes16(stmt_, col);
}
	
double SQLiteStatement::GetDoubleField(int col)
{
	if (stmt_ == NULL)
		return 0.0;
	return sqlite3_column_double(stmt_, col);
}
	
int SQLiteStatement::GetIntField(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_int(stmt_, col);
}
	
sqlite3_int64 SQLiteStatement::GetInt64Field(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_int64(stmt_, col);
}
	
const char* SQLiteStatement::GetTextField(int col)
{
	if (stmt_ == NULL)
		return NULL;
	return (const char*)sqlite3_column_text(stmt_, col);
}
	
const void* SQLiteStatement::GetText16Field(int col)
{
	if (stmt_ == NULL)
		return NULL;
	return sqlite3_column_text16(stmt_, col);
}
	
int SQLiteStatement::GetTypeField(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_type(stmt_, col);
}
	
sqlite3_value* SQLiteStatement::GetFieldValue(int col)
{
	if (stmt_ == NULL)
		return NULL;
	return sqlite3_column_value(stmt_, col);
}
	
int	SQLiteStatement::GetFieldBytes(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_bytes(stmt_, col);
}
	
int	SQLiteStatement::GetFieldBytes16(int col)
{
	if (stmt_ == NULL)
		return 0;
	return sqlite3_column_bytes16(stmt_, col);
}
	
int SQLiteStatement::BindBlob(int index, const void* data, int size_in_bytes)
{
	if (stmt_ != NULL)
		return sqlite3_bind_blob(stmt_, index, data, size_in_bytes, SQLITE_STATIC);
	return SQLITE_MISUSE;
}
	
int SQLiteStatement::BindDouble(int index, double data)
{
	if (stmt_ != NULL)
		return sqlite3_bind_double(stmt_, index, data);
	return SQLITE_MISUSE;
}
	
int SQLiteStatement::BindInt(int index, int data)
{
	if (stmt_ != NULL)
		return sqlite3_bind_int(stmt_, index, data);
	return SQLITE_MISUSE;
}
	
int SQLiteStatement::BindInt64(int index, int64_t data)
{
	if (stmt_ != NULL)
		return sqlite3_bind_int64(stmt_, index, data);
	return SQLITE_MISUSE;
}
	
int SQLiteStatement::BindNull(int index)
{
	if (stmt_ != NULL)
		return sqlite3_bind_null(stmt_, index);
	return SQLITE_MISUSE;
}

int SQLiteStatement::BindText(int index, const char* data)
{
	return BindText(index, data, (int)strlen(data));
}
	
int SQLiteStatement::BindText(int index, const char* data, int size_in_bytes)
{
	if (stmt_ != NULL)
		return sqlite3_bind_text(stmt_, index, data, size_in_bytes, SQLITE_STATIC);
	return SQLITE_MISUSE;
}

int SQLiteStatement::BindText16(int index, const wchar_t* data)
{
	return BindText16(index, data, (int)wcslen(data) * sizeof(wchar_t));
}

int SQLiteStatement::BindText16(int index, const wchar_t* data, int size_in_bytes)
{
	if (stmt_ != NULL)
		return sqlite3_bind_text16(stmt_, index, data, size_in_bytes, SQLITE_STATIC);
	return SQLITE_MISUSE;
}

int SQLiteStatement::BindValue(int index, const sqlite3_value* data)
{
	if (stmt_ != NULL && data != NULL)
		return sqlite3_bind_value(stmt_, index, data);
	return SQLITE_MISUSE;
}

int SQLiteStatement::BindZeroBlob(int index, int size)
{
	if (stmt_ != NULL)
		return sqlite3_bind_zeroblob(stmt_, index, size);
	return SQLITE_MISUSE;
}   

//////////////////////////////////////////////////////////////////////////////
// SQLiteAutoTransaction

SQLiteAutoTransaction::SQLiteAutoTransaction(SQLiteDB* db)
: db_(db), need_commit_(false)
{
	Begin();
}
	
SQLiteAutoTransaction::~SQLiteAutoTransaction()
{
	if (need_commit_)
		Commit();
}
	
bool SQLiteAutoTransaction::Begin()
{
	bool result = false;
	if (db_ && !need_commit_)
	{
		if (db_->Query("BEGIN") == SQLITE_OK)
			result = true;
		need_commit_ = true;
	}
	return result;
}

bool SQLiteAutoTransaction::Commit()
{
	bool result = 0;
	if (db_ != NULL)
	{
		if (db_->Query("END") == SQLITE_OK)
			result = true;
		need_commit_ = false;
	}
	return result;
}

bool SQLiteAutoTransaction::Rollback()
{
	bool result = false;
	if (db_ != NULL)
	{
		if (db_->Query("ROLLBACK") == SQLITE_OK)
			result = true;
		need_commit_ = false;
	}
	return result;
}

}  // namespace ndb
