#pragma once
#include <fstream>
#include "public_define/defines/nim_define/nim_talk_def.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_info.h"
namespace nim_comp
{
	enum class LogToken : int
	{
		ReceiverType = 0, //int, 接收者类型，0:个人,1:群组,2:临时多人
		ReceiverAccount = 1, //string, 消息接收者的第三方帐号
		fromAccount = 2, // string, 发送者帐号
		clientType = 4, // 发送方设备类型

		deviceId = 5, // (可选) string 发送方deviceid
		fromNick = 6, //(可选) string 发送方昵称

		time = 7, //int, 时间戳
		type = 8, //int, 消息类型
		body = 9, //(可选)string, 消息内容

		attach = 10, //(可选) string,给sdk用的附加消息

		msgid_client = 11, //string, 客户端生成的消息id
		msgid_server = 12, //long, 服务端生成的消息id

		ext = 15, // (可选) string, 第三方扩展字段,

		pushPayload = 16, //(可选) string, 第三方自定义的推送属性，限制json，长度2048
		pushContent = 17, //(可选) string, 自定义推送文案，长度限制200字节
		forcePushList = 18, //((可选) string,强推列表（可扩展为区别于普通推送的推送帐号列表）参数格式为JSONArray：["aaa","bbb","ccc"] 如果要设置为强推所有成员，则将此参数设置为"#%@all@%#"这一特殊字符串
		forcePushContent = 19, //(可选) string,强推文本（可扩展为区别与普通推送的推送文本）
		isForcePush = 20, //(可选) int，是否强推（可扩展为区别于普通推送的推送形式），0:不强推，1:强推，默认为1，针对forcePushList里的帐号

		antiSpamContentCustom = 21, // (可选) int, 显示指定是否需要使用自定义反垃圾字段, 0:不需要,1:需要, 默认0
		antiSpamContent = 22, //(可选) String, 开发者自定义的反垃圾字段
		antiSpamConfigId = 23, //(可选))String, 用户配置的对某些单条消息另外的反垃圾的业务ID
		antiSpamUsingYidun = 25, // (可选) int，在已经开通易盾服务条件下，单条消息是否使用易盾。0：不使用，1，使用

		pushEnable = 107, //(可选) int, 是否需要推送, 0:不需要,1:需要,默认1
		needBadge = 109, //(可选) int, 是否要做消息计数，0:不需要，1:需要，默认1
		needPushNick = 110 //((可选) int, 需要推送昵称，0：不需要，1：需要，会话消息默认1，自定义系统通知默认0
	};
	class BodyCellArchive
	{		
	public:
		static inline std::string LogTokenToStringKey(LogToken token);
		static inline nim::IMMessage ToMessageLog(const std::string& data);
		static inline std::string ToString(const nim::IMMessage& msg);
		static inline nim::IMMessage ToMessageLog(const Json::Value& json_msg);
		static inline Json::Value ToJsonValue(const nim::IMMessage& msg);
	};
	class RawFileArchive
	{
	public:
		RawFileArchive(const std::string& file_path, const std::function<void(int current_count, int count)>& progress_cb);
		~RawFileArchive();
		bool ReadFromFile(const std::shared_ptr<MultiMessageRetweetRawFileInfo>& file_info);
		bool WriteToFile(const std::shared_ptr<MultiMessageRetweetRawFileInfo>& raw_file_info);
	private:
		std::function<void(int current_count, int count)> progress_cb_;
		std::string file_path_;
		std::ofstream o_stream_;
		std::string o_stream_buffer_;
		std::ifstream i_stream_;
	};
}
#include "ui_kit/module/multi_retweet/multi_message_retweet_body_cell_archive.inl"
