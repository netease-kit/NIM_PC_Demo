// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/30
//
// Completion callbacks for aysnc I/O

#ifndef NET_BASE_COMPLETION_CALLBACK_H__
#define NET_BASE_COMPLETION_CALLBACK_H__
#pragma once

#include "base/callback/callback.h"

namespace net
{

// The unique parameter of the callbacks is usually used to report a byte count
// or network error code.
typedef std::function<void(bool, int)> CompletionCallback;

}  // namespace net

#endif  // NET_BASE_COMPLETION_CALLBACK_H__
