#ifndef UI_CONTROL_RICHEDIT_H_
#define UI_CONTROL_RICHEDIT_H_

#pragma once

#include "base/base_export.h"
#include <Imm.h>
#pragma comment(lib,"imm32.lib")
#include <textserv.h>
#include <richedit.h>
#include <RichOle.h>

namespace ui {

class CTxtWinHost;

class UILIB_API RichEdit : public ScrollableBox, public IUIMessageFilter
{
public:
    RichEdit();
    ~RichEdit();

    bool IsWantTab();
    void SetWantTab(bool bWantTab = true);
    bool IsNeedReturnMsg();
    void SetNeedReturnMsg(bool bNeedReturnMsg = true);
    bool IsReturnMsgWantCtrl();
    void SetReturnMsgWantCtrl(bool bReturnMsgWantCtrl = true);
    bool IsRich();
    void SetRich(bool bRich = true);
    bool IsReadOnly();
    void SetReadOnly(bool bReadOnly = true);
	bool IsPassword();
	void SetPassword(bool bPassword);
    bool GetWordWrap();
    void SetWordWrap(bool bWordWrap = true);
    int GetFont();
    void SetFont(int index);
    void SetFont(const std::wstring& pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    LONG GetWinStyle();
    void SetWinStyle(LONG lStyle);
	virtual void SetTextColor(const std::wstring& dwTextColor);
	std::wstring GetTextColor();
    int GetLimitText();
    void SetLimitText(int iChars);
    long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
    std::wstring GetText() const;
	virtual std::string GetUTF8Text() const;
    void SetText(const std::wstring& pstrText);
	virtual void SetUTF8Text(const std::string& pstrText);
    bool GetModify() const;
    void SetModify(bool bModified = true) const;
    void GetSel(CHARRANGE &cr) const;
    void GetSel(long& nStartChar, long& nEndChar) const;
    int SetSel(CHARRANGE &cr);
    int SetSel(long nStartChar, long nEndChar);
    void ReplaceSel(const std::wstring& lpszNewText, bool bCanUndo);
    void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
    std::wstring GetSelText() const;
    int SetSelAll();
    int SetSelNone();
    WORD GetSelectionType() const;
    bool GetZoom(int& nNum, int& nDen) const;
    bool SetZoom(int nNum, int nDen);
    bool SetZoomOff();
    bool GetAutoURLDetect() const;
    bool SetAutoURLDetect(bool bAutoDetect = true);
    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);
    std::wstring GetTextRange(long nStartChar, long nEndChar) const;
    void HideSelection(bool bHide = true, bool bChangeStyle = false);
    void ScrollCaret();
    int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
    int AppendText(const std::wstring& lpstrText, bool bCanUndo = false);
    DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
    bool SetDefaultCharFormat(CHARFORMAT2 &cf);
    DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
    bool SetSelectionCharFormat(CHARFORMAT2 &cf);
    bool SetWordCharFormat(CHARFORMAT2 &cf);
    DWORD GetParaFormat(PARAFORMAT2 &pf) const;
    bool SetParaFormat(PARAFORMAT2 &pf);
    bool Redo();
    bool Undo();
    void Clear();
    void Copy();
    void Cut();
    void Paste();
    int GetLineCount() const;
    std::wstring GetLine(int nIndex, int nMaxLength) const;
    int LineIndex(int nLine = -1) const;
    int LineLength(int nLine = -1) const;
    bool LineScroll(int nLines, int nChars = 0);
	CPoint GetCharPos(long lChar) const;
    long LineFromChar(long nIndex) const;
    CPoint PosFromChar(UINT nChar) const;
	int CharFromPos(CPoint pt) const;
    void EmptyUndoBuffer();
    UINT SetUndoLimit(UINT nLimit);
    long StreamIn(int nFormat, EDITSTREAM &es);
    long StreamOut(int nFormat, EDITSTREAM &es);

    void DoInit();
    // 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
    // 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
	bool SetDropAcceptFile(bool bAccept);
    virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
	void SetTimer(UINT idTimer, UINT uTimeout); 
	void KillTimer(UINT idTimer); 
    IDropTarget* GetTxDropTarget();
    virtual bool OnTxTextChanged();
    virtual void OnTxNotify(DWORD iNotify, void *pv);

    void SetScrollPos(CSize szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();

	virtual CSize EstimateSize(CSize szAvailable) override;
	virtual void SetPos(UiRect rc) override;
	virtual void HandleMessage(EventArgs& event) override;
	virtual void Paint(HDC hDC, const UiRect& rcPaint) override;
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true) override;
	virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	BOOL CreateCaret(INT xWidth, INT yHeight);
	BOOL ShowCaret(BOOL fShow);
	BOOL SetCaretPos(INT x, INT y);
	RECT GetCaretRect();
	void ChangeCaretVisiable();

