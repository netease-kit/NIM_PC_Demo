#pragma once
#include <map>

namespace ui
{
#define BarrageLineMax 30
struct BarrageInfo
{
	std::wstring text_;
	int text_h_;
	int text_w_;
	int top_;
	int speed_;
	int pos_;
	int font_;
	DWORD color_;
	BarrageInfo()
	{
		text_h_ = 0;
		text_w_ = 0;
		top_ = 0;
		speed_ = 0;
		pos_ = 0;
		font_ = 0;
		color_ = 0;
	}
};

/** @class BarrageControl
  * @brief 弹幕控件
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author gaoqi
  * @date 2014/3/10
  */
class BarrageControl : public ui::Control
{
public:
	BarrageControl();
	~BarrageControl(void);

	/**
	* 重写父类绘制函数
	* @param[in] hDC 目标DC
	* @param[in] rcPaint 可绘制范围
	* @return void	无返回值
	*/
	void Paint(IRenderContext* pRender, const UiRect& rcPaint) override;

	/**
	* 清理数据
	* @return void	无返回值
	*/
	void Clear();

	/**
	* 添加一条弹幕
	* @return bool true 成功，false 失败
	*/
	bool AddText(std::wstring text);

	/**
	* 是否没有弹幕内容
	* @return bool true 是，false 否
	*/
	bool IsEmpty();

	/**
	* 设置弹幕信息
	* @param[in] line_num_max 行数限制
	* @param[in] line_height 行高
	* @param[in] line_h_random 随机行高
	* @param[in] top_pos 顶部边距
	* @return void	无返回值
	*/
	void SetBarrageInfo(int32_t line_num_max, int32_t line_height, int32_t line_h_random, int32_t top_pos);

protected:
	/**
	* 插入一个弹幕
	* @param[in] BarrageInfo 弹幕信息
	* @param[in] single 是否单行
	* @param[in] random 是否随机位置
	* @param[in] cover 是否可以覆盖
	* @return void	无返回值
	*/
	bool InsertBarrageItem(BarrageInfo info, bool single, bool random, bool cover);

protected:
	std::list<BarrageInfo> barrage_info_group_[BarrageLineMax];
	int32_t barrage_line_num_max_;
	int32_t line_height_;
	int32_t line_h_random_;
	int32_t barrage_top_pos_;
};

}