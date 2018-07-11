// SoCHeaderCtrl.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoCHeaderCtrl

SoCHeaderCtrl::SoCHeaderCtrl()
{
	m_TextColor = RGB(0,0,0);  //ÎÄ×ÖÑÕÉ«
	m_BkColor   = BKCOLOR;     //±³¾°É«
}

SoCHeaderCtrl::~SoCHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(SoCHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(SoCHeaderCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoCHeaderCtrl message handlers
CString SoCHeaderCtrl::GetItemString(int nIndex)
{
	HDITEMA  HeaderItem;
	char     szText[100];

	memset(szText,0,sizeof(szText));
	memset(&HeaderItem,0,sizeof(HeaderItem));
	HeaderItem.mask       = HDI_TEXT;
	HeaderItem.pszText    = szText;
	HeaderItem.cchTextMax = sizeof(szText) - 1;
	GetItem(nIndex,&HeaderItem);
	return (CString)szText;
}

void SoCHeaderCtrl::OnPaint()
{
	CPaintDC dc(this);
	RECT     Rect,MainRect;
	HDITEMA  HeaderItem;
	char     szText[100];
	int      i,Max;

	Max = GetItemCount();
	GetClientRect(&MainRect);
	MainRect.left += 1;
	for(i = 0; i < Max; i++)
	{
		memset(szText,0,100);
		memset(&HeaderItem,0,sizeof(HeaderItem));

		HeaderItem.mask       = HDI_TEXT;
		HeaderItem.pszText    = szText;
		HeaderItem.cchTextMax = sizeof(szText) - 1;

		GetItem(i,&HeaderItem);
		GetItemRect(i,&Rect);

		Rect.left   = MainRect.left;
		dc.FillSolidRect(&Rect,m_BkColor);
		::SetBkColor(dc,m_BkColor);
		::SetTextColor(dc,m_TextColor);

		Rect.left +=4;
//		dc.DrawText(szText,&Rect,DT_CENTER | DT_END_ELLIPSIS);
		dc.DrawText(szText,&Rect,DT_LEFT | DT_END_ELLIPSIS);

		MainRect.left = Rect.right + 1;
	}
	dc.FillSolidRect(&MainRect,m_BkColor);
}
