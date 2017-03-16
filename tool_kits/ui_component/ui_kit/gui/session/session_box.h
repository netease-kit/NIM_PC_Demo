#pragma once
#include "callback/audio/audio_callback.h"
#include "module/service/photo_service.h"
#include "module/session/force_push_manager.h"
#include "gui/session/control/bubbles/bubble_text.h"
#include "gui/session/control/bubbles/bubble_image.h"
#include "gui/session/control/bubbles/bubble_snapchat.h"
#include "gui/session/control/bubbles/bubble_audio.h"
#include "gui/session/control/bubbles/bubble_location.h"
#include "gui/session/control/bubbles/bubble_notice.h"
#include "gui/session/control/bubbles/bubble_finger.h"
#include "gui/session/control/bubbles/bubble_unknown.h"
#include "gui/session/control/bubbles/bubble_writing.h"
#include "gui/session/control/bubbles/bubble_file.h"
#include "gui/session/control/bubbles/bubble_sticker.h"
#include "gui/session/control/bubbles/bubble_video.h"
#include "gui/session/control/team_item.h"
#include "gui/emoji/emoji_form.h"
#include "gui/team_info/team_info.h"

namespace nim_comp
{
const int kCellWritingTime = 5;
const int kCellCancelWriting = 7;

class AtMeView;
class AudioCaptureView;
class SessionForm;
class TaskbarTabItem;
/** @class SessionBox
  * @brief 会话盒子，作为会话窗口的子控件；用来创建个人会话、讨论组会话、高级群会话
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/10
  */
class SessionBox : public ui::VBox
{
public:
	friend class SessionForm;
#pragma region Init
	/** 
	* 构造函数
	* @param[in] id		对于个人会话，设置为对方的用户id；对于群会话，设置为群id
	* @param[in] type	设置会话类型(个人或者群)
	*/
	SessionBox(std::string id, nim::NIMSessionType type);
	~SessionBox();

	/**
	* 获取会话id
	* @return string 会话id
	*/
	std::string GetSessionId() const { return session_id_; }

	/**
	* 获取会话类型
	* @return nim::NIMSessionType 会话类型
	*/
	nim::NIMSessionType GetSessionType() const { return session_type_; }

	/**
	* 获取本会话盒子所属的会话窗口指针
	* @return SessionForm* 会话窗口
	*/
	SessionForm* GetSessionForm() const;

	/**
	* 初始化会话盒子
	* @return void	无返回值
	*/
	virtual void InitSessionBox();

	/**
	* 反初始化会话盒子
	* @return void	无返回值
	*/
	virtual void UninitSessionBox();
#pragma endregion Init

	//////////////////////////////////////////////////////////////////////////
	//会话消息相关的操作
#pragma region Message
public:
	/**
	* 执行加载历史消息的操作
	* @param[in] first_show_msg 是否是本会话窗体第一次加载历史消息(刚创建会话窗体时设置为true)
	* @return void 无返回值
	*/
	void InvokeShowMsgs(bool first_show_msg);

	/** 
	* 当收到新的会话消息后，添加新消息到聊天框里，并且根据消息的类型来调整会话框状态
	*（一般由TalkCallback类来自动调用此函数，不需要自己调用，调用ShowMsg函数完成真正的消息添加）
	* @param[in] msg	新消息的数据
	* @param[in] create	如果收到新消息时对应的会话窗体还没有创建，则设置为true	
	* @return void 无返回值
	*/
	void AddNewMsg(const nim::IMMessage &msg, bool create);

	/**
	* 添加一条新消息到聊天框里
	* @param[in] msg	新消息的数据
	* @param[in] first	是否把消息添加到聊天框的开头位置
	* @param[in] show_time	是否显示这条消息的接收时间
	* @return MsgBubbleItem* 新添加的消息控件的指针
	*/
	MsgBubbleItem* ShowMsg(const nim::IMMessage &msg, bool first, bool show_time);

	/**
	* 加载历史消息，把一组历史消息添加到聊天框的开头位置
	* @param[in] msg	历史消息数据
	* @return void 无返回值
	*/
	void ShowMsgs(const std::vector<nim::IMMessage> &msg);

