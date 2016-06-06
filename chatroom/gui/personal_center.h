#pragma once

class PersonalCenterForm : public nim_comp::WindowEx
{
public:
	PersonalCenterForm();
	~PersonalCenterForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual bool Notify(ui::EventArgs* msg);
	virtual bool OnClicked(ui::EventArgs* msg);
	virtual bool OnSelected(ui::EventArgs* msg);

private:
	void InitHeader();
	void InitRoomManageList();

	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);

	void OnApplyButtonClicked(ui::EventArgs* msg);

public:
	static const LPTSTR kClassName;

private:
	AutoUnregister unregister_cb;
};
