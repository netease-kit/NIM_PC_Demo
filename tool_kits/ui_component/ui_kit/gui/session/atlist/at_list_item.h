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
	
	/**
	* 初始化控件
	* @return void	无返回值
	*/
	void InitControls();

	/**
	* 重写父控件的虚函数，用于触发单击消息
	* @return void	无返回值
	*/
	virtual void Activate() override;

	/**
	* 是否为最近5个发言人项
	* @return bool true 是，false 否
	*/
	bool IsLastFive() const	{ return is_last_five_; }

	/**
	* 设置用户id
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void SetUserID(const std::string& uid);

	/**
	* 获取用户id
	* @return string 用户id
	*/
	std::string GetUserID() const { return uid_;	}

	/**
	* 设置群组id
	* @param[in] tid 群组id
	* @return void	无返回值
	*/
	void SetTeamID(const std::string& tid) { team_id_ = tid;	}

	/**
	* 获取群组id
	* @return string 群组id
	*/
	std::string GetTeamID() const { return team_id_;	}

	/**
	* 设置备注名
	* @param[in] name 备注名
	* @return void	无返回值
	*/
	void SetAliasName(const std::wstring& name);

	/**
	* 获取备注名
	* @return string 备注名
	*/
	std::string GetAliasName() const	{ return alias_name_; }

	/**
	* 设置群昵称
	* @param[in] name 群昵称
	* @return void	无返回值
	*/
	void SetTeamCardName(const std::wstring& name);
	 
	/**
	* 获取群昵称
	* @return string 群昵称
	*/
	std::string GetTeamCardName() const { return team_card_name_; }

	/**
	* 设置昵称
	* @param[in] name 昵称
	* @return void	无返回值
	*/
	void SetNickName(const std::wstring& name);

	/**
	* 获取用户昵称
	* @return string 昵称
	*/
	std::string GetNickName() const { return nick_name_; }

	/**
	* 设置用户头像
	* @param[in] head 头像路径
	* @return void	无返回值
	*/
	void SetHeadImage(std::wstring head);

	/**
	* 设置列表项实际显示的名字，优先显示备注名，其次显示群名片，最后显示昵称
	* @param[in] name 名字
	* @return void	无返回值
	*/
	void SetShowName(const std::wstring& name);

	/**
	* 获取实际显示的名字
	* @return string 名字
	*/
	std::wstring GetShowName() const { return name_label_->GetText(); }

	/**
	* 分别匹配备注名，群名片，昵称，uid
	* @param[in] search_key 匹配关键字
	* @return bool true 匹配成功，false 匹配失败
	*/
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