#if !defined(AFX_SOCLISTCTRL_H__A951FF7C_C3E1_403E_9891_BD5561112EB4__INCLUDED_)
#define AFX_SOCLISTCTRL_H__A951FF7C_C3E1_403E_9891_BD5561112EB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoCListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoCListCtrl window

class SoCListCtrl : public CListCtrl
{
public:
	SoCHeaderCtrl   m_ListHeader;
	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemBkColor;
	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemTextColor;
// Construction
public:
	SoCListCtrl();

// Attributes
public:
	
	//初始化列宽度
	void InilinColumnWidth();
	//初始化列表列项
	void InilinListInsertColumn(CString & AddrStr);
	//设置项文本颜色
	void SetItemTextColor(DWORD  iItem, COLORREF color);
	//设置项背景颜色
	void SetItemBkColor(DWORD iItem, COLORREF color);
	//增加空项
	long AddItem();
	//设置项字符串
	void SetItemStr(int nItem, int nSubItem,CString & InOutStr);
	//设置列宽度
	void ESetColumnWidth(int nSubItem,CString InStr);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SoCListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SoCListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(SoCListCtrl)
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);//双击鼠标左键
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);//鼠标右键消息
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCLISTCTRL_H__A951FF7C_C3E1_403E_9891_BD5561112EB4__INCLUDED_)
