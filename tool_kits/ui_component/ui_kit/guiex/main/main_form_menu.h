#pragma once
namespace nim_comp
{
	class IMainFormMenuHandler
	{
	public:
		IMainFormMenuHandler() : host_window_(nullptr){}
		virtual ~IMainFormMenuHandler(){};
		/**
		* 弹出主菜单
		* @param[in] point 弹出坐标
		* @return void	无返回值
		*/
		virtual void PopupMainMenu(POINT point)
		{
			if (host_window_ == nullptr)
				return;
			OnPopupMainMenu(point);
		}
		virtual void SetHostWindow(WindowEx* host_windows) { host_window_ = host_windows; }
	protected:
		virtual void OnPopupMainMenu(POINT point) = 0;
	protected:
		WindowEx* host_window_;
	};
	
}