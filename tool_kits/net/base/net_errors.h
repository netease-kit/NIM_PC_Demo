// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Ported & Modified by Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/30

#ifndef NET_BASE_NET_ERRORS_H__
#define NET_BASE_NET_ERRORS_H__
#pragma once

#include <vector>

#include "base/base_types.h"
#include "net/base/net_export.h"

namespace net {

// Error values are negative.
enum Error {
  // No error.
  OK = 0,

#define NET_ERROR(label, value) ERR_ ## label = value,
#include "net/base/net_error_list.h"
#undef NET_ERROR

  // The value of the first certificate error code.
  ERR_CERT_BEGIN = ERR_CERT_COMMON_NAME_INVALID,
};

// Returns a textual representation of the error code for logging purposes.
NET_EXPORT const char* ErrorToString(int error);

// Returns true if |error| is a certificate error code.
inline bool IsCertificateError(int error) {
  // Certificate errors are negative integers from net::ERR_CERT_BEGIN
  // (inclusive) to net::ERR_CERT_END (exclusive) in *decreasing* order.
  return error <= ERR_CERT_BEGIN && error > ERR_CERT_END;
}

// Map system error code to Error.
NET_EXPORT Error MapSystemError(int os_error);

}  // namespace net

#endif  // NET_BASE_NET_ERRORS_H__
