#pragma once

namespace nim_chatroom
{

class ChatroomFrontpage : public nim_comp::WindowEx
{
public:
	ChatroomFrontpage();
	~ChatroomFrontpage();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;


	virtual void InitWindow() override;

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	bool Notify(ui::EventArgs* msg);
	bool OnClicked(ui::EventArgs* msg);

	ChatRoomInfo GetRoomInfo(const  __int64& room_id);

	void SetAnonymity(bool anonymity);
	bool IsAnonymity(){ return anonymity_; }

private:
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);
	void InitHeader();

private:
	void InvokeGetRoomList();
	void CreateRoomItem(const ChatRoomInfo& room_info);
	void OnWndSizeMax(bool max);

	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

public:
	static const LPTSTR kClassName;

private:
	ui::TileBox*	room_list_ = NULL;
	
	std::map<__int64, ChatRoomInfo> id_info_map_;
	AutoUnregister	unregister_cb;

	bool anonymity_ = false;
};
}