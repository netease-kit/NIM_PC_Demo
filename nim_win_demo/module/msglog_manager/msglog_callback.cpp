#include "stdafx.h"
#include "msglog_callback.h"
#include "shared/zip.h"
#include "shared/unzip.h"
#include "module/session/session_util.h"
#include "gui/msglogmanage/msglog_manage_form.h"
#include "base/encrypt/encrypt.h"
#include "base/encrypt/encrypt_impl.h"
#include <fstream>
#include "openssl/aes.h"
#include "openssl/modes.h"
// #include "openssl/files.h"
// #include "openssl/hex.h"

bool CBC_AESDecryptFile(std::string sKey, std::string sIV, const std::string& src, const std::string& des)
{
	
	FILE* i_stream = fopen(src.c_str(), "rb");
	if (i_stream != NULL)
	{
		FILE* o_stream = fopen(des.c_str(), "wb+");
		if (o_stream != NULL)
		{
			nbase::EncryptInterface_var chiper;
			int file_length = 0;
			chiper.reset(new nbase::Encrypt_Impl());
			chiper->SetMethod(nbase::EncryptMethod::ENC_AES256_CBC);
			chiper->SetDecryptKey(sKey);
			chiper->SetDecryptIvParameterSpec(sIV);
			chiper->EnableDecryptPadding(true, 1);
			int count = 0;
			fseek(i_stream, 0, SEEK_END);
			file_length = ftell(i_stream);
			fseek(i_stream, 0, SEEK_SET);
			std::shared_ptr<char[]> buffer(new char[file_length]());
			std::string o_buffer;
			count = fread(buffer.get(), 1, file_length, i_stream);
			if (count > 0)
			{
				std::string temp_buffer;
				temp_buffer.append(buffer.get(), count);
				chiper->Decrypt(temp_buffer, o_buffer);
				fwrite(o_buffer.data(), o_buffer.size(), 1, o_stream);
			}			
			fclose(o_stream);
		}
		fclose(i_stream);
	}
	return true;
}

bool CBC_AESEncryptFile(std::string sKey, std::string sIV, const std::string& src, const std::string& des)
{
	FILE* i_stream = fopen(src.c_str(), "rb");
	if (i_stream != NULL)
	{
		FILE* o_stream = fopen(des.c_str(), "wb");
		if (o_stream != NULL)
		{		
			nbase::EncryptInterface_var chiper;
			int file_length = 0;
			chiper.reset(new nbase::Encrypt_Impl());
			chiper->SetMethod(nbase::EncryptMethod::ENC_AES256_CBC);
			chiper->SetEncryptKey(sKey);
			chiper->SetEncryptIvParameterSpec(sIV);
			chiper->EnableEncryptPadding(true, 1);
			int count = 0;
			fseek(i_stream, 0, SEEK_END);
			file_length = ftell(i_stream);
			fseek(i_stream, 0, SEEK_SET);
			std::shared_ptr<char[]> buffer(new char[file_length]());
			std::string o_buffer;			
			count = fread(buffer.get(), 1, file_length, i_stream);
			if (count > 0)
			{
				std::string temp_buffer;
				temp_buffer.append(buffer.get(), count);
				chiper->Encrypt(temp_buffer, o_buffer);
				fwrite(o_buffer.data(), o_buffer.size(), 1, o_stream);
			}			
			fclose(o_stream);
		}
		fclose(i_stream);
	}
	return true;
}

bool MsglogCallback::OnMigrateLogFilter(const nim::IMMessage& msg, void* user_data)
{
	static const std::map<nim::NIMMessageType, int> export_type_list =
	{
		{nim::kNIMMessageTypeText,0},
		{nim::kNIMMessageTypeImage,0},
		{nim::kNIMMessageTypeAudio,0},
		{nim::kNIMMessageTypeVideo,0},
		{nim::kNIMMessageTypeFile,0},
		{nim::kNIMMessageTypeCustom,0}
	};
	if (export_type_list.find(msg.type_) == export_type_list.end())
	{
		return false;
	}
	else	if (msg.type_ == nim::kNIMMessageTypeText)
	{
		if (nim_comp::IsRecallMsg(msg.type_, msg.attach_))
			return false;
		else
			return true;
	}
	else if (msg.type_ == nim::kNIMMessageTypeCustom)
	{
		if (nim_comp::IsJSBMsg(msg.type_, msg.attach_))
			return true;
		else
			return false;
	}
	else
	{
		return true;
	}	
}

