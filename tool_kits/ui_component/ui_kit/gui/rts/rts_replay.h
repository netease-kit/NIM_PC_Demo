#pragma once

#include "util/window_ex.h"
#include "ui_board_control.h"

namespace nim_comp
{
/** @class RtsReplay
  * @brief 白板操作内容重播窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @Author gaoqi
  * @date 2015/7/7
  */
class RtsReplay : public WindowEx
{
public:
	RtsReplay();
	~RtsReplay();
	
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
	* 处理窗口被销毁的消息
	* @param[in] hWnd 窗口句柄
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd) override;

	/**
	* 根据控件类名创建自定义控件
	* @param[in] pstrClass 控件类名
	* @return Control* 创建的控件的指针
	*/
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
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
	* 解析白板存档文件为白板操作数据
	* @param[in] data 白板存档文件内容
	* @param[out] data 白板操作数据列表
	* @return int 白板操作数据总数
	*/
	int RtsReplay::OnParseData(std::string data, std::list<ui::DrawOpInfo>& info_lists);

	/**
	* 选择白板存档文件
	* @param[in] mine 是否为自己的白板文件
	* @return void	无返回值
	*/
	void OnBtnFile(bool mine);

	/**
	* 响应选择白板存档文件的回调函数
	* @param[in] mine 是否为自己的白板文件
	* @param[in] ret 是否选择了文件
	* @param[in] file_path 选择的文件的路径
	* @return void	无返回值
	*/
	void OnFileSelected(bool mine, BOOL ret, std::wstring file_path);

	/**
	* 显示当前的白板操作内容
	* @param[in] mine 是否为自己的白板文件
	* @return void	无返回值
	*/
	void ReShowInfo(bool mine);

	/**
	* 播放白板操作
	* @param[in] mine 是否为自己的白板文件
	* @param[in] pos 开始播放的位置
	* @return void	无返回值
	*/
	void AutoPlay(bool mine, int pos);
public:
	static const LPCTSTR kClassName; // 类名

private:
	int type_;
	nbase::WeakCallbackFlag auto_play_mine_weakflag_;
	nbase::WeakCallbackFlag auto_play_other_weakflag_;
	ui::BoardControl* board_;
	ui::Slider*		mine_slider_;
	ui::Slider*		other_slider_;
	ui::Label*		mine_size_label_;
	ui::Label*		other_size_label_;
	ui::Label*		mine_pos_label_;
	ui::Label*		other_pos_label_;
	ui::ShowOpType show_type_;
	std::list<ui::DrawOpInfo> info_lists_mine_;
	std::list<ui::DrawOpInfo> info_lists_other_;
};
}