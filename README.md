# NIM PC Demo详解

## <span id="前言">前言</span>

这篇教程主要介绍NIM DEMO的nim工程的内容，demo的作用主要是展现sdk的功能，方便开发者快速熟悉使用sdk，但demo的代码不一定是最佳的。demo使用Visual Studio 2013开发。

demo的主要功能由demo工程本身以及`UI组件`工程共同完成，demo工程和`UI组件`工程具有相同的目录结构。`UI组件`位于`tool_kits\ui_component\ui_kit`目录，`UI组件的`相关文档详见：[云信UI组件](https://github.com/netease-im/NIM_PC_UIKit)


## <span id="目录结构">目录结构</span>

* callback：注册到SDK的一些回调的处理

* gui：所有功能的界面相关实现

* module：所有功能的逻辑相关实现

* util：一些公用的工具类

## <span id="打包说明">打包说明</span>
开发者在打包自己的应用时，应确保将以下云信SDK相关文件打包进去。

- nim.dll：云信SDK主要功能库文件。

- nim_audio.dll：语音消息功能库文件。

- nim_videochat.dll：音视频通话功能库文件。

- nim_tools_http.dll：http功能库文件。

- msvcr100.dll：SDK依赖的VS2010动态链接库。

- msvcp100.dll：SDK依赖的VS2010动态链接库。

- nim_conf：云信SDK配置文件目录，包含SDK版本控制等。

其他的文件及目录是应用程序相关的，开发者根据自己程序的使用情况选择是否打包。

- image_ole.dll：图像显示库文件，支持在RichEdit组件中插入和显示图片。

- translation.bin：中文翻译成拼音依赖的文件。

- app_ver.dll：云信Demo应用程序版本控制，开发者请勿打包到自己的应用。

- lang：Demo界面文案对照表，可支持多国语言。

- res：Demo资源文件目录。

- themes：Demo皮肤目录，包含XML配置文件和图片文件。

## <span id="功能点指引">功能点指引</span>

### <span id="SDK C++封装层">SDK C++封装层</span>

因为SDK所有接口都是C接口，为了方便使用C++的同学使用，我们提供了`nim_cpp_sdk`静态库。静态库位于`libs\nim_sdk_desktop\nim_cpp_sdk`目录，它将C接口SDK封装为C++代码，demo和`UI组件`都直接使用`nim_cpp_sdk`静态库的C++封装层代码。开发者可以直接在解决方案中导入`nim_cpp_sdk`工程。

封装层提供的包装文件如下：

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_client.h: 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_data_sync.h: 数据同步相关接口

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_friend.h: 好友功能，包含添加好友、删除好友、监听好友变化

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_global.h: NIM SDK提供的一些全局接口；释放从SDK申请的内存

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_msglog.h: 消息历史接口

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_nos.h: NOS云存储服务接口；上传或下载文件资源

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_rts.h: 白板功能

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_session.h: 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_sysmsg.h: 系统消息接口；主要包括查询系统消息、删除系统消息等功能

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_talk.h: 聊天功能；主要包括发送消息、接收消息等功能

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_team.h: 群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_tool.h: 提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5、语音转文字等

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_vchat.h: 音视频（包括设备）相关接口

* nim\_sdk\_cpp\\nim\_sdk\_cpp\_lib\\api\\nim\_cpp\_user.h: 用户功能，包含黑名单设置、个人消息提醒设置、状态设置

除了`nim_cpp_sdk`静态库，另外还提供了负责语音和http传输的dll的C++封装类：

* libs\\nim\_sdk\_desktop\\nim\_cpp\_sdk\\nim\_audio\_cpp\\nim\_tools\_audio\_cpp\_wrapper.h:提供的语音录制和播放接口
* libs\\nim\_sdk\_desktop\\nim\_cpp\_sdk\\nim\_http\_cpp\\nim\_tools\_http\_cpp\_wrapper.h:提供的http传输相关接口

### <span id="SDK 初始化">SDK 初始化</span>

SDK的初始化在main.cpp中InitNim方法进行。

示例：

	//初始化nim sdk
	nim::SDKConfig config;
	//sdk能力参数（必填）
	config.database_encrypt_key_ = "Netease"; //string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）
	bool ret = nim::Client::Init("Netease", "", config); // 载入云信sdk，初始化安装目录和用户目录
	assert(ret);
	
	//初始化语音和http
	nim_http::Init(); 
	std::string res_audio_path = nbase::UTF16ToUTF8(app_data_audio_path);	// app_data_audio_path为语音资源所在用户目录
	ret = nim_audio::Audio::Init(res_audio_path);
	assert(ret);

### <span id="界面开发">界面开发</span>

云信PC demo以及UI组件的界面开发都依赖`云信DuiLib库`，关于`云信DuiLib库`的使用方法和注意事项，请参考：[云信Duilib](https://github.com/netease-im/NIM_PC_UIKit/blob/master/doc/nim_duilib.md)


### <span id="登录">登录</span>

登录相关界面代码在`gui/login`目录下，登录相关的控制逻辑已经封装到了`UI组件的`，逻辑代码在`callback\login\login_callback.cpp`文件中。登录界面可以直接调用`UI组件`的登录函数，示例如下：

	std::string username = "123456789"；
	std::string password = "123456789"；
	nim_ui::LoginManager::GetInstance()->DoLogin(username, password);

登录窗体的开发注意事项请看：[云信UI组件](https://github.com/netease-im/NIM_PC_UIKit)。如果不使用`UI组件`提供的登录逻辑，可以直接调用C++封装层的登录函数，示例如下:

	std::string app_key = "45c6af3c98409b18a84451215d0bdd6e";//app key 申请获得
	std::string pass_md5 = QString::GetMd5(password);	//MD5加密
	nim::Client::Login(app_key, user, pass_md5, OnLoginCallback, nullptr);

### <span id="主窗体功能">主窗体功能</span>

Demo主窗体界面直接使用`UI组件`中开发好的最近会话列表组件、好友列表组件和群组列表组件。`UI组件的`相关文档详见：[云信UI组件](https://github.com/netease-im/NIM_PC_UIKit)。在demo的主窗体类`MainForm`的初始化函数`InitWindow`中集成了最近会话列表组件、好友列表组件和群组列表组件。示例如下：

	((OptionBox*) FindControl(L"btn_session_list"))->Selected(true, true);
	ui::ListBox* session_list = (ListBox*)FindControl(L"session_list");
	nim_ui::SessionListManager::GetInstance()->AttachListBox(session_list);
	ui::TreeView* friend_list = (TreeView*) FindControl(L"friend_list");
	nim_ui::ContactsListManager::GetInstance()->AttachFriendListBox(friend_list);
	ui::TreeView* group_list = (TreeView*) FindControl(L"group_list");
	nim_ui::ContactsListManager::GetInstance()->AttachGroupListBox(group_list);
	
	nim_ui::ContactsListManager::GetInstance()->InvokeGetAllUserInfo();
	nim_ui::SessionListManager::GetInstance()->InvokeLoadSessionList();
	nim_ui::SessionListManager::GetInstance()->QueryUnreadCount();

### <span id="会话">会话</span>

会话相关的界面代码在`UI组件的`的`gui/session`目录下，逻辑代码`在module/session`目录下。Demo直接使用`UI组件`中开发好的会话窗体组件,也可以调用C++封装层的会话相关函数直接收发消息。

**发送消息**

SDK原生支持发送三种消息，包括文本、图片和文件；同时支持用户自定义消息类型，用户可根据自己需要使用。发送消息的需要把数据组装成json、调用SDK接口发送。

发送文本消息示例：

	Json::Value json;
	json[nim::kNIMMsgKeyToType] = nim::kNIMSessionTypeP2P; //会话类型，好友是"kNIMSessionTypeP2P"，群组是"kNIMSessionTypeTeam"
	json[nim::kNIMMsgKeyType] = nim::kNIMMessageTypeText; //消息类型
	json[nim::kNIMMsgKeyBody] = text; //消息内容

	json[nim::kNIMMsgKeyToAccount] = receiver; //消息接收者账号
	json[nim::kNIMMsgKeyTime] = 1000 * nbase::Time::Now().ToTimeT();; // 消息发送时间（毫秒）
	json[nim::kNIMMsgKeyClientMsgid] = QString::GetGUID(); //消息id，一般使用guid
	json[nim::kNIMMsgKeyLocalLogStatus] = nim::kNIMMsgLogStatusSending; //消息状态
	nim::Talk::SendMsg(json.toStyledString());


发送图片消息示例：

	std::wstring utf16_image_local_path = nbase::UTF8ToUTF16(image_local_path);
	Json::Value json;
	json[nim::kNIMMsgKeyToType] = nim::kNIMSessionTypeP2P;				//会话类型，好友是"kNIMSessionTypeP2P"，群组是"kNIMSessionTypeTeam"
	json[nim::kNIMMsgKeyType] = nim::kNIMMessageTypeImage;				//消息类型

	//图片本地路径
	json[nim::kNIMMsgKeyLocalFilePath] = image_local_path;				//文件本地路径
	//图片详细信息
	Json::Value image_key;
	std::string md5 = GetFileMD5(image_local_path);
	image_key[nim::kNIMImgMsgKeyMd5] = md5;								//文件MD5
	long sz = nbase::GetFileSize(utf16_image_local_path);
	image_key[nim::kNIMImgMsgKeySize] = sz;								//文件大小

	Gdiplus::Image image(utf16_image_local_path.c_str());
	if (image.GetLastStatus() == Gdiplus::Ok)
	{
		image_key[nim::kNIMImgMsgKeyWidth] = image.GetWidth();		    //宽度
		image_key[nim::kNIMImgMsgKeyHeight] = image.GetHeight();	    //高度
	}
	json[nim::kNIMMsgKeyAttach] = image_key.toStyledString();

	json[nim::kNIMMsgKeyToAccount] = receiver;					    //消息接收者账号
	json[nim::kNIMMsgKeyTime] = 1000 * nbase::Time::Now().ToTimeT();;   //消息发送时间（毫秒）
	json[nim::kNIMMsgKeyClientMsgid] = QString::GetGUID();				//消息id，一般使用guid
	json[nim::kNIMMsgKeyLocalLogStatus] = nim::kNIMMsgLogStatusSending; //消息状态

	nim::Talk::SendMsg(json.toStyledString());


**接收消息**

接收消息的需要在程序刚启动时，提前注册好接收消息的回调函数。回调函数的参数是一个json字符串，首先我们要解析json，json字段跟发送消息相近，只需要根据这些字段解析就可以了。解析完之后，一般我们会需要把解析到的文本、图片等消息在会话窗口中进行展示。

示例:

	void TalkCallback::OnReceiveMsgCallback( const std::string& json_str)
	{
		QLOG_PRO(L"OnReceiveMsgCallback: {0}") << json_str;
	
		Json::Value value;
		Json::Reader reader;
		if (reader.parse(json_str, value))
		{
			int code = value[nim::kNIMMsgKeyLocalRescode].asInt();
			int feature = value[nim::kNIMMsgKeyLocalMsgFeature].asInt();
	
			Json::Value json = value[nim::kNIMMsgKeyLocalReceiveMsgContent];
			json[nim::kNIMMsgKeyLocalRescode] = code;
			json[nim::kNIMMsgKeyLocalMsgFeature] = feature;
	
			MsgData msg;
			JsonToMsg(json, msg);
			std::string id = GetSessionId(msg);
	
			//会话窗口
			if (msg.feature == nim::kNIMMessageFeatureDefault)
			{
				if (msg.msg_type == nim::kNIMMessageTypeNotification)
				{
					SessionForm* session = SessionManager::GetInstance()->Find(id);
					if (session)
					{
						session->AddNewMsg(msg, false);
					}
				}
				else
				{
					SessionManager::GetInstance()->AddNewMsg(msg);
				}
			}
			else if (msg.feature == nim::kNIMMessageFeatureSyncMsg || msg.feature == nim::kNIMMessageFeatureRoamMsg)
			{
				SessionForm* session = SessionManager::GetInstance()->Find(id);
				if (session)
				{
					session->AddNewMsg(msg, false);
				}
			}
			else if (msg.feature == nim::kNIMMessageFeatureCustomizedMsg)
			{
				SessionForm* session = SessionManager::GetInstance()->Find(id);
				if (session)
				{
					session->AddNewMsg(msg, false);
				}
			}
		}
		else
		{
			QLOG_ERR(L"parse receive msg fail: {0}") << json_str;
		}
	}
