#pragma once
#include "shared/base.h"


class AboutForm : public WindowEx
{
public:
	AboutForm();
	~AboutForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void InitWindow() override;
private:
	bool OnClicked(ui::EventArgs* arg);
public:
	static const LPCTSTR kClassName;
};