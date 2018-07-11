// IcoCButton.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IcoCButton

IcoCButton::IcoCButton()
{
	m_hIcon      = NULL;
	m_hIconOut   = NULL;
	m_hIconMouse = NULL;
	m_TextColor  = RGB(160,0,0); //文字颜色
	m_BkColor    = BKCOLOR;      //背景颜色
	m_bMouseOnButton  = 0;
	m_IocCenterOrLeft = 0;
}

IcoCButton::~IcoCButton()
{
}


BEGIN_MESSAGE_MAP(IcoCButton, CButton)
	//{{AFX_MSG_MAP(IcoCButton)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IcoCButton message handlers
void IcoCButton::PreSubclassWindow() 
{
	ModifyStyle(SS_TYPEMASK,BS_OWNERDRAW,SWP_FRAMECHANGED);
	CButton::PreSubclassWindow();
}

void IcoCButton::OnMouseMove(UINT nFlags, CPoint point) 
{	
	RECT Rect;
	
//	ClientToScreen(&point);
//	GetWindowRect(&Rect);

	GetClientRect(&Rect);
	if(PtInRect(&Rect,point))
	{
		if(0 == m_bMouseOnButton)
		{
			m_bMouseOnButton = 1;
			Invalidate();
			m_TimeID = SetTimer(100,50,NULL);
		}
	}
	else
	{
		if(m_bMouseOnButton)
		{
			m_bMouseOnButton = 0;
			Invalidate();
			KillTimer(m_TimeID);
		}
	}
	if(-1 != nFlags)
	{
		CButton::OnMouseMove(nFlags, point);
	}
}

void IcoCButton::OnTimer(UINT nIDEvent) 
{
	CPoint point(GetMessagePos());
	ScreenToClient(&point);
	OnMouseMove(-1,point);

//	CButton::OnTimer(nIDEvent);
}

void IcoCButton::OnDrawBorder(CDC* pDC,long OnBit)
{
	RECT     Rect;
	GetClientRect(&Rect);

	if(OnBit)
	{
		pDC->Draw3dRect(&Rect, ::GetSysColor(COLOR_BTNSHADOW), 
		                       ::GetSysColor(COLOR_BTNHILIGHT));
	}
	else
	{
		pDC->Draw3dRect(&Rect, ::GetSysColor(COLOR_BTNHILIGHT), 
		                       ::GetSysColor(COLOR_BTNSHADOW));
	}
}

void  IcoCButton::OnDrawIco(CDC* pDC,HICON In_hIcon,long On)
{
	CSize    SizeIoc(0,0);
	RECT     Rect;

	GetClientRect(&Rect);
	if(NULL == In_hIcon)
	{
		return;
	}
	SizeIoc = GethIconSize(In_hIcon);
	if((!SizeIoc.cy) || (!SizeIoc.cx))
	{
		return;
	}
	Rect.left = 4;
	Rect.top  = 4;
	if(0 == m_IocCenterOrLeft)
	{//中间靠上位置画图标
		if(Rect.right > SizeIoc.cx)
		{
			Rect.left = (Rect.right - SizeIoc.cx) / 2;
		}
	}
	else if(1 == m_IocCenterOrLeft)
	{//左边位置画图标
		if(Rect.bottom > SizeIoc.cy)
		{
			Rect.top    = (Rect.bottom - SizeIoc.cy) / 2;
		}
	}
	else
	{
		return;
	}

	if(On)
	{//选中需要缩进
		Rect.left --;
		Rect.top --;
	}
	pDC->DrawIcon(Rect.left,Rect.top,In_hIcon);//绘制图标
}

