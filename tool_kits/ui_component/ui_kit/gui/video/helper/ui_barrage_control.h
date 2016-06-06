// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: gaoqi
// Date: 2014/3/10

#ifndef _UI_BARRAGE_CONTROL_H_
#define _UI_BARRAGE_CONTROL_H_

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
class CBarrageControl : public ui::Control
{
public:
	CBarrageControl();
	~CBarrageControl(void);

	void Paint(HDC hDC, const UiRect& rcPaint) override;
	//清理数据
	void Clear();

	bool AddText(std::wstring text);

	bool IsEmpty();

	void SetBarrageInfo(int32_t line_num_max, int32_t line_height, int32_t line_h_random, int32_t top_pos);

protected:
	bool InsertBarrageItem(BarrageInfo info, bool single, bool random, bool cover);

protected:
	std::list<BarrageInfo> barrage_info_group_[BarrageLineMax];
	int32_t barrage_line_num_max_;
	int32_t line_height_;
	int32_t line_h_random_;
	int32_t barrage_top_pos_;
};

}

#endif // _UI_BARRAGE_CONTROL_H_