	/**
	* 在聊天框内添加一个“对方正在输入”的状态消息（当在输入框编辑时调用）
	* @param[in] msg	“对方正在输入”消息的数据
	* @return void 无返回值
	*/
	void AddWritingMsg(const nim::IMMessage &msg);

	/**
	* 添加一个被转发的消息到聊天框
	* @param[in] msg 消息数据
	* @return void 无返回值
	*/
	void AddRetweetMsg(const nim::IMMessage &msg);

	/** 
	* 新消息发送给对方后，获取消息发送状态的回调函数（是否发送成功等）
	* @param[in] cid	消息id 
	* @param[in] code	错误码(200代表无错误)
	* @param[in] msg_timetag	时间戳
	* @return void 无返回值
	*/
	void OnSendMsgCallback(const std::string &cid, int code, __int64 msg_timetag);

	/**
	* 新消息发送给对方后，获取消息发送状态的回调函数（对方是否已读）
	* @param[in] from_accid	发送方id
	* @param[in] timetag	临界消息time
	* @param[in] status	消息状态
	* @return void 无返回值
	*/
	void OnMsgStatusChangedCallback(const std::string &from_accid, const __int64 timetag, nim::NIMMsgLogStatus status);

	/**
	* 阅后即焚消息被（发送者或接收者）读了之后，从消息列表删除
	* @param[in] from_accid	发送方id
	* @param[in] timetag	临界消息time
	* @param[in] status	消息状态
	* @return void 无返回值
	*/
	void OnSnapchatReadCallback(const std::string& client_msg_id);

	/** 
	* 下载文件的回调函数
	* @param[in] res_id		消息id 
	* @param[in] success	是否下载成功	
	* @param[in] file_path	下载路径
	* @return void 无返回值
	*/
	void OnDownloadCallback(const std::string &res_id, bool success, const std::string& file_path);

	/**
	* 上传文件的回调函数
	* @param[in] res_id		消息id
	* @param[in] success	是否上传成功
	* @param[in] url		上传地址
	* @return void 无返回值
	*/
	void OnUploadCallback(const std::string &res_id, bool success, const std::string& url);

	/**
	* 消息撤回的结果回调
	* @param[in] code 返回码
	* @param[in] notify 撤回消息的通知内容
	* @return void 无返回值
	*/
	void OnRecallMsgCallback(nim::NIMResCode code, const nim::RecallMsgNotify &notify);

	/**
	* 被转发的消息的资源文件被清理了，则重新下载，否则加载本地资源文件的消息会显示有错误
	* @param[in] code 返回码
	* @param[in] file_path 文件下载保存的路径
	* @param[in] sid 会话id（好友id或群id）
	* @param[in] cid 消息id
	* @return void	无返回值
	*/
	void OnRetweetResDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid);
#pragma endregion Message

	//////////////////////////////////////////////////////////////////////////
	//音频相关的操作
#pragma region Audio
public:
	/** 
	* 单击了语音消息后，开始播放语音的回调函数
	* @param[in] cid	消息id 
	* @param[in] code	错误码(200代表无错误)	
	* @return void 无返回值
	*/
	void OnPlayAudioCallback(const std::string &cid, int code);

	/**
	* 第二次单击了语音消息后，停止播放语音的回调函数
	* @param[in] cid	消息id
	* @param[in] code	错误码(200代表无错误)
	* @return void 无返回值
	*/
	void OnStopAudioCallback(const std::string &cid, int code);
#pragma endregion Audio

