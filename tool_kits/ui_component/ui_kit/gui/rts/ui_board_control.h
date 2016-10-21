#pragma once

#include <map>

namespace ui
{
enum DrawOpType
{
	DrawOpStart = 1,
	DrawOpMove,
	DrawOpEnd,
	DrawOpUndo,
	DrawOpPktId,
	DrawOpClear,
	DrawOpClearCb,
};
enum ShowOpType
{
	ShowOpAll = 0,
	ShowOpMine,
	ShowOpOther,
};
struct DrawOpInfo
{
	int draw_op_type_;
	float x_;
	float y_;
	DrawOpInfo()
	{
		draw_op_type_ = -1;
		x_ = 0;
		y_ = 0;
	}
};

typedef std::function<void(DrawOpInfo info)> DrawOpCallback;

class DrawBaseTool;
/** @class BoardControl
  * @brief 白板绘制控件
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @Author gaoqi
  * @date 2015/7/7
  */
class BoardControl : public ui::Box
{
public:
	BoardControl(void);
	~BoardControl(void);

	/**
	* 重写父控件虚函数
	* @param[in] hDC 目标DC
	* @param[in] rcPaint 目标绘制区域
	* @return void	无返回值
	*/
	void Paint(HDC hDC, const UiRect& rcPaint) override;

	/**
	* 设置绘制白板时的回调函数
	* @param[in] cb 回调函数
	* @return void	无返回值
	*/
	void SetDrawCb(DrawOpCallback cb) { draw_op_cb_ = cb; }

	/**
	* 响应鼠标按下消息
	* @param[in] point 鼠标坐标
	* @return bool true 在本控件范围内，false 不在本控件范围内
	*/
	BOOL OnLButtonDown(POINT point);

	/**
	* 响应鼠标移动消息
	* @param[in] point 鼠标坐标
	* @return void 无返回值
	*/
	void OnMouseMove(POINT point);

	/**
	* 响应鼠标松开消息
	* @param[in] point 鼠标坐标
	* @return void 无返回值
	*/
	void OnLButtonUp(POINT point);

	/**
	* 控件重绘、保存截图时调用
	* @param[in] hdc 目标DC
	* @return void 无返回值
	*/
	void DrawExtraUnits(HDC hdc);

	/**
	* 撤销自己的操作
	* @return bool true 成功，false 失败
	*/
	bool MyUndo();

	/**
	* 撤销对方的操作
	* @return bool true 成功，false 失败
	*/
	bool OtherUndo();

	/**
	* 重做操作
	* @return void 无返回值
	*/
	void Redo();

	/**
	* 释放所有绘制单元
	* @param[in] type 释放类型
	* @return void 无返回值
	*/
	void ReleaseAllDrawUnits(ShowOpType type = ShowOpAll);

	/**
	* 触发控件的重绘操作
	* @return void	无返回值
	*/
	void PaintContent();

	/**
	* 接收到其他人的白板操作
	* @param[in] info_list 白板操作数据列表
	* @return void	无返回值
	*/
	void OnOtherDrawInfos(std::list<DrawOpInfo> info_list);

	/**
	* 设置显示操作的类型
	* @param[in] type 操作类型
	* @return void	无返回值
	*/
	void SetShowType(ShowOpType type);

	/**
	* 重新显示我的白板操作
	* @param[in] info_lists 白板操作数据
	* @param[in] play_pos 显示的开始位置
	* @return void	无返回值
	*/
	void ReShowMine(const std::list<DrawOpInfo>& info_lists, int play_pos);

	/**
	* 重新显示其他人的白板操作
	* @param[in] info_lists 白板操作数据
	* @param[in] play_pos 显示的开始位置
	* @return void	无返回值
	*/
	void ReShowOther(const std::list<DrawOpInfo>& info_lists, int play_pos);

	/**
	* 把某人的白板操作数据添加到绘制队列里
	* @param[in] vec_units 被添加的绘制队列
	* @param[in] info_lists 白板操作数据
	* @param[in] play_pos 显示的开始位置
	* @param[in] mine 是否为自己的白板操作数据
	* @return void	无返回值
	*/
	void AddDrawInfo(std::vector<DrawBaseTool*>& vec_units, const std::list<DrawOpInfo>& info_lists, int play_pos, bool mine);

