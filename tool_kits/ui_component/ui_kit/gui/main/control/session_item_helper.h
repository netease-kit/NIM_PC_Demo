#pragma once
namespace nim_comp
{
	class SessionItemHelper
	{
	public:
		static void GetMsgContent(const nim::SessionData &msg, std::wstring &show_text);
	};
}
