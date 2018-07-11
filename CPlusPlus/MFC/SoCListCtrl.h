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
	
	//��ʼ���п��
	void InilinColumnWidth();
	//��ʼ���б�����
	void InilinListInsertColumn(CString & AddrStr);
	//�������ı���ɫ
	void SetItemTextColor(DWORD  iItem, COLORREF color);
	//���������ɫ
	void SetItemBkColor(DWORD iItem, COLORREF color);
	//���ӿ���
	long AddItem();
	//�������ַ���
	void SetItemStr(int nItem, int nSubItem,CString & InOutStr);
	//�����п��
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
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);//˫��������
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);//����Ҽ���Ϣ
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCLISTCTRL_H__A951FF7C_C3E1_403E_9891_BD5561112EB4__INCLUDED_)
