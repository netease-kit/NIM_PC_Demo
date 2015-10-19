#pragma once
#include "shared/base.h"

namespace nim_comp
{
class BroadForm : public WindowEx
{
public:
	BroadForm();
	~BroadForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;

	void SetTid(const std::string &tid);

	void OnUpdateBroadCb(int rescode, int notification_id, const std::string& tid, const std::string& result);
private:
	bool Notify(ui::EventArgs* arg);
	bool OnClicked(ui::EventArgs* arg);
public:
	static const LPCTSTR kClassName;
private:
	std::string tid_;

	ui::RichEdit*	re_title_;
	ui::Label*		error_1_;

	ui::RichEdit*	re_content_;
	ui::Label*		error_2_;

	ui::Button*		btn_commit_;
};
}