	//////////////////////////////////////////////////////////////////////////
	//界面相关的操作
#pragma region UI
public:

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return LRESULT 处理结果
	*/
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandle);

	/**
	* 拦截并处理按下ESC键的消息
	* @param[in] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return void	无返回值
	*/
	void OnEsc(BOOL &bHandled);

	/**
	* 处理所有控件的所有消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* param);

	/**
	* 处理所有控件单击消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* param);

	/**
	* 处理所有控件的选中消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSelChanged(ui::EventArgs* param);

	/**
	* 处理输入框控件的回车消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnInputEditEnter(ui::EventArgs* param);

private:

	/**
	* 覆盖父类虚函数，用于指定本控件所属窗体及父控件信息
	* @param[in] pManager 所属窗口指针
	* @param[in] pParent 父控件指针
	* @param[in] bInit 是否要初始化本控件
	* @return void	无返回值
	*/
	virtual void SetWindow(ui::Window* pManager, ui::Box* pParent, bool bInit) override;

	/** 
	* 执行截图操作		
	* @return void 无返回值
	*/
	void DoClip();

	/** 
	* 截图完毕后的回调函数
	* @param[in] ret	是否截图成功 
	* @param[in] file_path	截图文件的保存路径		
	* @return void 无返回值
	*/
	void OnClipCallback(bool ret, const std::wstring& file_path);

	/**
	* 响应发送文本的消息
	* @return void 无返回值
	*/
	void OnBtnSend();

	/**
	* 响应发送图片按钮消息
	* @param[in] is_snapchat 是否阅后即焚
	* @return void 无返回值
	*/
	void OnBtnImage(bool is_snapchat);

	/**
	* 图片被选择后的回调函数
	* @param[in] is_snapchat 是否阅后即焚
	* @param[in] ret		 是否选择了图片
	* @param[in] file_path	 选择的图片的路径
	* @return void 无返回值
	*/
	void OnImageSelected(bool is_snapchat, BOOL ret, std::wstring file_path);

	/**
	* 响应发送文件按钮消息
	* @return void 无返回值
	*/
	void OnBtnFile();

	/**
	* 文件被选择后的回调函数
	* @param[in] ret		 是否选择了文件
	* @param[in] file_path	 选择的文件的路径
	* @return void 无返回值
	*/
	void OnFileSelected(BOOL ret, std::wstring file_path);

	/**
	* 响应“剪刀石头布”按钮消息
	* @return void 无返回值
	*/
	void OnBtnJsb();

	/**
	* 响应录制语音按钮消息
	* @return void 无返回值
	*/
	void OnBtnCaptureAudio();

	/**
	* 录制语音完成的回调
	* @param[in] rescode 操作结果，一切正常200
	* @param[in] sid 会话id
	* @param[in] file_path 语音文件绝对路径
	* @param[in] file_ext 文件扩展名
	* @param[in] file_size 文件大小
	* @param[in] audio_duration 语音时长
	* @return void 无返回值
	*/
	void OnAudioCaptureComplete(int rescode, const std::string& sid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration);

	/**
	* 响应表情按钮消息
	* @return void 无返回值
	*/
	void OnBtnEmoji();

	/**
	* 某个表情被选择后的回调函数
	* @param[in] emo		 选择的表情
	* @return void 无返回值
	*/
	void OnEmotionSelected(std::wstring emo);

	/**
	* 某个贴图表情被选择后的回调函数
	* @param[in] catalog	贴图所在目录
	* @param[in] name		贴图名字
	* @return void 无返回值
	*/
	void OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name);

	/**
	* 表情选择窗体关闭后的回调函数
	* @return void 无返回值
	*/
	void OnEmotionClosed();

	/**
	* 响应语音按钮消息
	* @return void 无返回值
	*/
	void OnBtnAudio();

	/**
	* 响应视频按钮消息
	* @return void 无返回值
	*/
	void OnBtnVideo();

	/**
	* 响应白板按钮消息
	* @return void 无返回值
	*/
	void OnBtnRts();

	/**
	* 响应单击转发菜单项的消息
	* @param[in] msg 被转发的消息
	* @return void 无返回值
	*/
	void OnMenuRetweetMessage(const nim::IMMessage &msg);

	/**
	* 收到转发列表后的回调函数
	* @param[in] msg 被转发的消息
	* @param[in] friend_list 转发的好友帐号数组
	* @param[in] team_list 转发的群组数组
	* @return void 无返回值
	*/
	void OnSelectedRetweetList(nim::IMMessage msg, const std::list<std::string>& friend_list, const std::list<std::string>& team_list);

	/**
	* 响应邀请按钮消息，邀请他人加入群聊
	* @param[in] param 被单击的按钮的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnBtnInvite(ui::EventArgs* param);

	/**
	* 邀请完毕后的回调函数，根据邀请的名单把个人会话升级带讨论组会话
	* @param[in] friend_list 邀请的好友帐号数组
	* @param[in] team_list 不使用
	* @return void 无返回值
	*/
	void OnSelectedInvateList(const std::list<std::string>& friend_list, const std::list<std::string>& team_list);

	/**
	* 关闭会话窗体时，如果有输入内容，会出现确认对话框，这里响应用户的选择
	* @return void 无返回值
	*/
	void OnCloseInputMsgBoxCallback(MsgBoxRet ret);

	/**
	* 移除聊天窗中的某个提示内容（比如对方正在输入、对方非好友）
	* @param[in] type
	* @return void 无返回值
	*/
	void AddTip(SessionTipType type);

	/**
	* 移除聊天窗中的某个提示内容
	* @param[in] type
	* @return void 无返回值
	*/
	void RemoveTip(SessionTipType type);

	/**
	* 响应个人信息(群信息)按钮消息，显示个人信息（群信息）
	* @param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnBtnHeaderClick(ui::EventArgs* param);

	/**
	* 处理窗口最大化消息
	* @param[in] max 是否最大化
	* @return void	无返回值
	*/
	void OnWndSizeMax(bool max);

	/**
	* 处理会话盒子被激活的消息
	* @return void	无返回值
	*/
	void OnActivate();

	/**
	* 显示自定义通知窗体
	* @return void 无返回值
	*/
	void ShowCustomMsgForm();
