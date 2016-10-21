#pragma once

namespace nim_comp
{
/** @class HeadSelectControl
  * @brief 头像选择控件
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author hj
  * @date 2013/12/03
  */
class HeadSelectControl : public ui::Control
{
public:
	HeadSelectControl();
	~HeadSelectControl();
	
	/**
	* 重写父类虚函数
	* @return void	无返回值
	*/
	virtual void HandleMessage(ui::EventArgs& event);

	/**
	* 重写父类虚函数
	* @return void	无返回值
	*/
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override; 

	/**
	* 根据鼠标坐标返回鼠标指针样式
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	int CheckCursor(POINT point);

	/**
	* 得到实际的选区（虚线区域）
	* @param[out] select_rect 实际选区
	* @return void	无返回值
	*/
	void GetRealSelectRect(RECT &select_rect);

	/**
	* 得到四角的拖拉小方块的宽度的一半
	* @return int 虚线区域宽度的一半
	*/
	int GetHalfBoxWidth();

	/**
	* 判断绘制功能是否有效
	* @return bool true 有效，false 无效
	*/
	bool CheckValid();

private:
	/**
	* 绘制一个四角的拖拉小方块
	* @return hdc 目标DC
	* @return rect 目标位置
	*/
	void DrawBox(HDC hdc,const ui::UiRect &rect);

	/**
	* 绘制虚线框
	* @return gp 目标绘图对象
	* @return rect 目标位置
	*/
	void DrawRect(Gdiplus::Graphics &gp, const ui::UiRect &rect);

	/**
	* 绘制原
	* @return gp 目标绘图对象
	* @return rect 目标位置
	*/
	void DrawCircle(Gdiplus::Graphics &gp, const ui::UiRect &rect);

	/**
	* 绘制蒙板
	* @return gp 目标绘图对象
	* @return rect 目标位置
	*/
	void DrawMask(Gdiplus::Graphics &gp, const ui::UiRect &rect);

public:
	enum
	{
		SIZE_NULL = 0,
		SIZE_NW,
		SIZE_NE,
		SIZE_SW,
		SIZE_SE,
		SIZE_ALL
	};

private:
	Gdiplus::Pen		dash_pen_;		// 用于绘制虚线框的画笔
	HBRUSH				null_brush_;    // 空画刷
	HPEN				box_pen_;		// 用于绘制四个BOX
	HBRUSH				white_brush_;   // 白色画刷
	Gdiplus::Pen		circle_pen_;    // 用于绘制圆形选区
	Gdiplus::SolidBrush mask_brush_;    // 用于绘制半透明蒙版
	int					width_;         // 绘制区域的宽度
	int					height_;		// 绘制区域的高度
	int					box_width_;     // 四边方块的宽度(同高度)
	ui::UiRect    box_rect_NW_;
	ui::UiRect    box_rect_NE_;
	ui::UiRect    box_rect_SW_;
	ui::UiRect    box_rect_SE_;
	ui::UiRect    box_rect_ALL_;
};
}