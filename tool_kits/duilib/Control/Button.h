#ifndef UI_CONTROL_BUTTON_H_
#define UI_CONTROL_BUTTON_H_

#pragma once

namespace ui
{


template<typename InheritType = Control>
class UILIB_API ButtonTemplate : public LabelTemplate<InheritType>
{
public:
	ButtonTemplate();

	virtual void Activate() override;
	virtual void HandleMessage(EventArgs& event) override;

	void AttachClick(const EventCallback& callback)
	{
		OnEvent[kEventClick] += callback;
	}

};


#include "ButtonImpl.h"

typedef ButtonTemplate<Control> Button;
typedef ButtonTemplate<Box> ButtonBox;



}	// namespace ui

#endif // UI_CONTROL_BUTTON_H_