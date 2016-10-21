#pragma once

#include "util/window_ex.h"
#include "ui_board_control.h"
#include "shared/ui/msgbox.h"

namespace nim_comp
{
/** @class RtsForm
  * @brief 白板窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @Author gaoqi
  * @date 2015/7/7
  */
class RtsForm : public WindowEx
{
public:
	/**
	* 构造函数
	* @param[in] type 通道类型
	* @param[in] uid 对方用户id
	* @param[in] session_id 会话id
	* @return void	无返回值
	*/
	RtsForm(int type, std::string uid, std::string session_id);
	~RtsForm();
	
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;	
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	
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
	* 根据控件类名创建自定义控件
	* @param[in] pstrClass 控件类名
	* @return Control* 创建的控件的指针
	*/
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;

	/**
	* 处理窗口被销毁的消息
	* @param[in] hWnd 窗口句柄
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd) override;
	
	/**
	* 获取会话id
	* @return std::string	会话id
	*/
	std::string GetSessionId() { return session_id_; }

	/**
	* 获取通道类型(NIMRtsChannelType)
	* @return int 通道类型
	*/
	int GetType() { return type_; }

	/**
	* 显示初始白板界面
	* @param[in] creater 自己是否为发起方
	* @return void	无返回值
	*/
	void ShowStartUI(bool creater);

	/**
	* 响应发起白板结果的回调函数
	* @param[in] code 错误码
	* @param[in] session_id 新的会话id
	* @return void	无返回值
	*/
	void StartResult(nim::NIMResCode code, std::string session_id);

	/**
	* 收到对方回复邀请的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	void OnAckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid);

	/**
	* 收到本人在其他端回复邀请的同步通知回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受
	* @return void 无返回值
	*/
	void OnSyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept);

	/**
	* 通道连接状态的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] code 状态码见NIMRtsConnectStatus
	* @param[in] json 成功时带录制的地址和文件名，服务器开启录制时有效
	* @return void 无返回值
	*/
	void OnConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json);

	/**
	* 通道连接成员变化的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 对方帐号
	* @param[in] code 成员变化类型见NIMRtsMemberStatus
	* @return void 无返回值
	*/
	void OnMemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code);

	/**
	* 挂断会话通知的回调函数
	* @param[in] session_id 会话id
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	void OnHangupNotifyCallback(const std::string& session_id, const std::string& uid);

	/**
	* 数据监听的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 对方帐号
	* @param[in] data 接受的数据
	* @return void 无返回值
	*/
	void OnRecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data);

	/**
	* 控制消息通知的回调函数
	* @param[in] session_id 会话id
	* @param[in] text 透传数据
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	void OnControlNotify(const std::string& session_id, const std::string& uid, const std::string& text);

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
	* 响应ShowStartUI中发起白板的回调函数
	* @param[in] old_id 旧的会话id
	* @param[in] res_code 错误码
	* @param[in] session_id 新的会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void OnStartRtsCb(std::string& old_id, nim::NIMResCode res_code, const std::string& session_id, int channel_type, const std::string& uid);

	/**
	* 响应回复接收邀请的回调函数
	* @param[in] res_code 错误码
	* @param[in] session_id 新的会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受邀请
	* @return void	无返回值
	*/
	void OnAckCallback(nim::NIMResCode res_code, const std::string& session_id, int channel_type, bool accept);

	/**
	* 界面上显示白板页面
	* @return void	无返回值
	*/
	void ShowBoardUI();

	/**
	* 界面上显示顶部界面
	* @return void	无返回值
	*/
	void ShowHeader();

	/**
	* 延迟关闭白板窗口
	* @return void	无返回值
	*/
	void DelayClose();

	/**
	* 定时关闭函数，如果指定时间内没有连接通白板，则关闭白板
	* @return void	无返回值
	*/
	void NoActiveTimer();

	/**
	* 定时关闭函数，如果指定时间内没有连接通白板，则关闭白板
	* @param[in] text 显示提示文本
	* @return void	无返回值
	*/
	void ShowTip(std::wstring text);

	/**
	* 响应收到白板控件正在绘制的回调函数
	* @param[in] info 正在绘制的数据
	* @return void	无返回值
	*/
	void OnDrawCallback(ui::DrawOpInfo info);

	/**
	* 把正在绘制的数据发送给对方
	* @return void	无返回值
	*/
	void SendCurData();

	/**
	* 解析当前监听到的数据
	* @param[in] data 接收到的数据
	* @return void 无返回值
	*/
	void OnParseTcpData(std::string data);

	/**
	* 初始化听筒功能
	* @return bool true 成功，false 失败
	*/
	bool InitAudio();

	/**
	* 释放听筒功能
	* @return void 无返回值
	*/
	void FreeAudio();

	/**
	* 初始化麦克风功能
	* @return bool true 成功，false 失败
	*/
	bool InitMic();

	/**
	* 释放麦克风功能
	* @return void 无返回值
	*/
	void FreeMic();

	/**
	* 响应关闭按钮的单击消息
	* @return void	无返回值
	*/
	void OnBtnClose();

	/**
	* 响应MsgBox的操作结果
	* @return ret	操作结果
	*/
	void OnQuitMsgBox(MsgBoxRet ret);

	/**
	* 开启或关闭语音功能
	* @param[in] show 开启或关闭
	* @return void	无返回值
	*/
	void SetAudioStatus(bool show);

	/**
	* 向会话窗口发送一条开启白板的会话消息
	* @return void	无返回值
	*/
	void SendCreateMsg();

	/**
	* 向会话窗口发送一条结束白板的会话消息
	* @return void	无返回值
	*/
	void ShowEndMsg();

	/**
	* 隐藏界面上显示当前操作的控件
	* @return void	无返回值
	*/
	void HideCtrlNotifyTip();

public:
	static const LPCTSTR kClassName; // 类名

private:
	bool show_endmsg_;
	bool need_ack_;
	bool talking_;
	bool closing_;
	int type_;
	std::string uid_;
	std::string session_id_;
	std::wstring uuid_;
	std::string cur_buffer_;

	ui::Label* chat_status_;
	ui::Label* chat_status2_;
	ui::Label* ctrl_notify_;
	ui::BoardControl* board_;
	ui::CheckBox* speak_;
	ui::ShowOpType show_type_;

};
}