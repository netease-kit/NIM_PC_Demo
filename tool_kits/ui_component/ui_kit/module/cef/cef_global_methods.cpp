#include "stdafx.h"
#include "cef_global_methods.h"

namespace nim_comp
{

CefGlobalFunctions::CefGlobalFunctions(CefNativeControl* cef_control)
	: cef_control_(cef_control)
{

}

CefGlobalFunctions::~CefGlobalFunctions()
{

}

void CefGlobalFunctions::Init()
{
	cef_control_->RegisterCppFunc(L"ShowMsgBox", nbase::Bind(&CefGlobalFunctions::OnShowMsgBox, this, std::placeholders::_1, std::placeholders::_2), true);
}

void CefGlobalFunctions::OnShowMsgBox(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(params, values))
	{
		std::wstring title = nbase::UTF8ToUTF16(values["title"].asString());
		std::wstring content = nbase::UTF8ToUTF16(values["content"].asString());
		std::wstring confirm = nbase::UTF8ToUTF16(values["confirm"].asString());
		std::wstring cancel = nbase::UTF8ToUTF16(values["cancel"].asString());
		ShowMsgBox(nullptr, ToWeakCallback([callback](MsgBoxRet ret) {
			if (callback)
			{
				Json::Value values;
				values["result"] = ret;
				callback(false, values.toStyledString());
			}
		}), content, false, title, false, confirm, false, cancel, false);
	}
	else
	{
		if (callback)
			callback(true, R"({ "message": "Invalid params." })");
	}
}

}