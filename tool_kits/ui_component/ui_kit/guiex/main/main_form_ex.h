#pragma once
#include "module/plugins/main_plugins_base.h"
#include "module/tray_icon/tray_icon_manager.h"
namespace nim_comp
{
	class IMainPlugin;
	class IMainFormMenuHandler;
	class MainFormEx : public WindowEx, public ITrayIconEventHandler
	{
	public:
		MainFormEx(IMainFormMenuHandler* main_menu_handler);
		virtual ~MainFormEx();
	public:
		//覆盖虚函数
		virtual std::wstring GetSkinFolder() override;
		virtual std::wstring GetSkinFile() override;
		virtual std::wstring GetWindowClassName() const override;
		virtual std::wstring GetWindowId() const override;	

		/**
		* 窗口初始化函数
		* @return void	无返回值
		*/
		virtual void InitWindow() override;
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		/**
		* 处理窗口销毁消息
		* @return void	无返回值
		*/
		virtual void OnFinalMessage(HWND hWnd);

		/**
		* 拦截并处理WM_CLOSE消息
		* @param[in] wParam 附加参数
		* @param[in] lParam 附加参数
		* @param[in] lParam 附加参数
		* @param[in out] bHandled 是否处理了消息，如果处理了不继续传递消息
		* @return void	无返回值
		*/
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		/**
		* 拦截ESC键的消息处理
		* @param[in out] bHandled 是否处理了消息，如果处理了不继续传递消息
		* @return void	无返回值
		*/
		virtual void OnEsc(BOOL &bHandled);
	protected:
		//托盘
		virtual bool OnLeftClick() override;
		virtual bool OnLeftDoubleClick() override;
		virtual bool OnRightClick() override;
	private:
		void LoadPlugins();
	private:
		/**
		* 处理所有控件的所有消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool Notify(ui::EventArgs* msg);

		/**
		* 处理所有控件单击消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnClicked(ui::EventArgs* msg);
		/**
		* 响应个人资料改变的回调函数
		* @param[in] uinfos 新的个人资料列表
		* @return void 无返回值
		*/
		void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

		/**
		* 响应头像下载完成的回调函数
		* @param[in] type 头像类型
		* @param[in] account 头像下载完成的用户id
		* @param[in] photo_path 头像本地路径
		* @return void 无返回值
		*/
		void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);		

		/**
		* 初始化用户头像
		* @return void	无返回值
		*/
		void InitHeader();

		bool OnReturnEventsClick(ui::EventArgs* param);
		/**
		* 弹出托盘菜单
		* @param[in] point 鼠标位置
		* @return void
		*/
		void PopupTrayMenu(POINT point);
		/**
		* 在线状态菜单按钮的单击消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnlineStateMenuButtonClick(ui::EventArgs* param);
		/**
		* 在线状态菜单项的单击消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnlineStateMenuItemClick(ui::EventArgs* param);
		/**
		* 设置在线状态
		* @return void	无返回值
		*/
		void SetOnlineState();
		/**
		* 处理主窗口弹出菜单按钮的单击消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool MainMenuButtonClick(ui::EventArgs* param);
		/**
		* 初始化搜索栏		
		* @return void
		*/
		void InitSearchBar();
		/**
		* 处理搜索栏内容改变的消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool SearchEditChange(ui::EventArgs* param);

		/**
		* 处理清理搜索栏按钮的单击消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnClearInputBtnClicked(ui::EventArgs* param);
		void OnWndSizeMax(bool max);
	public:
		static const LPCTSTR kClassName;
	private:
		bool is_trayicon_left_double_clicked_;
		bool			is_busy_;
		ui::VBox* main_bar_;
		ui::VBox* simple_plugin_bar_;
		ui::VBox* main_plugins_bar_;
		ui::TabBox* main_plugins_showpage_;
		ui::Button*		btn_header_;
		ui::Button*		btn_online_state_;
		ui::Label*		label_name_;
		ui::RichEdit*	search_edit_;
		ui::Button*		btn_clear_input_;
		ui::ListBox*	search_result_list_;
		ui::ButtonBox* search_bar_;
		ui::Button* btn_max_restore_;
		AutoUnregister	unregister_cb;
	private:
		IMainFormMenuHandler* main_menu_handler_;
	};
}