#if !defined(AFX_ICOCBUTTON_H__4C566D9B_54E3_4FBE_A0C7_1EC18740CEF7__INCLUDED_)
#define AFX_ICOCBUTTON_H__4C566D9B_54E3_4FBE_A0C7_1EC18740CEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcoCButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IcoCButton window

class IcoCButton : public CButton
{
private:
	COLORREF  m_TextColor;   //文字颜色
	COLORREF  m_BkColor;     //背景颜色
	HICON     m_hIcon;       //正常状态下显示的图标
	HICON     m_hIconMouse;  //鼠标在按钮上的图标
	HICON     m_hIconOut;    //按下按钮时显示的图标
	long      m_bMouseOnButton;//鼠标是否在按钮上
	long      m_TimeID; //时间ID
	long      m_IocCenterOrLeft;//画图标位置

private:
	void  OnDrawBorder(CDC* pDC,long OnBit);
	void  OnDrawIco(CDC* pDC,HICON In_hIcon,long On);
	void  OnDrawText(CDC* pDC,HICON In_hIcon,long On);
// Construction
public:
	IcoCButton();

// Attributes
public:
	void  SetIcon(int nIconIn,int nIcon2,int nIconOut);
	void  SetBkColor(COLORREF BkColor,long li = 1);
	void  SetTextColor(COLORREF TextColor,long li = 1);
	void  SetIocCenterOrLeft(long InCenterOrLeft);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IcoCButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~IcoCButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(IcoCButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICOCBUTTON_H__4C566D9B_54E3_4FBE_A0C7_1EC18740CEF7__INCLUDED_)
