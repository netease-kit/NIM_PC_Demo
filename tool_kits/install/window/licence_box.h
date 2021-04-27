
#ifndef LICENCE_BOX_H_
#define LICENCE_BOX_H_

#include "stdafx.h"

class LicenceForm : public ui::WindowImplBase
{
public:
	LicenceForm();
	virtual ~LicenceForm();

    SINGLETON_DEFINE(LicenceForm);

public:
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    static LicenceForm* ShowLicence();

public:
	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual UINT GetClassStyle() const override;
	virtual void OnFinalMessage( HWND hWnd );
	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* msg);

private:
	virtual std::wstring GetWindowId() const;
	std::wstring GetLicenceText();

public:
	static const LPCTSTR kClassName;
	static const LPCTSTR kCheckBoxName;

private:
	std::wstring		edit_text_;	//输入内容
	ui::RichEdit*	    edit_;
};

#endif // LICENCE_BOX_H_
