#pragma once

#include "include/cef_task.h"

#if defined(OS_WIN)
#include <windows.h>  // NOLINT(build/include_order)
#endif

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));