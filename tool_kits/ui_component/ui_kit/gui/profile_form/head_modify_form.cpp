#include "head_modify_form.h"
#include "shared/utf8_file_util.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "module/service/user_service.h"
#include "module/service/photo_service.h"
#include "module/login/login_manager.h"

using namespace ui;

namespace nim_comp
{
// 将最后修改头像的信息缓存起来
static std::wstring g_last_save_head_image_;
static UINT g_last_select_width_;
static RECT g_last_select_padding_;

const LPCTSTR HeadModifyForm::kClassName = L"HeadModifyForm";

HeadModifyForm::HeadModifyForm(UTF8String uid, const std::wstring& specific_cached_file/* = L""*/) : uid_(uid), specific_cached_file_(specific_cached_file)
{
	point_off_.x = 0;
	point_off_.y = 0;
	head_panel_ = NULL;
	head_select_ctrl_ = NULL;
	head_view_ctrl_ = NULL;
	big_head_preview_ctrl_ = NULL;
	small_head_preview_ctrl_ = NULL;
	head_select_moving_ = false;
	select_image_btn_ = NULL;
	save_image_btn_ = NULL;
	cancel_btn_ = NULL;
	caption_container_ = NULL;
	ani_gif_ctrl_ = NULL;
	head_view_default_width_ = 400;
	head_view_default_height_ = 300;
	head_view_real_width_ = 0;
	head_view_real_height_ = 0;
	head_select_cursor_ = HeadSelectControl::SIZE_NULL;
	is_misc_thread_rendering_ = false;
	is_loading_image_ = false;
	is_quit_ = false;
	load_global_last_image_ = false;
	// 可调整
	min_head_select_width_ = 20;
	min_head_select_height_ = 20;
	init_head_select_width_ = 100;
	init_head_select_height_ = 100;
}

HeadModifyForm::~HeadModifyForm()
{
	
}

std::wstring HeadModifyForm::GetWindowId() const
{
	return kClassName;
}

ui::Control* HeadModifyForm::CreateControl(const std::wstring& pstrClass)
{
	// 解析XML时，遇到自定义的节点
	if (pstrClass == _T("HeadViewControl"))
	{
		return new HeadViewControl();
	}
	else if(pstrClass == _T("HeadSelectControl"))
	{
		return new HeadSelectControl();
	}
	else if(pstrClass == _T("HeadPreviewControl"))
	{
		return new HeadPreviewControl();
	}
	return NULL;
}

std::wstring HeadModifyForm::GetSkinFolder()
{
	return L"profile_form";
}

std::wstring HeadModifyForm::GetSkinFile()
{
	return L"head_modify.xml";
}

LRESULT HeadModifyForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)  // press Esc
	{
		OnButtonClose();
		return 0;
	}
	else if (uMsg == WM_SYSKEYDOWN && wParam == VK_F4)  // Alt+F4
	{
		OnButtonClose();
		return 0;
	}
	else if (uMsg == WM_SYSCOMMAND)
	{
		if (wParam == SC_MINIMIZE)  // 最小化
		{
			RECT head_select_rect = head_select_ctrl_->GetPos();
			RECT head_panel_rect = head_panel_->GetPos();
			RECT select_padding = { head_select_rect.left - head_panel_rect.left,
				head_select_rect.top - head_panel_rect.top, 0, 0};
			g_last_select_padding_ = select_padding;
			g_last_select_width_ = head_select_rect.right - head_select_rect.left;
		}
		else if (wParam == SC_RESTORE) // 从最小化恢复
		{
			if (IsIconic(GetHWND()))
			{
				head_select_ctrl_->SetFixedWidth(g_last_select_width_);
				head_select_ctrl_->SetFixedHeight(g_last_select_width_);
				head_select_ctrl_->SetMargin(g_last_select_padding_);
			}
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool HeadModifyForm::Notify(ui::EventArgs* msg)
{
	if(msg->pSender == NULL)
		return true;

	std::wstring name = msg->pSender->GetName();
	if(msg->Type == ui::kEventClick) 
	{
		if(name == L"select_image_btn")
		{
			OnButtonSelectImage();
		}
		else if(name == L"local_upload_btn")
		{
			OnButtonSelectImage();
		}
		else if(name == L"save_image_btn")
		{
			OnButtonSaveImage();
		}
		else if(name == L"cancel_btn")
		{
			OnButtonClose();
		}
		else if(name == L"close_btn")
		{
			OnButtonClose();
			return true;
		}
	}

	return true;
}

void HeadModifyForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&HeadModifyForm::Notify, this, std::placeholders::_1));

	// 获取按钮
	select_image_btn_ = static_cast<ui::Button*>(FindControl(L"select_image_btn"));
	save_image_btn_ = static_cast<ui::Button*>(FindControl(L"save_image_btn"));
	save_image_btn_->SetEnabled(false);
	cancel_btn_ = static_cast<ui::Button*>(FindControl(L"cancel_btn"));
	local_upload_btn_ = static_cast<ui::ButtonBox*>(FindControl(L"local_upload_btn"));

	// 获取Caption Panel
	caption_container_ = static_cast<ui::Box*>(FindControl(L"caption_panel"));
	// 获取头像浏览选择区Panel
	head_panel_ = static_cast<ui::Box*>(FindControl(L"head_panel"));
	// 获取显示区（画布）控件
	head_view_ctrl_ = static_cast<HeadViewControl*>(FindControl(L"head_view_ctrl"));
	head_view_default_width_ = head_view_ctrl_->GetFixedWidth();
	head_view_default_height_ = head_view_ctrl_->GetFixedHeight();
	// 获取头像选区控件
	head_select_ctrl_ =	static_cast<HeadSelectControl*>(FindControl(L"head_select_ctrl"));
	head_select_ctrl_->SetVisible(false);
	// 获取头像预览控件（小）104*104
	small_head_preview_ctrl_ = static_cast<HeadPreviewControl*>(FindControl(L"small_head_preview_ctrl"));
	// 保存头像预览控件（大）200*200
	big_head_preview_ctrl_ = static_cast<HeadPreviewControl*>(FindControl(L"big_head_preview_ctrl"));
	// 小预览头像的蒙版
	std::wstring head_mask_dir =  ui::GlobalManager::GetResourcePath();
	std::wstring small_head_mask_path = head_mask_dir;
	small_head_mask_path.append(L"/public/headmask/btn_headimage_mask_52x52_small_normal.png");
	RECT small_head_padding_rect = { 4, 4, 4, 4 };
	small_head_preview_ctrl_->SetMaskPicturePath(small_head_mask_path, small_head_padding_rect);
	// 大头像预览图的蒙版
	std::wstring big_head_mask_path = head_mask_dir;
	big_head_mask_path.append(L"public/headmask/icon_headimage_mask_168x168.png");
	RECT big_head_padding_rect = { 2, 2, 2, 2 };
	big_head_preview_ctrl_->SetMaskPicturePath(big_head_mask_path, big_head_padding_rect);

	// 判断是否加载本次软件运行上次保存的头像图片
	load_global_last_image_ = false;

	// 预览图显示当前帐号头像
	LoadCurrentHeadImageForPreview();
	ShowPreviewWhileLoading();
	local_upload_btn_->SetVisible();
	//select_image_btn_->SetEnabled(false); 

	/*
	// 加载上次修改的图片(暂时不用)
	if(load_global_last_image_)
	{
		LoadHeadImage();
	}
	else
	{
		// 显示本地上传
		RECT local_upload_padding = { (head_panel_->GetFixedWidth() - local_upload_btn_->GetFixedWidth()) / 2,
			(head_panel_->GetFixedHeight() - local_upload_btn_->GetFixedHeight()) / 2, 0, 0 };
		local_upload_btn_->SetMargin(local_upload_padding);
		local_upload_btn_->SetVisible();
		select_image_btn_->SetEnabled(false); 
	}
	*/
}

