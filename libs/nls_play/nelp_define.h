/*
*	Author       biwei
*	Date         2016/10/19
*	Copyright    Hangzhou, Netease Inc.
*	Brief
*   Notice
*/

#ifndef _NELP_DEFINE_H_
#define _NELP_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__cdecl)
#define EXTAPI __cdecl
#else
#define EXTAPI
#endif

#ifdef NELIVEPLAYER_EXPORTS
#define EXPORTS_API _declspec(dllexport)
#else
#define EXPORTS_API _declspec(dllimport)
#endif

/*! \file */

#define  NELP_MASK_HANDLE_TYPE(_name)  struct stru_##_name##__ {int iUnused; } * 
#define  NELP_MASK_TYPE(_name, _type)  struct stru_##_name##_type {int iUnused; } *

/***NELP 函数返回值***/
typedef  NELP_MASK_TYPE(_RET, int)      NELP_RET;
#define  NELP_OK                       (NELP_RET)0
#define  NELP_ERR                      (NELP_RET)1
/***NELP 出参标识***/
#define  NELP_OUT
#define  NELP_IN_OUT

#ifdef __cplusplus
}
#endif

#endif//_NE_LIVEPLAYER_H_
