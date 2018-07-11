// SoCStatic.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoCStatic

SoCStatic::SoCStatic()
{
	m_crText    = RGB(0,128,255);//文字颜色
	m_crBackGnd = RGB(255,255,0);//背景色
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//背景画刷
}

SoCStatic::~SoCStatic()
{
}


BEGIN_MESSAGE_MAP(SoCStatic, CStatic)
	//{{AFX_MSG_MAP(SoCStatic)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoCStatic message handlers
HBRUSH SoCStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	//刷新前更改文本颜色
	pDC->SetTextColor(m_crText);
	//刷新前更改文本背景
	pDC->SetBkColor(m_crBackGnd);
	if(m_brBackGnd.m_hObject)
	{
		m_brBackGnd.DeleteObject();
	}
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//背景画刷
	return m_brBackGnd;
}
void SoCStatic::SetBkColor(COLORREF cr)//背景色
{
	m_crBackGnd = cr;
	Invalidate();
}
void SoCStatic::SetTextBkColor(COLORREF cr)//文本背景色
{
	SetBkColor(cr);
}
void SoCStatic::SetTextColor(COLORREF cr)//文字颜色
{
	m_crText = cr;
	Invalidate();
}
