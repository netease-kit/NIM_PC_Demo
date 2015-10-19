#pragma once

class MsglogManageForm : public WindowEx
{
public:
	MsglogManageForm();
	~MsglogManageForm();

	static MsglogManageForm* ShowForm(bool export_or_import);
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* param);
	virtual bool OnClicked(ui::EventArgs* param);

private:
	bool SetType(bool export_or_import);
	void SelPath();
	void SelPathCallback(BOOL ret, std::wstring file_path);

	void Export(const std::string& path);
	void ExportResult(nim::NIMResCode res_code, const std::string& msg_id);
	void Import(const std::string& path);
	void ImportProgress(int64_t imported_count, int64_t total_count);
	void ImportResult(nim::NIMResCode res_code, const std::string& msg_id);

	void DelayClose();
	void DoDelayClose();

	void SetDbStatus(bool running);

public:
	static const LPCTSTR kClassName;

private:
	nbase::WeakCallbackFlag close_timer_;

	bool export_or_import_;
	bool db_running_;
	bool open_file_;

	ui::Label* tip_text_;
	ui::Button* btn_sel_;
	ui::Button* btn_run_;
	ui::RichEdit* path_edit_;

	ui::Progress* progress_;
	ui::Label* result_text_;
	ui::Label* progress_text_;
	ui::Box* path_box_;
	ui::Box* prg_box_;

};