// [working on DUI thread]
void HeadModifyForm::LoadHeadImage()
{
	is_loading_image_ = true;
	BeginLoadAnimation(); // 开始转菊花

	/*
	// 目前首次加载或者更换图片过程中的加载，都采用当前头像的Bitmap
	// 释放历史数据，初始化参数。TIP:务必在DUI线程操作，避免DUI(Paint)和Misc(Init)同时操作参数
	if(big_head_preview_ctrl_)
		big_head_preview_ctrl_->Init();
	if(small_head_preview_ctrl_)
		small_head_preview_ctrl_->Init();
	*/
	if(head_view_ctrl_)
		head_view_ctrl_->Init(false);

	head_select_ctrl_->SetVisible(false); // 会引起HeadViewCtrl也重绘
	select_image_btn_->SetEnabled(false);
	save_image_btn_->SetEnabled(false);

	// MISC线程渲染图片
	is_misc_thread_rendering_ = true; // 渲染过程中不要退出，会崩溃
	StdClosure render_image_closure = 
		nbase::Bind(&HeadModifyForm::DoLoadHeadImage, this);
	nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(render_image_closure));
}

// [working on misc thread]
void HeadModifyForm::DoLoadHeadImage()
{
	// 渲染图片
	PreLoadHeadImage();

	// 投递到DUI线程将渲染好的图片显示到界面上
	StdClosure show_image_closure = 
		nbase::Bind(&HeadModifyForm::PostLoadHeadImage, this);
	nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(show_image_closure));
}

