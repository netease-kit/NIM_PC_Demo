#pragma once

/** @class MsglogManageForm
  * @brief 消息记录导入\导出窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/10/12
  */

enum MigrateMsglogOption	// 导入还是导出
{
	kExport,
	kImport
};

enum MigrateMsglogTarget	// 本地还是云储存
{
	kLocal,
	kRemote
};

class MsglogManageForm : public nim_comp::WindowEx
{
public:
	MsglogManageForm();
	~MsglogManageForm();
	
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 处理窗口销毁消息
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd);

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 设置窗口导入\导出类型
	* @param[in] export_or_import true:导出，false:导入
	* @return bool true 成功，false 失败
	*/
	bool SetType(nim::LogsBackupRemoteOperate option, MigrateMsglogTarget target);

	/**
	* 数据导入导出进度的回调
	* @param[in] operate 导入导出类型
	* @param[in] progress 进度
	* @return void	无返回值
	*/
	void OnMigrateMsglogProgressCallbackUI(nim::LogsBackupRemoteOperate operate, float progress);

	/**
	* 数据导入导出完成的回调
	* @param[in] operate 导入导出类型
	* @param[in] state 最终状态
	* @return void	无返回值
	*/
	void OnMigrateMsglogCompletedCallbackUI(nim::LogsBackupRemoteOperate operate, nim::LogsBackupRemoteState state);

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
	* 选择导出\导入路径
	* @return void	无返回值
	*/
	void SelectPath();

	/**
	* 响应路径被选择的回调函数
	* @param[in] ret 是否选择了路径
	* @param[in] file_path 路径值
	* @return void	无返回值
	*/
	void OnSelectPathCallback(BOOL ret, std::wstring file_path);

	/**
	* 执行导出操作
	* @param[in] path 导出路径
	* @return void	无返回值
	*/
	void Export(const std::string& path);

	/**
	* 导出到云端
	* @return bool	是否可以进行导出操作
	*/
	bool ExportToRemote();

	/**
	* 响应导出完成的回调函数
	* @param[in] res_code 导出结果错误码
	* @return void	无返回值
	*/
	void OnExportCompeleteCallback(nim::NIMResCode res_code);

	/**
	* 执行导入操作
	* @param[in] path 导入路径
	* @return void	无返回值
	*/
	void Import(const std::string& path);

	/**
	* 从云端导入
	* @return bool 是否可以进行导入操作
	*/
	bool ImportFromRemote();

	/**
	* 响应导入过程的回调函数
	* @param[in] imported_count 已导入数量
	* @param[in] total_count 总数
	* @return void	无返回值
	*/
	void OnImportProgressCallback(int64_t imported_count, int64_t total_count);

	/**
	* 响应导入完成的回调函数
	* @param[in] res_code 导入结果错误码
	* @return void	无返回值
	*/
	void OnImportCompeleteCallback(nim::NIMResCode res_code);

	/**
	* 延迟一定时间后关闭窗口
	* @return void	无返回值
	*/
	void DelayClose();

	/**
	* 设置是否正在执行导入\导出操作
	* @param[in] running true:正在执行，false没有执行
	* @return void	无返回值
	*/
	void SetDbStatus(bool running);

public:
	static const LPCTSTR kClassName;

private:
	nbase::WeakCallbackFlag close_timer_;

	nim::LogsBackupRemoteOperate export_or_import_;
	bool db_running_;
	bool open_file_;

	ui::Label*		tip_text_;
	ui::Button*		btn_sel_;
	ui::Button*		btn_run_;
	ui::Button*		btn_cancel_;
	ui::RichEdit*	path_edit_;

	ui::Progress*	progress_;
	ui::Label*		result_text_;
	ui::Label*		progress_text_;
	ui::Box*		path_box_;
	ui::Box*		prg_box_;
	ui::Button*		btn_close_;
};