#pragma endregion UI

	//////////////////////////////////////////////////////////////////////////
	//拖拽相关的操作
#pragma region DragDrop
public:
	/**
	* 检测某个坐标是否在拖放区域
	* @param[in] pt	坐标
	* @return bool true 在可拖放区域，false 不在可拖放区域
	*/
	bool CheckDropEnable(POINTL pt);

	/**
	* 处理被托放入文件
	* @param[in] hDrop	HDROP对象句柄
	* @return void	无返回值
	*/
	void OnDropFile(HDROP hDrop);

	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave(void);
	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);
#pragma endregion DragDrop

	//////////////////////////////////////////////////////////////////////////
	//窗口合并功能相关的操作
#pragma region Merge
public:
	/**
	* 获取与本会话盒子绑定的Tab指针
	* @return TaskbarItem*	Tab指针
	*/
	TaskbarTabItem* GetTaskbarItem();

	/**
	* 覆盖基类虚函数，为了在任务栏显示缩略图和预览图，重写了设置子控件SetInternVisible属性的逻辑
	* @param[in] bVisible 是否显示
	* @return void	无返回值
	*/
	virtual void SetInternVisible(bool bVisible = true) override;

	/**
	* 覆盖基类虚函数，为了在重绘任务栏显示的缩略图
	* @return void	无返回值
	*/
	virtual void Invalidate() const override;

	/**
	* 覆盖基类虚函数，为了在重绘任务栏显示的缩略图
	* @param[in] rc 要设置的控件的位置
	* @return void	无返回值
	*/
	virtual void SetPos(ui::UiRect rc) override;
#pragma endregion Merge

	//////////////////////////////////////////////////////////////////////////
	//@功能相关的操作
#pragma region At
public:

	/**
	* 填充AtMeView控件内容
	* @param[in] infos @me消息
	* @return void	无返回值
	*/
	void InitAtMeView(std::vector<ForcePushManager::ForcePushInfo> &infos);

