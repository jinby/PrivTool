// SoCkeck.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoCkeck

SoCkeck::SoCkeck()
{
	m_crText    = RGB(0,255,0);
	m_crBackGnd = RGB(255,255,0);
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//背景画刷
}

SoCkeck::~SoCkeck()
{
	if(m_brBackGnd.GetSafeHandle())
	{
		m_brBackGnd.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(SoCkeck, CButton)
	//{{AFX_MSG_MAP(SoCkeck)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoCkeck message handlers
HBRUSH SoCkeck::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if(m_brBackGnd.m_hObject)
	{
		m_brBackGnd.DeleteObject();
	}
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//背景画刷

	pDC->SetBkColor(m_crBackGnd);//设置文本背景颜色
	pDC->SetTextColor(m_crText);//设置字体颜色
	return m_brBackGnd;
}

void SoCkeck::SetBkColor(COLORREF crColor)//设置背景颜色
{//最好设置与父窗口一样的颜色
	m_crBackGnd=crColor;
	Invalidate();
}
void SoCkeck::SetTextColor(COLORREF crColor)//设置文本颜色
{
	m_crText=crColor;
	Invalidate();
}