std::string MsglogCallback::OnMigrateMsglogEncryptCallback(const std::string& local_packaged_file, const std::string& encrypt_key, void* user_data)
{	
	std::string encrypted_file = local_packaged_file.substr(0, local_packaged_file.rfind('.')) + ".aes";
	CBC_AESEncryptFile(encrypt_key, "0123456789012345", local_packaged_file, encrypted_file);
	return encrypted_file;
}

std::string MsglogCallback::OnMigrateMsglogPackageCallback(const std::string& local_source_file, void* user_data)
{
	QLOG_APP(L"MsglogCallback::OnMigrateMsglogPackageCallback running...， local_source_file = {0}") << local_source_file;

	// 取原始的文件名称
	std::string packaged_file_name;
	shared::FilePathApartFileName(local_source_file, packaged_file_name);

	// 找到用户目录并根据原始文件名创建 zip 文件
	std::string account = nim_ui::LoginManager::GetInstance()->GetAccount();
	std::string local_file_name = packaged_file_name.substr(0, packaged_file_name.rfind('.'));

	// 根据 SDK 传回的文件路径取目录
	std::string work_directory;
	shared::FilePathApartDirectory(local_source_file, work_directory);
    std::wstring account_dir = nbase::UTF8ToUTF16(nim::Tool::GetUserAppdataDir(account));
	std::wstring packaged_file_path = nbase::UTF8ToUTF16(work_directory + local_file_name + ".zip");

	// 压缩文件到压缩包中
	HZIP hz = CreateZip(packaged_file_path.c_str(), 0);
	ZipAdd(hz, nbase::UTF8ToUTF16(packaged_file_name).c_str(), nbase::UTF8ToUTF16(local_source_file).c_str());
	CloseZip(hz);

	return nbase::UTF16ToUTF8(packaged_file_path);
}

std::string MsglogCallback::OnMigrateMsglogDecryptCallback(const std::string& remote_encrypted_file, const std::string& encrypt_key, void* user_data)
{
	QLOG_APP(L"MsglogCallback::OnMigrateMsglogDecryptCallback running...");

	std::string packaged_file = remote_encrypted_file.substr(0, remote_encrypted_file.rfind('.')).append("_").append(std::to_string(1)) + ".zip";
	CBC_AESDecryptFile(encrypt_key, "0123456789012345", remote_encrypted_file, packaged_file);
	return packaged_file;
}

std::string MsglogCallback::OnMigrateMsglogUnPackageCallback(const std::string& remote_decrypted_file, void* user_data)
{
	QLOG_APP(L"MsglogCallback::OnMigrateMsglogUnPackageCallback running...");

	// 取 SDK 返回的临时目录，所有操作在该目录下完成
	std::string remote_file_dir;
	shared::FilePathApartDirectory(remote_decrypted_file, remote_file_dir);
	std::wstring work_directory = nbase::UTF8ToUTF16(remote_file_dir);
	std::wstring tmp_unzip_file = L"";

	// 解压解密后的文件
	HZIP hz = OpenZip(nbase::UTF8ToUTF16(remote_decrypted_file).c_str(), 0);
	SetUnzipBaseDir(hz, work_directory.c_str());
	ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
	for (int zi = 0; zi < numitems; zi++)
	{
		GetZipItem(hz, zi, &ze);
		UnzipItem(hz, zi, ze.name);
		// 只压缩了一个文件，记录文件的名字
		tmp_unzip_file = ze.name;
	}
	CloseZip(hz);

	std::string unzip_file = nbase::UTF16ToUTF8(work_directory + tmp_unzip_file);
	if (shared::FilePathIsExist(unzip_file, false))
	{
		return unzip_file;
	}
	else
	{
		return remote_decrypted_file;
	}
}

void MsglogCallback::OnMigrateMsglogProgressCallback(nim::LogsBackupRemoteOperate operate, float progress, void* user_data)
{
	Post2UI([operate, progress]() {
		auto form = (MsglogManageForm*)nim_comp::WindowsManager::GetInstance()->GetWindow(MsglogManageForm::kClassName, MsglogManageForm::kClassName);
		if (form)
			form->OnMigrateMsglogProgressCallbackUI(operate, progress);
	});
}

void MsglogCallback::OnMigrateMsglogCompletedCallback(nim::LogsBackupRemoteOperate operate, nim::LogsBackupRemoteState state, void* user_data)
{
	Post2UI([operate, state]() {
		auto form = (MsglogManageForm*)nim_comp::WindowsManager::GetInstance()->GetWindow(MsglogManageForm::kClassName, MsglogManageForm::kClassName);
		if (form)
			form->OnMigrateMsglogCompletedCallbackUI(operate, state);
	});
}
