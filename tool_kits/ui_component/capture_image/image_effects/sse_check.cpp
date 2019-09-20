#include "sse_check.h"
#include <intrin.h>
#include <excpt.h>

namespace sse_check
{
	const char*    simd_sse_names[] = {
		"Not supported",
		"SSE",
		"SSE2",
		"SSE3",
		"SSSE3",
		"SSE4.1",
		"SSE4.2",
	};

	// 取得CPU厂商（Vendor);
	int GetCPUVendor(char* pvendor)
	{
		int dwBuf[4];
		if (NULL==pvendor)    return 0;		
		__cpuid(dwBuf, 0);//0: Vendor-ID and Largest Standard Function;
		
		*(int*)&pvendor[0] = dwBuf[1];    // ebx: 前四个字符;
		*(int*)&pvendor[4] = dwBuf[3];    // edx: 中间四个字符;
		*(int*)&pvendor[8] = dwBuf[2];    // ecx: 最后四个字符;
		pvendor[12] = '\0';
		return 12;
	}

	// 取得CPU商标（Brand);
	int GetCPUBrand(char* pbrand)
	{
		int dwBuf[4];
		if (NULL==pbrand) return 0;		
		__cpuid(dwBuf, 0x80000000);//0x80000000: Largest Extended Function Number;

		if (dwBuf[0] < 0x80000004) return 0;
		// 80000002h,80000003h,80000004h: Processor Brand String
		__cpuid((int*)&pbrand[0], 0x80000002);    // 前16个字符
		__cpuid((int*)&pbrand[16], 0x80000003);    // 中间16个字符
		__cpuid((int*)&pbrand[32], 0x80000004);    // 最后16个字符
		pbrand[48] = '\0';
		return 48;
	}

	//获取sse版本名称;
	const char* GetSSELevelName(int sse_level)
	{
		int count = sizeof(simd_sse_names)/sizeof(const char*);
		if (sse_level < 0 || sse_level >= count)
			return 0;
		return simd_sse_names[sse_level];
	}

	// 检测SSE系列指令集的支持级别
	int    SimdSSELevel()
	{
		const int    BIT_D_SSE = 0x02000000;    // bit 25
		const int    BIT_D_SSE2 = 0x04000000;    // bit 26
		const int    BIT_C_SSE3 = 0x00000001;    // bit 0
		const int    BIT_C_SSSE3 = 0x00000100;    // bit 9
		const int    BIT_C_SSE41 = 0x00080000;    // bit 19
		const int    BIT_C_SSE42 = 0x00100000;    // bit 20
		int    rt = SIMD_SSE_NONE;    // result
		int dwBuf[4];

		// check processor support
		__cpuidex(dwBuf, 1,0);    // 1: Feature Information
		if ( dwBuf[3] & BIT_D_SSE )
		{
			rt = SIMD_SSE_1;
			if ( dwBuf[3] & BIT_D_SSE2 )
			{
				rt = SIMD_SSE_2;
				if ( dwBuf[2] & BIT_C_SSE3 )
				{
					rt = SIMD_SSE_3;
					if ( dwBuf[2] & BIT_C_SSSE3 )
					{
						rt = SIMD_SSE_3S;
						if ( dwBuf[2] & BIT_C_SSE41 )
						{
							rt = SIMD_SSE_41;
							if ( dwBuf[2] & BIT_C_SSE42 )
							{
								rt = SIMD_SSE_42;
							}
						}
					}
				}
			}
		}

		// check OS support
		__try 
		{
			__m128 xmm1 = _mm_setzero_ps();
			if (0!=*(int*)&xmm1)
				rt = SIMD_SSE_NONE;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			rt = SIMD_SSE_NONE;
		}

		return rt;
	}
}