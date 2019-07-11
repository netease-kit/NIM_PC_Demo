#pragma once
namespace nim_comp
{
	class IProfileUI
	{
	public:
		/**
		* 初始化用户信息，只能使用一次，否则会多次注册回调
		* @param[in] info 用户名片
		* @return void	无返回值
		*/
		virtual void InitUserInfo(const nim::UserNameCard & info) = 0;

		/**
		* 获取所展示名片信息
		* @return UserNameCard	名片信息
		*/
		virtual nim::UserNameCard	GetNameCard() const = 0;
		/**
		* 设置标题栏标题
		* @param[in] title 标题栏标题
		* @return void	无返回值
		*/
		virtual void SetTaskbarTitle(const std::wstring &title) = 0;
		virtual void Close(UINT nRet = 0) = 0;
		/**
		* 响应多端推送配置改变的回调函数
		* @param[in] switch_on 是否开启多端推送
		* @return void 无返回值
		*/
		virtual void OnMultiportPushConfigChange(bool switch_on) = 0;
	};
}