	ITextHost * GetTextHost();
	ITextServices * GetTextServices();
	BOOL SetOleCallback(IRichEditOleCallback* pCallback);
	BOOL CanPaste(UINT nFormat = 0);
	void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0);

	CSize GetNaturalSize(LONG width, LONG height);

	virtual void SetEnabled(bool bEnable = true);

	void SetImmStatus(BOOL bOpen);

	void SetPromptMode(bool prompt);
	std::wstring GetPromptText() const;
	std::string GetUTF8PromptText() const;
	void SetPromptText(const std::wstring& pstrText);
	void SetUTF8PromptText(const std::string& pstrText);
	void SetPromptTextId(const std::wstring& strTextId);
	void SetUTF8PromptTextId(const std::string& strTextId);

	void PaintPromptText(HDC hDC);

	std::wstring GetFocusedImage();
	void SetFocusedImage(const std::wstring& pStrImage);

	virtual void PaintStatusImage(HDC hDC) override;

	void SetNoSelOnKillFocus(bool no_sel);
	void SetSelAllOnFocus(bool sel_all);
	void SetNoCaretReadonly(); //只读模式不要光标

	void AddColorText(const std::wstring &str, const std::wstring &color);
	void AddLinkColorText(const std::wstring &str, const std::wstring &color, const std::wstring &link_info = L"");
	//根据point来hittest自定义link的数据，返回true表示在link上，info是link的自定义属性
	bool HittestCustomLink(CPoint pt, std::wstring& info);

	VerAlignType GetTextVerAlignType()
	{
		return m_textVerAlignType;
	}


	void AttachReturn(const EventCallback& callback)
	{
		OnEvent[kEventReturn] += callback;
	}

	void AttachTab(const EventCallback& callback)
	{
		OnEvent[kEventTab] += callback;
	}

	void AttachTextChange(const EventCallback& callback)
	{
		OnEvent[kEventTextChange] += callback;
	}
	void AttachCustomLinkClk(const EventCallback& callback)
	{
		OnEvent[kEventCustomLinkClick] += callback;
	}

protected:
	void PaintCaret(HDC hDC, const UiRect& rcPaint);

protected:
    CTxtWinHost* m_pTwh;
    bool m_bVScrollBarFixing;
    bool m_bWantTab;
    bool m_bNeedReturnMsg;
    bool m_bReturnMsgWantCtrl;
    bool m_bRich;
    bool m_bReadOnly;
	bool m_bPassword;
    bool m_bWordWrap;
	bool m_bNumberOnly;
	bool m_bInited;
	bool m_bAllowPrompt;
	bool m_bSelAllEver;		 //只在第一次时全选
	bool m_bNoSelOnKillFocus; //针对 m_bEnabled && m_bReadOnly
	bool m_bSelAllOnFocus;	 //针对 m_bEnabled && !m_bReadOnly
	bool m_bNoCaretReadonly;
	bool m_bIsCaretVisiable;
	bool m_bIsComposition;
	int	 m_iCaretPosX;
	int  m_iCaretPosY;
	int  m_iCaretWidth;
	int  m_iCaretHeight;
	int  m_iFont;
	int  m_iLimitText;
	LONG m_lTwhStyle;
	VerAlignType m_textVerAlignType;
	std::wstring m_dwCurrentColor;
	std::wstring m_dwTextColor;
	std::wstring m_dwDisabledTextColor;
	std::wstring m_dwPromptColor;
	std::wstring m_sText;
	std::wstring m_sPromptText;
	std::wstring m_sPromptTextId;
	nbase::WeakCallbackFlag drawCaretFlag;
	std::weak_ptr<nbase::WeakFlag> windowFlag; //记录所属窗体的flag

private:
	struct LinkInfo
	{
		CHARRANGE cr_;
		std::wstring info_;
	};
	std::map<UINT, nbase::WeakCallbackFlag> m_timeFlagMap;
	std::vector<LinkInfo> m_linkInfo_;
	Image m_sFocusedImage;
};

//判断是否是字节： 可打印字符（0x20-0x7e）
bool IsAsciiChar(const wchar_t ch);
//获取字符串的字节数
int  GetAsciiCharNumber(const std::wstring &str);
//删除字符串中超过limit字节个数之后的字符
void LimitAsciiNumber(std::wstring &src, int limit);
//获取粘贴板字符串
void GetClipboardText(std::wstring &out);
//设置粘贴板字符串
void SetClipBoardText(const std::wstring &str);

} // namespace ui

#endif // UI_CONTROL_RICHEDIT_H_
