// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef NIM_CPP_WRAPPER_UTIL_CPPTL_JSON_ASSERTIONS_H_INCLUDED
#define NIM_CPP_WRAPPER_UTIL_CPPTL_JSON_ASSERTIONS_H_INCLUDED

#include <stdlib.h>
#include <sstream>

#if !defined(JSON_IS_AMALGAMATION)
#include "config.h"
#endif // if !defined(JSON_IS_AMALGAMATION)

#if JSON_USE_EXCEPTION
#include <stdexcept>
#define JSON_ASSERT(condition)                                                 \
  {if (!(condition)) {assert(false); /*throw std::logic_error( "assert json failed" );*/}} // @todo <= add detail about condition in exception //edited by litianyi 20150311
#define JSON_FAIL_MESSAGE(message)                                             \
  {                                                                            \
    std::ostringstream oss; oss << message;                                    \
	assert(false); }//edited by litianyi 20150311 							   \
    //throw std::logic_error(oss.str());                                         \
  }
//#define JSON_FAIL_MESSAGE(message) throw std::logic_error(message)
#else // JSON_USE_EXCEPTION
#define JSON_ASSERT(condition) assert(condition)

// The call to assert() will show the failure message in debug builds. In
// release bugs we abort, for a core-dump or debugger.
#define JSON_FAIL_MESSAGE(message)                                             \
  {                                                                            \
    std::ostringstream oss; oss << message;                                    \
    assert(false && oss.str().c_str());                                        \
    abort();                                                                   \
  }


#endif

#define JSON_ASSERT_MESSAGE(condition, message)                                \
  if (!(condition)) {                                                          \
    JSON_FAIL_MESSAGE(message);                                                \
  }

#endif // NIM_CPP_WRAPPER_UTIL_CPPTL_JSON_ASSERTIONS_H_INCLUDED