// [working on misc thread]
bool HeadModifyForm::PreLoadHeadImage()
{
	// 获取实际图片大小
	int image_width, image_height;
	if(!head_view_ctrl_->RenderImage(image_path_, true))
		return false;  // 如果图像解析失败

	head_view_ctrl_->GetImageSize(image_width, image_height);

	// 计算实际要绘制的大小
	int real_width, real_height;
	float a = head_view_default_height_ * 1.0f;
	float b = head_view_default_width_ * 1.0f;
	float w = image_width * 1.0f;
	float h = image_height * 1.0f;
	float ww, hh;
	if( (a / b) >= (h / w) )
	{
		ww = b;
		hh = h * ww / w;
	}
	else
	{
		hh = a;
		ww = w * hh / h;
	}
	head_view_real_width_ = real_width = (int)ww;
	head_view_real_height_ = real_height = (int)hh;
	float zoom_ratio = (image_width * 1.0f) / (ww); // 压缩比例
	head_view_ctrl_->SetZoomRatio(zoom_ratio);

	// 计算图像居中的padding
	int padding_left = 0;
	int padding_top = 0;
	if(real_width < head_view_default_width_)
		padding_left = (head_view_default_width_ - real_width) / 2; 
	if(real_height < head_view_default_height_)
		padding_top = (head_view_default_height_ - real_height) / 2;
	RECT padding_rect = {padding_left, padding_top, 0 ,0};
	head_view_padding_rect_ = padding_rect;

	// 生成画布图，为了后面抠图
	head_view_ctrl_->RenderCanvas(real_width, real_height);

	// 计算初始选区的大小
	int select_width =0, select_height = 0;
	select_width = init_head_select_width_;
	select_height = init_head_select_height_;
	if(real_width < real_height)
		select_height = select_width = (select_width > real_width) ? real_width : select_width;
	else
		select_width = select_height = (select_height > real_height) ? real_height : select_height;
	select_width_ = select_width;
	select_height_ = select_height;

	// 根据初始的头像选区生成抠图用的临时画布（解决大图像拖动过程中缩放头像会卡的问题）
	RECT init_select_rect = {0, 0, select_width_, select_height_};
	head_select_ctrl_->GetRealSelectRect(init_select_rect); // 实际虚线区域
	head_view_ctrl_->RenderTempCanvas(init_select_rect.right - init_select_rect.left);

	return true;
}

// [working on DUI thread]
void HeadModifyForm::PostLoadHeadImage()
{
	is_misc_thread_rendering_ = false; // MISC线程渲染完毕

	// [1]初始化头像画布
	head_view_ctrl_->SetFixedWidth(head_view_real_width_);
	head_view_ctrl_->SetFixedHeight(head_view_real_height_);
	head_view_ctrl_->SetMargin(head_view_padding_rect_);

	// [2]计算头像选区的初始位置
	int caption_panel_height = caption_container_->GetFixedHeight();
	RECT head_panel_inset = head_panel_->GetLayout()->GetPadding();
	int head_select_client_x = 0; // 头像选区起始位置（客户区坐标）
	int head_select_client_y = 0;
	head_select_client_y += caption_panel_height;
	RECT view_padding_rect = head_view_ctrl_->GetMargin();
	head_select_ctrl_->SetVisible();
	if(!load_global_last_image_)
	{
		// 设置初始大小
		head_select_ctrl_->SetFixedWidth(select_width_);
		head_select_ctrl_->SetFixedHeight(select_height_);
		// 计算初始位置（居中）
		int padding_x = view_padding_rect.left + (head_view_real_width_ - select_width_) / 2;
		int padding_y = view_padding_rect.top + (head_view_real_height_ - select_height_) / 2;
		// 设置初始位置
		RECT margin = {padding_x, padding_y, 0, 0};
		head_select_ctrl_->SetMargin(margin);
		// 计算头像选区的客户区坐标
		head_select_client_x += padding_x;
		head_select_client_y += padding_y;
	}
	else
	{
		// 加载历史选区位置
		select_width_ = select_height_ = g_last_select_width_;
		head_select_ctrl_->SetFixedWidth(select_width_);
		head_select_ctrl_->SetFixedHeight(select_height_);
		head_select_ctrl_->SetMargin(g_last_select_padding_);
		// 计算头像选区的客户区坐标
		head_select_client_x += g_last_select_padding_.left;
		head_select_client_y += g_last_select_padding_.top;
	}
	head_select_client_x += head_panel_inset.left;
	head_select_client_y += head_panel_inset.top;

	// [3]生成初始预览图
	RECT head_select_rect = {head_select_client_x, head_select_client_y,
		head_select_client_x + select_width_, head_select_client_y + select_height_};
	RECT head_view_rect = {head_panel_inset.left + view_padding_rect.left,
		caption_panel_height + head_panel_inset.top + view_padding_rect.top,
		0,0}; // 此时无法GetPos获取，只能手动计算
	head_view_rect.right = head_view_rect.left + head_view_real_width_;
	head_view_rect.bottom = head_view_rect.top + head_view_real_height_;
	RenderPreviewPicture(head_select_rect, head_view_rect, true); // 加载后用原图渲染

	// 按钮恢复可以用
	select_image_btn_->SetEnabled(true);
	save_image_btn_->SetEnabled(true);
	StopLoadAnimation();
	is_loading_image_ = false;
	load_global_last_image_ = false;

	// 判断是否用户要退出
	if(is_quit_)
	{
		// 用户在MISC渲染期间点击了退出，在这时候弹出退出提示
		is_quit_ = false;
		OnButtonClose();
	}
}

