#ifndef UI_CONTROL_SCROLLBAR_H_
#define UI_CONTROL_SCROLLBAR_H_

#pragma once

namespace ui
{
	class UILIB_API ScrollBar : public Control
	{
	public:
		ScrollBar();

		Box* GetOwner() const;
		void SetOwner(ScrollableBox* pOwner);

		void SetEnabled(bool bEnable = true);
		void SetFocus();
		virtual void SetVisible_(bool bVisible) override;
		virtual bool ButtonUp(EventArgs& msg) override;

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		bool IsValid()
		{
			return GetScrollRange() != 0;
		}
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos);
		int GetLineSize() const;
		void SetLineSize(int nSize);
		int GetThumbMinLength() const;
		void SetThumbMinLength(int nThumbMinLength);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		std::wstring GetButton1StateImage(ControlStateType stateType);
		void SetButton1StateImage(ControlStateType stateType, const std::wstring& pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		std::wstring GetButton2StateImage(ControlStateType stateType);
		void SetButton2StateImage(ControlStateType stateType, const std::wstring& pStrImage);

		std::wstring GetThumbStateImage(ControlStateType stateType);
		void SetThumbStateImage(ControlStateType stateType, const std::wstring& pStrImage);

		std::wstring GetRailStateImage(ControlStateType stateType);
		void SetRailStateImage(ControlStateType stateType, const std::wstring& pStrImage);

		std::wstring GetBkStateImage(ControlStateType stateType);
		void SetBkStateImage(ControlStateType stateType, const std::wstring& pStrImage);

		bool IsAutoHideScroll(){return m_bAutoHide;}
		void SetAutoHideScroll(bool hide);

		void SetPos(UiRect rc);
		void HandleMessage(EventArgs& event);
		bool MouseEnter(EventArgs& msg);
		bool MouseLeave(EventArgs& msg);
		void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue);

		void Paint(HDC hDC, const UiRect& rcPaint) override;

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

		ControlStateType GetThumbState()
		{
			return m_uThumbState;
		}

	private:
		void ScrollBar::ScrollTimeHandle();

	protected:
		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
		};

		bool m_bHorizontal;
		bool m_bShowButton1;
		bool m_bShowButton2;
		bool m_bAutoHide;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		int m_nThumbMinLength;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;
		ScrollableBox* m_pOwner;
		POINT ptLastMouse;
		UiRect m_rcButton1;
		UiRect m_rcButton2;
		UiRect m_rcThumb;
		ControlStateType m_uButton1State;
		ControlStateType m_uButton2State;
		ControlStateType m_uThumbState;
		std::wstring m_sImageModify;
		StateImage m_bkStateImage;
		StateImage m_button1StateImage;
		StateImage m_button2StateImage;
		StateImage m_thumbStateImage;
		StateImage m_railStateImage;
		nbase::WeakCallbackFlag weakFlagOwner;
	};
}

#endif // UI_CONTROL_SCROLLBAR_H_