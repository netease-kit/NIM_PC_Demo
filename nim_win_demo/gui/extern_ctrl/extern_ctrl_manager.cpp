// Copyright (c) 2015-2016, NetEase Inc. All rights reserved.
//
// Author: haokui <haokui01@corp.netease.com>
// Date: 2017/10/16
//
// extern ctrl manager impl file
#include "extern_ctrl_manager.h"
#include "richedit_with_defmenu.h"
ui::Control* ExternCtrlManager::CreateExternCtrl(const std::wstring& name)
{
	ui::Control* ret = nullptr;
	if (name.compare(L"MenuedRichEdit") == 0)
		ret = new RichEditWithDefMenu;
	return ret;
}