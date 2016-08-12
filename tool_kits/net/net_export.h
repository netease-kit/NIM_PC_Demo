// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/30
//
// This file defines NET_BASE_EXPORT_H_ macro

#ifndef NET_BASE_EXPORT_H_
#define NET_BASE_EXPORT_H_

#if defined(NET_BUILD)
#if defined(WIN32)
#if defined(NET_IMPLEMENTATION)
#define NET_EXPORT __declspec(dllexport)
#else
#define NET_EXPORT __declspec(dllimport)
#endif  // defined(BASE_IMPLEMENTATION)
#else
#define NET_EXPORT __attribute__((visibility("default")))
#endif  // defined(WIN32)
#else
#define NET_EXPORT
#endif  // defined(COMPONENT_BUILD)

#endif  // NET_BASE_EXPORT_H_