private:

	/**
	* 如果msg包含atme消息，就添加到AtMeView控件
	* @param[in] msg 消息
	* @return void	无返回值
	*/
	void AddAtMessage(const nim::IMMessage &msg);

	/**
	* 开始执行滚动到某个控件位置的动画，用于@me消息查看
	* @param[in] control 滚动的目标控件
	* @return void	无返回值
	*/
	void ScrollToControl(const ui::Control *control);

	/**
	* 当前位置滚动到某个控件位置的动画过程的回调函数
	* @param[in] control 滚动的目标控件
	* @return void	无返回值
	*/
	void ScrollToControlCallback(const ui::Control *control);

	/**
	* 获取某uid显示的名称，优先显示备注名，其次显示群名片，最后显示昵称
	* @param[in] uid 用户id
	* @return string 显示名字
	*/
	std::string GetShowName(const std::string& uid);

	/**
	* 处理@我预览窗被单击的消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnAtMeViewClick(ui::EventArgs* param);

	/**
	* 处理@我预览窗关闭按钮被单击的消息
	* @param[in] param 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnCloseAtMeView(ui::EventArgs* param);

	/**
	* 处理输入框的@相关的按键消息
	* @param[in] wParam 附加消息
	* @param[in] lParam 附加消息
	* @return bool true 不继续传递消息，false 继续传递消息
	*/
	bool HandleAtMsg(WPARAM wParam, LPARAM lParam);

	/**
	* 处理输入框的@相关的鼠标滚轮消息
	* @param[in] wParam 附加消息
	* @param[in] lParam 附加消息
	* @return bool true 不继续传递消息，false 继续传递消息
	*/
	bool HandleAtMouseWheel(WPARAM wParam, LPARAM lParam);

	/**
	* 隐藏@列表窗口
	* @return void	无返回值
	*/
	void HideAtListForm();

	/**
	* 响应@列表被单击的回调函数
	* @param[in] uid 被选择的用户id
	* @return void	无返回值
	*/
	void OnSelectAtItemCallback(const std::string& uid);

	/**
	* 获取最近发消息的5个人（不包括自己）,最新发言的在列表最前
	* @param[out] uid_last_five 最近发消息的5个人
	* @return void	无返回值
	*/
	void GetLastFiveSender(std::list<std::string> &uid_last_five);
#pragma endregion At

	//////////////////////////////////////////////////////////////////////////
	//逻辑相关的操作
