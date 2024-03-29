// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

#ifndef _WIN64
#if !defined(SUPPORTLOCALPLAYER)
#define SUPPORTLOCALPLAYER
#endif
#endif

//c++ header
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <memory>

//base header
#include "base/callback/callback.h"
#include "base/util/string_util.h"
#include "base/util/unicode.h"
#include "base/util/string_number_conversions.h"
#include "base/memory/deleter.h"
#include "base/memory/singleton.h"
#include "base/win32/platform_string_util.h"
#include "base/win32/path_util.h"
#include "base/win32/win_util.h"
#include "base/thread/thread_manager.h"
#include "base/macros.h"
#include "base/base_types.h"
#include "base/file/file_util.h"
#include "base/framework/task.h"

//shared project
#include "shared/utf8_file_util.h"
#include "shared/tool.h"
#include "shared/auto_unregister.h"
#include "shared/util.h"
//log
#include "shared/log.h"

//third_party
#include "duilib/UIlib.h"
#include "json/json.h"
#include "libyuv.h"

//sdk
#include "nim_cpp_wrapper/nim_cpp_api.h"

#include "nim_service/module/service/user_service.h"
#include "nim_service/module/service/http_service.h"
#include "nim_service/module/service/photo_service.h"
#include "nim_service/module/login/login_manager.h"


