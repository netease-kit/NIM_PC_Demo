/** @file nim_msglog_helper.h
  * @brief msglog 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/22
  */

#ifndef _NIM_SDK_CPP_MSGLOG_HELPER_H_
#define _NIM_SDK_CPP_MSGLOG_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @brief 查询到消息历史结果 */
struct NIM_SDK_CPPWRAPPER_DLL_API QueryMsglogResult
{
	int count_;						/**< 消息历史数 */
	NIMMsglogQuerySource source_;	/**< 消息历史查询来源 */
	std::list<IMMessage> msglogs_;	/**< 消息历史 */

	/** 构造函数 */
	QueryMsglogResult() : count_(0), source_(kNIMMsglogQuerySourceLocal) {}
};

/** @brief 发送消息已读回执 */
struct NIM_SDK_CPPWRAPPER_DLL_API MessageStatusChanged
{
	NIMMsgLogStatus status_;	/**< 变更后的状态 */
	std::string		talk_id_;	/**< 会话ID */
	int64_t			msg_timetag_;	/**< 临界的消息的时间戳 */

	MessageStatusChanged() : msg_timetag_(0) {}
};

/** @brief 发送消息已读回执 */
struct NIM_SDK_CPPWRAPPER_DLL_API MessageStatusChangedResult
{
	NIMResCode rescode_;	/**< 错误码 */
	std::list<MessageStatusChanged> results_;	/**< 结果 */

	MessageStatusChangedResult(int rescode, std::string &result)
	{
		rescode_ = (NIMResCode)rescode;
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		if (reader.parse(result, values) && values.isArray())
		{
			int size = values.size();
			for (int i = 0; i < size; i++)
			{
				MessageStatusChanged changed;
				changed.status_ = (NIMMsgLogStatus)values[i][kNIMMsglogStatusChangedKeyStatus].asUInt();
				changed.talk_id_ = values[i][kNIMMsglogStatusChangedKeyTalkID].asString();
				changed.msg_timetag_ = values[i][kNIMMsglogStatusChangedKeyMsgTimetag].asInt64();
				results_.push_back(changed);
			}
		}
	}
};
/** @brief 导出消息记录到云端参数定义 */
class NIM_SDK_CPPWRAPPER_DLL_API LogsBackupExportInfo : protected NIMLogsBackupExportInfo
{
public:
	LogsBackupExportInfo() : cloned_(false)
	{
		ExportBackupToRemotePackageCallback = [](const std::string& file_path, void* user_data) {return file_path; };
		ExportBackupToRemoteEncryptCallback = [](const std::string& file_path, const std::string& encrypt_key, void* user_data) {return file_path; };
		ExportBackupToRemoteLogFiter = [](const IMMessage& msg, void* user_data) {return true; };
		LogsBackupProgressCallback = [](LogsBackupRemoteOperate, float, void* user_data) {};
		LogsBackupCompleteCallback = [](LogsBackupRemoteOperate, LogsBackupRemoteState state, void* user_data) {
			if (state >= LogsBackupRemoteState::LogsBackupRemoteState_FinalState_Begin)
				delete CastToCPP(user_data);
		};
	}
public:
	void SetEncryptKey(const std::string& encrypt_key)
	{
		encrypt_key_ = encrypt_key;
	}
	std::string GetEncryptKey() const
	{
		return encrypt_key_;
	}
	void SetUserData(void* user_data)
	{
		NIMLogsBackupExportInfo::user_data_ = user_data;
	}
	void* GetUserData() const
	{
		return NIMLogsBackupExportInfo::user_data_;
	}
	NIMLogsBackupExportInfo* ToCStruct() const
	{
		LogsBackupExportInfo* temp = new LogsBackupExportInfo(*this);
		temp->SetUserData(GetUserData());
		temp->cloned_ = true;
		return temp->ToCStruct_i();
	}
private:
	static inline LogsBackupExportInfo* CastToCPP(void* data)
	{
		return ((LogsBackupExportInfo*)(data));
	}
	NIMLogsBackupExportInfo* ToCStruct_i();
public:
	/** 自定义的打包方式回调，SDK生成原始数据文后会调用开发者自定义的打包（压缩）方法
* @param[in] file_path		原始数据文件路径
* @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return std::string 生成的打包（压缩）文件的路径
*/
	std::function<std::string(const std::string& file_path, void* user_data)> ExportBackupToRemotePackageCallback; 

