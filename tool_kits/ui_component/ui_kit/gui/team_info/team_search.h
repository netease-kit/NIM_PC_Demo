#pragma once
#include "shared/base.h"
#include "module/service/photo_service.h"

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

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 响应获取到群信息的回调函数
	* @param[in] team_event 群组事件
	* @return void	无返回值
	*/
	void OnGetTeamInfoCb(const nim::TeamEvent& team_event);

	/**
	* 响应申请入群的回调函数
	* @param[in] team_event 群组事件
	* @return void	无返回值
	*/
	void OnApplyJoinCb(const nim::TeamEvent& team_event);

	/**
	* 跳转到某个页面
	* @param[in] page 目标页面
	* @return void	无返回值
	*/
	void GotoPage(TeamSearchPage page);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] tid 群组id
	* @param[in] photo_path 头像路径
	* @return void	无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& tid, const std::wstring &photo_path);
private:
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/**
	* 根据群事件修改相关界面显示效果
	* @param[in] team_event 群组事件
	* @return void	无返回值
	*/
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
	AutoUnregister	unregister_cb;

};
}