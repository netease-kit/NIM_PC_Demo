// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Wang Rongtao <rtwang@corp.netease.com>
// 2013/8/30
//
// This file defines DB_EXPORT macro

#ifndef DB_DB_EXPORT_H_
#define DB_DB_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)
#if defined(DB_IMPLEMENTATION)
#define DB_EXPORT __declspec(dllexport)
#else
#define DB_EXPORT __declspec(dllimport)
#endif  // defined(BASE_IMPLEMENTATION)
#else
#define DB_EXPORT __attribute__((visibility("default")))
#endif  // defined(WIN32)
#else
#define DB_EXPORT
#endif  // defined(COMPONENT_BUILD)

#endif  // DB_DB_EXPORT_H_