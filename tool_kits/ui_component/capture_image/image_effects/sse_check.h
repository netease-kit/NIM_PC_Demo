#ifndef SSE_CHECH_H__
#define SSE_CHECH_H__
namespace sse_check
{

	enum sse_version
	{
		SIMD_SSE_NONE = 0,	// Not supported;
		SIMD_SSE_1,			// SSE
		SIMD_SSE_2,			// SSE2
		SIMD_SSE_3,			// SSE3
		SIMD_SSE_3S,		// SSSE3
		SIMD_SSE_41,		// SSE4.1
		SIMD_SSE_42,		// SSE4.2
	};

	
	// 取得CPU厂商（Vendor);
	int GetCPUVendor(char* pvendor);

	// 取得CPU商标（Brand);
	int GetCPUBrand(char* pbrand);

	//获取sse版本名称;
	const char* GetSSELevelName(int sse_level);

	// 检测SSE系列指令集的支持级别
	int    SimdSSELevel();
}
#endif //SSE_CHECH_H__