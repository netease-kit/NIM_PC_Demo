#pragma once

namespace nim_comp
{
	class CustomButtonBox : public ui::ButtonBox
	{
	public:
		CustomButtonBox() {}
		~CustomButtonBox() {}

		virtual void HandleMessage(ui::EventArgs& event) override;

	private:

	};
}