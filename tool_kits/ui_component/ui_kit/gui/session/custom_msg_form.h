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
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* param);
	virtual bool OnClicked(ui::EventArgs* param);

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