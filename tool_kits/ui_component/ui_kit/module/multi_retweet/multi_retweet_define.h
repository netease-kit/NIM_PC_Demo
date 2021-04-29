#pragma once
namespace nim_comp
{	
	enum class MultiMessageRetweetState
	{
		MultiMessageRetweetState_Begin = -2,
		MultiMessageRetweetState_UnDef,			// 未定义
		MultiMessageRetweetState_Start,				// 开始
		MultiMessageRetweetState_ExportRaw,	// 导出到文件
		MultiMessageRetweetState_Uploading,			// 上传到nos
		MultiMessageRetweetState_Downloading,	// 从nos下载文件
		MultiMessageRetweetState_Canceled = 5,	// 已取消
		MultiMessageRetweetState_Error,					// 出错
		//导入状态
		MultiMessageRetweetState_IMP_NoFile,//没有备份文件
		MultiMessageRetweetState_IMP_DownloadFailed,//下载备份文件出错
		MultiMessageRetweetState_IMP_RAWError,//解密/解压出来的源文件格式错误
		MultiMessageRetweetState_IMP_ParseRAWError,//解析源文件格式错误
		//导出状态
		MultiMessageRetweetState_EXP_RAWError,//导出到源文件失败
		MultiMessageRetweetState_EXP_UploadFailed,//上传文件出错

		MultiMessageRetweetState_Done,				// 完成
		MultiMessageRetweetState_End,
		MultiMessageRetweetState_FinalState_Begin = MultiMessageRetweetState_Canceled//是否已是最终状态的一个标识，可以判断state是否为终态(state >= MultiMessageRetweetState_FinalState_Begin)

	};
	using MultiMessageRetweetProgressCallback = std::function<void(float)>;
	using MultiMessageRetweetCompleteCallback = std::function<void(MultiMessageRetweetState)>;
	using MultiMessageRetweetCustomEncryptExportFileCallback = std::function<bool(const std::wstring& file_path, const std::wstring& temp_dir,const std::string& encrypt_key)>;
	using MultiMessageRetweetCustomDecryptImportFileCallback = std::function<bool(const std::wstring& file_path, const std::wstring& temp_dir, const std::string& encrypt_key)>;

	
}