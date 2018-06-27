#pragma once
namespace nim_comp
{
	class ITeamInfoUI
	{
	public:
		/**
		* 设置标题栏标题
		* @param[in] title 标题栏标题
		* @return void	无返回值
		*/
		virtual void SetTaskbarTitle(const std::wstring &title) = 0;
		virtual void Close(UINT nRet = 0) = 0;
		virtual void ActiveWindow() = 0;
		/**
		* 更新群成员信息
		* @return void	无返回值
		*/
		virtual void UpdateTeamMember() = 0;
	};
}