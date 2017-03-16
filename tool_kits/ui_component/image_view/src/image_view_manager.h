#ifndef IMAGE_VIEW_MANAGER_H__
#define IMAGE_VIEW_MANAGER_H__

#include "image_view_form.h"


class ImageViewManager
{
public:
	SINGLETON_DEFINE(ImageViewManager);

	struct ImageViewInfo
	{
		std::wstring id_;
		std::wstring url_;
		std::wstring path_;
	};
	typedef std::vector<ImageViewInfo> ImageViewInfoList;

	ImageViewManager();
	~ImageViewManager();

	bool StartViewPic(const std::wstring &path_pic, std::wstring message_id, bool size, bool nosave = false);
	//判断form_id是否与图片预览窗体的类名一致
	bool JudgeViewImage(const std::wstring &form_id);
	//图片预览窗体关闭时，用来重置指向图片预览窗体的指针为NULL
	void ResetImageViewWndPoint(void);

	bool SetImageViewList(ImageViewInfoList info_list, std::wstring message_id);

	//显示下一张/上一张图片
	bool ShowNextImageView(bool next);
	//判断是否存在下一张/上一张图片
	bool IsExistNextImage(bool next);
	void GetImagePage(long& cur, long& sum);

	//图片准备好了
	void OnImageReady(std::wstring &path_pic, bool size);

private:
	ImageViewForm *image_view_window_;
	std::wstring   message_id_;

	ImageViewInfoList image_view_info_list_;
};
#endif // IMAGE_VIEW_MANAGER_H__