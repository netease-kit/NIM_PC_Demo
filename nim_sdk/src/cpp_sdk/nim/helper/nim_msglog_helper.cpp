/** @file nim_msglog_helper.cpp
  * @brief msglog 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/22
  */

#include "src/cpp_sdk/nim/helper/nim_msglog_helper.h"
#include "include/depend_lib/include/nim_json_util.h"
namespace nim
{

bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msgs_json, values) && values.isObject())
	{
		res.count_ = values[kNIMMsglogQueryKeyCount].asUInt();
		res.source_ = (NIMMsglogQuerySource)values[kNIMMsglogQueryKeySource].asUInt();
		nim_cpp_wrapper_util::Json::Value msgs = values[kNIMMsglogQueryKeyContent];
		int len = msgs.size();
		for (int i = 0; i < len; i++)
		{
			IMMessage msg;
			ParseMessage(msgs[i], msg);
			res.msglogs_.push_back(msg);
		}
		return true;
	}
	return false;
}
NIMLogsBackupExportInfo* LogsBackupExportInfo::ToCStruct_i()
{
	if (!cloned_)
		return nullptr;
	src_user_data_ = NIMLogsBackupExportInfo::user_data_;
	NIMLogsBackupExportInfo::ExportBackupToRemoteLogFiter = [](const char* const msg, void* user_data) {
		IMMessage msg_out;
		ParseMessage(PCharToString(msg), msg_out);
		return (CastToCPP(user_data))->ExportBackupToRemoteLogFiter(msg_out, (CastToCPP(user_data))->src_user_data_);
	};
	NIMLogsBackupExportInfo::ExportBackupToRemotePackageCallback = [](const char* const file_path, void* user_data) {
		auto ret = (CastToCPP(user_data))->ExportBackupToRemotePackageCallback(PCharToString(file_path), (CastToCPP(user_data))->src_user_data_);
		char* pret = new char[ret.length() + 1];
		memset(pret, 0, ret.length() + 1);
		memcpy(pret, ret.data(), ret.length());
		return pret;
	};
	NIMLogsBackupExportInfo::ExportBackupToRemoteEncryptCallback = [](const char* const  file_path, const char* const  encrypt_key, void* user_data) {
		auto ret = (CastToCPP(user_data))->ExportBackupToRemoteEncryptCallback(PCharToString(file_path), PCharToString(encrypt_key), (CastToCPP(user_data))->src_user_data_);
		char* pret = new char[ret.length() + 1];
		memset(pret, 0, ret.length() + 1);
		memcpy(pret, ret.data(), ret.length());
		return pret;
	};
	NIMLogsBackupExportInfo::LogsBackupProgressCallback = [](enum LogsBackupRemoteOperate op, float progress, void* user_data) {
		(CastToCPP(user_data))->LogsBackupProgressCallback(op, progress, (CastToCPP(user_data))->src_user_data_);
	};
	NIMLogsBackupExportInfo::LogsBackupCompleteCallback = [](enum LogsBackupRemoteOperate op, enum LogsBackupRemoteState state, void* user_data) {
		(CastToCPP(user_data))->LogsBackupCompleteCallback(op, state, (CastToCPP(user_data))->src_user_data_);
		if (state >= LogsBackupRemoteState::LogsBackupRemoteState_FinalState_Begin)
			delete CastToCPP(user_data);
	};
	NIMLogsBackupExportInfo::NewPathRelease = [](char** pathptr) {
		if (*pathptr != nullptr)
		{
			delete[] * pathptr;
			*pathptr = nullptr;
		}
	};
	NIMLogsBackupExportInfo::encrypt_key_ = const_cast<char*>(encrypt_key_.c_str());
	NIMLogsBackupExportInfo::user_data_ = this;
	return this;
}
NIMLogsBackupImportInfo* LogsBackupImportInfo::ToCStruct_i()
{
	if (!cloned_)
		return false;
	src_user_data_ = NIMLogsBackupImportInfo::user_data_;
	NIMLogsBackupImportInfo::ImportBackupFromRemoteDecryptCallback = [](const char* const file_path, const char* const encrypt_key, void* user_data) {
		auto ret = (CastToCPP(user_data))->ImportBackupFromRemoteDecryptCallback(PCharToString(file_path), PCharToString(encrypt_key), (CastToCPP(user_data))->src_user_data_);
		char* pret = new char[ret.length() + 1];
		memset(pret, 0, ret.length() + 1);
		memcpy(pret, ret.data(), ret.length());
		return pret;
	};
	NIMLogsBackupImportInfo::ImportBackupFromRemoteUnPackageCallback = [](const char* const file_path, void* user_data) {
		auto ret = (CastToCPP(user_data))->ImportBackupFromRemoteUnPackageCallback(PCharToString(file_path), (CastToCPP(user_data))->src_user_data_);
		char* pret = new char[ret.length() + 1];
		memset(pret, 0, ret.length() + 1);
		memcpy(pret, ret.data(), ret.length());
		return pret;
	};
	NIMLogsBackupImportInfo::LogsBackupProgressCallback = [](enum LogsBackupRemoteOperate op, float progress, void* user_data) {
		(CastToCPP(user_data))->LogsBackupProgressCallback(op, progress, (CastToCPP(user_data))->src_user_data_);
	};
	NIMLogsBackupImportInfo::LogsBackupCompleteCallback = [](enum LogsBackupRemoteOperate op, enum LogsBackupRemoteState state, void* user_data) {
		(CastToCPP(user_data))->LogsBackupCompleteCallback(op, state, (CastToCPP(user_data))->src_user_data_);
		if (state >= LogsBackupRemoteState::LogsBackupRemoteState_FinalState_Begin)
			delete CastToCPP(user_data);
	};
	NIMLogsBackupImportInfo::NewPathRelease = [](char** pathptr) {
		if (*pathptr != nullptr)
		{
			delete[] * pathptr;
			*pathptr = nullptr;
		}
	};
	NIMLogsBackupImportInfo::user_data_ = this;
	return this;
}
}
