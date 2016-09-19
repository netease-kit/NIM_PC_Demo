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

	void InitControl();
	// 当新的@么消息到来后，追加到@me消息列表末尾
	void AddMessage(const UTF8String &at_me_json);

	void ShowNextMessage();	
	void ShowLoadingTip(bool show);
	UINT GetAtMeCount();
	std::wstring GetUuid();

private:
	void UpdateView();
	
private:
	std::vector<AtMeInfo>	at_me_info_;

	ui::Label		*content_;
	ui::Label		*sender_name_;

};
}