/** @file nim_sdk_cpp_wrapper_dll.h
  * @brief 定义导出宏
  * @copyright (c) 2015-2018, NetEase Inc. All rights reserved
  * @date 2018/11/01
  */
#ifndef NIM_SDK_CPP_WRAPPER_DLL_H_
#define NIM_SDK_CPP_WRAPPER_DLL_H_
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 NIM_SDK_CPPWRAPPER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// NIM_SDK_CPPWRAPPER_DLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifdef CPPWRAPPER_DLL
#if defined(NIM_SDK_CPPWRAPPER_EXPORTS)
# 	define NIM_SDK_CPPWRAPPER_DLL_API		__declspec(dllexport)
# else
# 	define NIM_SDK_CPPWRAPPER_DLL_API		__declspec(dllimport)
#endif
#else
#define  NIM_SDK_CPPWRAPPER_DLL_API
#endif
#endif
