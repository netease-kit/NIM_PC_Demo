// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2013/8/28
//
// Xml document utilities

#ifndef SHARED_XML_UTIL_H_
#define SHARED_XML_UTIL_H_

#include <string>

namespace shared
{

#if defined(BUILD_WITH_XML_UTIL)
bool LoadXmlFromFile(TiXmlDocument &xml, const std::wstring &file_path);
#endif

}


#endif // SHARED_XML_UTIL_H_
