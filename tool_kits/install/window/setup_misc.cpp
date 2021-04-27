//work MiscThread
#include "setup_wnd.h"
#include "utils/7zDec.h"
#include "Resource.h"
#include "main/setup_data.h"

#include <ShlObj.h>

//开始删除旧文件
void SetupForm::DelFile()
{
	QLOG_APP(L"DelFile begin");
	int num = CSetupData::GetDelFileInfoListNum();
	for (int i = 0; i < num && !destroy_wnd_; i++)
	{
		CSetupData::DeleteOldFile(i);
		SetProgressCurStepPos(i*PROGRESS_DELFILE / num);
	}
	if (!destroy_wnd_)
	{
		pre_progress_pos_ = PROGRESS_DELFILE;
		QLOG_APP(L"DelFile end");
		StdClosure cb = std::bind(&SetupForm::BeginUnzip, this);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb));
	}
}

void SetupForm::BeginUnzip()
{
	QLOG_APP(L"BeginUnzip");
	std::wstring tip;
	CFileInStream archiveStream;
	CLookToRead lookStream;
	CSzArEx db;
	int32_t res;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	LPCWSTR temp = NULL;
	LPCWSTR zipPath = NULL;
	size_t tempSize = 0;

	allocImp.Alloc = SzAlloc;
	allocImp.Free = SzFree;

	allocTempImp.Alloc = SzAllocTemp;
	allocTempImp.Free = SzFreeTemp;

	HRSRC   hrsc = FindResource(NULL, MAKEINTRESOURCE(IDR_YIXIN_ZIP), L"ZIP");
	HGLOBAL hG = LoadResource(NULL, hrsc);
	DWORD   dwSize = SizeofResource(NULL, hrsc);
	//if (InFile_Open(&archiveStream.file, str7ZipFilePath))
	//{
	//	PrintError("can not open input file");
	//	return 1;
	//}
	if (hG && dwSize > 0)
	{
		InFile_OpenEx(&archiveStream.file, hG, dwSize);
	}
	else
	{
		QLOG_ERR(L"can not open input file");
		res = SZ_ERROR_OPEN_7Z;
		EndUnzip(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FILE_EXTRACTION_FAILED") , res);
		return;
	}

	FileInStream_CreateVTable(&archiveStream);
	LookToRead_CreateVTable(&lookStream, False);

	lookStream.realStream = &archiveStream.s;
	LookToRead_Init(&lookStream);

	CrcGenerateTable();

	SzArEx_Init(&db);
	res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
	if (res != SZ_OK)
	{
		QLOG_ERR(L"can not open db");
		tip = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FILE_EXTRACTION_FAILED");
	}
	else
	{
		int listCommand = 0, testCommand = 0, extractCommand = 0, fullPaths = 0;
		extractCommand = 1; fullPaths = 1;

		UInt32 i;

		UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
		Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
		size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

		UInt32 iFileNum = db.db.NumFiles;
		QLOG_APP(L"{0}") << ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_EXTRACTING_FILES").c_str();
		for (i = 0; i < iFileNum && !destroy_wnd_; i++)
		{
			//interface
			SetProgressCurStepPos(i*PROGRESS_UNZIP / iFileNum);

			size_t offset = 0;
			size_t outSizeProcessed = 0;
			const CSzFileItem *f = db.db.Files + i;
			size_t len;

			len = SzArEx_GetFileNameUtf16(&db, i, NULL);

			if (len > tempSize)
			{
				SzFree(NULL, (void *)zipPath);
				tempSize = len;
				zipPath = (LPCWSTR)SzAlloc(NULL, tempSize * sizeof(zipPath[0]));
				if (zipPath == 0)
				{
					res = SZ_ERROR_MEM;
					tip = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FILE_EXTRACTION_FAILED");
					break;
				}
			}
			SzArEx_GetFileNameUtf16(&db, i, (UInt16 *)zipPath);
			std::wstring strTemPath(zipPath);
			if (f->IsDir)
			{
				strTemPath = CSetupData::CheckDirAndReplace(strTemPath);
			}
			strTemPath = CSetupData::GetFileCopyPath(strTemPath);

			if (strTemPath.empty())
			{
				continue;
			}
			nbase::StringReplaceAll(L"\\", L"/", strTemPath);
			temp = strTemPath.c_str();

			if (f->IsDir)
			{
				//printf("/");
				//CFileFind tempFind;
				//if (tempFind.FindFile(strTemPath))
				//{
				//	QLOG_APP("MyCreateDir：%s", strTemPath);
				//	MyCreateDir((LPCWSTR)temp);
				//}
				continue;
			}
			else
			{
				//interface
				//QLOG_APP(L"正在释放：%s", strTemPath.c_str());

				res = SzArEx_Extract(&db, &lookStream.s, i,
					&blockIndex, &outBuffer, &outBufferSize,
					&offset, &outSizeProcessed,
					&allocImp, &allocTempImp);
				if (res != SZ_OK)
				{
					QLOG_ERR(L"can not open db file {0}") << strTemPath;
					res = SZ_ERROR_FAIL;
					tip = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FILE_EXTRACTION_FAILED");
					//std::wstring msg_tip = nbase::StringPrintf(L"释放文件“%s”失败！\r\n是否重试？", strTemPath.c_str());
					//uint32_t msg_res = ShowUnzipMsg(msg_tip);
					//if (msg_res == kMsgBtn2)//重试
					//{
					//	i--;
					//	continue;
					//}
					//else if (msg_res == kMsgBtn3)//忽略继续
					//{
					//	continue;
					//}
					break;
				}
			}
			if (!testCommand)
			{
				CSzFile outFile;
				size_t processedSize;
				size_t j;
				UInt16 *name = (UInt16 *)temp;
				const UInt16 *destPath = (const UInt16 *)name;
				for (j = 0; name[j] != 0; j++)
				{
					if (name[j] == '/')
					{
						if (fullPaths)
						{
							name[j] = 0;
							MyCreateDir((LPCWSTR)name);
							name[j] = CHAR_PATH_SEPARATOR;
						}
						else
							destPath = name + j + 1;
					}
				}

				//if (f->IsDir)
				//{
				//	MyCreateDir((LPCWSTR)destPath);
				//	printf("\n");
				//	continue;
				//}
				//else 
				if (OutFile_OpenUtf16(&outFile, (const WCHAR *)destPath))
				{
					QLOG_ERR(L"can not open output file {0}") << strTemPath;
					res = SZ_ERROR_FAIL;
					uint32_t msg_res = ShowUnzipMsg(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FILE_OCCUPIED"), strTemPath);
					if (msg_res == kMsgBtn2)//重试
					{
						i--;
						continue;
					}
					else if (msg_res == kMsgBtn3)//忽略继续
					{
						continue;
					}
					break;
				}
				processedSize = outSizeProcessed;
				if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
				{
					QLOG_ERR(L"can not write output file {0}") << strTemPath;
					File_Close(&outFile);
					res = SZ_ERROR_FAIL;
					uint32_t msg_res = ShowUnzipMsg(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_FILE_CANNOT_BE_WRITTEN"), strTemPath);
					if (msg_res == kMsgBtn2)//重试
					{
						i--;
						continue;
					}
					else if (msg_res == kMsgBtn3)//忽略继续
					{
						continue;
					}
					break;
				}
				if (File_Close(&outFile))
				{
					QLOG_ERR(L"can not close output file");
					res = SZ_ERROR_FAIL;
					//break;
				}
#ifdef USE_WINDOWS_FILE
				if (f->AttribDefined)
					SetFileAttributes((LPCWSTR)destPath, f->Attrib);
#endif
			}
		}
		IAlloc_Free(&allocImp, outBuffer);
	}
	SzArEx_Free(&db, &allocImp);
	SzFree(NULL, (void*)zipPath);

	//File_Close(&archiveStream.file);
	if (res == SZ_OK)
	{
		QLOG_APP(L"Everything is Ok");
	}
	else if (res == SZ_ERROR_UNSUPPORTED)
	{
		QLOG_ERR(L"decoder doesn't support this archive");
	}
	else if (res == SZ_ERROR_MEM)
	{
		QLOG_ERR(L"can not allocate memory");
	}
	else if (res == SZ_ERROR_CRC)
	{
		QLOG_ERR(L"CRC error");
	}
	else
	{
		QLOG_ERR(L"ERROR #{0}") << res;
	}
	EndUnzip(tip, res);
}

void SetupForm::EndUnzip(std::wstring tip, uint32_t res)
{
	QLOG_APP(L"EndUnzip tip:{0}, res: {1}") << tip << res;
	if (!destroy_wnd_)
	{
		if (res == SZ_OK)
		{
			pre_progress_pos_ = PROGRESS_DELFILE + PROGRESS_UNZIP;
			StdClosure cb_temp = std::bind(&SetupForm::InstallRedist, this);
			nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb_temp));
		} 
		else
		{
			StdClosure cb_temp = std::bind(&SetupForm::EndSetupCallback, this, tip, res);
			nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb_temp));
		}
	}
}
//开始结束后操作，创建快捷方式等
void SetupForm::CreateLink()
{
	if (!destroy_wnd_)
	{
		QLOG_APP(L"CreateLink begin");
		CSetupData::CreateLnkList();
		SetProgressCurStepPos(PROGRESS_CREATE_LINK);
		::SHChangeNotify(SHCNE_ASSOCCHANGED, NULL, NULL, NULL);
		QLOG_APP(L"CreateLink end");
		pre_progress_pos_ = PROGRESS_DELFILE + PROGRESS_UNZIP + PROGRESS_CREATE_LINK;
		StdClosure cb_temp = std::bind(&SetupForm::WriteRegList, this);
		nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb_temp));
	}
}
//写注册表
void SetupForm::WriteRegList()
{
	if (!destroy_wnd_)
	{
		QLOG_APP(L"WriteRegList begin");
		uint32_t num = CSetupData::GetAddRegInfoListNum();
		for (int i = 0; i < num && !destroy_wnd_; i++)
		{
			CSetupData::WriteRegInfo(i);
			SetProgressCurStepPos(i*PROGRESS_WRITE_REG / num);
		}
		SetProgressCurStepPos(PROGRESS_WRITE_REG);
		pre_progress_pos_ = 100;
		QLOG_APP(L"WriteRegList end");

		StdClosure cb_temp = std::bind(&SetupForm::EndSetupCallback, this, L"", 0);
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb_temp));
	}
}
uint32_t SetupForm::ShowUnzipMsg(std::wstring tip, std::wstring path, std::wstring tip2)
{
	show_msg_res_ = -1;
	path = CSetupData::CheckPathAndReplace(path);
	StdClosure cb = std::bind(&SetupForm::ShowUnzipMsgUI, this, tip, path, tip2);
	nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb));
	while (show_msg_res_ == -1)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		Sleep(10);
	}
	return show_msg_res_;
}

void SetupForm::SetProgressCurStepPos(uint32_t pos)
{
	if (!destroy_wnd_)
	{
		uint32_t progress_pos = pre_progress_pos_ + pos;
		StdClosure cb_temp = std::bind(&SetupForm::ShowProgress, this, progress_pos);
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb_temp));
	}
}