void HeadModifyForm::BeginLoadAnimation()
{
	if(is_loading_image_ && ani_gif_ctrl_ == NULL)
	{
		ani_gif_ctrl_ = new Control;
		ani_gif_ctrl_->SetFixedWidth( 32 );
		ani_gif_ctrl_->SetFixedHeight( 32 );
		ani_gif_ctrl_->SetHorAlignType(kHorAlignCenter);
		ani_gif_ctrl_->SetVerAlignType(kVerAlignCenter);
		ani_gif_ctrl_->SetBkImage( L"../public/animation/ani_loading_ex.gif" );
		head_panel_->Add( ani_gif_ctrl_ );
	}
}

void HeadModifyForm::StopLoadAnimation()
{
	if( is_loading_image_ && ani_gif_ctrl_ != NULL )
	{
		head_panel_->Remove( ani_gif_ctrl_ );
		ani_gif_ctrl_ = NULL;
	}
}

bool HeadModifyForm::SetHitTestCursor( int hit_test )
{
	ASSERT(hit_test > HeadSelectControl::SIZE_NULL);
	
	HCURSOR cursor = NULL;
	switch(hit_test)
	{
	case HeadSelectControl::SIZE_ALL : cursor = LoadCursor(NULL, IDC_SIZEALL); break;
	case HeadSelectControl::SIZE_SE :
	case HeadSelectControl::SIZE_NW :  cursor = LoadCursor(NULL, IDC_SIZENWSE); break;
	case HeadSelectControl::SIZE_SW:
	case HeadSelectControl::SIZE_NE :  cursor = LoadCursor(NULL, IDC_SIZENESW); break;
	}

	SetCursor(cursor);
	return true;
}

// 开始移动图片
LRESULT HeadModifyForm::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(is_loading_image_)
		return 0;

	POINT point;
	GetCursorPos(&point); 
	point_off_ = point;
	ScreenToClient(m_hWnd, &point);

	int hit_test = head_select_ctrl_->CheckCursor(point);
	if(hit_test > HeadSelectControl::SIZE_NULL)
	{
		head_select_moving_ = true;
		head_select_cursor_ = hit_test;
		// 记下Point与选区控件右下角的偏差，在MouseMove过程中要修复这个偏差(BOX内点与BOX右下角的偏差)
		RECT head_select_rect = head_select_ctrl_->GetPos(); // 客户区坐标
		if(hit_test == HeadSelectControl::SIZE_SE)
		{
			delta_x_ = head_select_rect.right - point.x; 
			delta_y_ = head_select_rect.bottom - point.y;
		}
		else if(hit_test == HeadSelectControl::SIZE_SW)
		{
			delta_x_ = point.x - head_select_rect.left;
			delta_y_ = head_select_rect.bottom - point.y;
		}
		else if(hit_test == HeadSelectControl::SIZE_NW)
		{
			delta_x_ = point.x - head_select_rect.left;
			delta_y_ = point.y - head_select_rect.top;
		}
		else if(hit_test == HeadSelectControl::SIZE_NE)
		{
			delta_x_ = head_select_rect.right - point.x;
			delta_y_ = point.y - head_select_rect.top;
		}

		SetHitTestCursor(hit_test);
		return 0;
	}
	
	return __super::OnLButtonDown(uMsg, wParam, lParam, bHandled);
}

// 在鼠标按下移动过程中调整图片的显示
LRESULT HeadModifyForm::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(is_loading_image_)
		return 0;

	POINT point;
	GetCursorPos(&point);
	POINT client_pt = point;
	ScreenToClient(m_hWnd, &client_pt);

	if(head_select_moving_)
	{ 
		if (point_off_.x != point.x || point_off_.y != point.y)
		{
			AdjustHeadSelectCtrlOffset(point, point_off_, head_select_cursor_);
			point_off_ = point;
		}
		bHandled = TRUE;
	}
	else
	{
		RECT caption_rect = caption_container_->GetPos();
		if( !PtInRect(&caption_rect, client_pt) )
		{
			int hit = head_select_ctrl_->CheckCursor(client_pt);
			if(hit > HeadSelectControl::SIZE_NULL)
				SetHitTestCursor(hit);
		}	
	}

	return __super::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

