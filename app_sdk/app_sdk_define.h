#pragma once
namespace app_sdk
{
	//格式化账号密码回调
	using OnFormatAccountAndPasswordCallback = std::function<void(bool ret,/*是否进行登录*/const std::string& account, const std::string& password)>;
	//注册新账号回调
	using OnRegisterAccountCallback = std::function<void(int code, const std::string& err_msg)>;
	//获取聊天室列表回调
	using OnGetChatroomListCallback = std::function<void(int code, const std::vector< nim_chatroom::ChatRoomInfo>& chatroom_list)>;
	//获取聊天室地址回调
	using OnGetChatroomAddressCallback = std::function<void(int code, const std::list< std::string>& address_list)>;
}
