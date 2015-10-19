#pragma once
#include "shared/util.h"


//command
static const std::wstring kCmdAppCrash = L"AppCrash";
static const std::wstring kCmdRestart  = L"Restart";
static const std::wstring kCmdAccount  = L"Account";
static const std::wstring kCmdExitWhy  = L"ExitWhy";

//log file
static const std::wstring kLogFile = L"log.txt";

//user resource folder
static const std::wstring kResImage = L"image\\";
static const std::wstring kResAudio = L"audio\\";

namespace nim_comp
{
//user folder: end with "\\"
std::wstring GetUserDataPath();
std::wstring GetUserImagePath();
std::wstring GetUserAudioPath();
}