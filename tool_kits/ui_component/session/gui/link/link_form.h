#pragma once

namespace nim_comp
{
class LinkForm : public WindowEx
{
	enum LinkPage
	{
		LINK_TIP,
		LINK_ING,
		LINK_SUCCESS
	};
public:
	LinkForm();
	~LinkForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void InitWindow() override;
	virtual bool OnClicked( ui::EventArgs* msg );
private:
	void SwitchTo(LinkPage lp);

	void OnRelink(const Json::Value &json);
	void OnTimeup();
public:
	static const LPCTSTR kClassName;
private:
	nbase::WeakCallbackFlag timer_;
	AutoUnregister unregister_cb;
};

void ShowLinkForm();
}