#pragma once

#include "base/synchronization/lock.h"
#include "base/memory/singleton.h"

class MsglogCallback
{
public:
	/**
	* 数据迁移导出数据前对要导出的数据进行过滤
	* @param[in] msg 消息的完整信息
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return bool 返回 true 导出该消息，返回 false 则排除该消息
	*/
	static bool OnMigrateLogFilter(const nim::IMMessage& msg, void* user_data);

	/**
	* 数据迁移导出数据前对数据加密的回调函数
	* @param[in] local_packaged_file 打包后的文件完整路径
	* @param[in] encrypt_key 加密秘钥
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return std::string 返回加密后的文件路径
	*/
	static std::string OnMigrateMsglogEncryptCallback(const std::string& local_packaged_file, const std::string& encrypt_key, void* user_data);

	/**
	* 数据迁移导出数据前对数据打包的回调函数
	* @param[in] local_source_file 导出数据原始数据的文件路径，用于对文件进行打包产生新的文件返回给 SDK 处理
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return std::string 返回打包后的文件完整路径
	*/
	static std::string OnMigrateMsglogPackageCallback(const std::string& local_source_file, void* user_data);

	/**
	* 数据迁移导入数据时对数据解密的回调函数
	* @param[in] remote_encrypted_file 云端下载后得到的文件路径
	* @param[in] decrypt_key 从服务器得到的解密文档所需的密钥
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return std::string 返回解密后的文件完整路径（压缩包的完整路径，后经过解包处理才能得到原始数据）
	*/
	static std::string OnMigrateMsglogDecryptCallback(const std::string& remote_encrypted_file, const std::string& decrypt_key, void* user_data);

	/**
	* 数据迁移导入数据时对数据解包的回调函数
	* @param[in] remote_decrypted_file 在调用解密回调后返回的文件完整路径
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return std::string 返回解包后的文件完整路径
	*/
	static std::string OnMigrateMsglogUnPackageCallback(const std::string& remote_decrypted_file, void* user_data);

	/**
	* 数据迁移（导入、导出）过程中进度的回调函数。
	* @param[in] operate 迁移操作的类型，导入或导出
	* @param[in] progress 导入或导出的进度
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return void	无返回值
	*/
	static void OnMigrateMsglogProgressCallback(nim::LogsBackupRemoteOperate operate, float progress, void* user_data);

	/**
	* 数据迁移（导入、导出）完成的回调函数。
	* @param[in] operate 迁移操作的类型，导入或导出
	* @param[in] state 最终的状态
	* @param[in] user_data 用户自定义传入的数据，SDK 只负责透传
	* @return void	无返回值
	*/
	static void OnMigrateMsglogCompletedCallback(nim::LogsBackupRemoteOperate operate, nim::LogsBackupRemoteState state, void* user_data);
};