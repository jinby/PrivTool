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
	m_crText    = RGB(0,128,255);//������ɫ
	m_crBackGnd = RGB(255,255,0);//����ɫ
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//������ˢ
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
	//ˢ��ǰ�����ı���ɫ
	pDC->SetTextColor(m_crText);
	//ˢ��ǰ�����ı�����
	pDC->SetBkColor(m_crBackGnd);
	if(m_brBackGnd.m_hObject)
	{
		m_brBackGnd.DeleteObject();
	}
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);//������ˢ
	return m_brBackGnd;
}
void SoCStatic::SetBkColor(COLORREF cr)//����ɫ
{
	m_crBackGnd = cr;
	Invalidate();
}
void SoCStatic::SetTextBkColor(COLORREF cr)//�ı�����ɫ
{
	SetBkColor(cr);
}
void SoCStatic::SetTextColor(COLORREF cr)//������ɫ
{
	m_crText = cr;
	Invalidate();
}
