#ifndef UI_UTILS_ONSCREENKEYBOARDMANAGER_H_
#define UI_UTILS_ONSCREENKEYBOARDMANAGER_H_

#pragma once

namespace ui
{
/** @class OnScreenKeyboardManager
  * @brief Dé?a?ü?ì1üàíàà
  * @copyright (c) 2018, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2018/08/28
  */
class UILIB_API OnScreenKeyboardManager : public virtual nbase::SupportWeakCallback
{
public:
	static OnScreenKeyboardManager* GetInstance();

	/**
	  * Dé?a?ü?ìê?·??y?ú??ê?
	  * @return bool
	  */
	bool IsVisible();

	/**
	  * ??ê?Dé?a?ü?ì
	  * @param[in] show ê?·???ê?
	  * @return void
	  */
	void ShowOSK(bool show);

private:
	OnScreenKeyboardManager();
	~OnScreenKeyboardManager() {};
	OnScreenKeyboardManager(const OnScreenKeyboardManager&) = delete;
	OnScreenKeyboardManager& operator = (const OnScreenKeyboardManager&) = delete;

	void TimerCallback();
	void StartTabTip();

private:
	bool m_bShow = false;
};
}
#endif //UI_UTILS_ONSCREENKEYBOARDMANAGER_H_