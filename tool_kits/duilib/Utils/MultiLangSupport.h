#ifndef UI_UTILS_MULTILANGSUPPORT_H_
#define UI_UTILS_MULTILANGSUPPORT_H_

#pragma once

namespace ui 
{

class UILIB_API MutiLanSupport
{
public:
	static MutiLanSupport* GetInstance();

	std::wstring GetStringViaID(const std::wstring& id);
	bool LoadStringTable(const std::wstring& strFilePath);

private:
	MutiLanSupport() {};
	~MutiLanSupport() {};
	MutiLanSupport(const MutiLanSupport&) = delete;
	MutiLanSupport& operator = (const MutiLanSupport&) = delete;

	void ClearAll();
	bool AnalyzeStringTable(const std::vector<std::wstring>& list);

private: 
	std::map<std::wstring, std::wstring>  m_stringTable;
};

}
#endif //UI_UTILS_MULTILANGSUPPORT_H_
