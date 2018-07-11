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
	COLORREF  m_TextColor;   //������ɫ
	COLORREF  m_BkColor;     //������ɫ
	HICON     m_hIcon;       //����״̬����ʾ��ͼ��
	HICON     m_hIconMouse;  //����ڰ�ť�ϵ�ͼ��
	HICON     m_hIconOut;    //���°�ťʱ��ʾ��ͼ��
	long      m_bMouseOnButton;//����Ƿ��ڰ�ť��
	long      m_TimeID; //ʱ��ID
	long      m_IocCenterOrLeft;//��ͼ��λ��

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
