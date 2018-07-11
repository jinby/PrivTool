#if !defined(AFX_ICOCSTATIC_H__D3E7C8A9_7B4D_4BF8_BCB5_899D1608A40B__INCLUDED_)
#define AFX_ICOCSTATIC_H__D3E7C8A9_7B4D_4BF8_BCB5_899D1608A40B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcoCStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IcoCStatic window

class IcoCStatic : public CStatic
{
private:
	COLORREF    m_TextColor;   //ÎÄ×ÖÑÕÉ«
	COLORREF    m_BkColor;     //±³¾°ÑÕÉ«
	CBrush      m_brBackGnd;   //±³¾°»­Ë¢
	HICON       m_hIcon;
	CMapIocList IocImageList;
	ULONG       OnCountIoc;
	long        TimeID;

private:
	void SoOnDrawItem(CDC * pDC);
// Construction
public:
	IcoCStatic();

// Attributes
public:
	void  StartTimeOutIoc(long InTimer);
	void  KillTimeOutIoc();
	CSize GethIconSize(HICON InhIcon);
	CSize GethIconSize();
	void  AddIocImageList(int nIcon);
	void  DeleteImageList();
	void  SetIcoFile(CString & FileName);
	void  SetBkColor(COLORREF cr);    //±³¾°É«
	void  SetTextBkColor(COLORREF cr);//ÎÄ±¾±³¾°É«
	void  SetTextColor(COLORREF cr);  //ÎÄ×ÖÑÕÉ«

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~IcoCStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(IcoCStatic)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICOCSTATIC_H__D3E7C8A9_7B4D_4BF8_BCB5_899D1608A40B__INCLUDED_)
