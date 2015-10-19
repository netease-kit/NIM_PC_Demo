#include "nim_sdk_function.h"

namespace nim
{

static const std::wstring kSdkNimDll_d = L"nim_d.dll";
static const std::wstring kSdkNimDll = L"nim.dll";
HINSTANCE NimSdk::instance_nim_ = NULL;
std::map<std::string, void*> NimSdk::function_map;


std::string ListToJson(const std::list<std::string>& list)
{
	Json::Value json_ids;
	for (auto it = list.begin(); it != list.end(); it++)
	{
		json_ids.append(*it);
	}
	return json_ids.toStyledString();
}

std::string PCharToString(const char* str)
{
	std::string res_str;
	if (str) {
		res_str = str;
	}
	else {
		assert(false);
	}
	return res_str;
}

bool NimSdk::LoadSdkDll()
{
	std::wstring dir = QPath::GetAppPath();
#ifdef _DEBUG
	dir.append(kSdkNimDll_d);
#else
	dir.append(kSdkNimDll);
#endif
	instance_nim_ = ::LoadLibraryW(dir.c_str());
	if (instance_nim_ == NULL)
	{
		QLOG_ERR(L"sdk nim load fail {0} {1}") << dir << GetLastError();
		return false;
	}

	return true;
}

void NimSdk::UnLoadSdkDll()
{
	assert(instance_nim_);
	if (instance_nim_)
	{
		QLOG_APP(L"client cleanup");

		::FreeLibrary(instance_nim_);
		instance_nim_ = NULL;
		QLOG_APP(L"free nim library");
	}
}

}