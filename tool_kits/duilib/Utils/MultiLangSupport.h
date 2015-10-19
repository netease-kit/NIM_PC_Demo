#ifndef UI_UTILS_MULTILANGSUPPORT_H_
#define UI_UTILS_MULTILANGSUPPORT_H_


namespace ui 
{

class UILIB_API MutiLanSupport
{
public:
	MutiLanSupport() {};
	virtual ~MutiLanSupport() {};

public:
    static MutiLanSupport* GetInstance();
	std::wstring GetStringViaID(const std::wstring& id);
	bool LoadStringTable(const std::wstring& file_path);

private:
	void ClearAll();
	bool AnalyzeStringTable(const std::vector<std::wstring>& list);

private:	//成员    
	std::map<std::wstring,std::wstring>  string_table_;	//字符串列表
	std::wstring	new_lang_setting_;//用户重新设置过的界面语言设置信息（退出程序后才会保存）
};

}
#endif //UI_UTILS_MULTILANGSUPPORT_H_
