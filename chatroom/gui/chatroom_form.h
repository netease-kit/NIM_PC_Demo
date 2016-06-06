#pragma once

#include "module/emoji/richedit_util.h"
#include "module/emoji/richedit_olecallback.h"
#include <time.h>

namespace nim_chatroom
{
class ChatroomForm : public nim_comp::WindowEx, public IDropTarget
{
public:
	
	ChatroomForm(__int64 room_id);
	~ChatroomForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;

	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void OnWndSizeMax(bool max);
	virtual bool Notify(ui::EventArgs* msg);
	virtual bool OnClicked(ui::EventArgs* msg);

public:
	void RequestEnter(const __int64 room_id);
	__int64 GetRoomId();
	void OnReceiveMsgCallback(const ChatRoomMessage& result);
	void OnEnterCallback(int error_code, const ChatRoomInfo& info, const ChatRoomMemberInfo& my_info);
	void OnGetChatRoomInfoCallback(__int64 room_id, int error_code, const ChatRoomInfo& info);
	void OnNotificationCallback(const ChatRoomNotification& notification);
	void OnGetMembersCallback(__int64 room_id, int error_code, const std::list<ChatRoomMemberInfo>& infos);
	void GetMsgHistoryCallback(__int64 room_id, int error_code, const std::list<ChatRoomMessage>& msgs);
	void SetMemberAttributeCallback(__int64 room_id, int error_code, const ChatRoomMemberInfo& info);
	void KickMemberCallback(__int64 room_id, int error_code);
	void OnChatRoomRequestEnterCallback(int error_code, const std::string& result);
	void OnRegLinkConditionCallback(__int64 room_id, const NIMChatRoomLinkCondition condition);

	void RequestRoomError();
private:
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);
	void OnHttoDownloadReady(HttpResourceType type, const std::string& account, const std::wstring& photo_path);
	void InitHeader();

private:
	void OnDropFile(HDROP hDrop);
	BOOL CheckDropEnable(POINTL pt);
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave(void);
	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);

	std::wstring GetCurrentTimeString();
	void TimeHandle();
private:
	bool OnEditEnter(ui::EventArgs* param);
	void OnBtnEmoji();
	bool OnSelChanged(ui::EventArgs* param);
	void OnEmotionSelected(std::wstring emo);
	void OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name);
	void OnEmotionClosed();
	bool OnSelOnlineMembers(ui::EventArgs* param);
	bool OnLinkClick(WPARAM wParam, LPARAM lParam);
	bool OnMemberMenu(ui::EventArgs* args);

	void ShowMemberMenu(std::wstring &name);
	bool KickMenuItemClick(ui::EventArgs* args);
	bool AddMuteMenuItemClick(ui::EventArgs* args);
	bool RemoveMuteMenuItemClick(ui::EventArgs* args);
	bool AddBlacklistMenuItemClick(ui::EventArgs* args);
	bool RemoveBlacklistMenuItemClick(ui::EventArgs* args);
	bool AddAdminMenuItemClick(ui::EventArgs* args);
	bool RemoveAdminMenuItemClick(ui::EventArgs* args);

private:
	void SetMemberAdmin(const std::string &id, bool is_admin);	//设置成员列表界面中某个成员的类型
	void SetMemberBlacklist(const std::string &id, bool is_black);
	void SetMemberMute(const std::string &id, bool is_mute);
	void SetMemberFixed(const std::string &id, bool is_fixed);
	void RemoveMember(const std::string &uid);
	void UpdateOnlineCount();	// 刷新在线人数
	void GetMembers();
	void GetHistorys();
private:
	void AddMsgItem(const ChatRoomMessage& result, bool is_history);
	void AddNotifyItem(const ChatRoomNotification& notification, bool is_history);
	void OnBtnSend();
	void AddText(const std::wstring &text, const std::wstring &sender_name, bool is_history);
	void AddNotify(const std::wstring &notify, bool is_history);
	void SendText(const std::string &text);
	void OnBtnJsb();
	void AddJsb(const int value, const std::wstring &sender_name, bool is_history);
	void SendJsb(const std::string &attach);

private:
	// 提供发送图片消息的例子
	void SendImage(const std::wstring &src);
	void OnUploadImageCallback(nim::NIMResCode res_code, const std::string& url, nim::IMImage img);

public:
	static const LPTSTR kClassName;
	static const int kAllowClose = 10;

private:
	__int64			room_id_;
	std::string		room_enter_token_;

private:	
	ui::Control*	header_icon_ = NULL; //标题栏头像
	ui::Label*		name_ = NULL; //标题栏姓名

	ui::Control*	host_icon_ = NULL; //主播头像
	ui::Label*		room_name_ = NULL;
	ui::Label*		host_name_ = NULL; //主播名称
	ui::Label*		online_num_ = NULL;
	ui::RichEdit*	bulletin_ = NULL;//公告

	ui::TabBox*		list_tab_ = NULL;
	ui::RichEdit*	input_edit_ = NULL;
	ui::CheckBox*	btn_face_ = NULL;
	ui::Button*		btn_send_ = NULL;
	ui::RichEdit*	msg_list_ = NULL;
	ui::ListBox*	online_members_list_ = NULL;
	ui::Control*	empty_members_list_ = NULL;

	bool			has_enter_;
	std::string		creater_id_;	//创建者信息
	std::map<std::string, ChatRoomMemberInfo> managers_list_;//管理员信息 <id,info>
	std::map<std::string, ChatRoomMemberInfo> members_list_;//成员信息 <id,info>
	std::map<std::string, std::string> nick_account_map_;//记录接受到的消息的昵称对应的帐号，方便通过昵称查到帐号

	std::string		clicked_user_account_;	//被单击了的消息列表中的帐号
	std::string		kicked_user_account_;	//被踢出的帐号
	__int64			time_start_history;		//获取的最早的历史消息时间
	time_t			time_refresh_;			//上一次刷新在线成员列表的时间
	bool			has_add_creater_ = false;	//是否在在线成员里添加了创建者
	bool			is_loading_history_ = false;

	IDropTarget		*droptarget_;
	nim_comp::IRichEditOleCallbackEx *richeditolecallback_;
	AutoUnregister	unregister_cb;

};

}