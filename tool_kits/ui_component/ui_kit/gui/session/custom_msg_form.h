#pragma once

namespace nim_comp
{
/** @class CustomMsgForm
  * @brief 自定义通知消息窗体
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/10
  */
class CustomMsgForm : public WindowEx
{
public:
	CustomMsgForm();
	~CustomMsgForm();
	
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
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/** 
	* 设置自定义消息窗体所属的会话的信息
	* @param[in] id	  会话窗体的会话id
	* @param[in] type 会话类型 
	* @param[in] name 消息接收方的名字			
	* @return void 无返回值
	*/
	void SetSession(const std::string& id, nim::NIMSessionType type, const std::wstring& name);
public:
	static const LPCTSTR kClassName;
private:
	std::string		session_id_;
	nim::NIMSessionType		session_type_;

	ui::RichEdit* richedit_apns_;
	ui::RichEdit* richedit_msg_;
	ui::RichEdit* richedit_attach_;
	ui::Label* rec_name_;
	ui::CheckBox* msg_mode_;
};
}