#pragma once

#include "nim_all_cpp_wrapper.h"

namespace nim_test
{
	class Talk
	{
	public:
		//发送文本
		static void SendTextMsg(const std::string& receiver, const std::string& text)
		{
			Json::Value json;
			json[nim::kNIMMsgKeyToType] = nim::kNIMSessionTypeP2P; //会话类型，好友是"kNIMSessionTypeP2P"，群组是"kNIMSessionTypeTeam"
			json[nim::kNIMMsgKeyType] = nim::kNIMMessageTypeText; //消息类型
			json[nim::kNIMMsgKeyBody] = text; //消息内容

			json[nim::kNIMMsgKeyToAccount] = receiver; //消息接收者帐号
			json[nim::kNIMMsgKeyTime] = 1000 * nbase::Time::Now().ToTimeT();; // 消息发送时间（毫秒）
			json[nim::kNIMMsgKeyClientMsgid] = QString::GetGUID(); //消息id，一般使用guid


			json[nim::kNIMMsgKeyLocalLogStatus] = nim::kNIMMsgLogStatusSending; //消息状态
			nim::Talk::SendMsg(json.toStyledString());
		}

		static std::string GetFileMD5(const std::string& utf8file)
		{
			const char* md5 = nim::Tool::GetFileMd5(utf8file);
			std::string out = std::string(md5);
			QString::NIMFreeBuf((char*)md5);
			return out;
		}

		//发送图片
		static void SendPictureMsg(const std::string& receiver, const std::string& image_local_path)
		{
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

			json[nim::kNIMMsgKeyToAccount] = receiver;					    //消息接收者帐号
			json[nim::kNIMMsgKeyTime] = 1000 * nbase::Time::Now().ToTimeT();;   //消息发送时间（毫秒）

			json[nim::kNIMMsgKeyClientMsgid] = QString::GetGUID();				//消息id，一般使用guid
			json[nim::kNIMMsgKeyLocalLogStatus] = nim::kNIMMsgLogStatusSending; //消息状态

			nim::Talk::SendMsg(json.toStyledString());
		}

	};
}

