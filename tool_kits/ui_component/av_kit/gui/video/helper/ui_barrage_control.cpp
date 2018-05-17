#include "ui_barrage_control.h"

namespace ui
{

BarrageControl::BarrageControl()
{
	SetBarrageInfo(25, 40, 10, 20);
}

BarrageControl::~BarrageControl(void)
{
	Clear();
}
void BarrageControl::SetBarrageInfo(int32_t line_num_max, int32_t line_height, int32_t line_h_random, int32_t top_pos)
{
	Clear();
	if (line_num_max <= BarrageLineMax)
	{
		barrage_line_num_max_ = line_num_max;
	}
	line_height_ = line_height;
	line_h_random_ = line_h_random;
	barrage_top_pos_ = top_pos;
}

void BarrageControl::Paint(IRenderContext* pRender, const UiRect& rcPaint)
{
	try
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
			return;
		Control::Paint(pRender, rcPaint);

		for (int i = 0; i < barrage_line_num_max_; ++i)
		{
			for (auto it = barrage_info_group_[i].begin(); it != barrage_info_group_[i].end(); )
			{
				BarrageInfo& barrage_info = *it;
				if (barrage_info.speed_ > 0)
				{
					barrage_info.pos_ += barrage_info.speed_;
					if (barrage_info.pos_ - barrage_info.text_w_ > rcPaint.GetWidth())
					{
						barrage_info.speed_ = 0;
					}
					else
					{
						int x = m_rcItem.right - barrage_info.pos_;
						int y = m_rcItem.top + i * line_height_ + barrage_top_pos_ + barrage_info.top_;
						if (x < m_rcPaint.right && x + barrage_info.text_w_ > m_rcPaint.left)
						{
							if (y < m_rcPaint.bottom && y + barrage_info.text_h_ > m_rcPaint.top)
							{
								UiRect rc;
								rc.left = x;
								rc.top = y;
								rc.right = min(x + barrage_info.text_w_, m_rcPaint.right);
								rc.bottom = min(y + barrage_info.text_h_, m_rcPaint.bottom);
								pRender->DrawText(rc, barrage_info.text_, barrage_info.color_, barrage_info.font_, DT_SINGLELINE, 255, false);
							}
						}
					}
				}
				if (barrage_info.speed_ > 0)
				{
					it++;
				}
				else
				{
					it = barrage_info_group_[i].erase(it);
				}
			}
		}
	}
	catch (...)
	{
		throw "CBarrageControl::DoPaint";
	}
}
//清理失效数据
void BarrageControl::Clear()
{
	for (int i = 0; i < BarrageLineMax; ++i)
	{
		barrage_info_group_[i].clear();
	}
}
bool BarrageControl::AddText(std::wstring text)
{
	if (!IsVisible())
	{
		return false;
	}
	BarrageInfo barrage_info;
	switch (rand() % 5)
	{
	case 0:
		barrage_info.color_ = 0xffff0000;
		break;
	case 1:
		barrage_info.color_ = 0xffffff00;
		break;
	case 2:
		barrage_info.color_ = 0xff8f7f40;
		break;
	case 3:
		barrage_info.color_ = 0xff7f3080;
		break;
	default:
		barrage_info.color_ = 0xffffffff;
		break;
	}
	switch (rand() % 4)
	{
	//case 0:
	//	barrage_info.font_ = 25;
	//	break;
	//case 1:
	//	barrage_info.font_ = 22;
	//	break;
	//case 2:
	//	barrage_info.font_ = 20;
	//	break;
	//case 3:
	//	barrage_info.font_ = 14;
	//	break;
	default:
		barrage_info.font_ = 25;
		break;
	}
	barrage_info.text_ = text;
	UiRect rc = this->GetWindow()->GetRenderContext()->MeasureText(text, barrage_info.font_, DT_SINGLELINE, 2000);
	barrage_info.text_w_ = rc.GetWidth();
	barrage_info.text_h_ = rc.GetHeight();
	barrage_info.speed_ = rand() % 4 + rand() % 4 + 4;
	barrage_info.pos_ = 0;
	if (line_h_random_ != 0)
	{
		barrage_info.top_ = rand() % line_h_random_;
	}
	if (!InsertBarrageItem(barrage_info, true, false, false))
	{
		if (!InsertBarrageItem(barrage_info, false, true, false))
		{
			InsertBarrageItem(barrage_info, false, true, true);
		}
	}
	return true;
}
bool BarrageControl::InsertBarrageItem(BarrageInfo info, bool single, bool random, bool cover)
{
	int height = m_rcItem.bottom - m_rcItem.top;
	int barrage_line_num = (height - barrage_top_pos_) / line_height_;
	if (barrage_line_num > barrage_line_num_max_)
	{
		barrage_line_num = barrage_line_num_max_;
	}
	if (barrage_line_num < 1)
	{
		barrage_line_num = 1;
	}
	int start_line_pos = 0;
	if (random)
	{
		start_line_pos = rand() % barrage_line_num;
	}
	if (!single && cover)
	{
		barrage_info_group_[start_line_pos].push_back(info);
		return true;
	}
	int line_pos = start_line_pos;
	do 
	{
		if (barrage_info_group_[line_pos].empty())
		{
			barrage_info_group_[line_pos].push_back(info);
			return true;
		}
		else if (!single)
		{
			if (cover)
			{
				barrage_info_group_[line_pos].push_back(info);
				return true;
			}
			else
			{
				BarrageInfo info_temp = barrage_info_group_[line_pos].back();
				int width = m_rcItem.right - m_rcItem.left;
				int info_temp_right = info_temp.pos_ - info_temp.text_w_;
				if (width < 100)
				{
					width = 100;
				}
				if (info_temp_right > 0)
				{
					if ((width - info_temp_right) / info_temp.speed_ < width / info.speed_)
					{
						barrage_info_group_[line_pos].push_back(info);
						return true;
					}
				}
			}
		}
		line_pos++;
		if (line_pos >= barrage_line_num)
		{
			line_pos = 0;
		}
	} while (line_pos != start_line_pos);
	return false;
}

bool BarrageControl::IsEmpty()
{
	for (int i = 0; i < barrage_line_num_max_; ++i)
	{
		if (!barrage_info_group_[i].empty())
		{
			return false;
		}
	}
	return true;
}

}