#pragma region Logic
private:
	/**
	* 显示一条"对方正在输入"的提示消息
	* @param[in] msg 会话消息
	* @return void	无返回值
	*/
	void ShowMsgWriting(const nim::IMMessage &msg);

	/**
	* 删除"对方正在输入"的提示消息
	* @return void	无返回值
	*/
	void CancelWriting();

	/**
	* 发送一条文本消息
	* @param[in] text 文本内容
	* @return void	无返回值
	*/
	void SendText(const std::string &text);

	/**
	* 发送一条图片消息
	* @param[in] src 图片路径
	* @return void	无返回值
	*/
	void SendImage(const std::wstring &src);

	/**
	* 发送一条语音消息
	* @param[in] file_path 语音文件路径
	* @param[in] file_ext 语音文件扩展名
	* @param[in] file_size 文件大小
	* @param[in] audio_duration 语音时长
	* @return void	无返回值
	*/
	void SendAudio(const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration);

	/**
	* 发送一条阅后自焚消息
	* @param[in] src 图片路径
	* @return void	无返回值
	*/
	void SendSnapChat(const std::wstring &src);

	/**
	* 检测文件大小是否符合发送要求
	* @param[in] src 文件路径
	* @return bool true 符合，false 不符合
	*/
	bool CheckFileSize(const std::wstring &src);

	/**
	* 发送一条文件消息
	* @param[in] src 文件路径
	* @return void	无返回值
	*/
	void SendFile(const std::wstring &src);

	/**
	* 发送一条猜拳消息
	* @param[in] attach 包含猜拳结果的json串
	* @return void	无返回值
	*/
	void SendJsb(const std::string &attach);

	/**
	* 发送一条贴图消息
	* @param[in] catalog 贴图所属的分类
	* @param[in] name 贴图名字
	* @return void	无返回值
	*/
	void SendSticker(const std::string &catalog, const std::string &name);

	/**
	* 发送一条提示消息
	* @param[in] tip 提示内容
	* @return void	无返回值
	*/
	void SendTip(const std::wstring &tip);

	/**
	* 在界面上添加一个当前正在被发送的消息
	* @param[in] msg 会话消息
	* @return void	无返回值
	*/
	void AddSendingMsg(const nim::IMMessage &msg);

	/**
	* 重新发送某条的消息
	* @param[in] msg 会话消息
	* @return void	无返回值
	*/
	void ReSendMsg(nim::IMMessage &msg);

	/**
	* 自动填充nim::IMMessage结构体一些基本的字段，方便发送消息时使用
	* @param[out] msg 会话消息
	* @return void	无返回值
	*/
	void PackageMsg(nim::IMMessage &msg);

	/**
	* 检查最后发送的一条消息
	* @return void	无返回值
	*/
	void CheckLastReceiptMsg();

	/**
	* 获取最后一条需要发送回执信息的消息
	* @param[out] msg 会话消息
	* @return bool true 检测到，false 检测不到
	*/
	bool GetLastNeedSendReceiptMsg(nim::IMMessage &msg);

	/**
	* 标记最后一条需要已读回执的消息
	* @return bool true 已标记，false 未标记
	*/
	bool ResetLastMsgNeedMarkReceipt();

	/**
	* 移除某个消息气泡
	* @param[in] client_msg_id 消息id
	* @return int 被移除的气泡的索引
	*/
	int RemoveMsgItem(const std::string& client_msg_id);

	/**
	* 根据消息时间戳查找消息应在的位置
	* @param[in] msg_time 消息时间戳
	* @return int 位置索引
	*/
	int FindIndexByMsgTime(int64_t msg_time);

	/**
	* 获取回撤消息的提示文本
	* @param[in] msg_from_id 消息发送者的id
	* @param[in] msg_from_nick 消息发送者的昵称
	* @return std::wstring 提示文本
	*/
	std::wstring GetRecallNotifyText(const std::string& msg_from_id, const std::string& msg_from_nick);

	/** 
	* 根据会话的信息，设置投降
	* @return void 无返回值
	*/
	void CheckHeader();

	/** 
	* 用户掉线重连后的回调函数
	* @param[in] json 消息体Json，包含是否重连成功的信息	
	* @return void 无返回值
	*/
	void OnRelink(const Json::Value &json);

	/**
	* 是否正在与本帐号的手机端通信
	* @return bool 返回值true: 是， false: 否
	*/ 
	bool IsFileTransPhone();

	/**
	* 设置会话盒子的标题
	* @param[in] name 标题
	* @return void	无返回值
	*/
	void SetTitleName(const std::wstring &name);

	/**
	* 设置会话盒子的头像
	* @param[in] photo 头像路径
	* @return void	无返回值
	*/
	void SetHeaderPhoto(const std::wstring &photo);

	/**
	* 设置任务栏显示的标题
	* @param[in] title 标题
	* @return void	无返回值
	*/
	void SetTaskbarTitle(const std::wstring &title);

	/**
	* 更新任务栏显示的图标
	* @param[in] icon 图标路径
	* @return void	无返回值
	*/
	void SetTaskbarIcon(const std::wstring &icon);

	/**
	* 在本地数据库写入消息记录后的回调函数
	* @param[in] res_code 错误码
	* @param[in] msg_id 消息id
	* @param[in] msg 写入成功后需要在界面显示的消息
	* @param[in] index 消息插入的位置
	* @param[in] is_keep_end 插入消息后是否让滚掉条置底
	* @return void	无返回值
	*/
	void WriteMsglogCallback(nim::NIMResCode res_code, const std::string& msg_id, const nim::IMMessage& msg, int index, bool is_keep_end);

	/**
	* 对方个人资料改变的回调函数
	* @param[in] uinfos 新的个人资料列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* 会话框中某人的头像下载完成的回调函数
	* @param[in] accid 头像下载完成的用户id
	* @param[in] photo_path 头像本地路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path);
#pragma endregion Logic

	//////////////////////////////////////////////////////////////////////////
	//群相关的操作
#pragma region Team
public:
	/** 
	* 执行获取群信息的操作	
	* @return void 无返回值
	*/
	void InvokeGetTeamInfo(bool sync_block = false);

	/** 
	* 获取了群信息后自动调用的回调函数，根据群信息设置会话窗体的显示状态
	* @param[in] tid	群ID 
	* @param[in] result	群信息结构体			
	* @return void 无返回值
	*/
	void OnGetTeamInfoCallback(const std::string& tid, const nim::TeamInfo& result);

	/**
	* 执行获取群成员信息的操作
	* @return void 无返回值
	*/
	void InvokeGetTeamMember();

	/**
	* 获取了群成员信息后自动调用的回调函数，填充高级群会话右侧的群成员列表
	* @param[in] tid	群ID
	* @param[in] count	群成员数
	* @param[in] team_member_info_list 群成员信息表
	* @return void 无返回值
	*/
	void OnGetTeamMemberCallback(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list);

	/**
	* 获取群成员的信息
	* @param[in] uid 群成员用户ID
	* @return void	无返回值
	*/
	nim::TeamMemberProperty GetTeamMemberInfo(const std::string& uid);

	/**
	* 判断会话盒子群组聊天的有效性，离开群或者解散群后无法正常聊天
	* @return bool true 有效，false 无效
	*/
	bool IsTeamValid() { return is_team_valid_; };
