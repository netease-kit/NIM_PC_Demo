#pragma once
#include <string>
#include <list>
#include "json.h"
#include "base/util/string_number_conversions.h"
#include "shared/utf8_file_util.h"
#include "base/util/string_util.h"

namespace app_sdk
{
	enum MultiVchatMsgType
	{
		kMsgMultiVchatAudio = 2001,//语音
		kMsgMultiVchatVideo = 2002,//视频 发送群视频邀请
		kMsgMultiVchatAck = 2100,//消息2 收到被邀请方的同意，拒绝群视频邀请
		kMsgMultiVchatJoin = 2101,//消息3 给被邀请方发准许进入或失败的通知
		kMsgMultiVchatTeamChangeMode = 2102,//消息4 群消息，本人音视频模式切换通知
		kMsgMultiVchatP2PChangeMode = 2103,//消息5 单点通知音视频模式切换
	};

	enum SDK_TYPE
	{
		kLogin = 0,
		kFindPersion = 1,
		kMq = 2,
		kInitTeam = 3,
		kGetUnreadCount = 4,
		kGetReadList = 5,
		kUpdateTeamStatus = 6,
		kUpdateHeaderImg = 7,
		kUpdate = 8,
		kCreateTeamImg = 9
	};

	//登陆
	const static char *g_login_api = "/third/api/login.do?method=login";
	//查询好友
	const static char *g_find_person_api = "/etransfarOA/api/org/person/findPerson";
	//mq信息
	const static char *g_send_mq_api = "/etransfarOA/a/api/oa/im/sendImMqMessage";
	//初始化群信息
	const static char *g_init_team_info_api = "/etransfarOA/a/api/oa/im/initTeamInfo";
	//获取消息未读数
	const static char *g_get_unread_count_api = "/etransfarOA/a/api/oa/im/getUnreadCount";
	//已读未读列表
	const static char *g_get_read_list_api = "/etransfarOA/a/api/oa/im/getReadList";
	//群状态更新
	const static char *g_update_team_status_api = "/etransfarOA/a/api/oa/im/updateTeamStatus";
	//头像上传
	const static char *g_update_head_image_api = "/third/api/login.do?method=uploadHeadimage";
	//检测更新
	const static char *g_update_api = "/etransfarOA/api/update/pc";
	//获取群头像
	const static char *g_create_team_img_api = "/etransfarOA/a/api/oa/im/group/image/createGroupImage";
	static std::string UriEncode(const std::string & sSrc);
	struct ResponseBaseParam
	{
		int response_code_ = 0;
		int error_code_ = 0;
		std::string msg_;

		virtual void Release() { delete this; }
		bool Parse(const std::string& response)
		{
			Json::Reader reader;
			if (reader.parse(response, values_) && values_.isObject())
			{
				if (values_.isMember("code"))
					nbase::StringToInt(values_["code"].asString(), &error_code_);
				if (values_.isMember("msg"))
					msg_ = values_["msg"].asString();
				return error_code_ == 200;
			}
			return false;
		}
	protected:
		Json::Value values_;
	};

	struct RequestBaseParam
	{
		virtual void Release() { delete this; }
		virtual bool UsePostMethod(){ return true; }
		virtual std::string GetAPI(){ return ""; }
		template<typename TSubRequest>
		static auto CreateRequest()->std::shared_ptr<TSubRequest>
		{
			return std::make_shared<TSubRequest>();
		}
		virtual std::string ToJsonString()
		{
			Json::FastWriter fw;
			return fw.write(values_);
		}
	protected:
		virtual void AppendUrlParam(std::string& params, const std::string& key, const std::string& value)
		{
			if (key.empty() || value.empty())
				return;
			if (!params.empty() && *params.rbegin() != '&')
				params.append(1, '&');
			params.append(key).append(1, '=').append(UriEncode(value));
		}
		virtual void AppendUrlParam(std::string& params, const std::string& key, bool value)
		{
			AppendUrlParam(params, key, value ? std::string("true") : std::string("false"));			
		}
		virtual void AppendUrlParam(std::string& params, const std::string& key, int value)
		{
			AppendUrlParam(params, key, nbase::IntToString(value));
		}
	protected:
		Json::Value values_;	
	};

