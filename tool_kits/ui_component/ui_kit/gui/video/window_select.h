#pragma once

namespace nim_comp
{
typedef std::function<void(HWND, bool cut)> SelectWndCallback;
class WindowSelectForm : public nim_comp::WindowEx
{
public:
	WindowSelectForm();
	~WindowSelectForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual bool Notify(ui::EventArgs* msg);
	virtual bool OnClicked(ui::EventArgs* msg);
	virtual bool OnSelected(ui::EventArgs* msg);

	void SetSelWndCb(SelectWndCallback cb){ select_wnd_cb_ = cb; }

private:
	void RefreshWndlist();

public:
	static const LPTSTR kClassName;

private:
	SelectWndCallback select_wnd_cb_;
	ui::ListBox* wnd_list_;
};
}