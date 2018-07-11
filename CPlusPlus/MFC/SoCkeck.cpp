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
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//������ˢ
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
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//������ˢ

	pDC->SetBkColor(m_crBackGnd);//�����ı�������ɫ
	pDC->SetTextColor(m_crText);//����������ɫ
	return m_brBackGnd;
}

void SoCkeck::SetBkColor(COLORREF crColor)//���ñ�����ɫ
{//��������븸����һ������ɫ
	m_crBackGnd=crColor;
	Invalidate();
}
void SoCkeck::SetTextColor(COLORREF crColor)//�����ı���ɫ
{
	m_crText=crColor;
	Invalidate();
}