// 松开，停止移动图片
LRESULT HeadModifyForm::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(is_loading_image_)
		return 0;

	if(head_select_moving_)
	{
		// 从真实图抠图
		RECT rect = head_select_ctrl_->GetPos();
		// 根据选区(虚线)生成预览图
		RenderPreviewPicture(rect, true);
		// 生成新的临时图
		head_view_ctrl_->RenderTempCanvas(rect.right - rect.left);
		head_select_moving_ = false; // 停止移动选区
	}

	return 0;
}

// 调整选区的位置或大小
void HeadModifyForm::AdjustHeadSelectCtrlOffset( POINT point_dest, POINT point_from, int hit_test )
{
	RECT rect = head_select_ctrl_->GetPos();
	int width, height;
	if(hit_test == HeadSelectControl::SIZE_ALL)
	{
		// 计算鼠标移动距离
		int x = point_dest.x - point_from.x;
		int y = point_dest.y - point_from.y;
		FixHeadSelectCtrlOffset(x, y);
		rect.left += x;
		rect.top += y;
		rect.right += x;
		rect.bottom += y;

		// 保证宽与高相等
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		if(width == height)
			head_select_ctrl_->SetPos(rect);
	}
	else
	{
		POINT client_point_dest = point_dest;
		ScreenToClient(m_hWnd, &client_point_dest);
		switch(hit_test)
		{
		case  HeadSelectControl::SIZE_SE:
			{
				// 新位置(加上因BOX导致的小偏移)
				client_point_dest.x += delta_x_;
				client_point_dest.y += delta_y_;
				// 防止超出活动区域
				FixPointOutOfBounds(client_point_dest);
				// rect新位置
				rect.right = client_point_dest.x; 
				rect.bottom = client_point_dest.y;
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				// 修正45°
				if(width > height)
					rect.right = rect.left + height;
				else
					rect.bottom = rect.top + width;
				// 修正小于最小选区的情况
				if(rect.right - rect.left < min_head_select_width_)
					rect.right = rect.left + min_head_select_width_;
				if(rect.bottom - rect.top < min_head_select_height_)
					rect.bottom = rect.top + min_head_select_height_;
				head_select_ctrl_->SetPos(rect);
			}
			break;
		case HeadSelectControl::SIZE_NE:
			{
				// 新位置(加上因BOX导致的小偏移)
				client_point_dest.x += delta_x_;
				client_point_dest.y -= delta_y_;
				// 防止超出活动区域
				FixPointOutOfBounds(client_point_dest);
				// rect新位置
				rect.right = client_point_dest.x;
				rect.top = client_point_dest.y;
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				// 修正45°
				if(width > height)
					rect.right = rect.left + height;
				else
					rect.top = rect.bottom - width;
				// 修正小于最小选区的情况
				if(rect.right - rect.left < min_head_select_width_)
					rect.right = rect.left + min_head_select_width_;
				if(rect.bottom - rect.top < min_head_select_height_)
					rect.top = rect.bottom - min_head_select_height_;
				head_select_ctrl_->SetPos(rect);
			}
			break;
		case HeadSelectControl::SIZE_NW:
			{
				// 新位置(加上因BOX导致的小偏移)
				client_point_dest.x -= delta_x_;
				client_point_dest.y -= delta_y_;
				// 防止超出活动区域
				FixPointOutOfBounds(client_point_dest);
				// rect新位置
				rect.left = client_point_dest.x;
				rect.top = client_point_dest.y;
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				// 修正45°
				if(width > height)
					rect.left = rect.right - height;
				else
					rect.top = rect.bottom - width;
				// 修正小于最小选区的情况
				if(rect.right - rect.left < min_head_select_width_)
					rect.left = rect.right - min_head_select_width_;
				if(rect.bottom - rect.top < min_head_select_height_)
					rect.top = rect.bottom - min_head_select_height_;
				head_select_ctrl_->SetPos(rect);
			}
			break;
		case HeadSelectControl::SIZE_SW:
			{
				// 新位置(加上因BOX导致的小偏移)
				client_point_dest.x -= delta_x_;
				client_point_dest.y += delta_y_;
				// 防止超出活动区域
				FixPointOutOfBounds(client_point_dest);
				// rect新位置
				rect.left = client_point_dest.x;
				rect.bottom = client_point_dest.y;
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				// 修正45°
				if(width > height)
					rect.left = rect.right - height;
				else
					rect.bottom = rect.top + width;
				// 修正小于最小选区的情况
				if(rect.right - rect.left < min_head_select_width_)
					rect.left = rect.right - min_head_select_width_;
				if(rect.bottom - rect.top < min_head_select_height_)
					rect.bottom = rect.top + min_head_select_height_;
				head_select_ctrl_->SetPos(rect);
			}
			break;
		default:
			break;
		}
	}

	// 生成新的临时图（当拉伸时生成）
	head_view_ctrl_->RenderTempCanvas(rect.right - rect.left);
	// 根据选区(虚线)生成预览图
	RenderPreviewPicture(rect);
}

