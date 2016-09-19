#pragma once

namespace nim_comp
{
/** @class AtListItem
* @brief @列表窗口中的列表项
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2016/9/6
*/
class AtListItem :public ui::ListContainerElement
{
public:
	AtListItem(bool is_last_five);
	~AtListItem();
	
	void InitControls();
	virtual void Activate() override;

	bool IsLastFive() const	{ return is_last_five_; }

	void SetUserID(const std::string& uid);

	std::string GetUserID() const { return uid_;	}

	void SetTeamID(const std::string& tid) { team_id_ = tid;	}

	std::string GetTeamID() const { return team_id_;	}

	void SetAliasName(const std::wstring& name);

	std::string GetAliasName() const	{ return alias_name_; }

	void SetTeamCardName(const std::wstring& name);

	std::string GetTeamCardName() const { return team_card_name_; }

	void SetNickName(const std::wstring& name);

	std::string GetNickName() const { return nick_name_; }

	void SetHeadImage(std::wstring head);

	// 设置列表项显示的名字，优先显示备注名，其次显示群名片，最后显示昵称
	void SetShowName(const std::wstring& name);
	std::wstring GetShowName() const { return name_label_->GetText(); }

	// 分别匹配备注名，群名片，昵称，ui;如果成功则返回true
	bool Match(const std::string& search_key);

public:
	static const char *kLastFiveItemName;
private:
	bool		is_last_five_;	//是否为显示在列表最前面的最近发言的5人之一

	std::string	team_id_;
	ui::Control	*head_icon_;
	ui::Label	*name_label_;

	std::string	uid_;

	std::string	alias_name_;	//用户备注名,优先显示
	std::string	alias_full_name_;
	std::string	alias_simple_name_;
	std::string	team_card_name_;//用户群名片,其次显示
	std::string	team_card_full_name_;
	std::string	team_card_simple_name_;
	std::string	nick_name_;		//用户昵称,最后显示
	std::string	nick_full_name_;
	std::string	nick_simple_name_;

};
}