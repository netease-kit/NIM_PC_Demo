#pragma once
#include "ui_kit_tray_icon.h"
namespace nim_comp
{
	class ITrayIconEventHandler : public virtual nbase::SupportWeakCallback
	{
	public:
		virtual bool LeftClick(){ return OnLeftClick(); }
		virtual bool LeftDoubleClick(){ return OnLeftDoubleClick(); }
		virtual bool RightClick(){ return OnRightClick(); }
		virtual bool RightDoubleClick(){ return OnRightClick(); }
	protected:
		virtual bool OnLeftClick(){ return false; }
		virtual bool OnLeftDoubleClick(){ return false; }
		virtual bool OnRightClick(){ return false; }
		virtual bool OnRightDoubleClick(){ return false; }
	};
	/** @class TrayIconManager
	* @brief 托盘图标管类
	* @copyright (c) 2015, NetEase Inc. All rights reserved
	* @date 2016/10/12
	*/
	class TrayIconManager : public ITrayIconDelegate
	{
	public:
		SINGLETON_DEFINE(TrayIconManager);
		TrayIconManager();
		virtual ~TrayIconManager();

	public:
		void AddTrayIconEventHandler(ITrayIconEventHandler* handler);
		/**
		* 销毁托盘图标
		* @return void	无返回值
		*/
		void Destroy();

		/**
		* 设置托盘图标和提示文本
		* @param[in] icon 图标句柄
		* @param[in] tray_icon_text 提示文本
		* @return void	无返回值
		*/
		void SetTrayIcon(HICON icon, const std::wstring& tray_icon_text);

		/**
		* 从资源中加载一组图标资源并缓存起来
		* @param[in] icon_res_start_index 要图标资源的第一个资源的ID
		* @param[in] count 要加载的图标的数量
		* @return void	无返回值
		*/
		void LoadIconList(int icon_res_start_index, int count);

		/**
		* 设置托盘动画中，用于动画效果图标帧的索引号
		* @param[in] aniIndex 图标帧索引号数组
		* @param[in] anim_escape_time 动画间隔时间
		* @return void	无返回值
		*/
		void SetAnimateTray(const std::vector<int>& aniIndex, int anim_escape_time);

		/**
		* 开始托盘图标动画效果
		* @return void	无返回值
		*/
		void StartTrayIconAnimate();

		/**
		* 停止托盘图标动画效果
		* @return void	无返回值
		*/
		void StopTrayIconAnimate();

		/**
		* 获取任务栏图标创建完成的消息的消息值
		* @return UINT 消息值
		*/
		UINT GetTaskbarCreatedMsgId();

		virtual void LeftClick() override;
		virtual void LeftDoubleClick() override;
		virtual void RightClick() override;
		virtual void RightDoubleClick() override;
	private:
		std::unique_ptr<TrayIcon> tray_icon_;
		std::list<std::pair<nbase::SupportWeakCallback::_TyWeakFlag, ITrayIconEventHandler*>> handler_list_;
	};
}

