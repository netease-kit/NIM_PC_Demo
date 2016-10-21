# 聊天室 PC端 DEMO导读

## <span id="前言">前言</span>

这篇教程主要介绍聊天室Demo模块，聊天室Demo作为NIM Demo的一个模块，如果对NIM Demo工程的目录结构，模块介绍想要更多的了解请查看NIM Demo PC导读。

## <span id="打包说明">打包说明</span>
开发者在打包自己的应用时，应确保将以下聊天室SDK（以下简称SDK）相关文件打包进去。

- nim_chatroom.dll：云信SDK主要功能库文件。

- msvcr100.dll：SDK依赖的VS2010动态链接库。

- msvcp100.dll：SDK依赖的VS2010动态链接库。

## <span id="功能点指引">功能点指引</span>

### <span id="SDK C++封装层">SDK C++封装层</span>

在SDK包里nim\_chatroom\_cpp\_sdk\nim\_chatroom\_sdk\_cpp_vs2010下包含nim\_chatroom\_sdk\_cpp\_lib.vcxproj项目文件，该工程是基于Visual Studio 2010开发，开发者可以通过解决方案右键添加已有项目将该项目文件添加到自己的工程，如果开发者已有解决方案的开发环境高于VS2010，请在导入的同时升级SDK项目，导入后编辑需要引用到SDK API的项目，在属性-通用属性-框架和引用中添加SDK项目为新的引用。当然开发者也可以仅仅将提供的SDK静态库添加到自己的工程，但是这里还是建议开发者将源码引入到自己的工程，方便扩展和调试。

封装层提供的包装文件如下：

* nim\_chatroom\_sdk\_cpp\\nim\_chatroom\_sdk\_cpp\_vs2010\\api\\nim\_chatroom\_cpp.h: 聊天室核心功能：主要包括SDK初始化/清理、聊天室进入/退出、消息收发查询、成员管理等。

* nim\_chatroom\_sdk\_cpp\\nim\_chatroom\_sdk\_cpp\_vs2010\\helper: 该目录下为辅助工具类。

### <span id="SDK 初始化">SDK 初始化</span>

SDK的初始化在main.cpp中InitNim方法中进行。

示例：

	bool ret = nim_chatroom::ChatRoom::Init();

### <span id="界面开发">界面开发</span>

