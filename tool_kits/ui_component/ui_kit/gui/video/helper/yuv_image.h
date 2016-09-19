#pragma once

namespace nim_comp
{
enum ImageYuvDataType
{
	kYuvDataTypeNormal = 0,
	kYuvDataTypeImage,
	kYuvDataTypeImageAlpha,
};
struct ImageFrame {
	std::string data_;
	int height_;
	int width_;
	int time_;
	bool alpha_;
	int index_;
	ImageFrame()
	{
		height_ = 0;
		width_ = 0;
		time_ = 100;
		alpha_ = false;
		index_ = 0;
	}
};
class YuvImage
{
public:
	YuvImage();
	~YuvImage();
	bool LoadImage(std::wstring path);
	bool AddText(const std::wstring& text, int width, int height, DWORD color, int iFont, UINT uStyle, BYTE uFade, bool bLineLimit);
	ImageFrame* GetImageFrame(int index, int width = -1, int height = -1);
	ImageFrame* GetImageFrameEx(int64_t time, int width = -1, int height = -1);

private:
	bool CheckImageFrame(int index, ImageFrame* image_frame, int width, int height);
	bool CopyImageData(const char* src, int byte_width, std::string& dest, int width, int height);
	bool DrawText(int index, int width, int height, DWORD color, int iFont, UINT uStyle, BYTE uFade, bool bLineLimit);
private:
	std::vector<ImageFrame> image_list_;
	std::shared_ptr<Gdiplus::Bitmap> src_image_;
	int time_all_;
	std::wstring image_path_;
	std::wstring text_;
	int64_t time_start_;
};
void YUV420ToARGB(char* src, char* dst, int width, int height);
void ARGBToYUV420(char* src, char* dst, int width, int height);

}