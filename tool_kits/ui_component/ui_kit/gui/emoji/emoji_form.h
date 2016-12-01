#pragma once
#include "module/emoji/emoji_info.h"

typedef std::function<void(std::wstring)> OnSelectEmotion;
typedef std::function<void(std::wstring, std::wstring)> OnSelectSticker;
typedef std::function<void()> OnEmotionClose;

namespace nim_comp
{
/** @class EmojiForm
  * @brief 表情和贴图的选择窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class EmojiForm : public WindowEx
{
public:
	EmojiForm();
	~EmojiForm();
	
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
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 拦截并窗体关闭消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[out] bHandled 是否处理了消息，如果设置为true则不会继续传递消息
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	/**
	* 重写窗口关闭函数
	* @param[in] nRet WM_CLOSE消息返回值
	* @return void	无返回值
	*/
	virtual void Close(UINT nRet = IDOK) override;

	/**
	* 显示表情窗口
	* @param[in] pt 显示的左上角位置
	* @param[in] sel 选中表情的回调函数
	* @param[in] sel_sticker 选中贴纸的回调函数
	* @param[in] close_cb 窗口的回调函数
	* @param[in] only_emoj 是否只显示表情
	* @return void	无返回值
	*/
	void ShowEmoj(POINT pt, OnSelectEmotion sel, OnSelectSticker sel_sticker, OnEmotionClose close_cb, bool only_emoj = false);

private:
	/**
	* 添加一个贴纸表情
	* @param[in] name 贴纸的名字
	* @param[in] num 贴纸数量
	* @return void	无返回值
	*/
	void AddSticker(std::wstring name, int num);

	/**
	* 处理下方表情类型切换按钮的选中消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSelChanged(ui::EventArgs* param);

	/**
	* 处理表情按钮的单击消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnEmojiClicked(ui::EventArgs* param);

	/**
	* 处理贴图按钮的单击消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnStickerClicked(ui::EventArgs* param);
public:
	static const LPCTSTR kClassName;
private:
	ui::TileBox*	emoj_;

	OnSelectEmotion sel_cb_;
	OnSelectSticker sel_sticker_cb_;
	OnEmotionClose	close_cb_;

	bool			only_emoj_;
	bool			is_closing_;
};
}