void  IcoCButton::OnDrawText(CDC* pDC,HICON In_hIcon,long On)
{
	CSize    SizeIoc(0,0),SizeText;
	RECT     Rect;
	CString  sTitle;

	GetWindowText(sTitle);
	GetClientRect(&Rect);
	if(In_hIcon)
	{
		SizeIoc = GethIconSize(In_hIcon);
	}
	SizeText = pDC->GetTextExtent(sTitle);

	pDC->SetTextColor(m_TextColor);
	pDC->SetBkColor(m_BkColor);

	Rect.left = 4;
	Rect.top  = 4;

	if(0 == m_IocCenterOrLeft)
	{//中间靠上位置画图标
		if(Rect.right > SizeText.cx)
		{
			Rect.left = (Rect.right - SizeText.cx) / 2;
		}
		if(Rect.bottom > (SizeText.cy + 4))
		{
			Rect.top += SizeIoc.cy + 2;
		}
		if(NULL == In_hIcon)
		{
			Rect.top = (Rect.bottom - SizeText.cy) / 2;
		}
	}
	else if(1 == m_IocCenterOrLeft)
	{//左边位置画图标
		Rect.left  = SizeIoc.cy + 10;

		if(Rect.bottom > SizeText.cy)
		{
			Rect.top = (Rect.bottom - SizeText.cy) / 2;
		}
	}
	else
	{
		return;
	}

	Rect.right  = Rect.left + SizeText.cx;
	Rect.bottom = Rect.top + SizeText.cy;
	
	if(On)
	{//选中需要缩进
		Rect.left --;
		Rect.top --;
		Rect.right --;
		Rect.bottom --;
	}

	pDC->DrawText(sTitle,&Rect,0);
//	pDC->DrawText(sTitle,&Rect,DT_CENTER);
}

void IcoCButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC *    pDC;
	CBrush	 brBackground(m_BkColor);
	RECT     Rect;

	pDC  = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->FillRect(&lpDrawItemStruct->rcItem,&brBackground);

	if(ODS_SELECTED & lpDrawItemStruct->itemState)
	{//选中状态
		OnDrawBorder(pDC,1);
		OnDrawIco(pDC,m_hIconOut,1);
		OnDrawText(pDC,m_hIconOut,1);
	}
//	else if(ODS_DISABLED & lpDrawItemStruct->itemState)
//	{//失效状态
//	}
	else if(1 == m_bMouseOnButton)
	{//鼠标位置在窗口上
		OnDrawBorder(pDC,0);
		OnDrawIco(pDC,m_hIconMouse,0);
		OnDrawText(pDC,m_hIconMouse,0);
	}
	else
	{//正常状态
		OnDrawIco(pDC,m_hIcon,0);
		OnDrawText(pDC,m_hIcon,0);
	}
	return;
}

void  IcoCButton::SetBkColor(COLORREF BkColor,long li)
{
	m_BkColor = BkColor;
	if(li)
	{
		Invalidate();
	}
}

void  IcoCButton::SetTextColor(COLORREF TextColor,long li)
{
	m_TextColor = TextColor;
	if(li)
	{
		Invalidate();
	}
}

void IcoCButton::SetIcon(int nIconIn,int nIcon2,int nIconOut)
{
	HICON hIconIn = NULL;

	hIconIn = AfxGetApp()->LoadIcon(nIconIn);
	if(hIconIn)
	{
		if(m_hIcon)
		{
			::DestroyIcon(m_hIcon);
		}
		m_hIcon = hIconIn;
	}
	
	hIconIn = AfxGetApp()->LoadIcon(nIcon2);
	if(hIconIn)
	{
		if(m_hIconMouse)
		{
			::DestroyIcon(m_hIconMouse);
		}
		m_hIconMouse = hIconIn;
	}
	
	hIconIn = AfxGetApp()->LoadIcon(nIconOut);
	if(hIconIn)
	{
		if(m_hIconOut)
		{
			::DestroyIcon(m_hIconOut);
		}
		m_hIconOut = hIconIn;
	}
}

void  IcoCButton::SetIocCenterOrLeft(long InCenterOrLeft)
{
	if((0 == InCenterOrLeft) || (1 == InCenterOrLeft))
	{
		m_IocCenterOrLeft = InCenterOrLeft;
	}
}