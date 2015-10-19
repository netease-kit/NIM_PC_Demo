#ifndef UI_CORE_CHILDBOX_H_
#define UI_CORE_CHILDBOX_H_

#pragma once

namespace ui
{
	class UILIB_API ChildBox : public Box
	{
	public:
		ChildBox();

		void Init();
		virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;
		void SetChildLayoutXML(std::wstring pXML);
		std::wstring GetChildLayoutXML();

	private:
		std::wstring m_pstrXMLFile;
	};
} // namespace ui
#endif // UI_CORE_CHILDBOX_H_
