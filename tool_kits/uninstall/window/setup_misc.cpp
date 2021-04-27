//work MiscThread
#include "setup_wnd.h"
#include "utils/7zDec.h"
#include "Resource.h"
#include "main/setup_data.h"

#include <ShlObj.h>

//开始删除旧文件
void SetupForm::DelFile(bool del_user_data)
{
	QLOG_APP(L"DelFile begin");
	QLOG_APP(L"Del Link");
	int num = CSetupData::GetDelFileInfoListNum(0);
	for (int i = 0; i < num && !destroy_wnd_; i++)
	{
		CSetupData::DeleteOldFile(i);
		SetProgressCurStepPos(i*PROGRESS_DEL_LINK / num);
	}
	pre_progress_pos_ = PROGRESS_DEL_LINK;
	QLOG_APP(L"Del Install Dir");
	num = CSetupData::GetDelFileInfoListNum(1);
	for (int i = 0; i < num && !destroy_wnd_; i++)
	{
		CSetupData::DeleteOldFile(i);
		SetProgressCurStepPos(i*PROGRESS_DEL_INSTALL / num);
	}
	pre_progress_pos_ = PROGRESS_DEL_LINK + PROGRESS_DEL_INSTALL;
	if (del_user_data)
	{
		QLOG_APP(L"Del Install userdata");
		num = CSetupData::GetDelFileInfoListNum(2);
		for (int i = 0; i < num && !destroy_wnd_; i++)
		{
			CSetupData::DeleteOldFile(i);
			SetProgressCurStepPos(i*PROGRESS_DEL_USERDATA / num);
		}
	} 
	else
	{
		QLOG_APP(L"Not Del Install userdata");
	}
	pre_progress_pos_ = PROGRESS_DEL_LINK + PROGRESS_DEL_INSTALL + PROGRESS_DEL_USERDATA;
	QLOG_APP(L"DelFile end");
	StdClosure cb = std::bind(&SetupForm::DelReg, this);
	nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback(cb));
}
//删除注册表
void SetupForm::DelReg()
{
	QLOG_APP(L"DelReg begin");
	int num = CSetupData::GetDelRegInfoListNum();
	for (int i = 0; i < num && !destroy_wnd_; i++)
	{
		CSetupData::DeleteRegInfo(i);
		SetProgressCurStepPos(i*PROGRESS_DEL_REG / num);
	}
	pre_progress_pos_ = PROGRESS_DEL_LINK + PROGRESS_DEL_INSTALL + PROGRESS_DEL_USERDATA + PROGRESS_DEL_REG;
	SetProgressCurStepPos(0);
	QLOG_APP(L"DelReg end");
	StdClosure cb = std::bind(&SetupForm::EndSetupCallback, this);
	nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(cb));
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