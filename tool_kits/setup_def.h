
#pragma once

#include <list>
#include <tchar.h>
#include <string>


//参数
#define PRODUCT_NAME				_T("网易云信")	//产品名称，快捷发式相关
#define PRODUCT_VERSION				_T("1.7.0.0")	//产品版本
#define PRODUCT_PUBLISHER			_T("网易公司")	//公司名称
#define PRODUCT_DIR_REGKEY			_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\nim_demo.exe")	//启动路径
#define PRODUCT_UNINST_KEY			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\网易云信")		//卸载相关
#define PRODUCT_AUTORUN_KEY			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")		//开机自启动相关
#define PRODUCT_REGKEY				_T("Software\\Netease\\NIM")	//注册表文件夹
#define PRODUCT_DOC					_T("Netease\\NIM\\")			//产品路径，包括用户数据路径
#define PRODUCT_UNINST_ROOT_KEY		HKLM
#define	RUN_NAME					_T("nim_demo.exe")				//启动文件
#define	UNINSTALL_NAME				_T("uninstall.exe")				//卸载文件
#define PROCESS_NAME				_T("nim_demo.exe")				//进程检测的文件名
#define CLIENT_TYPE					_T("pc")						//系统类型
#define START_CMD_KEY				_T("nimdemo")					//cmd 启动的key用于网页等启动


std::list<std::wstring> GetDelFilesList()
{
	std::list<std::wstring> ret_files;
	ret_files.push_back(_T("UnInstall.exe"));
	ret_files.push_back(_T("render.exe"));
	ret_files.push_back(PROCESS_NAME);
	ret_files.push_back(_T("nim.exe"));
	ret_files.push_back(_T("nim_demo.exe"));
	ret_files.push_back(_T("duilib.dll"));
	ret_files.push_back(_T("image_ole.dll"));
	ret_files.push_back(_T("nim_audio.dll"));
	ret_files.push_back(_T("nim_videochat.dll"));
	ret_files.push_back(_T("nrtc.dll"));
	ret_files.push_back(_T("nrtc_audio_process.dll"));
	ret_files.push_back(_T("nim_audio_hook.dll"));
	ret_files.push_back(_T("nim_http.dll"));
	ret_files.push_back(_T("nim_tools_http.dll"));
	ret_files.push_back(_T("nim.dll"));
	ret_files.push_back(_T("nim_chatroom.dll"));
	ret_files.push_back(_T("msvcp100.dll"));
	ret_files.push_back(_T("msvcr100.dll"));
	ret_files.push_back(_T("app_ver.dll"));
	ret_files.push_back(_T("translation.bin"));
	ret_files.push_back(_T("directui_license.txt"));
	ret_files.push_back(_T("duilib_license.txt"));
	ret_files.push_back(_T("msftedit50.dll"));
	ret_files.push_back(_T("dbghelp.dll"));
	ret_files.push_back(_T("cef\\*.*"));
	ret_files.push_back(_T("cef\\"));
	ret_files.push_back(_T("lang\\*.*"));
	ret_files.push_back(_T("lang\\"));
	ret_files.push_back(_T("res\\*.*"));
	ret_files.push_back(_T("res\\"));
	ret_files.push_back(_T("themes\\*.*"));
	ret_files.push_back(_T("themes\\"));
	ret_files.push_back(_T("nim_conf\\*.*"));
	ret_files.push_back(_T("nim_conf\\"));
	ret_files.push_back(_T("p2p\\*.*"));
	ret_files.push_back(_T("p2p\\")); 
	ret_files.push_back(_T("live_player\\*.*"));
	ret_files.push_back(_T("live_player\\"));
	ret_files.push_back(_T("cef_themes\\*.*"));
	ret_files.push_back(_T("cef_themes\\"));
	ret_files.push_back(_T("resources\\*.*"));
	ret_files.push_back(_T("resources\\"));
	ret_files.push_back(_T("*.*"));
	return ret_files;
}