#pragma once

interface IDropTargetHelper;
namespace nim_comp
{

/** @class SessionForm
  * @brief 会话窗体,会话盒子的容器.如果开启会话合并功能，则一个会话窗体包含多个会话盒子，否则只包含一个
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/8/28
  */
class SessionBox;
class MergeItem;
class SessionForm : public WindowEx, public IDropTarget
{
public:
	friend class SessionBox;
	SessionForm();
	~SessionForm();
	
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return LRESULT 处理结果
	*/
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	/**
	* 拦截并处理WM_CLOSE消息
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return void	无返回值
	*/
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	/**
	* 处理窗口销毁消息
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd) override;	

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 处理所有控件的所有消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnNotify(ui::EventArgs* param);

	/**
	* 处理所有控件单击消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* param);

public:
	/**
	* 在本窗口内创建一个新会话盒子
	* @param[in] session_id 会话id
	* @param[in] session_type 会话类型
	* @return SessionBox* 会话盒子
	*/
	SessionBox* CreateSessionBox(const std::string &session_id, nim::NIMSessionType session_type);

	/**
	* 关闭本窗口内的一个会话盒子
	* @param[in] session_id 会话id
	* @return void	无返回值
	*/
	void CloseSessionBox(const std::string &session_id);

	/**
	* 把一个其他窗口内的会话盒子附加到本窗口内
	* @param[in] session_box 会话盒子
	* @return bool true 成功，false 失败
	*/
	bool AttachSessionBox(SessionBox *session_box);

	/**
	* 把本窗口内的一个会话盒子脱离窗口
	* @param[in] session_box 会话盒子
	* @return bool true 成功，false 失败
	*/
	bool DetachSessionBox(SessionBox *session_box);

	/**
	* 获取当前显示的会话盒子
	* @return SessionBox* 会话盒子
	*/
	SessionBox* GetSelectedSessionBox();

	/**
	* 激活并切换到某个会话盒子(同时让会话窗体激活)
	* @param[in] session_id 会话id
	* @return void 无返回值
	*/
	void SetActiveSessionBox(const std::string &session_id);

	/**
	* 判断会话盒子是否处于激活状态(同时判断会话窗体是否被激活)
	* @param[in] session_box 会话盒子
	* @return bool true 是，false 否
	*/
	bool IsActiveSessionBox(const SessionBox *session_box);

	/**
	* 判断会话盒子是否处于激活状态(同时判断会话窗体是否被激活)
	* @param[in] session_id 会话id
	* @return bool true 是，false 否
	*/
	bool IsActiveSessionBox(const std::wstring &session_id);

	/**
	* 获取本窗口内会话盒子的总量
	* @return int	总量
	*/
	int GetSessionBoxCount() const;

	/**
	* 在执行拖拽操作前，如果被拖拽的会话盒子属于本窗口，则通知本窗口
	* @param[in] session_id 会话id
	* @return void	无返回值
	*/
	void OnBeforeDargSessionBoxCallback(const std::wstring &session_id);

	/**
	* 在执行拖拽操作后，如果被拖拽的会话盒子属于本窗口，则通知本窗口操作结果
	* @param[in] drop_succeed 会话盒子是否被拖拽到了外部
	* @return void	无返回值
	*/
	void OnAfterDragSessionBoxCallback(bool drop_succeed);

	/**
	* 设置会话合并列表中某个合并项未读消息数
	* @param[in] id 会话id
	* @param[in] unread 未读消息数
	* @return void 无返回值
	*/
	void InvokeSetSessionUnread(const std::string &id, int unread);

private:

	/**
	* 处理合并列表项控件的选中消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMergeItemSelected(ui::EventArgs* param);

	/**
	* 处理合并列表项控件的关闭按钮的单击消息
	* @param[in] param 消息的相关信息
	* @param[in] session_id 列表项对应的会话id
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMergeItemClose(ui::EventArgs* param, const std::string& session_id);

	/**
	* 在本窗口内查找会话盒子
	* @param[in] session_id 会话id
	* @return SessionBox* 会话盒子
	*/
	SessionBox* FindSessionBox(const std::wstring &session_id);

	/**
	* 在本窗口内查找合并列表项
	* @param[in] session_id 会话id
	* @return SessionBox* 会话盒子
	*/
	MergeItem* FindMergeItem(const std::wstring &session_id);

	/**
	* 切换某个会话盒子为显示显示状态
	* @param[in] session_id 会话id
	* @return bool true 成功，false 失败
	*/
	bool ChangeToSessionBox(const std::wstring &session_id);

	/**
	* 调整会话窗体的大小
	* @return void 无返回值
	*/
	void AdjustFormSize();

private:
	/**
	* 设置某个会话对应的左侧合并列表项的标题
	* @param[in] session_id 会话id
	* @param[in] name 标题
	* @return void	无返回值
	*/
	void SetMergeItemName(const std::wstring &session_id, const std::wstring &name);

	/**
	* 设置某个会话对应的左侧合并列表项的图标
	* @param[in] session_id 会话id
	* @param[in] photo 图标路径
	* @return void	无返回值
	*/
	void SetMergeItemHeaderPhoto(const std::wstring &session_id, const std::wstring &photo);

	/**
	* 收到新消息时,改变窗体状态来通知用户
	* @param[in] create	是否刚创建会话窗体
	* @param[in] flash	是否需要让任务栏图标闪烁
	* @return void 无返回值
	*/
	void OnNewMsg(bool create, bool flash);

	/**
	* 在任务栏闪动
	* @return void	无返回值
	*/
	void FlashTaskbar();

	/**
	* 更新当前窗口任务显示的图标
	* @param[in] icon 图标路径
	* @return void	无返回值
	*/
	void SetTaskbarIcon(const std::wstring &icon);

	/**
	* 判断是否为RichEdit中超链接的单击事件
	* @param[in]
	* @return bool true 处理了单击事件，false 没有处理
	*/
	bool CheckRichEditLink(WPARAM wParam, LPARAM lParam);

public:

	/**
	* 初始化窗口拖放功能
	* @return void	无返回值
	*/
	bool InitDragDrop();

	/**
	* 反初始化窗口拖放功能
	* @return void	无返回值
	*/
	void UnInitDragDrop();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);
	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);	
	HRESULT STDMETHODCALLTYPE DragLeave(void);
	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);

private:
	/**
	* 判断是否要拖拽会话盒子
	* @param[in] param 处理会话盒子的头像控件发送的事件
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnProcessSessionBoxHeaderDrag(ui::EventArgs* param);

	/**
	* 判断是否要拖拽会话盒子
	* @param[in] param 处理会话窗口左侧会话合并列表项发送的事件
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnProcessMergeItemDrag(ui::EventArgs* param);

	/**
	* 生成当前窗体中某个区域对应的位图
	* @param[in] src_rect 目标位图的位置
	* @return HBITMAP 生成的位图
	*/
	HBITMAP GenerateSessionBoxBitmap(const ui::UiRect &src_rect);

public:
	static const LPCTSTR kClassName;

private:
	HICON				icon_handle_;
	
	ui::ListBox			*merge_list_;
	ui::TabBox			*session_box_tab_;
	SessionBox			*active_session_box_;

	// 处理会话盒子拖放事件
	IDropTargetHelper	*drop_helper_;

	// 处理会话盒子拖拽事件
	bool				is_drag_state_;
	POINT				old_drag_point_;
	std::wstring		draging_session_id_;
};


}
