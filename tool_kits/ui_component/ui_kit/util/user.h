#pragma once
#include "shared/util.h"

//command
static const std::wstring kCmdAppCrash = L"AppCrash";
static const std::wstring kCmdRestart  = L"Restart";
static const std::wstring kCmdAccount  = L"Account";
static const std::wstring kCmdExitWhy  = L"ExitWhy";

//log file
static const std::wstring kLogFile = L"log.txt";

namespace nim_comp
{
/**
* 获取用户数据目录位置，"\\"结尾
* @return wstring 用户数据目录
*/
std::wstring GetUserDataPath();

/**
* 获取用户图片数据目录位置，"\\"结尾
* @return wstring 用户图片数据目录
*/
std::wstring GetUserImagePath();

/**
* 获取用户语音数据目录位置，"\\"结尾
* @return wstring 用户语音数据目录
*/
std::wstring GetUserAudioPath();
}