	/**
	* 播放绘制操作
	* @param[in] info_lists 白板操作数据
	* @param[in] mine 是否为自己的白板操作数据
	* @return void	无返回值
	*/
	void PlayDrawInfo(DrawOpInfo info, bool mine);

protected:

	/**
	* 开始一个新的绘制操作
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	void OnDrawUnitStart(const ui::CPoint &point);

	/**
	* 处理当前正在进行中的绘制操作
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	void OnDrawUnitProcess(const ui::CPoint &point);

	/**
	* 结束当前正在进行中的绘制操作
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	void OnDrawUnitEnd(const ui::CPoint &point);

	/**
	* 创建属于我的绘制单元
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	void CreateMyDrawUnit(const ui::CPoint& point);

	/**
	* 创建属于其他人的绘制单元
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	void CreateOtherDrawUnit(const ui::CPoint& point);

	/**
	* 保存当前的绘制操作
	* @return void	无返回值
	*/
	void SubmitDrawUnit();

	/**
	* 清理保存起来的操作历史
	* @return void	无返回值
	*/
	void ClearHistoryVector();

	/**
	* 创建一个位图对象
	* @param[in] hdc 目标DC
	* @param[out] bitmap 创建的位图对象
	* @return void	无返回值
	*/
	void CreateHBitmap(HDC hdc, HBITMAP& bitmap);

	/**
	* 释放一个位图对象
	* @param[in] bitmap 被释放的位图对象
	* @return void	无返回值
	*/
	void ReleaseHBitmap(HBITMAP& bitmap);

private:
	DrawOpCallback		draw_op_cb_;
	std::vector<DrawBaseTool*>			vec_my_draw_units_;			// 当前绘制单元列表
	std::vector<DrawBaseTool*>			vec_other_draw_units_;		// 当前对方绘制单元列表
	std::vector<DrawBaseTool*>			vec_other_waiting_draw_units_;		// 当前对方绘制单元列表等待列表
	std::vector<DrawBaseTool*>			vec_history_draw_units_;	// 撤销/回滚绘制单元列表
	DrawBaseTool*						current_my_draw_unit_;         // 当前正在绘制的单元
	DrawBaseTool*						current_other_draw_unit_;         // 当前正在绘制的单元
	BOOL								is_begin_draw_;				// 是否开始绘制
	HBITMAP								extra_bitmap_;
	ShowOpType							show_type_;

};

/** @class DrawBaseTool
  * @brief 基本画笔
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @Author gaoqi
  * @date 2015/7/7
  */
class DrawBaseTool
{
public:
	/**
	* 构造函数
	* @param[in] x 起始横坐标
	* @param[in] y 起始纵坐标
	* @param[in] rc_valid 可以绘制的范围
	*/
	DrawBaseTool(int x, int y, const RECT& rc_valid);
	virtual ~DrawBaseTool();

	/**
	* 把本绘制单元的内容绘制到目标DC
	* @param[in] hdc 目标DC
	* @param[in] bitmap 暂时无用
	* @param[in] is_continue 是否在上次绘制的位置继续绘制
	* @return void	无返回值
	*/
	void Render(HDC hdc, HBITMAP bitmap=nullptr, bool is_continue = false);

	/**
	* 设置线宽
	* @param[in] line_width 线宽
	* @return void	无返回值
	*/
	void SetLineWidth(int line_width)	{ line_width_ = line_width; }

	/**
	* 设置线颜色
	* @param[in] dw_color 颜色
	* @return void	无返回值
	*/
	void SetColor(COLORREF dw_color)	{ color_ = dw_color; }

	/**
	* 获取绘制起始点
	* @return ui::CPoint 起始点
	*/
	ui::CPoint	GetStartPoint()		{ return ui::CPoint(left_, top_); }

	/**
	* 获取绘制范围
	* @return ui::CPoint 绘制范围
	*/
	ui::UiRect GetWindowRect()	{ return ui::UiRect(left_, top_, right_, bottom_); }

