#pragma once
#include "module/runtime_data/runtime_data_manager.h"
namespace nim_comp
{
	class SessionBox;
	class ISessionDock
	{
	public:
		static ISessionDock* InstantDock();
	public:
		virtual HWND Create() = 0;
		virtual void CenterWindow() = 0;
		virtual HWND GetHWND() = 0;
		virtual ui::UiRect GetPos(bool bContainShadow = false) const = 0;
		virtual void SetPos(const ui::UiRect& rc, bool bNeedDpiScale, UINT uFlags, HWND hWndInsertAfter = NULL, bool bContainShadow = false) = 0;
		virtual void ActiveWindow() = 0;
		virtual LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L) = 0;
		/**
		* 设置窗口标题
		* @param[in] title 窗口标题
		* @return void 无返回值
		*/
		virtual void SetTaskbarTitle(const std::wstring &title) = 0;
		/**
		* 更新当前窗口任务显示的图标
		* @param[in] icon 图标路径
		* @return void	无返回值
		*/
		virtual void SetTaskbarIcon(const std::wstring &icon) = 0;
		/**
		* 在本窗口内创建一个新会话盒子
		* @param[in] session_id 会话id
		* @param[in] session_type 会话类型
		* @return SessionBox* 会话盒子
		*/
		virtual SessionBox* CreateSessionBox(const std::string &session_id, nim::NIMSessionType session_type) = 0;

		/**
		* 关闭本窗口内的一个会话盒子
		* @param[in] session_id 会话id
		* @return void	无返回值
		*/
		virtual void CloseSessionBox(const std::string &session_id) = 0;

		/**
		* 把一个其他窗口内的会话盒子附加到本窗口内
		* @param[in] session_box 会话盒子
		* @return bool true 成功，false 失败
		*/
		virtual bool AttachSessionBox(SessionBox *session_box) = 0;

		/**
		* 把本窗口内的一个会话盒子脱离窗口
		* @param[in] session_box 会话盒子
		* @return bool true 成功，false 失败
		*/
		virtual bool DetachSessionBox(SessionBox *session_box) = 0;

		/**
		* 获取当前显示的会话盒子
		* @return SessionBox* 会话盒子
		*/
		virtual SessionBox* GetSelectedSessionBox() = 0;

		/**
		* 激活并切换到某个会话盒子(同时让会话窗体激活)
		* @param[in] session_id 会话id
		* @return void 无返回值
		*/
		virtual void SetActiveSessionBox(const std::string &session_id) = 0;

		/**
		* 判断会话盒子是否处于激活状态(同时判断会话窗体是否被激活)
		* @param[in] session_box 会话盒子
		* @return bool true 是，false 否
		*/
		virtual bool IsActiveSessionBox(const SessionBox *session_box) = 0;

		/**
		* 判断会话盒子是否处于激活状态(同时判断会话窗体是否被激活)
		* @param[in] session_id 会话id
		* @return bool true 是，false 否
		*/
		virtual bool IsActiveSessionBox(const std::wstring &session_id) = 0;

		/**
		* 获取本窗口内会话盒子的总量
		* @return int	总量
		*/
		virtual int GetSessionBoxCount() const = 0;

		/**
		* 在执行拖拽操作前，如果被拖拽的会话盒子属于本窗口，则通知本窗口
		* @param[in] session_id 会话id
		* @return void	无返回值
		*/
		virtual void OnBeforeDragSessionBoxCallback(const std::wstring &session_id) = 0;

		/**
		* 在执行拖拽操作后，如果被拖拽的会话盒子属于本窗口，则通知本窗口操作结果
		* @param[in] drop_succeed 会话盒子是否被拖拽到了外部
		* @return void	无返回值
		*/
		virtual void OnAfterDragSessionBoxCallback(bool drop_succeed) = 0;

		/**
		* 设置会话合并列表中某个合并项未读消息数
		* @param[in] id 会话id
		* @param[in] unread 未读消息数
		* @return void 无返回值
		*/
		virtual void InvokeSetSessionUnread(const std::string &id, int unread) = 0;
		/**
		* 设置某个会话对应的左侧合并列表项的标题
		* @param[in] session_id 会话id
		* @param[in] name 标题
		* @return void	无返回值
		*/
		virtual void SetMergeItemName(const std::wstring &session_id, const std::wstring &name) = 0;

		/**
		* 设置某个会话对应的左侧合并列表项的图标
		* @param[in] session_id 会话id
		* @param[in] photo 图标路径
		* @return void	无返回值
		*/
		virtual void SetMergeItemHeaderPhoto(const std::wstring &session_id, const std::wstring &photo) = 0;

		/**
		* 收到新消息时,改变窗体状态来通知用户
		* @param[in] session_box 收到消息的会话盒子
		* @param[in] create	是否刚创建会话窗体
		* @param[in] flash	是否需要让任务栏图标闪烁
		* @return void 无返回值
		*/
		virtual void OnNewMsg(SessionBox &session_box, bool create, bool flash) = 0;
		/**
		* 调整会话窗体的大小
		* @return void 无返回值
		*/
		virtual void AdjustFormSize() = 0;
		/**
		* 拦截并处理底层窗体消息
		* @param[in] uMsg 消息类型
		* @param[in] wParam 附加参数
		* @param[in] lParam 附加参数
		* @param[in out] bHandled 是否处理了消息，如果处理了不继续传递消息
		* @return LRESULT 处理结果
		*/
		virtual LRESULT HostWindowHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		/**
		* 拦截并处理底层窗体消息
		* @param[in] uMsg 消息类型
		* @param[in] wParam 附加参数
		* @param[in] lParam 附加参数
		* @param[in out] bHandled 是否处理了消息，如果处理了不继续传递消息
		* @return LRESULT 处理结果
		*/
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	};
}