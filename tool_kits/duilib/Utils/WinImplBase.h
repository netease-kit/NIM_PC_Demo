#ifndef UI_UTILS_WINIMPLBASE_H_
#define UI_UTILS_WINIMPLBASE_H_

namespace ui
{
	#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
	#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

	#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
	#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)

	enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,				// 来自磁盘文件
		UILIB_ZIP,						// 来自磁盘zip压缩包
		UILIB_RESOURCE,			// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};

	class UILIB_API WindowImplBase
		: public Window
		, public IUIMessageFilter
	{
	public:
		WindowImplBase(){};
		virtual ~WindowImplBase(){}
		virtual void InitWindow(){}
		virtual void OnFinalMessage( HWND hWnd ) override;
		
		virtual UINT GetClassStyle() const;
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual std::wstring GetZIPFileName() const;
		virtual std::wstring GetResourceID() const;
		virtual Control* CreateControl(const std::wstring& pstrClass);
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
		virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		virtual LONG GetStyle();

	protected:
		virtual std::wstring GetSkinFolder() = 0;
		virtual std::wstring GetSkinFile() = 0;
		virtual std::wstring GetWindowClassName(void) const = 0 ;

		static LPBYTE m_lpResourceZIPBuffer;

	private:
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	//故意不弄成虚函数，需要定制处理使用InitWindow函数
		bool BtnClick(EventArgs* param);
	};
}

#endif // UI_UTILS_WINIMPLBASE_H_
