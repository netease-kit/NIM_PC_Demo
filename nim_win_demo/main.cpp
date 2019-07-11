#include "main.h"
#include "nim_app.h"
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	wprintf_s(L"Command:\n%s\n\n", lpszCmdLine);
#endif
	int ret = NimAPP::GetInstance()->Exec(hInst, hPrevInst, lpszCmdLine, nCmdShow);
#ifdef _DEBUG
	AllocConsole();
	if (fp != nullptr)
	{
		fclose(fp);
		fp = nullptr;
	}
#endif
	return ret;
}

