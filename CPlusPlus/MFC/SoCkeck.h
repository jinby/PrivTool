#if !defined(AFX_SOCKECK_H__12E4D1BD_426B_4FF4_8D52_A83C24DE785B__INCLUDED_)
#define AFX_SOCKECK_H__12E4D1BD_426B_4FF4_8D52_A83C24DE785B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoCkeck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoCkeck window

class SoCkeck : public CButton
{
	COLORREF m_crBackGnd;//����ɫ
	COLORREF m_crText;//������ɫ
	CBrush m_brBackGnd;//������ˢ

public:
	void SetTextColor(COLORREF crColor);//�����ı���ɫ
	void SetBkColor(COLORREF crColor);//���ñ�����ɫ

// Construction
public:
	SoCkeck();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoCkeck)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SoCkeck();

	// Generated message map functions
protected:
	//{{AFX_MSG(SoCkeck)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKECK_H__12E4D1BD_426B_4FF4_8D52_A83C24DE785B__INCLUDED_)
