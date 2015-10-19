#pragma once


class TrayIconDelegate
{
public:
	virtual void LeftClick(){}
	virtual void LeftDoubleClick(){}
	virtual void RightClick(){}
	virtual void RightDoubleClick(){}
};

class TrayIcon
{
public:
	SINGLETON_DEFINE(TrayIcon);
	TrayIcon();
	virtual ~TrayIcon();
public:
	void Init(TrayIconDelegate* tray_icon_delegate);
	void Destroy();
	void SetTrayIcon(HICON icon, const std::wstring& tray_icon_text);
	void SetImageListTray(int start_icon, int count);
	void StartTrayIconAnimate();
	void StopTrayIconAnimate();
	void SetAnimateTray(const std::vector<int>& aniIndex, int anim_escape_time);

	BOOL RestoreTrayIcon();
	UINT GetTaskbarCreatedMsgId(){return trayicon_msgid_;}

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SetTrayIconIndex(int index);
	void OnTimer(UINT event_id);
	
	void ClearAnimateInfo()
	{
		anim_index_array_.clear();
		anim_current_index_ = 0;
		anim_escape_time_ = 0;
	};

	void ModifyTrayIcon(HWND hWnd, UINT uTrayID, DWORD dwTrayMsg, HICON hTrayIcon, const std::wstring& strTrayText);

private:
	HWND wnd_;
	std::vector<int> anim_index_array_;
	std::map<int, HICON> index_icon_map_;
	int anim_escape_time_;
	int anim_current_index_;
	TrayIconDelegate* tray_icon_delegate_;
	HICON icon_;
	std::wstring tray_icon_text_;
	UINT trayicon_msgid_;
};
