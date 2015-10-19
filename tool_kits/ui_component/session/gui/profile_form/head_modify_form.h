/*
 *
 *	Author       huangjun
 *	Email        hzhuangjun2013@corp.netease.com
 *	Date         2013-12-05
 *	Copyright    Hangzhou, Netease Inc.
 *	Brief        修改头像界面
 *
 */

#ifndef YIXIN_WIN_GUI_HEAD_MODIFY_H_
#define YIXIN_WIN_GUI_HEAD_MODIFY_H_

#include "ui_head_view_ctrl.h"
#include "ui_head_select_ctrl.h"
#include "shared/ui/msgbox.h"

namespace nim_comp
{
class HeadModifyForm : public WindowEx
{
public:
	HeadModifyForm(UTF8String uid);
	virtual ~HeadModifyForm();
	
	// 接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	// 覆盖虚函数
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override; // 当窗口创建完成后初始化
	virtual bool Notify(ui::EventArgs* msg); // 窗口控件消息通知
	virtual	ui::Control* CreateControl(const std::wstring& pstrClass) override; // 构造自定义的扩展控件

	//窗口事件
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// 对外函数
	void CloseHeadModifyForm();
//	void UpdateUInfoCallback(const nbiz::UpdateUInfoParam& update_info);

private:
	static void DeleteFileCallback(const std::wstring& tmp_head_image_path);
	void LoadHeadImage();
	void DoLoadHeadImage();
	bool PreLoadHeadImage();	// 初始化并渲染图像(非DUI线程中执行)
	void PostLoadHeadImage();   // 显示已渲染好的头像(DUI线程执行)
	void BeginLoadAnimation();  // 开始加载动画
	void StopLoadAnimation();	// 停止加载动画

	void LoadCurrentHeadImageForPreview(); // 首次加载使用当前账号头像作为预览图
	void SaveCurrentHeadImageForPreview(); // 更换图片过程中以当前修改的头像作为预览图 
	void ShowPreviewWhileLoading();		   // 显示预览图

	RECT GetHeadSelectActiveRect();					// 获取矩形选区可以活动的范围
	bool SetHitTestCursor(int hit_test);			// 设置鼠标手型
	void AdjustHeadSelectCtrlOffset(POINT point_dest, POINT point_from, int hit_test);	// 移动头像选区
	void FixHeadSelectCtrlOffset(int &x, int &y);	// 修正头像选区移动时的偏移(不能超出图片显示区域)
	void FixPointOutOfBounds(POINT &point);			// 修正头像选区拉伸时的偏移
	void RenderPreviewPicture(RECT select_rect, bool use_original_image = false);
	void RenderPreviewPicture(RECT select_rect, RECT view_rect, bool use_original_image = false);
	
	void OnButtonSelectImage();
	void OnButtonSaveImage();
	void OnButtonSelectImageCallback(BOOL ret, std::wstring file_path);
	void OnButtonClose(bool forced_close = false);
	void OnButtonCloseTipCallback(MsgBoxRet ret);
	void NotifyHeadModify(std::wstring new_head_image_path, const std::string uid);
	void OnUploadHeadIconCallback(nim::NIMResCode res_code, const std::string& url);
	void OnNotifyHeadModifyCallback();
	void OnNotifyHeadModifyFailed();
	void OnNotifyImageInvalid();
	void UpdateUInfoCallback(int res);
public:
	static const LPCTSTR kClassName; // 类名
	
private:
	ui::Box *head_panel_;				// 头像浏览选择区
	HeadViewControl		 *head_view_ctrl_;		    // 图片浏览区
	HeadSelectControl	 *head_select_ctrl_;		// 头像选区
	HeadPreviewControl   *big_head_preview_ctrl_;   // 保存头像预览区
	HeadPreviewControl	 *small_head_preview_ctrl_; // 大头像预览区

	ui::Box *caption_container_;		// 标题区
	ui::Button    *select_image_btn_;		// 选择图片
	ui::Button	 *save_image_btn_;			// 生成头像
	ui::Button	 *cancel_btn_;				// 取消
	ui::Control	 *ani_gif_ctrl_;			// 转菊花动画
	ui::ButtonBox    *local_upload_btn_;		// 本地上传

	int head_view_default_width_;					// 图片浏览控件最大宽度
	int head_view_default_height_;					// 图片浏览控件最大高度
	int head_view_real_width_;						// 图片浏览控件实际宽度
	int head_view_real_height_;						// 图片浏览控件实际高度
	RECT head_view_padding_rect_;					// 图片浏览控件初始与外部容器间隔（居中用）
	int select_width_;								// 选区宽度
	int select_height_;								// 选区高度

	POINT point_off_;								// 记录鼠标变化之前的位置
	int delta_x_;									// 四个边角拉伸时，鼠标按下的坐标与BOX边缘的距离
	int delta_y_;									// 同上
	bool head_select_moving_;						// 判断是否能移动头像选区
	int  head_select_cursor_;					    // 当前变化头像选区的状态
	int  min_head_select_width_;				    // 最小的头像选区宽度
	int  min_head_select_height_;					// 最小的头像选区高度
	int  init_head_select_width_;					// 初始的头像选区的宽度
	int  init_head_select_height_;					// 初始的头像选区的高度
	std::wstring image_path_;						// 要显示的图片地址
	bool is_loading_image_;							// 是否在加载图像过程
	bool is_misc_thread_rendering_;					// MISC线程渲染过程中退出会崩溃
	bool is_quit_;									// 是否用户要退出

	UTF8String uid_;								// 窗口ID（UserID）
	std::wstring new_temp_head_image_path_;			// 新头像临时保存路径
	bool load_global_last_image_;					// 加载本次软件运行上次修改的图像
	std::unique_ptr<Gdiplus::Bitmap> current_head_bitmap_;	// 当前用户头像(200*200)
};
}
using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(nim_comp::HeadModifyForm);

#endif  // YIXIN_WIN_GUI_HEAD_MODIFY_H_
