#ifndef UI_IMAGE_VIEW_H__
#define UI_IMAGE_VIEW_H__


class UiImageView : public ui::Control
{
public:
	enum PicDirectionOfRotation
	{
		kClockWise = 0,		//顺时针
		kAntiClockWise		//逆时针
	};
	enum PicZoom
	{
		kMagnifyPic = 0,	//放大
		kReducePic			//缩小
	};

	UiImageView();
	~UiImageView(void);

public:
	virtual void DoInit() override;
	virtual void HandleMessage(ui::EventArgs& event) override;
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override;
	

	//外部接口
public:
	//旋转图片
	void RotatePic(PicDirectionOfRotation direction);
	//缩放图片
	void ScalePic(PicZoom zoom);
	//设置要显示的图片路径
	int SetPicPath(std::wstring image_path);

	UINT GetImageWidth();
	UINT GetImageHeight();

	//判断鼠标是否在图片内以及图片是否可以被拖动
	bool PtInImageView(POINT point); 
	
	//重置图片为原始尺寸
	void SetOriginalSize();

	/*
	*	@brief 根据传入的预设控件宽高和图片宽高，计算将要显示的图片的缩放比
	*	@param control_width:预设的控件宽度
	*	@param control_height:预设的控件高度
	*	@param image_width :预设的图片宽度
	*	@param image_height :预设的图片高度
	*/
	float GetDefaultScale(int control_width, int control_height, int image_width, int image_height);

	//设置显示的图片的偏移量，用于用户拖动图片
	void SetPicOffset(int offset_x, int offset_y);

	bool IsAdjustScale(void){ return bAdjustScale_; };
	int GetAngle(void){ return angle_; }
	float GetScale(void){ return scale_; }
	static LPCTSTR kControlName;

private:
	void SetAngle(int angle);
	void SetScale(float scale);

	//生成一个旋转后的位图
	HBITMAP GetRotatedBitmap(HDC hDC, HBITMAP hBitmap, float radians, COLORREF clrBack);

	//获取图片的旋转方向
	static Gdiplus::RotateFlipType GetRotateFlipType(Gdiplus::Image& image);
	static Gdiplus::RotateFlipType GetRotateFlipType(short direction);

	//修正偏移量，根据控件的大小和显示的图片的大小，修正传入的偏移量
	void CheckOffset(int& nOffset, int nControl, int nImage);

private:

	int angle_;							//图片的旋转角度
	float scale_;						//图片缩放比例
	std::wstring image_path_;			//图片本地地址

	std::unique_ptr<Gdiplus::Image> image_;//存放图片信息
	std::unique_ptr<Gdiplus::Bitmap> image_transform_;//存放处理后的位图
	
	RECT rect_image_;					//图片大小
	bool can_moving_;					//图片是否可以移动
	int offset_x_;						//图片绘制的横向偏移量
	int offset_y_;						//图片绘制的纵向偏移量


	bool bAdjustScale_;

};
#endif // UI_IMAGE_VIEW_H__