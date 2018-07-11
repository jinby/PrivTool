#if !defined(AFX_SOCTABCTR_H__C1EBBD3F_A73B_4A14_9993_E2E552D23D85__INCLUDED_)
#define AFX_SOCTABCTR_H__C1EBBD3F_A73B_4A14_9993_E2E552D23D85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoCTabCtr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoCTabCtr window

class SoCTabCtr : public CTabCtrl
{
private:
	COLORREF m_OkTextColor; //选中的文字颜色
	COLORREF m_OkBkColor;   //选中的背景颜色

	COLORREF m_TextColor;   //文字颜色
	COLORREF m_BkColor;     //背景颜色
// Construction
public:
	SoCTabCtr();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoCTabCtr)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SoCTabCtr();

	// Generated message map functions
protected:
	//{{AFX_MSG(SoCTabCtr)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCTABCTR_H__C1EBBD3F_A73B_4A14_9993_E2E552D23D85__INCLUDED_)
