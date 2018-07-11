#if !defined(AFX_SOCHEADERCTRL_H__D95BC77B_3E70_4829_9D6B_0B3DE8DD3182__INCLUDED_)
#define AFX_SOCHEADERCTRL_H__D95BC77B_3E70_4829_9D6B_0B3DE8DD3182__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoCHeaderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoCHeaderCtrl window

class SoCHeaderCtrl : public CHeaderCtrl
{
private:

	COLORREF m_TextColor;//ÎÄ×ÖÑÕÉ«
	COLORREF m_BkColor;  //±³¾°É«
// Construction
public:
	SoCHeaderCtrl();

// Attributes
public:
	CString GetItemString(int nIndex);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoCHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SoCHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(SoCHeaderCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCHEADERCTRL_H__D95BC77B_3E70_4829_9D6B_0B3DE8DD3182__INCLUDED_)