private:
	/** 
	* 有群成员增加的回调函数
	* @param[in] tid 群ID 
	* @param[in] team_member_info 新增群成员的信息			
	* @return void 无返回值
	*/
	void OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info);

	/**
	* 有群成员退出的回调函数
	* @param[in] tid 群ID
	* @param[in] uid 退出的群成员的帐号
	* @return void 无返回值
	*/
	void OnTeamMemberRemove(const std::string& tid, const std::string& uid);
	
	/**
	* 有群成员信息改变的回调函数
	* @param[in] tid_uid 群ID和群成员的帐号，用'#'字符连接起来
	* @param[in] team_card 群昵称
	* @return void 无返回值
	*/
	void OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card);

	/**
	* 有群成员成为/取消管理员身份的回调函数
	* @param[in] tid 群ID
	* @param[in] uid 群成员的帐号
	* @param[in] admin 设置/取消管理员身份
	* @return void 无返回值
	*/
	void OnTeamAdminSet(const std::string& tid, const std::string& uid, bool admin);

	/**
	* 有群成员被禁言/被取消禁言的回调函数
	* @param[in] tid 群ID
	* @param[in] uid 群成员的帐号
	* @param[in] set_mute 禁言/取消禁言
	* @return void 无返回值
	*/
	void OnTeamMuteMember(const std::string& tid, const std::string& uid, bool set_mute);

	/**
	* 有群成员成为群主的回调函数
	* @param[in] tid 群ID
	* @param[in] uid 群成员的帐号
	* @return void 无返回值
	*/
	void OnTeamOwnerChange(const std::string& tid, const std::string& uid);

	/**
	* 群名更新的回调函数
	* @param[in] team_info 更新后的群信息
	* @return void 无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);

	/**
	* 退群或解散群的回调函数
	* @param[in] tid 退出的群id
	* @return void 无返回值
	*/
	void OnTeamRemove(const std::string& tid);

	/**
	* 判断用户类型是否可以显示在群成员中
	* @param[in] user_type 用户信息
	* @return void 无返回值
	*/
	bool IsTeamMemberType(const nim::NIMTeamUserType user_type);

	/**
	* 发送消息已读回执
	* @param[in] auto_detect 是否检测当前需要不需要发送回执，如果设置为true则只有会话盒子在激活状态并且消息列表处于末端才发送绘制，如果设置为false则直接发送回执
	* @return void	无返回值
	*/
	void SendReceiptIfNeeded(bool auto_detect = false);

	/**
	* 刷新消息列表中的名字
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void RefreshMsglistShowname(const std::string& uid);

	/**
	* 设置群静音状态
	* @param[in] mute 是否静音
	* @return void 无返回值
	*/
	void SetTeamMuteUI(bool mute);

	/**
	* 设置群成员列表禁言状态
	* @param[in] tid 群ID
	* @param[in] uid 群成员的帐号
	* @param[in] set_mute 禁言/取消禁言
	* @param[in] team_mute 设置/取消 群成员全员禁言状态
	* @return void 无返回值
	*/
	void SetTeamMemberMute(const std::string& tid, const std::string& uid, bool set_mute, bool team_mute);

	/**
	* 更新群公告
	* @param[in] broad 消息体Json，包含群公告信息
	* @return void 无返回值
	*/
	void UpdateBroad(const Json::Value &broad);

	/**
	* 根据群组类型设置会话窗体的外观
	* @param[in] type 群组类型
	* @return void 无返回值
	*/
	void CheckTeamType(nim::NIMTeamType type);

	/**
	* 是否为高级群会话
	* @return bool 是否为高级群会话
	*/
	bool IsAdvancedTeam();

	/**
	* 处理进入群的事件
	* @return void	无返回值
	*/
	void HandleEnterTeamEvent();

	/**
	* 处理离开群的事件
	* @return void	无返回值
	*/
	void HandleLeaveTeamEvent();

	/**
	* 处理解散群的事件
	* @return void	无返回值
	*/
	void HandleDismissTeamEvent();
