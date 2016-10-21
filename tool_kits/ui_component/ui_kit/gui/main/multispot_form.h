#pragma once

namespace nim_comp
{
/** @class MultispotForm
  * @brief 多端登录窗口。移动端登录同一帐号，电脑端会话列表会多出一项“正在使用云信手机版”，点击之会弹出此窗口，可将移动端踢下线。
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
 */
class MultispotForm : public WindowEx
{
public:
	MultispotForm();
	~MultispotForm();

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
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnClicked(ui::EventArgs* msg);

	/**
	* 更新多端信息的界面显示
	* @param[in] map_multispot_infos 多端登录信息
	* @return void	无返回值
	*/
	void OnMultispotChange(const std::map<nim::NIMClientType, nim::OtherClientPres>& map_multispot_infos);

private:
public:
	static const LPCTSTR kClassName;
private:
	//ui::ListBox* dev_list_;
};
}