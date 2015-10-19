#ifndef UI_CORE_DEFINE_H_
#define UI_CORE_DEFINE_H_

#pragma once


namespace ui
{

class Control;

#define DUI_NOSET_VALUE  -1
#define DUI_LENGTH_STRETCH  -1
#define DUI_LENGTH_AUTO  -2

enum HorAlignType
{
	kHorAlignLeft,
	kHorAlignCenter,
	kHorAlignRight,
};

enum VerAlignType
{
	kVerAlignTop,
	kVerAlignCenter,
	kVerAlignBottom,
};

enum StateImageType
{
	kStateImageBk,
	kStateImageFore,
	kStateImageSelectedBk,
	kStateImageSelectedFore,
};

// Flags used for controlling the paint
enum ControlStateType
{
	kControlStateNormal,
	kControlStateHot,
	kControlStatePushed,
	kControlStateDisabled
};

enum AnimationType
{
	kAnimationNone,
	kAnimationAlpha,
	kAnimationHeight,
	kAnimationWidth,
	kAnimationHot,
	kAnimationInoutXFromLeft,
	kAnimationInoutXFromRight,
	kAnimationInoutYFromTop,
	kAnimationInoutYFromBottom,
};

enum GifStopType
{
	kGifStopFirst = 0,//gif 停止时定位到第一帧
	kGifStopCurrent,//当前帧
	kGifStopLast,//最后一帧
};

enum CursorType
{
	kCursorArrow,
	kCursorHand,
	kCursorHandIbeam
};

//定义所有消息类型
enum EventType
{
	kEventInternalDoubleClick,
	kEventInternalMenu,
	kEventInternalSetFocus,
	kEventInternalKillFocus ,

	kEventNone,

	kEventFirst,

	kEventAll,

	kEventKeyBegin,
	kEventKeyDown,
	kEventKeyUp,
	kEventChar,
	kEventSystemKey,
	kEventKeyEnd,

	kEventMouseBegin,
	kEventMouseMove,
	kEventMouseEnter,
	kEventMouseLeave,
	kEventMouseHover,
	kEventMouseButtonDown,
	kEventMouseButtonUp,
	kEventMouseRightButtonDown,
	kEventMouseDoubleClick,
	kEventMouseMenu,
	kEventMouseScrollWheel,
	kEventMouseEnd,

	kEventSetFocus,
	kEventKillFocus,
	kEventWindowSize,
	kEventSetCursor,

	kEventClick,
	kEventSelect,
	kEventUnSelect,
	kEventTextChange,
	kEventCustomLinkClick,

	kEventScrollChange,

	kEventValueChange,
	kEventReturn,
	kEventTab,
	kEventWindowClose,

	kEventResize,

	kEventNotify,	//仅作简单的通知，有复杂数据请通过其他方式实现

	kEventLast,
};


#define	EVENTSTR_ALL				(_T("all"))
#define	EVENTSTR_KEYDOWN			(_T("keydown"))
#define	EVENTSTR_KEYUP				(_T("keyup"))
#define	EVENTSTR_CHAR				(_T("char"))
#define	EVENTSTR_SYSKEY				(_T("syskey"))

#define EVENTSTR_SETFOCUS			(_T("setfocus"))
#define EVENTSTR_KILLFOCUS			(_T("killfocus"))
#define	EVENTSTR_SETCURSOR			(_T("setcursor"))

#define EVENTSTR_MOUSEMOVE			(_T("mousemove"))
#define	EVENTSTR_MOUSEENTER			(_T("mouseenter"))
#define	EVENTSTR_MOUSELEAVE			(_T("mouseleave"))
#define	EVENTSTR_MOUSEHOVER			(_T("mousehover"))

#define	EVENTSTR_BUTTONDOWN			(_T("buttondown"))
#define	EVENTSTR_BUTTONUP			(_T("buttonup"))
#define	EVENTSTR_RBUTTONDOWN		(_T("rbuttondown"))
#define	EVENTSTR_DOUBLECLICK		(_T("doubleclick"))

#define EVENTSTR_SELECT				(_T("select"))
#define EVENTSTR_UNSELECT			(_T("unselect"))
#define	EVENTSTR_MENU				(_T("menu"))

#define	EVENTSTR_SCROLLWHEEL		(_T("scrollwheel"))
#define EVENTSTR_SCROLLCHANGE		(_T("scrollchange"))

#define EVENTSTR_VALUECHANGE		(_T("valuechange"))
#define EVENTSTR_RETURN				(_T("return"))
#define EVENTSTR_TAB				(_T("tab"))
#define EVENTSTR_WINDOWCLOSE		(_T("windowclose"))



struct EventArgs
{
	EventArgs()
	: Type(kEventNone),
     pSender(nullptr),
	 dwTimestamp(0),
	 chKey(0),
	 wKeyState(0),
	 wParam(0),
	 lParam(0)
	{
		ptMouse.x = ptMouse.y = 0;
	}

	EventType Type;
	Control* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	TCHAR chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;
};

EventType StringToEnum(const std::wstring& messageType);


//定义所有控件类型
#define  DUI_CTR_CONTROL                         (_T("Control"))
#define  DUI_CTR_LABEL                           (_T("Label"))
#define  DUI_CTR_BUTTON                          (_T("Button"))
#define  DUI_CTR_TEXT                            (_T("Text"))
#define  DUI_CTR_OPTION                          (_T("Option"))
#define  DUI_CTR_CHECKBOX                        (_T("CheckBox"))

#define  DUI_CTR_LABELBOX                        (_T("LabelBox"))
#define  DUI_CTR_BUTTONBOX                       (_T("ButtonBox"))
#define  DUI_CTR_OPTIONBOX                       (_T("OptionBox"))
#define  DUI_CTR_CHECKBOXBOX                     (_T("CheckBoxBox"))

#define  DUI_CTR_BOX							 (_T("Box"))
#define  DUI_CTR_HBOX							 (_T("HBox"))
#define  DUI_CTR_VBOX                            (_T("VBox"))
#define  DUI_CTR_TABBOX                          (_T("TabBox"))
#define  DUI_CTR_TILEBOX						 (_T("TileBox"))
#define  DUI_CTR_CHILDBOX                        (_T("ChildBox"))

#define  DUI_CTR_LISTCONTAINERELEMENT            (_T("ListContainerElement"))
#define  DUI_CTR_LISTBOX						 (_T("ListBox"))
#define  DUI_CTR_HLISTBOX						 (_T("HListBox"))
#define  DUI_CTR_VLISTBOX                        (_T("VListBox"))
#define  DUI_CTR_TILELISTBOX                     (_T("TileListBox"))

#define  DUI_CTR_TREENODE                        (_T("TreeNode"))
#define  DUI_CTR_TREEVIEW                        (_T("TreeView"))

#define  DUI_CTR_RICHEDIT                        (_T("RichEdit"))
#define  DUI_CTR_COMBO                           (_T("Combo"))
#define  DUI_CTR_SLIDER                          (_T("Slider"))
#define  DUI_CTR_PROGRESS                        (_T("Progress"))
#define  DUI_CTR_SCROLLBAR                       (_T("ScrollBar"))

}// namespace ui

#endif // UI_CORE_DEFINE_H_