	// Only alphanum is safe.
	const char SAFE[256] =
	{
		/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
		/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

		/* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		/* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
		/* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		/* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

		/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		/* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	static std::string UriEncode(const std::string & sSrc)
	{
		const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
		const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
		const int SRC_LEN = (const int)sSrc.length();
		unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
		unsigned char * pEnd = pStart;
		const unsigned char * const SRC_END = pSrc + SRC_LEN;

		for (; pSrc < SRC_END; ++pSrc)
		{
			if (SAFE[*pSrc])
				*pEnd++ = *pSrc;
			else
			{
				// escape this char
				*pEnd++ = '%';
				*pEnd++ = DEC2HEX[*pSrc >> 4];
				*pEnd++ = DEC2HEX[*pSrc & 0x0F];
			}
		}

		std::string sResult((char *)pStart, (char *)pEnd);
		delete[] pStart;
		return sResult;
	}

	struct LoginRequestParam : RequestBaseParam
	{
		std::string user_name_;
		std::string password_;
		std::string version_ = "pc1.0";

		std::string ToJsonString()
		{
			std::string post = "userName=%s&passWord=%s&version=pc1.0";
			post = nbase::StringPrintf(post.c_str(), UriEncode(user_name_).c_str(), UriEncode(password_).c_str());
			return post;
		}
	};

	struct LoginResponseParam : ResponseBaseParam
	{
		std::string accid_;
		std::string im_token_;
		std::string token_;
		std::string permission_;
		bool Parse(const std::string& response)
		{
			std::wstring test = nbase::UTF8ToUTF16(response);
			if (__super::Parse(response))
			{
				accid_ = values_["data"]["accid"].asString();
				im_token_ = values_["data"]["imToken"].asString();
				token_ = values_["data"]["token"].asString();
				permission_ = values_["data"]["hasImPermission"].asString();
				return true;
			}
			return false;
		}
	};

	struct FindPersionRequestParam : RequestBaseParam
	{
		std::string page_no_;
		std::string page_size_;
		std::string inquired_;
		std::string ToJsonString()
		{
// 			values_["pageNo"] = page_no_;
// 			values_["pageSize"] = page_size_;
// 			values_["inquired"] = inquired_;
// 			return __super::ToJsonString();
			std::string post = "pageNo=%s&pageSize=%s&inquired=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(page_no_).c_str(), UriEncode(page_size_).c_str(), UriEncode(inquired_).c_str());
			return post;
		}
	};

	struct Person
	{
		std::string accid_;
		std::string number_;
		std::string dept_;
		std::string mobile_;
		bool		private_;
		std::string post_;
		std::string name_; 
		std::string work_phone_;
		std::string id_;
		std::string email_;
		std::string profile_;//相对路径，使用时需要拼接host

		void Parse(const Json::Value& value)
		{
			accid_ = value["accid"].asString();
			number_ = value["no"].asString();
			dept_ = value["dept"].asString();
			mobile_ = value["mobileNo"].asString();
			private_ = value["isContactPrivate"].asBool();
			post_ = value["post"].asString();
			name_ = value["name"].asString();
			work_phone_ = value["workPhone"].asString();
			id_ = value["id"].asString();
			email_ = value["email"].asString();
			profile_ = value["profile"].asString();
		}
	};

	struct FindPersionResponseParam : ResponseBaseParam
	{
		std::string msg_;
		int count_ = 0;
		std::list<Person> persons_;
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				nbase::StringToInt(values_["data"]["count"].asString(), &count_);
				Json::Value &lst = values_["data"]["list"];
				size_t sz = lst.size();
				for (int i = 0; i < (int)sz; i++)
				{
					Person ps;
					ps.Parse(lst[i]);
					persons_.push_back(ps);
				}
				return true;
			}
			return false;
		}
	};

	struct SendIMMqMsgRequestParam : RequestBaseParam
	{
		std::string from_id_;
		std::list<std::string> messages_;
		std::string tid_;
		std::string uid_;
		std::string ToJsonString()
		{
// 			std::string msgs;
// 			for (auto &msg : messages_)
// 			{
// 				msgs.append(msg);
// 				msgs.append(",");
// 			}
// 			if (!msgs.empty())
// 				msgs.pop_back();

// 			std::string post = "tid=%s&from=%s&uid=%s&mids=%s";
// 			post = nbase::StringPrintf(post.c_str(), UriEncode(tid_).c_str(), UriEncode(from_id_).c_str(), UriEncode(uid_).c_str(), UriEncode(msgs).c_str());
// 			return post;

			values_["from"] = from_id_;
			for (auto &msg : messages_)
			{
				values_["mids"].append(msg);
			}
// 			values_["mids"] = msgs;
			values_["tid"] = tid_;
			values_["uid"] = uid_;
			return __super::ToJsonString();
		}
	};

	struct InitTeamInfoRequestParam : RequestBaseParam
	{
		std::string team_id_;
		std::list<std::string> members_;
		std::string ToJsonString()
		{
			//values_["teamId"] = team_id_;
			std::string members;
			for (auto &member : members_)
			{
				members.append(member);
				members.append(",");
				//values_["members"].append(member);
			}
			if (!members.empty())
				members.pop_back();

			std::string post = "teamId=%s&members=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(team_id_).c_str(), UriEncode(members).c_str());
			return post;

// 			return __super::ToJsonString();
		}
	};

	struct GetUnreadCountRequestParam : RequestBaseParam
	{
		
		std::string team_id_;
		std::list<std::string> msgs_;
		std::string from_id_;
		std::string ToJsonString()
		{			
			std::string msgs;
			for (auto &msg : msgs_)
			{
				msgs.append(msg);
				msgs.append(",");
			}
			if (!msgs.empty())
				msgs.pop_back();

			std::string post = "tid=%s&from=%s&mids=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(team_id_).c_str(), UriEncode(from_id_).c_str(), UriEncode(msgs).c_str());
			return post;

// 			values_["tid"] = team_id_;
// 			for (auto &msg : msgs_)
// 			{
// 				values_["mids"].append(msg);
// 			}
// 			return __super::ToJsonString();
		}
	};

	struct GetUnreadCountResponseParam : ResponseBaseParam
	{
		int64_t server_id_ = 0;
		std::map<std::string, int> unread_count_;
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				server_id_ = values_["id"].asInt64();
				Json::Value &lst = values_["data"];
				size_t sz = lst.size();
				for (int i = 0; i < (int)sz; i++)
				{
					Json::Value &unread = lst[i];
					int count;
					nbase::StringToInt(unread["unreadCount"].asString(), &count);
					unread_count_[unread["mid"].asString()] = count;
				}
				return true;
			}
			return false;
		}
	};

	struct GetReadListRequestParam : RequestBaseParam
	{
		std::string msg_id_;
		std::string tid_;
		std::string from_id_;
		std::string ToJsonString()
		{
			std::string post = "tid=%s&from=%s&mid=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(tid_).c_str(), UriEncode(from_id_).c_str(), UriEncode(msg_id_).c_str());
			return post;

// 			values_["mid"] = msg_id_;
// 			values_["tid"] = tid_;
// 			values_["from"] = from_id_;
// 			return __super::ToJsonString();
		}
	};

	struct GetReadListResponseParam : ResponseBaseParam
	{
		std::string msg_id_;
		std::list<std::string> unreads_;
		std::list<std::string> reads_;
		int64_t server_id_ = 0;
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				server_id_ = values_["id"].asInt64();
				msg_id_ = values_["data"]["mid"].asString();
				Json::Value &unread_lst = values_["data"]["unread"];
				size_t sz = unread_lst.size();
				for (int i = 0; i < (int)sz; i++)
				{
					unreads_.push_back(unread_lst[i].asString());
				}
				Json::Value &read_lst = values_["data"]["read"];
				sz = read_lst.size();
				for (int i = 0; i < (int)sz; i++)
				{
					reads_.push_back(read_lst[i].asString());
				}
				return true;
			}
			return false;
		}
	};

	struct UpdateTeamStatusRequestParam : RequestBaseParam
	{
		std::string tid_;
		std::string ToJsonString()
		{
			std::string post = "teamId=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(tid_).c_str());
			return post;

// 			values_["teamId"] = tid_;
// 			return __super::ToJsonString();
		}
	};

	struct UploadHeadImageRequestParam : RequestBaseParam
	{
		std::string token_;
		std::string file_path_;
		std::string ToJsonString()
		{

			return __super::ToJsonString();
		}
	};

	struct UploadHeadImageResponseParam : ResponseBaseParam
	{
		std::string url_;
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				url_ = values_["data"]["fdHeadimageUrl"].asString();
				return true;
			}
			return false;
		}
	};

	struct UpdateRequestParam : RequestBaseParam
	{
		std::string version_;
		std::string ToJsonString()
		{
			std::string post = "version=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(version_).c_str());
			return post;

// 			values_["version"] = version_;
// 			return __super::ToJsonString();
		}
	};

	struct UpdateResponseParam : ResponseBaseParam
	{
		int result_ = 0;
		std::string remote_version_;
		std::string url_;
		std::string md5_;
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				result_ = values_["data"]["result"].asInt();
				remote_version_ = values_["data"]["version"].asString();
				url_ = values_["data"]["url"].asString();
				md5_ = values_["data"]["md5"].asString();
				return true;
			}
			return false;
		}
	};

	struct CreateTeamImgRequestParam : RequestBaseParam
	{
		std::string team_id_;
		std::list<std::string> members_;
		std::string owner_id_;
		std::string ToJsonString()
		{
			std::string members;
			for (auto &member : members_)
			{
				members.append(member);
				members.append(",");
			}
			if (!members.empty())
				members.pop_back();

			std::string post = "tid=%s&accids=%s&owner=%s";
			post = nbase::StringPrintf(post.c_str(), UriEncode(team_id_).c_str(), UriEncode(members).c_str(), UriEncode(owner_id_).c_str());
			return post;
		}
	};

	struct CreateTeamImgResponseParam : ResponseBaseParam
	{
		std::string url_;
		std::string tid_;
		std::string owner_;
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				url_ = values_["data"]["icon"].asString();
				tid_ = values_["data"]["tid"].asString();
				owner_ = values_["data"]["owner"].asString();
				return true;
			}
			return false;
		}
	};
	//通讯录节点信息
	struct AddresBookNode
	{
		AddresBookNode() : has_child_(false), hasImPermission_(false)
		{

		}
		std::string title_;
		bool hasImPermission_;
		std::string fdno_;//部门编号
		bool has_child_;//是否有子部门或成员	
		std::string parent_id_;//父部门ID
		std::string name_;
		std::string id_;//id
		std::string accid_;
	};
	//常用群组节点信息
	typedef AddresBookNode FrequentGroupNode;
	//struct FrequentGroupNode : public AddresBookNode
	//{

	//};
	//获取通讯录树型节构请求/应答
	struct GetAddresBookTreeStructRequestParam : RequestBaseParam
	{		
		std::string ToJsonString()
		{
			std::string post;
			AppendUrlParam(post, "fdId", department_id_);
			return post;
			//values_["fdId"] = department_id_;
			//return RequestBaseParam::ToJsonString();
		}
		virtual bool UsePostMethod(){ return true; }
		virtual std::string GetAPI(){ return "//third/api/contact.do?method=queryChildren"; }
		std::string department_id_;
	};
	struct GetAddresBookTreeStructResponseParam : ResponseBaseParam
	{		
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				if (values_["data"]["deptArraty"].isArray())
				{
					for (auto it : values_["data"]["deptArraty"])
					{
						AddresBookNode node;
						if (it.isMember("fdId"))
							node.id_ = it["fdId"].asString();
						if (it.isMember("parent"))
							node.has_child_ = it["parent"].asBool();
						if (it.isMember("fdName"))
							node.name_ = it["fdName"].asString();			
						node.hasImPermission_ = true;
						department_list_.emplace_back(node);						
					}
				}	
				if (values_["data"]["personArray"].isArray())
				{
					for (auto it : values_["data"]["personArray"])
					{
						AddresBookNode node;
						if (it.isMember("fdNo"))
							node.fdno_ = it["fdNo"].asString();					
						if (it.isMember("fdName"))
							node.name_ = it["fdName"].asString();
						if (it.isMember("fdDept"))
							node.parent_id_ = it["fdDept"].asString();
						if (it.isMember("fdId"))
							node.id_ = it["fdId"].asString();
						if (it.isMember("accid"))
							node.accid_ = it["accid"].asString();
						if (it.isMember("hasImPermission") && it["hasImPermission"].isBool())
							node.hasImPermission_ = it["hasImPermission"].asBool();
						contact_list_.emplace_back(node);
					}
				}
				return true;
			}
			return false;
		}
		std::list<AddresBookNode> department_list_;
		std::list<AddresBookNode> contact_list_;		
	};

	//获取常用群组树型节构请求/应答
	struct GetFrequentGroupTreeStructRequestParam : RequestBaseParam
	{
		std::string ToJsonString()
		{
			std::string post;
			AppendUrlParam(post, "fdId", department_id_);
			return post;
			//values_["fdId"] = department_id_;
			//return RequestBaseParam::ToJsonString();
		}
		virtual bool UsePostMethod(){ return true; }
		virtual std::string GetAPI(){ return "//third/api/contact.do?method=commonGroups"; }
		std::string department_id_;
	};
	struct GetFrequentGroupTreeStructResponseParam : ResponseBaseParam
	{
		bool Parse(const std::string& response)
		{
			if (__super::Parse(response))
			{
				if (values_["data"]["deptArraty"].isArray())
				{
					for (auto it : values_["data"]["deptArraty"])
					{
						AddresBookNode node;
						if (it.isMember("fdId"))
							node.id_ = it["fdId"].asString();
						if (it.isMember("parent"))
							node.has_child_ = it["parent"].asBool();
						if (it.isMember("fdName"))
							node.name_ = it["fdName"].asString();
						node.hasImPermission_ = true;
						department_list_.emplace_back(node);
					}
				}
				if (values_["data"]["personArray"].isArray())
				{
					for (auto it : values_["data"]["personArray"])
					{
						AddresBookNode node;
						if (it.isMember("fdNo"))
							node.fdno_ = it["fdNo"].asString();
						if (it.isMember("fdName"))
							node.name_ = it["fdName"].asString();
						if (it.isMember("fdDept"))
							node.parent_id_ = it["fdDept"].asString();
						if (it.isMember("fdId"))
							node.id_ = it["fdId"].asString();
						if (it.isMember("accid"))
							node.accid_ = it["accid"].asString();
						if (it.isMember("hasImPermission") && it["hasImPermission"].isBool())
							node.hasImPermission_ = it["hasImPermission"].asBool();
						contact_list_.emplace_back(node);
					}
				}
				return true;
			}
			return false;
		}
		std::list<FrequentGroupNode> department_list_;
		std::list<FrequentGroupNode> contact_list_;
	};
}