RECT HeadModifyForm::GetHeadSelectActiveRect()
{
	RECT active_rect = head_view_ctrl_->GetPos();
	// 扩展四个BOX占的宽度
	static int box_width = head_select_ctrl_->GetHalfBoxWidth();
	active_rect.left -= box_width;
	active_rect.top -= box_width;
	active_rect.right += box_width;
	active_rect.bottom += box_width;
	return active_rect;
}

void HeadModifyForm::FixHeadSelectCtrlOffset( int &x, int &y )
{
	if(head_view_ctrl_ == NULL || head_select_ctrl_ == NULL)
		return;

	// 获取选区的区域
	RECT select_rect = head_select_ctrl_->GetPos();
	// 获取可活动区域(new)
	RECT active_rect = GetHeadSelectActiveRect();
	// 修正超出活动区域的偏移
	int padding_right = active_rect.right - select_rect.right;
	int padding_left  = select_rect.left - active_rect.left;
	int padding_top = select_rect.top - active_rect.top;
	int padding_bottom = active_rect.bottom - select_rect.bottom;
	if(x > 0 && x > padding_right)
	{
		x = padding_right;
	}
	else if(x < 0 && (-x) > padding_left)
	{
		x = -padding_left;
	}
	if(y > 0 && y > padding_bottom)
	{
		y = padding_bottom;
	}
	else if(y < 0 && (-y) > padding_top)
	{
		y = -padding_top;
	}
}

void HeadModifyForm::FixPointOutOfBounds( POINT &point )
{
	if(head_view_ctrl_ == NULL || head_select_ctrl_ == NULL)
		return;

	// 获取可活动区域
	RECT active_rect = GetHeadSelectActiveRect();
	// x越界情况
	if(point.x > active_rect.right)
	{
		point.x = active_rect.right;
	}
	else if(point.x < active_rect.left)
	{
		point.x = active_rect.left;
	}
	// y越界情况
	if(point.y > active_rect.bottom)
	{
		point.y = active_rect.bottom;
	}
	else if(point.y < active_rect.top)
	{
		point.y = active_rect.top;
	}
}

void HeadModifyForm::RenderPreviewPicture( RECT select_rect, bool use_original_image)
{
	RECT view_rect = head_view_ctrl_->GetPos();
	RenderPreviewPicture(select_rect, view_rect, use_original_image);	
}

void HeadModifyForm::RenderPreviewPicture( RECT select_rect, RECT view_rect, bool use_original_image)
{
	// 计算相对偏移
	int offset_x = select_rect.left - view_rect.left;
	int offset_y = select_rect.top - view_rect.top;
	int width = select_rect.right - select_rect.left;
	int height = select_rect.bottom - select_rect.top;
	RECT relative_select_rect = {offset_x, offset_y, offset_x + width, offset_y + height};
	// 获取虚线框对应的区域
	head_select_ctrl_->GetRealSelectRect(relative_select_rect); 
	// 生成大小预览图
	Bitmap* select_bitmap = head_view_ctrl_->GetSelectBitmap(relative_select_rect, use_original_image);
	if(select_bitmap)
	{
		if(big_head_preview_ctrl_)
			big_head_preview_ctrl_->SetPaintBitmap(select_bitmap);
		if(small_head_preview_ctrl_)
			small_head_preview_ctrl_->SetPaintBitmap(select_bitmap);
	}
}

// 选择图像
void HeadModifyForm::OnButtonSelectImage()
{
	if(is_loading_image_)
		return;

	select_image_btn_->SetEnabled(false);
	std::wstring file_type = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_PIC_FILE");
	LPCTSTR filter_text = L"*.jpg;*.jpeg;*.png";
	std::wstring text = nbase::StringPrintf(L"%s(%s)", file_type.c_str(),filter_text);
	CFileDialogEx* file_dlg = new CFileDialogEx();
	PTR_VOID(file_dlg);
	std::map<LPCTSTR,LPCTSTR> filters;
	filters[text.c_str()] = filter_text;
	file_dlg->SetFilter(filters);
	file_dlg->SetParentWnd(m_hWnd);
	CFileDialogEx::FileDialogCallback2 callback2 = nbase::Bind(&HeadModifyForm::OnButtonSelectImageCallback, this, std::placeholders::_1, std::placeholders::_2);
	file_dlg->AyncShowOpenFileDlg(ToWeakCallback(callback2));
}

