#include "stdafx.h"
#include "ui_kit/module/multi_retweet/multi_retweet_session.h"
#include <fstream>
#include "base/encrypt/encrypt.h"
#include "base/encrypt/encrypt_impl.h"
#include "shared/closure.h"
#include "ui_kit/export/nim_ui_user_config.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_info.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_data_def.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_logs_archive.h"


namespace nim_comp
{	
	class MultiMessageRetweetSessionHelper
	{
		friend class MultiMessageRetweetSession;
	private:
		static void ProgreessCallback(const MultiMessageRetweetProgressCallback& progress_cb, float progress);
		static bool EncryptCallback(const std::wstring& file_path, const std::wstring& temp_dir, const std::string& encrypt_key);
		static bool DecryptCallback(const std::wstring& file_path, const std::wstring& temp_dir, const std::string& encrypt_key);
	};
	void MultiMessageRetweetSessionHelper::ProgreessCallback(const MultiMessageRetweetProgressCallback& progress_cb, float progress)
	{
		Post2UI(std::bind(progress_cb, progress));
	}	
	bool MultiMessageRetweetSessionHelper::EncryptCallback(const std::wstring& encrypt_file, const std::wstring& decrypt_file, const std::string& encrypt_key)
	{
		FILE* i_stream = fopen(nbase::UTF16ToUTF8(decrypt_file).c_str(), "r");
		if (i_stream != NULL)
		{
			FILE* o_stream = fopen(nbase::UTF16ToUTF8(encrypt_file).c_str(), "wb");
			if (o_stream != NULL)
			{
				nbase::EncryptInterface_var chiper;
				chiper.reset(new nbase::Encrypt_Impl());
				chiper->SetMethod(nbase::EncryptMethod::ENC_ARC4);
				chiper->SetEncryptKey(encrypt_key);
				int count = 0;
				char buffer[1024] = {0};
				std::string o_buffer;
				do
				{
					count = fread(buffer, 1, 1024, i_stream);
					if (count > 0)
					{
						std::string temp_buffer;
						temp_buffer.append(buffer, count);
						chiper->Encrypt(temp_buffer, o_buffer);
						fwrite(o_buffer.data(), o_buffer.size(), 1, o_stream);
						o_buffer.clear();
					}					
				} while (count > 0);		
				fclose(o_stream);
			}
			fclose(i_stream);
		}
		return true;
	}
	bool MultiMessageRetweetSessionHelper::DecryptCallback(const std::wstring& encrypt_file, const std::wstring& decrypt_file, const std::string& encrypt_key)
	{
		FILE* i_stream = fopen(nbase::UTF16ToUTF8(encrypt_file).c_str(), "rb");
		if (i_stream != NULL)
		{
			FILE* o_stream = fopen(nbase::UTF16ToUTF8(decrypt_file).c_str(), "wt+");
			if (o_stream != NULL)
			{
				nbase::EncryptInterface_var chiper;
				chiper.reset(new nbase::Encrypt_Impl());
				chiper->SetMethod(nbase::EncryptMethod::ENC_ARC4);
				chiper->SetDecryptKey(encrypt_key);
				int count = 0;
				char buffer[1024] = { 0 };
				std::string o_buffer;
				do
				{
					count = fread(buffer, 1, 1024, i_stream);
					if (count > 0)
					{
						std::string temp_buffer;
						temp_buffer.append(buffer, count);
						chiper->Decrypt(temp_buffer, o_buffer);
						fwrite(o_buffer.data(), o_buffer.size(), 1, o_stream);
						o_buffer.clear();
					}
				} while (count > 0);
				fclose(o_stream);
			}
			fclose(i_stream);
		}
		return true;
	}
	MultiMessageRetweetSession::MultiMessageRetweetSession(const std::string& session_id, const std::string& show_title,
		const std::list<nim::IMMessage>& message_list,
		const MultiMessageRetweetProgressCallback& progress_cb,
		const std::function<void(bool ret, MultiMessageRetweetState state, const std::shared_ptr< MultiMessageRetweetData>& data)>& cb) 
	{
		Init(session_id,show_title,message_list,progress_cb, cb);
	}
	MultiMessageRetweetSession::MultiMessageRetweetSession(
		const std::shared_ptr< MultiMessageRetweetData>& data,
		const MultiMessageRetweetProgressCallback& progress_cb,
		const std::function<void(bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list)>& cb) :
		data_(data)
	{
		Init(progress_cb, cb);
	}
	void MultiMessageRetweetSession::Init(const std::string& session_id, const std::string& show_title,
		const std::list<nim::IMMessage>& message_list,
		const MultiMessageRetweetProgressCallback& progress_cb,
		const std::function<void(bool ret, MultiMessageRetweetState state, const std::shared_ptr< MultiMessageRetweetData>& data)>& cb)
	{
		data_ = std::make_shared<MultiMessageRetweetData>();
		data_->compressed_ = false;
		data_->encrypted_ = true;
		data_->password_ = CreatePassword();
		data_->FillAbstract(message_list);
		data_->session_id_ = session_id;
		data_->session_name_ = show_title;
		raw_file_info_ = std::make_shared<MultiMessageRetweetRawFileInfo>();
		raw_file_info_->body_.message_list_.assign(message_list.begin(), message_list.end());
		raw_file_info_->head_.message_count_ = raw_file_info_->body_.message_list_.size();
		raw_file_info_->head_.sdk_version_ = nim::Client::GetSDKVersion();
		
		std::wstring ver;
		nim_ui::UserConfig::GetInstance()->GetAppLocalVersion(ver);		
		raw_file_info_->head_.app_version_ = nbase::UTF16ToUTF8(ver);

		exportinfo_ = std::make_shared< MultiMessageRetweetExportInfo>();
		exportinfo_->migrate_temp_directory_ = nim_ui::UserConfig::GetInstance()->GetUserDataPath().append(L"retweet_res/").append(nbase::UTF8ToUTF16(nim::Tool::GetUuid())).append(L"/");
		if (!nbase::FilePathIsExist(exportinfo_->migrate_temp_directory_, true))
			nbase::CreateDirectory(exportinfo_->migrate_temp_directory_);
		exportinfo_->decrypt_path_ = exportinfo_->migrate_temp_directory_ + L"decrypt_file";
		exportinfo_->encrypt_path_ = exportinfo_->migrate_temp_directory_ + L"encrypt_file";
		exportinfo_->complete_callback_ = [this, cb](MultiMessageRetweetState state) {
			nbase::DeleteFile(exportinfo_->decrypt_path_);
			nbase::DeleteFile(exportinfo_->encrypt_path_);
			::RemoveDirectory(exportinfo_->migrate_temp_directory_.c_str());
			Post2UI(std::bind(cb, state == MultiMessageRetweetState::MultiMessageRetweetState_Done, state, this->data_));
		};
		exportinfo_->progreess_callback_ = std::bind(MultiMessageRetweetSessionHelper::ProgreessCallback, progress_cb,std::placeholders::_1);
		exportinfo_->encrypt_callback_ = MultiMessageRetweetSessionHelper::EncryptCallback;
	}
	void MultiMessageRetweetSession::Init(
		const MultiMessageRetweetProgressCallback& progress_cb,
		const std::function<void(bool ret, MultiMessageRetweetState state, const std::list<nim::IMMessage>& message_list)>& cb)
	{
		raw_file_info_ = std::make_shared<MultiMessageRetweetRawFileInfo>();
		importinfo_ = std::make_shared< MultiMessageRetweetImportInfo>();
		importinfo_->migrate_temp_directory_ = nim_ui::UserConfig::GetInstance()->GetUserDataPath().append(L"retweet_res/").append(nbase::UTF8ToUTF16(data_->md5_)).append(L"/");
		if (!nbase::FilePathIsExist(importinfo_->migrate_temp_directory_, true))
			nbase::CreateDirectory(importinfo_->migrate_temp_directory_);
		importinfo_->decrypt_path_ = importinfo_->migrate_temp_directory_ + L"decrypt_file";
		importinfo_->encrypt_path_ = importinfo_->migrate_temp_directory_ + L"encrypt_file";
		importinfo_->complete_callback_ = [this, cb](MultiMessageRetweetState state) {
			nbase::DeleteFile(importinfo_->decrypt_path_);
			Post2UI(std::bind(cb, state == MultiMessageRetweetState::MultiMessageRetweetState_Done, state, this->raw_file_info_->body_.message_list_));
		};
		importinfo_->progreess_callback_ = std::bind(MultiMessageRetweetSessionHelper::ProgreessCallback, progress_cb, std::placeholders::_1);
		importinfo_->decrypt_callback_ = MultiMessageRetweetSessionHelper::DecryptCallback;
	}
	void MultiMessageRetweetSession::DoExport()
	{
		auto task = ToWeakCallback([this]() {
			exportinfo_->progreess_callback_(0);
			RawFileArchive archive(nbase::UTF16ToUTF8(exportinfo_->decrypt_path_), [this](int current_count, int count) {
				if (current_count * 100 / count % 5 == 0)
				{
					float prog = current_count * 1.0f / count;
					exportinfo_->progreess_callback_(0 + prog * 0.5f);
				}
			});
			archive.WriteToFile(raw_file_info_);
			exportinfo_->progreess_callback_(0.5f);
			if (data_->encrypted_)
				exportinfo_->encrypt_callback_(exportinfo_->encrypt_path_, exportinfo_->decrypt_path_, data_->password_);
			else
				nbase::CopyFile(exportinfo_->decrypt_path_, exportinfo_->encrypt_path_);
			exportinfo_->progreess_callback_(0.6f);
			data_->md5_ = nim::Tool::GetFileMd5(nbase::UTF16ToUTF8(exportinfo_->encrypt_path_));
			
			nim::NOS::UploadResource2(nbase::UTF16ToUTF8(exportinfo_->encrypt_path_), "nim_default_im", 
				ToWeakCallback([this](nim::NIMResCode res_code, const std::string& url) {
				if (res_code == nim::kNIMResSuccess)
				{
					data_->url_ = url;
					exportinfo_->complete_callback_(MultiMessageRetweetState::MultiMessageRetweetState_Done);
				}
				else
				{
					exportinfo_->complete_callback_(MultiMessageRetweetState::MultiMessageRetweetState_EXP_UploadFailed);
				}
			}),
				ToWeakCallback([this](int64_t completed_size, int64_t file_size) {
				if (completed_size * 100 / file_size % 5 == 0)
				{
					float prog = completed_size * 1.0f / file_size;
					exportinfo_->progreess_callback_(0 + prog * 0.35f);
				}
			}));
		});
		Post2GlobalMisc(task);
	}
	void MultiMessageRetweetSession::DoImport()
	{
		auto task = ToWeakCallback([this]() {
			importinfo_->progreess_callback_(0);
			auto task_after_download = [this](bool ret) {
				if (!ret)
				{
					importinfo_->complete_callback_(MultiMessageRetweetState::MultiMessageRetweetState_IMP_DownloadFailed);
				}
				else
				{
					importinfo_->progreess_callback_(0.35f);
					if (data_->encrypted_)
						importinfo_->decrypt_callback_(importinfo_->encrypt_path_, importinfo_->decrypt_path_, data_->password_);
					else
						nbase::CopyFile(importinfo_->encrypt_path_, importinfo_->decrypt_path_);
					importinfo_->progreess_callback_(0.45f);
					RawFileArchive archive(nbase::UTF16ToUTF8(importinfo_->decrypt_path_), [this](int current_count, int count) {
						if (current_count * 100 / count % 5 == 0)
						{
							float prog = current_count * 1.0f / count;
							importinfo_->progreess_callback_(0.5f + prog * 0.4f);
						}
					});
					archive.ReadFromFile(raw_file_info_);
					importinfo_->progreess_callback_(1.0f);
					importinfo_->complete_callback_(MultiMessageRetweetState::MultiMessageRetweetState_Done);
				}
			};
			if (nbase::FilePathIsExist(importinfo_->encrypt_path_, false))
			{
				task_after_download(true);
			}
			else
			{
				nim::NOS::DownloadResource(data_->url_, 
					ToWeakCallback([task_after_download,this](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
					if (res_code != nim::kNIMResSuccess)
					{
						task_after_download(false);
					}
					else
					{
						nbase::CopyFile(nbase::UTF8ToUTF16(file_path), importinfo_->encrypt_path_);
						nbase::DeleteFile(nbase::UTF8ToUTF16(file_path));
						task_after_download(true);
					}
				}),
					ToWeakCallback([this](int64_t completed_size, int64_t file_size) {
					if (file_size != 0 && completed_size != 0 && (completed_size * 100 / file_size % 5 == 0))
					{
						float prog = completed_size * 1.0f / file_size;
						importinfo_->progreess_callback_(0 + prog*0.35f);
					}
				}));
			}
		});
		Post2GlobalMisc(task);
	}
	std::string MultiMessageRetweetSession::CreatePassword() const
	{
		static const std::string PWT = "0123456789abcdef";
		static const int PWDLENGTH = 16;
		std::string ret("");
		for (int index = 0; index < PWDLENGTH; index++)
		{
			ret.append(1, PWT[rand() % PWT.length()]);
		}
		return ret;
	}
}