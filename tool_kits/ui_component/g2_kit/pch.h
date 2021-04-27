// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"

//c++ header
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <mutex>
#include <atomic>

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
#include "base/time/time.h"
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

// sdk
#include "nim_chatroom_cpp_wrapper/nim_cpp_chatroom_api.h"
#include "nim_cpp_wrapper/nim_cpp_api.h"
#include "nim_tools_cpp_wrapper/nim_audio_cpp.h"
#include "nim_tools_cpp_wrapper/nim_tools_http_cpp.h"

#include "nim_service/module/service/user_service.h"
#include "nim_service/module/service/http_service.h"
#include "nim_service/module/service/photo_service.h"
#include "nim_service/module/login/login_manager.h"

//helper
#include "shared/closure.h"

//#include "util/window_ex.h"
//#include "util/windows_manager.h"

// ui_kits
//#include "export/nim_ui_all.h"
#include "include/app_sdk.h"

#endif //PCH_H