#pragma endregion Team

public:
	static const LPCTSTR kClassName;

private:
	// 顶部控件
	ui::Button*		btn_max_restore_;
	ui::Label*		label_title_;
	ui::Label*		label_tid_;
	bool			is_header_enable_;	// 因为拖拽效果的需要，现在头像按钮一直可用，通过这个变量记录是否响应头像的单击事件
	ui::Button*		btn_header_;
	ui::Button*		btn_invite_;
	
	// 聊天功能控件
	ui::Box*		msg_content_;		// 消息列表控件的父控件
	ui::ListBox*	msg_list_;
	ui::CheckBox*	btn_face_;
	ui::Option *	btn_capture_audio_;
	ui::Button*		btn_send_;
	ui::RichEdit*	input_edit_;
	IDropTarget		*input_edit_droptarget_;
	IRichEditOleCallbackEx *richedit_ole_callback_;

	// 群功能相关控件
	ui::Button*		btn_new_broad_;
	ui::RichEdit*	edit_broad_;
	ui::Label*		label_member_;
	ui::Button*		btn_refresh_member_;
	ui::ListBox*	member_list_;
	std::map<std::string, nim::TeamMemberProperty> team_member_info_list_;
	bool			mute_all_;
private:
	// @功能相关变量
	nbase::WeakCallbackFlag	scroll_weak_flag_;		//一键顶部查看和@me滚动条动画所用的弱引用标志
	LONG					scroll_step_;			//滚动条步长
	bool					scroll_top_;			//单击@me消息滚动的方向

	AtMeView				*atme_view_;			//@我 消息预览
	std::map<std::string, std::string>	uid_at_someone_;//当前输入框被@的人的昵称和uid

	// 语音录制相关变量
	AudioCaptureView		*audio_capture_view_;	//语音录制界面

	// 任务栏缩略图管理
	TaskbarTabItem				*taskbar_item_;
private:
	SessionForm*	session_form_;
	std::string		session_id_;
	nim::NIMSessionType session_type_;
	nim::TeamInfo	team_info_;
	bool			is_team_valid_;

	bool			first_show_msg_;
	bool			mark_receipt_when_load_msgs_;
	long long		last_msg_time_;  //最近收到的消息时间
	long long		farst_msg_time_; //最远收到的消息时间
	
	bool			has_writing_cell_;				//是否已经包含“正在输入”的提示
	nbase::WeakCallbackFlag cancel_writing_timer_;	//“正在输入”所用的弱引用标志
	long long		writing_time_;					//记录最后一次在输入框输入消息的时间
	
	std::string		last_receipt_msg_id_;			//最近一条发送消息的id
	bool			receipt_need_send_ = false;		//当下会话是否有已读回执需要发送

	typedef std::map<std::string,MsgBubbleItem*> IdBubblePair;
	IdBubblePair	id_bubble_pair_;				//记录消息id与对应消息控件
	std::list<MsgBubbleItem*> cached_msgs_bubbles_; //记录消息前后顺序

	AutoUnregister	unregister_cb;
};
}