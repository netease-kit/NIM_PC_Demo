#pragma once
#include "resource.h"

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
#include "base/time/time.h"
#include "base/framework/task.h"

//shared project
#include "shared/utf8_file_util.h"
#include "shared/tool.h"

//json
#include "json/json.h"

//xml
#include "tinyxml.h"

//window header
#include <atlbase.h>
#include <WTypes.h>
#include <shellapi.h>

//log
#include "shared/log.h"

//util
#include "shared/util.h"

//sdk
#include "nim_cpp_wrapper/nim_cpp_api.h"
#include "nim_chatroom_cpp_wrapper/nim_cpp_chatroom_api.h"
#include "nim_tools_cpp_wrapper/nim_tools_http_cpp.h"

//notify
#include "module/service/notify_center.h"
#include "module/service/user_service.h"
#include "module/service/team_service.h"
#include "module/service/mute_black_service.h"
#include "module/service/http_service.h"
#include "module/service/photo_service.h"

//helper
#include "shared/closure.h"

//custom_app_sdk
#include "include/app_sdk.h"

#include <GdiPlus.h>