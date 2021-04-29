#pragma once
class TestGifForm : public nim_comp::WindowEx
{
public:
	TestGifForm();
	~TestGifForm();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;
public:
	static const LPCTSTR kClassName;
};