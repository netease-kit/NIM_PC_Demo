# NIM PC Demo详解 #

#前言

这篇教程主要介绍NIM DEMO的nim工程的内容


#目录结构

* callback：注册到SDK的一些回调的处理

* gui：所有功能的界面相关实现

* module：所有功能的逻辑相关实现；其中，module\sdk目录下存放了针对SDK C接口的C++封装层代码。

* util：一些公用的工具类


#功能点指引

##SDK C++封装层

因为SDK所有接口都是C接口，为了方便使用C++的同学使用，DEMO在SDK C接口的基础上，增加了一层C++的接口封装，可以更加方便的使用SDK接口，使用C++编写客户端的同学，可以从DEMO源码中获取这些代码。

* module\sdk\nim_client_cpp_wrapper.h: 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能

* module\sdk\nim_talk_cpp_wrapper.h: 聊天功能；主要包括发送消息、接收消息等功能

* module\sdk\nim_session_cpp_wrapper.h: 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能

* module\sdk\nim_team_cpp_wrapper.h: 群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能

* module\sdk\nim_session_cpp_wrapper.h: 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能

* module\sdk\nim_vchat_cpp_wrapper.h: 音视频（包括设备）相关接口

* module\sdk\nim_msglog_cpp_wrapper.h: 消息历史接口

* module\sdk\nim_data_sync_cpp_wrapper.h: 数据同步相关接口

* module\sdk\nim_http_cpp_wrapper.h: 通过http协议上传资源、下载资源等功能

* module\sdk\nim_friend_cpp_wrapper.h: 好友功能，包含添加好友、删除好友、监听好友变化

* module\sdk\nim_user_cpp_wrapper.h: 用户功能，包含黑名单设置、个人消息提醒设置、状态设置

* module\sdk\nim_rts_cpp_wrapper.h: 白板功能

* module\sdk\nim_tool_cpp_wrapper.h: 提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5、语音转文字等

##SDK 初始化

SDK的初始化在main.cpp中InitNim方法进行。

示例：

	Json::Value config_root;
	//sdk能力参数（必填）
	Json::Value config_values;
	config_values[nim::kNIMDataBaseEncryptKey] = "Netease"; //string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）
	config_values[nim::kNIMPreloadAttach] = true;        //bool （选填，是否需要预下载附件缩略图， sdk默认预下载）
	config_root[nim::kNIMGlobalConfig] = config_values;
	bool ret = nim::Client::Init("Netease", "", config_root.toStyledString().c_str());



##登录

登录相关界面代码在gui/login下，逻辑代码在login_callback.cpp的DoLogin

示例：

	std::string app_key = "45c6af3c98409b18a84451215d0bdd6e";//app key 申请获得
	std::string pass_md5 = QString::GetMd5(password);	//MD5加密
	nim::Client::Login(app_key, user, pass_md5, OnLoginCallback, nullptr);

##会话

会话相关的界面代码在gui/session下，逻辑代码在module/session下。

###发送消息

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


###接受消息

接收消息的需要在程序刚启动时，提前注册好接收消息的回调函数。
回调函数的参数是一个json字符串，首先我们要解析json，json字段跟发送消息相近，只需要根据这些字段解析就可以了。解析完之后，一般我们会需要把解析到的文本、图片等消息在会话窗口中进行展示。

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