void HeadModifyForm::OnButtonSelectImageCallback( BOOL ret, std::wstring file_path )
{
	if(ret)
	{
		// 重复加载不处理
		if(file_path == image_path_)
		{
			select_image_btn_->SetEnabled(true);
			return;
		}

		image_path_ = file_path;
		if(HeadImageUtil::CheckImageValid(image_path_))
		{
			// 加入到DUI图像缓存中(可能底层会解析失败)
			if (!ui::GlobalManager::GetImage(image_path_))
			{
				OnNotifyImageInvalid();
				return;
			}

			// 是点击本地上传选择图片的
			if(local_upload_btn_->IsVisible())
			{
				local_upload_btn_->SetVisible(false); 
			}
			// 加载过程中预览图显示当前修改的头像
			SaveCurrentHeadImageForPreview();
			ShowPreviewWhileLoading();
			// 线程中加载，界面转菊花
			LoadHeadImage();
		}
		else
		{
			OnNotifyImageInvalid();
		}
	}
	else
	{
		// 用户点击取消
		select_image_btn_->SetEnabled(true);
	}
}

// 保存头像
void HeadModifyForm::OnButtonSaveImage()
{
	if(is_loading_image_)
		return;

	//if (!shared::FilePathIsExist(attach_path, true))
	//	return shared::CreateDirRecursively(attach_path);

	static const std::wstring mime_type = L"image/png";
	std::wstring temp_file_path;
	if (!specific_cached_file_.empty())
	{
		temp_file_path = specific_cached_file_;
	}
	else
	{
		temp_file_path = PhotoService::GetInstance()->GetPhotoDir(kUser);
		std::wstring temp_file_name;
		nbase::Time::TimeStruct now = nbase::Time::Now().ToTimeStruct(true);
		nbase::StringPrintf(temp_file_name, L"temp_head_icon_%d%d%d%d%d%d",
			now.year(), now.month(), now.day_of_month(), now.hour(), now.minute(), now.second());
		temp_file_path.append(temp_file_name);
	}

	// 覆盖本地的旧头像
	if(head_view_ctrl_->SaveSelectBitmap(head_select_ctrl_->GetPos(), temp_file_path, mime_type))
	{
		g_last_save_head_image_ = image_path_;
		RECT head_select_rect = head_select_ctrl_->GetPos();
		RECT head_panel_rect = head_panel_->GetPos();
		RECT select_padding = { head_select_rect.left - head_panel_rect.left,
			head_select_rect.top - head_panel_rect.top, 0, 0};
		g_last_select_padding_ = select_padding;
		g_last_select_width_ = head_select_rect.right - head_select_rect.left;
		new_temp_head_image_path_ = temp_file_path;

		// 在Core线程触发头像修改监听
		StdClosure notify_head_modify_closure = 
			nbase::Bind(&HeadModifyForm::NotifyHeadModify, this, temp_file_path);
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(notify_head_modify_closure));
	}
	else
	{
		// 提示用户，保存失败
		QLOG_ERR(L"Save new head icon failed!");
		OnNotifyHeadModifyFailed();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// 头像修改上传 ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void HeadModifyForm::NotifyHeadModify(std::wstring new_head_image_path)
{
	// 首先上传头像，返回URL
	UTF8String file_path = nbase::UTF16ToUTF8(new_head_image_path);
	auto cb = nbase::Bind(&HeadModifyForm::OnUploadHeadIconCallback, this, std::placeholders::_1, std::placeholders::_2);
	nim::NOS::UploadResource(file_path, cb);
}

void HeadModifyForm::OnUploadHeadIconCallback(int res_code, const std::string& url)
{
	if (res_code == nim::kNIMResSuccess)
	{
		QLOG_APP(L"Head icon upload success and call callback!");

		if (complete_callback_)
			complete_callback_(uid_, url);

		if (specific_cached_file_.empty())
		{
			//删除缓存头像
			StdClosure closure = nbase::Bind(&HeadModifyForm::DeleteFileCallback, new_temp_head_image_path_);
			nbase::ThreadManager::PostTask(kThreadGlobalMisc, closure);
		}

		OnNotifyHeadModifyCallback();
	}
	else
	{
		// 头像上传失败
		QLOG_ERR(L"Upload head icon failed");
		StdClosure closure = nbase::Bind(&HeadModifyForm::OnNotifyHeadModifyFailed, this);
		nbase::ThreadManager::PostTask(kThreadUI, ToWeakCallback(closure));
	}
}

void HeadModifyForm::DeleteFileCallback(const std::wstring& tmp_head_image_path)
{
	// 多端同步成功，保存成功
	if(!tmp_head_image_path.empty() && nbase::FilePathIsExist(tmp_head_image_path, false))
	{
		nbase::DeleteFile(tmp_head_image_path); // 删除临时文件
	}
}

void HeadModifyForm::OnNotifyHeadModifyCallback()
{
	CloseHeadModifyForm(); // 直接关闭窗口
}

void HeadModifyForm::OnNotifyHeadModifyFailed()
{
	auto multilan = MutiLanSupport::GetInstance();
	std::wstring content = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_SAVE_FAILED");
	std::wstring title = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_TITLE");
	std::wstring yes = multilan->GetStringViaID(L"STRING_OK");
	MsgboxCallback cb = nbase::Bind(&HeadModifyForm::OnButtonCloseTipCallback, this, std::placeholders::_1);
	cb = ToWeakCallback(cb);
	ShowMsgBox(m_hWnd, content, cb, title, yes, L"");

	//删除缓存头像
	StdClosure closure = nbase::Bind(&HeadModifyForm::DeleteFileCallback, new_temp_head_image_path_);
	nbase::ThreadManager::PostTask(kThreadGlobalMisc, closure);
}

// 逻辑父窗口关闭时询问
void HeadModifyForm::CloseHeadModifyForm()
{
	OnButtonClose(true);
}

// 退出修改
void HeadModifyForm::OnButtonClose(bool forced_close /*= false*/)
{
	if(forced_close)
	{
		// 强制退出
		this->Close();
	}
	else
	{
		// 是否已经设置退出标记
		if(is_quit_)
			return;

		if(is_misc_thread_rendering_)
		{
			// 设置退出标记，在渲染回调DUI时再弹出提示
			is_quit_ = true; 
		}
		else
		{
			// 判断是否需要弹出提示
			if(head_view_ctrl_->CheckHasLoadImage())
			{
				// 退出提示：是否放弃修改？
				auto multilan = MutiLanSupport::GetInstance();
				std::wstring content = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_CLOSE_TIP");
				std::wstring title = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_TITLE");
				std::wstring yes = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_GIVEUP_MODIFY");
				std::wstring no = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_CONTINUE_MODIFY");
				MsgboxCallback cb = nbase::Bind(&HeadModifyForm::OnButtonCloseTipCallback, this, std::placeholders::_1); 
				cb = ToWeakCallback(cb);
				ShowMsgBox(m_hWnd, content, cb, title, yes, no);
			}
			else
			{
				this->Close();
			}
		}
	}
}

void HeadModifyForm::OnButtonCloseTipCallback( MsgBoxRet ret )
{
	if(ret == MsgBoxRet::MB_YES)
	{
		// 放弃修改
		this->Close();
	}
}

// 加载当前帐号头像为预览图
void HeadModifyForm::LoadCurrentHeadImageForPreview()
{
	nim::UserNameCard info;
	UserService::GetInstance()->GetUserInfo(uid_, info);
	UTF8String img_path = nbase::UTF16ToUTF8(PhotoService::GetInstance()->GetUserPhoto(info.GetAccId()));
	if(!img_path.empty())
	{
		if(shared::FilePathIsExist(img_path, false))
		{
			std::wstring wstr_image_path = nbase::UTF8ToUTF16(img_path);

			current_head_bitmap_.reset(HeadImageUtil::GetFormatBitmap(wstr_image_path));
		}
	}
}

// 保存当前预览头像
void HeadModifyForm::SaveCurrentHeadImageForPreview()
{
	Bitmap* bitmap = big_head_preview_ctrl_->GetPaintBitmap();
	if(bitmap)
	{
		current_head_bitmap_.reset(HeadImageUtil::GetFormatBitmap(bitmap));
	}
}

// 在首次加载或者更换图片加载过程中显示预览图
void HeadModifyForm::ShowPreviewWhileLoading()
{
	if(current_head_bitmap_.get() != NULL
		&& current_head_bitmap_->GetWidth() == 200
		&& current_head_bitmap_->GetHeight() == 200)
	{
		big_head_preview_ctrl_->SetPaintBitmap(current_head_bitmap_.get());
		small_head_preview_ctrl_->SetPaintBitmap(current_head_bitmap_.get());
	}
}

// 图片无效或者DUILIB图片引擎Read图片失败
void HeadModifyForm::OnNotifyImageInvalid()
{
	// 提示图片无效，不是图片或者图片为超过200*200
	auto multilan = MutiLanSupport::GetInstance();
	std::wstring content = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_IMAGE_VALID");
	std::wstring title = multilan->GetStringViaID(L"STRID_HEAD_MODIFY_BUTTON_LOCAL_UPLOAD");
	std::wstring yes = multilan->GetStringViaID(L"STRING_OK");
	MsgboxCallback cb = nbase::Bind(&HeadModifyForm::OnButtonCloseTipCallback, this, std::placeholders::_1);
	cb = ToWeakCallback(cb);
	ShowMsgBox(m_hWnd, content, MsgboxCallback(), title, yes, L"");

	select_image_btn_->SetEnabled(true);
}

void HeadModifyForm::RegCompleteCallback(const OnModifyCompleteCallback &callback)
{
	complete_callback_ = callback;
}
}
