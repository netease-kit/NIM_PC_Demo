#pragma once
#include "shared/base.h"

namespace nim_comp
{
class TeamSearchForm : public WindowEx
{
	enum TeamSearchPage
	{
		TSP_SEARCH,
		TSP_APPLY,
		TSP_APPLY_OK,
		TSP_COUNT
	};
public:
	TeamSearchForm();
	~TeamSearchForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;

	void OnGetTeamInfoCb(const nim::TeamEvent& team_event);
	void OnApplyJoinCb(const nim::TeamEvent& team_event);

	void GotoPage(TeamSearchPage page);
private:
	bool Notify(ui::EventArgs* arg);
	bool OnClicked(ui::EventArgs* arg);

	void ShowTeamInfo(const nim::TeamEvent& team_event);
public:
	static const LPCTSTR kClassName;
private:
	ui::Control*	tsp_[TSP_COUNT];

	ui::RichEdit*	re_tid_;
	ui::Label*		error_tip_;
	ui::Button*		btn_search_;

	ui::Button*		team_icon_;
	ui::Label*		team_name_;
	ui::Label*		team_id_;
	ui::Label*		label_group_;
	ui::Button*		btn_apply_;

	ui::RichEdit*	re_apply_;

	std::string		tid_;
	std::wstring	tname_;
};
}