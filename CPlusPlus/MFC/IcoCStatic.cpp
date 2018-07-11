// IcoCStatic.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IcoCStatic

IcoCStatic::IcoCStatic()
{
	TimeID      = 1;
	OnCountIoc  = 0;
	m_hIcon     = NULL;
	m_BkColor   = BKCOLOR;
	m_TextColor = TEXTCOLOR;
	m_brBackGnd.CreateSolidBrush(m_BkColor);
}

IcoCStatic::~IcoCStatic()
{
}


BEGIN_MESSAGE_MAP(IcoCStatic, CStatic)
	//{{AFX_MSG_MAP(IcoCStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IcoCStatic message handlers

void IcoCStatic::OnPaint()
{
	CPaintDC dc(this);
	SoOnDrawItem(&dc);
}

void IcoCStatic::SoOnDrawItem(CDC * pDC)
{
	CSize    SizeText,SizeIoc;
	CString  sTitle;
	RECT     Rect,TextRect;
	int      x,y;

	GetWindowText(sTitle);
	GetClientRect(&TextRect);
	Rect     = TextRect;
	SizeText = pDC->GetTextExtent(sTitle);

	if(NULL == m_hIcon)
	{
		m_hIcon = IocImageList[0];
	}

	if(NULL == m_hIcon)
	{
		if(SizeText.cy < TextRect.bottom)
		{
			TextRect.top = (TextRect.bottom - SizeText.cy ) / 2;
		}
	}
	else
	{
		SizeIoc = GethIconSize(m_hIcon);
		y       = 2;
		if(SizeIoc.cx > Rect.right)
		{
			x = 0;
		}
		else
		{
			x = (Rect.right - SizeIoc.cx) / 2;
		}
		Rect.left  = x;
		Rect.top   = y;
		Rect.right = SizeIoc.cx + x;
		Rect.bottom = SizeIoc.cy + y;
		pDC->FillRect(&Rect,&m_brBackGnd);

		pDC->DrawIcon(x,y,m_hIcon);//绘制图标

		if((SizeText.cy + 2) <= TextRect.bottom)
		{
			TextRect.top = TextRect.bottom - SizeText.cy - 2;
		}
	}

	pDC->SetTextColor(m_TextColor);
	pDC->SetBkColor(m_BkColor);
	pDC->DrawText(sTitle,&TextRect,DT_CENTER);//绘制文本
}

void IcoCStatic::OnTimer(UINT nIDEvent) 
{
	long Count = IocImageList.GetCount();
	if(Count <= OnCountIoc)
	{
		OnCountIoc = 1;
	}
	else
	{
		OnCountIoc++;
	}
	m_hIcon = IocImageList[OnCountIoc - 1];
	CStatic::OnTimer(nIDEvent);
	Invalidate();
}

///////////////////////////////////////////////////////

CSize IcoCStatic::GethIconSize(HICON InhIcon)
{//获取ICO图标高度和宽度
	CSize SizeIoc(0,0);
	ICONINFO IocInf = {0};
	if((InhIcon) && (::GetIconInfo(InhIcon, &IocInf)))
	{
		SizeIoc.cx	= (DWORD)(IocInf.xHotspot * 2);
		SizeIoc.cy	= (DWORD)(IocInf.yHotspot * 2);
		::DeleteObject(IocInf.hbmMask);
		::DeleteObject(IocInf.hbmColor);
	}
	return SizeIoc;
}
CSize IcoCStatic::GethIconSize()
{
	return GethIconSize(m_hIcon);
}
void IcoCStatic::AddIocImageList(int nIcon)
{
	HICON  hIconIn = AfxGetApp()->LoadIcon(nIcon);
	IocImageList.SetAt(IocImageList.GetCount(),hIconIn);
}

void  IcoCStatic::StartTimeOutIoc(long InTimer)
{
	InTimer = (100 > InTimer) ? 100 : InTimer;
	InTimer = (2000 < InTimer) ? 2000 : InTimer;

	KillTimer(TimeID);
	TimeID = SetTimer(TimeID,InTimer,NULL);
}
void  IcoCStatic::KillTimeOutIoc()
{
	KillTimer(TimeID);
	m_hIcon = IocImageList[0];
	OnCountIoc = 0;
}

void  IcoCStatic::DeleteImageList()
{ 
	IocImageList.RemoveAll();
}
void  IcoCStatic::SetIcoFile(CString & FileName)
{
	HICON hi = (HICON)LoadImage(NULL,FileName,IMAGE_ICON,0,0,LR_LOADFROMFILE);
	IocImageList.SetAt(IocImageList.GetCount(),hi);
}
void IcoCStatic::SetBkColor(COLORREF cr)//背景色
{
	m_BkColor = cr;
	Invalidate();
}
void IcoCStatic::SetTextBkColor(COLORREF cr)//文本背景色
{
	SetBkColor(cr);
}
void IcoCStatic::SetTextColor(COLORREF cr)//文字颜色
{
	m_TextColor = cr;
	Invalidate();
}