#pragma once


struct MultispotInfo
{
	int32_t client_type_;
	std::string client_os_;
	std::string device_id_;
	std::string mac_;
	int64_t login_time_;
};

namespace nim_comp
{
/** @class MultispotForm
  * @brief 多端登录窗口。移动端登录同一账号，电脑端会话列表会多出一项“正在使用云信手机版”，点击之会弹出此窗口，可将移动端踢下线。
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
 */
class MultispotForm : public WindowEx
{
public:
	MultispotForm();
	~MultispotForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* msg);
	virtual bool OnClicked(ui::EventArgs* msg);

	void OnMultispotChange(const std::map<int32_t, MultispotInfo>& map_multispot_infos);

private:
public:
	static const LPCTSTR kClassName;
private:
	//ui::ListBox* dev_list_;
};
}