	/** 自定义的加密方式回调，SDK生成原始数据经过打包（压缩）后会调用开发者自定义的加密方法
  * @param[in] file_path		打包（压缩）后的文件路径
  * @param[in] encrypt_key	加密秘钥 与 encrypt_key_为同一个值
  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return std::string 加密后的文件路径
  */
	std::function<std::string(const std::string& file_path, const std::string& encrypt_key, void* user_data)> ExportBackupToRemoteEncryptCallback; 
	
	/**开发者自定义的导出消息的过滤器
	* @param[in] msg		消息
	* @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	* @return bool true:导出这条消息;false:不导出这条消息
	*/
	std::function<bool(const IMMessage& msg, void* user_data)> ExportBackupToRemoteLogFiter;
	/**导出过程的进度回调
	  * @param[in] op		操作类型 导入/导出
	  * @param[in] progress	进度 0-1
	  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	  * @return void
	  */
	std::function<void(LogsBackupRemoteOperate op, float progress, void* user_data)> LogsBackupProgressCallback;
	/** 导出完成后的结果回调
  * @param[in] op		操作类型 导入/导出
  * @param[in] state	完成状态参考 LogsBackupRemoteState
  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void
  */
	std::function<void(LogsBackupRemoteOperate op, LogsBackupRemoteState state, void* user_data)>  LogsBackupCompleteCallback;
private:
	std::string encrypt_key_;
	void* src_user_data_;
	bool cloned_;
};

/** @brief 从云端导入消息记录参数定义 */
class NIM_SDK_CPPWRAPPER_DLL_API LogsBackupImportInfo : public NIMLogsBackupImportInfo
{
public:
	LogsBackupImportInfo()
	{
		ImportBackupFromRemoteUnPackageCallback = [](const std::string& file_path, void* user_data) {return file_path; };
		ImportBackupFromRemoteDecryptCallback = [](const std::string& file_path, const std::string& encrypt_key, void* user_data) {return file_path; };
		LogsBackupProgressCallback = [](LogsBackupRemoteOperate, float, void* user_data) {};
		LogsBackupCompleteCallback = [](LogsBackupRemoteOperate, LogsBackupRemoteState state, void* user_data) {
			if (state >= LogsBackupRemoteState::LogsBackupRemoteState_FinalState_Begin)
				delete CastToCPP(user_data);
		};
	}
public:
	void SetUserData(void* user_data)
	{
		NIMLogsBackupImportInfo::user_data_ = user_data;
	}
	void* GetUserData() const
	{
		return NIMLogsBackupImportInfo::user_data_;
	}
	NIMLogsBackupImportInfo* ToCStruct() const
	{
		LogsBackupImportInfo* temp = new LogsBackupImportInfo(*this);
		temp->SetUserData(GetUserData());
		temp->cloned_ = true;
		return temp->ToCStruct_i();
	}
private:
	NIMLogsBackupImportInfo* ToCStruct_i();
	static inline LogsBackupImportInfo* CastToCPP(void* data)
	{
		return ((LogsBackupImportInfo*)(data));
	}
public:
	/**自定义的解包方式回调，从云端步的备份文件经解密后会回调开发者自定义的解包（解压）方法
* @param[in] file_path		经解密后文件路径
* @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return std::string 解包后的文件路径
*/
	std::function<std::string(const std::string& file_path, void* user_data)> ImportBackupFromRemoteUnPackageCallback;

	/**自定义的解密方式回调，SDK从云端同步完备份文件后会调用开发者自定义的解密方法
  * @param[in] file_path		从云端同步到的文件路径
  * @param[in] encrypt_key	解密秘钥 与导出时加密密钥相同
  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return std::string 解密后的文件路径
  */
	std::function<std::string(const std::string& file_path, const std::string& encrypt_key, void* user_data)> ImportBackupFromRemoteDecryptCallback; 

	/** 导入过程的进度回调
	  * @param[in] op		操作类型 导入/导出
	  * @param[in] progress	进度 0-1
	  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
	  * @return void
	  */
	std::function<void(LogsBackupRemoteOperate, float, void* user_data)> LogsBackupProgressCallback; 

	/**导入完成后的结果回调
* @param[in] op		操作类型 导入/导出
* @param[in] state	 完成状态参考 LogsBackupRemoteState
* @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
	std::function<void(LogsBackupRemoteOperate, LogsBackupRemoteState, void* user_data)>  LogsBackupCompleteCallback;
private:
	bool cloned_;
	void* src_user_data_;
};
/** @fn bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res)
  * @brief 解析查询到的消息历史结果
  * @param[in] msgs_json 查询到的消息历史(Json Value数据字符串)
  * @param[out] res 消息历史结果
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseMsglogs(const std::string& msgs_json, QueryMsglogResult& res);

} //namespace nim

#endif //_NIM_SDK_CPP_MSGLOG_HELPER_H_