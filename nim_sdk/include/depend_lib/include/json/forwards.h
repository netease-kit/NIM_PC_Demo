// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef NIM_CPP_WRAPPER_UTIL_JSON_FORWARDS_H_INCLUDED
#define NIM_CPP_WRAPPER_UTIL_JSON_FORWARDS_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#include "config.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
namespace nim_cpp_wrapper_util {
	namespace Json {

		// writer.h
		class FastWriter;
		class StyledWriter;

		// reader.h
		class Reader;

		// features.h
		class Features;

		// value.h
		typedef unsigned int ArrayIndex;
		class StaticString;
		class Path;
		class PathArgument;
		class Value;
		class ValueIteratorBase;
		class ValueIterator;
		class ValueConstIterator;

	} // namespace Json
}
#endif // NIM_CPP_WRAPPER_UTIL_JSON_FORWARDS_H_INCLUDED
