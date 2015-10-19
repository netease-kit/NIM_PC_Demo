#ifndef UI_CORE_TILEBOX_H_
#define UI_CORE_TILEBOX_H_

#pragma once

namespace ui
{
	class UILIB_API TileLayout : public Layout
	{
	public:
		TileLayout();
		virtual CSize ArrangeChild(const std::vector<Control*>& m_items, UiRect rc) override;
		virtual CSize AjustSizeByChild(const std::vector<Control*>& m_items, CSize szAvailable) override;

		virtual bool SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;

		CSize GetItemSize() const;
		void SetItemSize(CSize szItem);
		int GetColumns() const;
		void SetColumns(int nCols);

	protected:
		int m_nColumns;
		CSize m_szItem;
	};

	class UILIB_API TileBox : public Box
	{
	public:
		TileBox();
	};
}
#endif // UI_CORE_TILEBOX_H_
