#ifndef _UIBITMAPCONTROL_G2_H_
#define _UIBITMAPCONTROL_G2_H_
#pragma once

namespace ui
{
class BitmapControlG2 : public ui::Box
{
public:
	/**
	* 构造函数
	* @param[in] video_frame_mng 视频帧管理器
	* @return void	无返回值
	*/
	BitmapControlG2(Window* parent);
	~BitmapControlG2(void);
	void SetAutoSize(bool auto_size);
	bool ScaleVideoFrame(char* out_data, int& width, int& height);
	void EnableCaptureVideoFrame(bool enable);
	bool IsEnableCaptureVideoFrame();
	void Clear();
	/**
	* 重写父类绘制函数
	* @param[in] hDC 目标DC
	* @param[in] rcPaint 可绘制区域
	* @return void	无返回值
	*/
	void Paint(IRenderContext* pRender, const UiRect& rcPaint) override;
	bool Refresh();
	void CaptureVideoFrame(uint64_t uid,   /**< uid */
		void* data,         /**< 数据指针 */
		uint32_t type,      /**< NERtcVideoType */
		uint32_t width,     /**< 宽度 */
		uint32_t height,    /**< 高度 */
		uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
		uint32_t offset[4], /**< 每类数据偏移 */
		uint32_t stride[4], /**< 每类数据步进 */
		uint32_t rotation,  /**< NERtcVideoRotation */
		void* user_data     /**< 用户透传数据 */);
	/**
	* 获取图像宽高比
	* @return float 比例
	*/
	float GetPicWHPercent() { return scaled_height_ > 0 ? scaled_width_*1.0f / scaled_height_ : 0; }

protected:
	nbase::NLock	camera_data_lock_;
	int				camera_width_;
	int				camera_height_;
	std::string		camera_data_;

	std::string		scaled_data_;
	int				scaled_width_;
	int				scaled_height_;
	bool			auto_size_;
	bool			enable_capture_;
	Window* parent_wnd_;
};

}

#endif // _UIBITMAPCONTROL_H_
