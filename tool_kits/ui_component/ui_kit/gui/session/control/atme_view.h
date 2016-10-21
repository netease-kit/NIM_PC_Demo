#pragma once

namespace nim_comp
{
/** @class AtMeView
  * @brief 在会话盒子中显示自己被@的消息
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/9/6
  */
class AtMeView : public ui::ButtonBox
{
public:
	struct AtMeInfo
	{
		std::wstring msg_body;
		std::wstring sender_name;
		std::wstring uuid;
	};

	/**
	* 初始化控件
	* @return void	无返回值
	*/
	void InitControl();

	/**
	* 当新的@me消息到来后，追加到@me消息列表末尾
	* @param[in] at_me_json 包含@me信息的json串
	* @return void	无返回值
	*/
	void AddMessage(const UTF8String &at_me_json);

	/**
	* 显示下一条@me消息
	* @return void	无返回值
	*/
	void ShowNextMessage();	

	/**
	* 当需要查找历史消息才能找到@me消息时，显示“正在加载中”的提示，并且禁用本控件
	* @param[in] show 是否显示提示
	* @return void	无返回值
	*/
	void ShowLoadingTip(bool show);

	/**
	* 获取@me消息总数
	* @return UINT 消息总数
	*/
	UINT GetAtMeCount();

	/**
	* 获取当前的@me消息的uuid
	* @return wstring uuid
	*/
	std::wstring GetUuid();

private:
	/**
	* 根据最后一条@me消息显示对应的界面效果
	* @return void	无返回值
	*/
	void UpdateView();
	
private:
	std::vector<AtMeInfo>	at_me_info_;

	ui::Label		*content_;
	ui::Label		*sender_name_;

};
}