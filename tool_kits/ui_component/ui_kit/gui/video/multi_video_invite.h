#pragma once
#include "module/session/ring.h"
namespace nim_comp
{
#define MULTI_VCHAT_INVITE_TIMEOUT 45
	class MultiVchatInfo
	{
	public:
		std::string team_id_;
		uint64_t channel_id_;
		std::string creator_id_;
		std::string team_name_;
		std::string room_name_;
		int member_count_;
		MultiVchatInfo() 
		{
			member_count_ = 0;
		}
	};

	class MultiVideoInviteForm :public WindowEx
	{

	public:
		MultiVideoInviteForm(MultiVchatInfo vchat_info);
		~MultiVideoInviteForm();

		// 覆盖虚函数
		virtual std::wstring GetSkinFolder() override;
		virtual std::wstring GetSkinFile() override;
		virtual std::wstring GetWindowClassName() const override { return kClassName; };
		virtual std::wstring GetWindowId() const override { return kClassName; };
		virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

		/**
		* 窗口初始化函数
		* @return void	无返回值
		*/
		virtual void InitWindow() override;

		/**
		* 拦截并处理底层窗体消息
		* @param[in] uMsg 消息类型
		* @param[in] wParam 附加参数
		* @param[in] lParam 附加参数
		* @return LRESULT 处理结果
		*/
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		/**
		* 处理窗口被销毁的消息
		* @param[in] hWnd 窗口句柄
		* @return void	无返回值
		*/
		virtual void OnFinalMessage(HWND hWnd) override;

		/** 
		* 获取群聊信息
		* @return MultiVchatInfo 群聊信息
		*/
		MultiVchatInfo GetVchatInfo() { return vchat_info_; }

		void AddInviteMembers(std::set<std::string> uids);

	private:
		/**
		* 处理所有控件的所有消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool Notify(ui::EventArgs* msg);

		/**
		* 处理所有控件单击消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnClicked(ui::EventArgs* msg);

		/**
		* 发送群聊接听通知
		* @param[in]accept
		*/
		void SendMultiVchatNotify(bool accept);

		/**
		* 设置邀请界面头像信息
		*/
		void CheckHeadIcon();


		/**
		* 初始化控件信息
		*/
		void InitControl();

		/**
		* 跳转到会话页面
		*/
		void JumpSession();

		/**
		* 邀请超时控制
		*/
		void InviteTimeOut();

		/** 
		*  成员信息更改回调处理
		*/
		void OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos);

		void SetInviteName();

		
	public:
		static const LPCTSTR kClassName;
		MultiVchatInfo vchat_info_;

	private:
		nbase::WeakCallbackFlag invite_game_timer_;
		Ring	  voip_ring_;
		

	private:
		std::set<std::string> invite_members_;
		ui::Button* btn_jump_session_;
		ui::Button* btn_accept_;
		ui::Button* btn_refuse_;
		ui::Button*	headicon_btn_;
		ui::Label* lb_multi_vchat_member_info_;
		ui::Label* lb_multi_vchat_member_count_;
		ui::Label* lb_multi_vchat_inviter_;
		AutoUnregister	unregister_cb_;
	};
}