云信PC demo以及UI组件的界面开发都依赖`云信DuiLib库`，关于`云信DuiLib库`的使用方法和注意事项，请参考：[云信Duilib](https://github.com/netease-im/NIM_PC_UIKit/blob/master/doc/nim_duilib.md)


### <span id="进入聊天室">进入聊天室</span>

进入聊天室前需要获取进入权限。

*代码摘抄自DEMO源码，chatroom\_form\_msg.cpp。*

	void ChatroomForm::RequestEnter(const __int64 room_id)
	{
		if (room_id == 0)
			return;

		room_id_ = room_id;
		//获取聊天室登录信息
		nim::PluginIn::ChatRoomRequestEnterAsync(room_id_, nbase::Bind(&ChatroomForm::OnChatRoomRequestEnterCallback, this, std::placeholders::_1, std::placeholders::_2));
	}

进入聊天室。

*代码摘抄自DEMO源码，chatroom\_form\_msg.cpp。*

	void ChatroomForm::OnChatRoomRequestEnterCallback(int error_code, const std::string& result)
	{
		...
		if (!result.empty())
		{
			StdClosure cb = [result, this](){
				room_login_token_ = result;

				//设置可选参数
				ChatRoomEnterInfo info;
				info.SetNick("my_nick_of_room");//聊天室内的昵称
				info.SetAvatar("http://my_avatar_url");//聊天室内的头像
				Json::Value ext_values,notify_ext_values;
				Json::Reader reader;
				if (reader.parse(content, ext_values) && ext_values.isObject())
					info.SetExt(ext_values);//扩展字段
				if (reader.parse(content, notify_ext_values) && notify_ext_values.isObject())
					info.SetNotifyExt(notify_ext_values);//用于通知的扩展字段

				bool bRet = ChatRoom::Enter(room_id_, room_login_token_, info);
				if (bRet)
				{
					this->Create(NULL, ChatroomForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
					this->CenterWindow();
					this->ShowWindow();
				}
				else
				{
					QLOG_APP(L"ChatRoom::Enter error");

					ChatroomFrontpage* front_page = (ChatroomFrontpage*)nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomFrontpage::kClassName, ChatroomFrontpage::kClassName);
					if (!front_page) return;
					ShowMsgBox(front_page->GetHWND(), L"进入直播间失败，请重试", nullptr, L"提示", L"确定", L"");
				}
			};

处理进入聊天室的结果：
	
注意几种错误码的处理方式。

	void ChatroomCallback::OnEnterCallback(__int64 room_id, const NIMChatRoomEnterStep step, int error_code, const ChatRoomInfo& info, const ChatRoomMemberInfo& my_info)
	{
		QLOG_APP(L"Chatroom:OnEnterCallback: id={0} step={1} code={2}") << room_id << step << error_code;

		if (step != kNIMChatRoomEnterStepRoomAuthOver)
			return;

		StdClosure cb = [=](){
			if (error_code != nim::kNIMResSuccess && error_code != nim::kNIMResTimeoutError)
			{
				ChatroomForm* chat_form = static_cast<ChatroomForm*>(nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomForm::kClassName, nbase::Int64ToString16(room_id)));
				if (chat_form)
				{
					if (error_code == nim::kNIMResRoomLocalNeedRequestAgain)
					{//重新登录
						chat_form->RequestEnter(room_id);
						return;
					}
					else
						chat_form->Close();
				}
	
				ChatroomFrontpage* front_page = nim_ui::WindowsManager::GetInstance()->SingletonShow<ChatroomFrontpage>(ChatroomFrontpage::kClassName);
				if (!front_page) return;

				std::wstring kick_tip_str;
				ui::MutiLanSupport *multilan = ui::MutiLanSupport::GetInstance();
				switch (error_code)
				{
				case nim::kNIMResNotExist:
					kick_tip_str = L"聊天室不存在";
					break;
				case nim::kNIMResForbidden:
					kick_tip_str = L"权限问题";
					break;
				case nim::kNIMResRoomLinkError:
				case nim::kNIMResRoomError:
					kick_tip_str = L"聊天室异常";
					break;
				case nim::kNIMResRoomBlackBeOut:
					kick_tip_str = L"黑名单用户禁止进入聊天室";
					break;
				case nim::kNIMResFrequently:
					kick_tip_str = L"操作太频繁,稍后重试";
					break;
				default:
					QLOG_APP(L"enter faled: {0} , {1}") << room_id << error_code;
					return;
				}

				ui::Box* kicked_tip_box = (ui::Box*)front_page->FindControl(L"kicked_tip_box");
				kicked_tip_box->SetVisible(true);
				nbase::ThreadManager::PostDelayedTask(front_page->ToWeakCallback([kicked_tip_box]() {
					kicked_tip_box->SetVisible(false);
				}), nbase::TimeDelta::FromSeconds(2));
	
				ui::Label* kick_tip_label = (ui::Label*)kicked_tip_box->FindSubControl(L"kick_tip");
				kick_tip_label->SetText(kick_tip_str);
	
				ui::Label* room_name_label = (ui::Label*)kicked_tip_box->FindSubControl(L"room_name");
				room_name_label->SetDataID(nbase::Int64ToString16(room_id));
				ChatRoomInfo info = front_page->GetRoomInfo(room_id);
				if (!info.name_.empty())
					room_name_label->SetUTF8Text(info.name_);
				else
					room_name_label->SetText(nbase::StringPrintf(L"直播间(id %lld)", room_id));
			}
			else
			{
				ChatroomForm* chat_form = static_cast<ChatroomForm*>(nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomForm::kClassName, nbase::Int64ToString16(room_id)));
				if (chat_form != NULL)
					chat_form->OnEnterCallback(error_code, info, my_info);
			}
		};
		Post2UI(cb);
	}


### <span id="聊天室消息">聊天室消息</span>

**发送消息**

SDK 支持文本、图片、音频、视频、地理位置、通知消息、提醒消息、文件消息和自定义消息等多种种类型消息，Demo目前展示了发送文本类型和自定义类型2种消息，代码中也包含发送图片消息的部分

*代码摘抄自DEMO源码，chatroom\_form\_msg.cpp。*

	//发送文本消息
	void ChatroomForm::SendText(const std::string &text)
	{
		std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
		std::string send_msg = ChatRoom::CreateTextMessage(my_id, kNIMChatRoomMsgTypeText, QString::GetGUID(), text);
		ChatRoom::SendMsg(room_id_, send_msg);
	}

	//发送图片消息
	void ChatroomForm::SendImage(const std::wstring &src)
	{
		nim::IMImage img;
		std::string utf8 = nbase::UTF16ToUTF8(src);
		img.md5_ = nim::Tool::GetFileMd5(utf8);
		img.size_ = (long)nbase::GetFileSize(src);
		std::wstring file_name, file_ext;
		nbase::FilePathApartFileName(src, file_name);
		nbase::FilePathExtension(src, file_ext);
		img.display_name_ = nbase::UTF16ToUTF8(file_name);
		img.file_extension_ = nbase::UTF16ToUTF8(file_ext);
		Gdiplus::Image image(src.c_str());
		if (image.GetLastStatus() != Gdiplus::Ok)
		{
			assert(0);
		}
		else
		{
			img.width_ = image.GetWidth();
			img.height_ = image.GetHeight();
		}

		auto weak_flag = this->GetWeakFlag();
		nim::NOS::UploadResource(utf8, [this, img, weak_flag](nim::NIMResCode res_code, const std::string& url) {
			if (!weak_flag.expired() && res_code == nim::kNIMResSuccess)
			{
				std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
				std::string send_msg = ChatRoom::CreateRoomMessage(my_id, kNIMChatRoomMsgTypeImage, QString::GetGUID(), img.ToJsonString());
				ChatRoom::SendMsg(room_id_, send_msg);
			}
		});
	}


**接收消息**

进入聊天室前注册好接收消息的回调函数。

*代码摘抄自DEMO源码，chatroom\_callback.cpp。*

	void ChatroomCallback::OnReceiveMsgCallback(__int64 room_id, const ChatRoomMessage& result)
	{
		QLOG_PRO(L"Chatroom:OnReceiveMsgCallback: {0}") << result.client_msg_id_;
	
		StdClosure cb = [=](){
			ChatroomForm* chat_form = static_cast<ChatroomForm*>(nim_ui::WindowsManager::GetInstance()->GetWindow(ChatroomForm::kClassName, nbase::Int64ToString16(room_id)));
			if (chat_form != NULL && result.msg_type_ != kNIMChatRoomMsgTypeUnknown)
			{
				chat_form->OnReceiveMsgCallback(result);
			}
		};
		Post2UI(cb);
	}
