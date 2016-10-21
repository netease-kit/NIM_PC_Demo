#pragma once
#include "ui_head_view_ctrl.h"
#include "ui_head_select_ctrl.h"
#include "shared/ui/msgbox.h"

namespace nim_comp
{
typedef std::function<void(const std::string& id, const std::string &url)> OnModifyCompleteCallback;

/** @class HeadModifyForm
  * @brief 
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author hj
  * @date 2013/12/05
  */
class HeadModifyForm : public WindowEx
{
public:
	HeadModifyForm(UTF8String uid, const std::wstring& specific_cached_file = L"");
	virtual ~HeadModifyForm();
	
	// 覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

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
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool Notify(ui::EventArgs* msg);

	/**
	* 根据控件类名创建自定义控件
	* @param[in] pstrClass 控件类名
	* @return Control* 创建的控件的指针
	*/
	virtual	ui::Control* CreateControl(const std::wstring& pstrClass) override;

	/**
	* 拦截并处理鼠标左键按下的消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[out] bHandled true 不继续传递消息，false 继续传递消息
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/**
	* 拦截并处理鼠标左键松开的消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[out] bHandled true 不继续传递消息，false 继续传递消息
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/**
	* 拦截并处理鼠标移动的消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[out] bHandled true 不继续传递消息，false 继续传递消息
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/**
	* 关闭头像修改窗口
	* @return void	无返回值
	*/
	void CloseHeadModifyForm();

	/**
	* 注册头像修改完成的回调函数
	* @param[in] callback 回调函数
	* @return void	无返回值
	*/
	void RegCompleteCallback(const OnModifyCompleteCallback &callback);

private:
	/**
	* 删除临时头像文件的回调函数
	* @param[in] tmp_head_image_path 零时文件路径
	* @return void	无返回值
	*/
	static void DeleteFileCallback(const std::wstring& tmp_head_image_path);

	/**
	* 请求开始加载头像文件
	* @return void	无返回值
	*/
	void LoadHeadImage();

	/**
	* 开始加载头像文件
	* @return void	无返回值
	*/
	void DoLoadHeadImage();

	/**
	* 初始化并渲染图像(非DUI线程中执行)
	* @return bool true 渲染成功，false 渲染失败
	*/
	bool PreLoadHeadImage();

	/**
	* 显示已渲染好的头像(DUI线程执行)
	* @return void	无返回值
	*/
	void PostLoadHeadImage();

	/**
	* 显示开始加载动画
	* @return void	无返回值
	*/
	void BeginLoadAnimation();

	/**
	* 显示停止加载动画
	* @return void	无返回值
	*/
	void StopLoadAnimation();

	/**
	* 首次加载使用当前帐号头像作为预览图
	* @return void	无返回值
	*/
	void LoadCurrentHeadImageForPreview();

	/**
	* 更换图片过程中以当前修改的头像作为预览图
	* @return void	无返回值
	*/
	void SaveCurrentHeadImageForPreview();

	/**
	* 显示预览图
	* @return void	无返回值
	*/
	void ShowPreviewWhileLoading();

	/**
	* 获取矩形选区可以活动的范围
	* @return RECT	可以活动的范围
	*/
	RECT GetHeadSelectActiveRect();

	/**
	* 设置鼠标手型
	* @param[in] hit_test 命中测试类型
	* @return void	无返回值
	*/
	bool SetHitTestCursor(int hit_test);

	/**
	* 移动头像选区
	* @param[in] point_dest 当前鼠标的坐标
	* @param[in] point_from 旧的鼠标坐标
	* @param[in] hit_test 命中测试
	* @return void	无返回值
	*/
	void AdjustHeadSelectCtrlOffset(POINT point_dest, POINT point_from, int hit_test);

	/**
	*  修正头像选区移动时的偏移(不能超出图片显示区域)
	* @param[in] x 要修正的横坐标
	* @param[in] y 要修正的纵坐标
	* @return void	无返回值
	*/
	void FixHeadSelectCtrlOffset(int &x, int &y);

	/**
	* 修正头像选区拉伸时的偏移
	* @param[in] point 鼠标坐标
	* @return void	无返回值
	*/
	void FixPointOutOfBounds(POINT &point);

	/**
	* 渲染预览图
	* @param[in] select_rect 当前选中的位置
	* @param[in] use_original_image 是否使用原图来渲染
	* @return void	无返回值
	*/
	void RenderPreviewPicture(RECT select_rect, bool use_original_image = false);

	/**
	* 渲染预览图
	* @param[in] select_rect 当前选中的位置
	* @param[in] view_rect 头像操作区域的位置
	* @param[in] use_original_image 是否使用原图来渲染
	* @return void	无返回值
	*/
	void RenderPreviewPicture(RECT select_rect, RECT view_rect, bool use_original_image = false);
	
	/**
	* 响应选择图片按钮单击消息
	* @return void	无返回值
	*/
	void OnButtonSelectImage();

	/**
	* 响应保存图片按钮单击消息
	* @return void	无返回值
	*/
	void OnButtonSaveImage();

	/**
	* 响应选择图片的回调函数
	* @param[in] ret 是否选择了图片
	* @param[in] file_path 选择的图片的路径
	* @return void	无返回值
	*/
	void OnButtonSelectImageCallback(BOOL ret, std::wstring file_path);

	/**
	* 响应关闭窗口按钮单击消息
	* @param[in] forced_close 是否强制关闭
	* @return void	无返回值
	*/
	void OnButtonClose(bool forced_close = false);

	/**
	* 响应关闭窗口提示的回调函数
	* @param[in] ret 提示窗的返回值
	* @return void	无返回值
	*/
	void OnButtonCloseTipCallback(MsgBoxRet ret);

	/**
	* 把新的头像文件上传到服务器
	* @param[in] new_head_image_path 头像路径
	* @return void	无返回值
	*/
	void NotifyHeadModify(std::wstring new_head_image_path);

	/**
	* 响应上传头像文件的回调函数
	* @param[in] res_code 错误码
	* @param[in] url 上传文件的url
	* @return void	无返回值
	*/
	void OnUploadHeadIconCallback(int res_code, const std::string& url);

	/**
	* 响应上传头像成功的回调函数
	* @return void	无返回值
	*/
	void OnNotifyHeadModifyCallback();

	/**
	* 响应上传头像失败的回调函数
	* @return void	无返回值
	*/
	void OnNotifyHeadModifyFailed();

	/**
	* 图片无效或者DUILIB图片引擎Read图片失败
	* @return void	无返回值
	*/
	void OnNotifyImageInvalid();

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

	OnModifyCompleteCallback complete_callback_;
	std::wstring	specific_cached_file_;
};
}
using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(nim_comp::HeadModifyForm);