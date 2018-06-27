#ifndef TEAM_INFO_FORM_H_
#define TEAM_INFO_FORM_H_

#include "util/window_ex.h"
#include "iteam_info_ui.h"

namespace nim_comp
{
	class TeamInfoBox;
/** @class TeamInfoForm
  * @brief 群信息展示窗口，发起聊天窗口和邀请群成员共用
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/21
  */
class TeamInfoForm : public WindowEx,public ITeamInfoUI
{
public:
	static TeamInfoForm *ShowTeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info);
	static void ShowTeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info, bool just_box);
	static void UpdateTeamMember(const std::wstring& tid);
public:
	/**
	* 构造函数
	* @param[in] create_or_display 发起群聊还是显示群信息
	* @param[in] type 群组类型
	* @param[in] team_id 群组id
	* @param[in] team_info 群组信息
	*/
	TeamInfoForm(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info);
	virtual ~TeamInfoForm();
	
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
	* 设置标题栏标题
	* @param[in] title 标题栏标题
	* @return void	无返回值
	*/
	virtual void SetTaskbarTitle(const std::wstring &title) override;
	virtual void Close(UINT nRet = 0) override;
	virtual void ActiveWindow() override;
	/**
	* 更新群成员信息
	* @return void	无返回值
	*/
	virtual void UpdateTeamMember() override;
	static const LPCTSTR kClassName;
	static const LPCTSTR kGroupInfoWindowId;
	static const LPCTSTR kTeamInfoWindowId;
private:
	TeamInfoBox* team_info_box_;
	
};
}
#endif // TEAM_INFO_FORM_H_
