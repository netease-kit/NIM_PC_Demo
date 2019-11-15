#pragma once
namespace nim_comp
{
	class MultiMessageRetweetData
	{
	public:
		class MessageAbstract
		{
		public:
			MessageAbstract(const std::string& sender, const std::string& message);
			MessageAbstract(std::string&& sender, std::string&& message);
			~MessageAbstract() = default;
		public:
			std::string sender_;
			std::string message_;
		};
		using MessageAbstractList = std::list< MessageAbstract>;
	public:
		MultiMessageRetweetData();
		~MultiMessageRetweetData() = default;
		void ToJsonOjbect(Json::Value& json_data);
		void FromJsonObject(const Json::Value& json_data);
		void FillAbstract(const std::list<nim::IMMessage>& message_list);
	public:
		static const int abstract_list_length_;
		static const int abstract_message_max_length_;
		std::string session_id_;
		std::string session_name_;
		std::string url_;
		std::string md5_;
		bool compressed_;
		bool encrypted_;
		std::string password_;
		MessageAbstractList msg_abstract_list_;
	};
}