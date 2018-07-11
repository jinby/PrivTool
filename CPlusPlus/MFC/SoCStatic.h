#if !defined(AFX_SOCSTATIC_H__189C52AB_2B8D_42E6_A596_74F1C0D6EB6B__INCLUDED_)
#define AFX_SOCSTATIC_H__189C52AB_2B8D_42E6_A596_74F1C0D6EB6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoCStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoCStatic window

class SoCStatic : public CStatic
{
//private:
public:
	COLORREF m_crText;   //文字颜色
	COLORREF m_crBackGnd;//背景色
	CBrush   m_brBackGnd;//背景画刷


// Construction
public:
	SoCStatic();

// Attributes
public:

	void SetBkColor(COLORREF cr);    //背景色
	void SetTextBkColor(COLORREF cr);//文本背景色
	void SetTextColor(COLORREF cr);  //文字颜色

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoCStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SoCStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(SoCStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCSTATIC_H__189C52AB_2B8D_42E6_A596_74F1C0D6EB6B__INCLUDED_)