	/**
	* 获取是否保存绘制内容
	* @return bool true 保存，false不保存
	*/
	bool NeedSaveDraw() { return need_save_draw_; }

	/**
	* 设置结束位置
	* @param[in] x 结束横坐标
	* @param[in] y 结束纵坐标
	* @return bool true
	*/
	virtual bool SetEndPoint(int x, int y)	{ right_ = x; bottom_ = y;  return true; }

	/**
	* 虚函数，用于被子控件重写
	* @return void	无返回值
	*/
	virtual void ResetDrawPos(){}

protected:
	/**
	* 把本绘制单元的内容绘制到目标DC的接口
	* @param[in] hdc 目标DC
	* @param[in] bitmap 暂时无用
	* @param[in] is_continue 是否在上次绘制的位置继续绘制
	* @return void	无返回值
	*/
	virtual	void RenderSelf(HDC hdc, HBITMAP bitmap, bool is_continue) = 0;

	/**
	* 确保要绘制的点在截图区域范围内
	* @param[in out] x 要绘制的横坐标
	* @param[in out] y 要绘制的纵坐标
	* @return void	无返回值
	*/
	void MeasurePoint(int &x, int &y);

protected:
	int left_, top_, right_, bottom_;	//起始坐标，终点坐标
	int line_width_;					//线粗
	COLORREF color_;					//颜色
	ui::UiRect rc_valid_;			//合法范围
	bool need_save_draw_;
};

/** @class DrawSinglePen
  * @brief 简单画笔
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @Author gaoqi
  * @date 2015/7/7
  */
class DrawSinglePen : public DrawBaseTool
{
public:
	/**
	* 构造函数
	* @param[in] x 起始横坐标
	* @param[in] y 起始纵坐标
	* @param[in] rc_valid 可以绘制的范围
	*/
	DrawSinglePen(int x, int y, const RECT& rc_valid)
		:DrawBaseTool(x, y, rc_valid),
		draw_pos_(0)
	{
		m_vtPoints.push_back(ui::CPoint(x, y));
	}

	/**
	* 设置结束位置
	* @param[in] x 结束横坐标
	* @param[in] y 结束纵坐标
	* @return bool true
	*/
	virtual	bool SetEndPoint(int x, int y) override;

	/**
	* 重置绘制起始点
	* @return void	无返回值
	*/
	virtual void ResetDrawPos() override { draw_pos_ = 0; }
protected:
	/**
	* 把本绘制单元的内容绘制到目标DC
	* @param[in] hdc 目标DC
	* @param[in] bitmap 暂时无用
	* @param[in] is_continue 是否在上次绘制的位置继续绘制
	* @return void	无返回值
	*/
	virtual	void RenderSelf(HDC hdc, HBITMAP bitmap, bool is_continue);
private:
	/**
	* 绘制一条线段
	* @param[in] hdc 目标DC
	* @param[in] pt_start 起点
	* @param[in] pt_end 终点
	* @return void	无返回值
	*/
	void DrawSingleLine(HDC hdc, const ui::CPoint &pt_start, const ui::CPoint &pt_end);

	/**
	* 绘制一条线段
	* @param[in] graphics 目标绘图对象
	* @param[in] brush 画刷
	* @param[in] pen 画笔
	* @param[in] pt_start 起点
	* @param[in] pt_end 终点
	* @return void	无返回值
	*/
	void DrawSingleLine(Gdiplus::Graphics &graphics, Gdiplus::Brush &brush, Gdiplus::Pen &pen, const ui::CPoint &pt_start, const ui::CPoint &pt_end);

	/**
	* 根据一个坐标获得一个范围，用于绘制线帽
	* @param[in] pt 坐标
	* @return Gdiplus::Rect	线帽范围
	*/
	Gdiplus::Rect GetRectByPt(const ui::CPoint& pt);
private:
	std::vector<ui::CPoint>	m_vtPoints;
	int draw_pos_;	//绘制度（对于复杂的一步操作设置完成度，不需要每次都重新画）
};

}