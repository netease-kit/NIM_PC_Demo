#pragma once

#include "util/window_ex.h"
#include "ui_board_control.h"

namespace nim_comp
{
class RtsReplay : public WindowEx
{
public:
	RtsReplay();
	~RtsReplay();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual void InitWindow() override; 

private:
	bool Notify(ui::EventArgs* msg);
	bool OnClicked(ui::EventArgs* msg);

	int RtsReplay::OnParseData(std::string data, std::list<ui::DrawOpInfo>& info_lists);

	void OnBtnFile(bool mine);
	void OnFileSelected(bool mine, BOOL ret, std::wstring file_path);

	void ReShowInfo(bool mine);
	void AutoPlay(bool mine, int pos);
public:
	static const LPCTSTR kClassName; // 类名

private:
	int type_;
	nbase::WeakCallbackFlag auto_play_mine_weakflag_;
	nbase::WeakCallbackFlag auto_play_other_weakflag_;
	ui::BoardControl* board_;
	ui::Slider*		mine_slider_;
	ui::Slider*		other_slider_;
	ui::Label*		mine_size_label_;
	ui::Label*		other_size_label_;
	ui::Label*		mine_pos_label_;
	ui::Label*		other_pos_label_;
	ui::ShowOpType show_type_;
	std::list<ui::DrawOpInfo> info_lists_mine_;
	std::list<ui::DrawOpInfo> info_lists_other_;
};
}