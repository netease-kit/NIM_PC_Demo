#pragma once
#include "module/emoji/emoji_info.h"

typedef std::function<void(std::wstring)> OnSelectEmotion;
typedef std::function<void(std::wstring, std::wstring)> OnSelectEmotion2;
typedef std::function<void()> OnEmotionClose;

namespace nim_comp
{
class EmojiForm : public WindowEx
{
public:
	EmojiForm();
	~EmojiForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual LRESULT OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;
public:
	void ShowEmoj(POINT pt, OnSelectEmotion sel, OnSelectEmotion2 sel2, OnEmotionClose cls);
private:
	void AddSticker(std::wstring name, int num);
	bool OnSelChanged(ui::EventArgs* param);
	bool OnClicked(ui::EventArgs* arg);
	bool OnClicked2(ui::EventArgs* arg);
public:
	static const LPCTSTR kClassName;
private:
	ui::TileBox*	emoj_;

	OnSelectEmotion sel_cb_;
	OnSelectEmotion2 sel2_cb_;
	OnEmotionClose	cls_cb_;
};
}