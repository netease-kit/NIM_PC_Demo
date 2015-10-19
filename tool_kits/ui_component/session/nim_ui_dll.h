// 定义导出宏

#ifdef	NIM_UI_DLL
#ifdef		NIM_UI_DLL_EXPORTS
#define			NIM_UI_DLL_API __declspec(dllexport)
#else
#define			NIM_UI_DLL_API __declspec(dllimport)
#endif
#else
#define		NIM_UI_DLL_API 
#endif
