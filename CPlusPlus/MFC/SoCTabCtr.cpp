// SoCTabCtr.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoCTabCtr

SoCTabCtr::SoCTabCtr()
{
	m_TextColor = RGB(0,0,0);   //文字颜色
	m_BkColor   = BKCOLOR;      //背景颜色
//	m_BkColor   = RGB(0,215,215);     //背景颜色

	m_OkTextColor = RGB(125,0,0);   //选中的文字颜色
//	m_OkBkColor   = RGB(0,255,0);   //选中的背景颜色
	m_OkBkColor   = m_BkColor;
}

SoCTabCtr::~SoCTabCtr()
{
}


BEGIN_MESSAGE_MAP(SoCTabCtr, CTabCtrl)
	//{{AFX_MSG_MAP(SoCTabCtr)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoCTabCtr message handlers
void SoCTabCtr::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{  
    CBrush brushBK(m_BkColor); 

    char      szTabText[100];                
    TC_ITEM   tci;       
	RECT      Rect,MainRect; 

    memset(szTabText,0,sizeof(szTabText));      
    tci.mask       = TCIF_TEXT;      
    tci.pszText    = szTabText;      
    tci.cchTextMax = sizeof(szTabText) - 1;
    GetItem(lpDrawItemStruct->itemID,&tci);

	GetClientRect(&MainRect);
	GetItemRect(lpDrawItemStruct->itemID,&Rect);

	if(GetCurSel() == lpDrawItemStruct->itemID)
	{
		::SetBkColor(lpDrawItemStruct->hDC,m_OkBkColor);
		::SetTextColor(lpDrawItemStruct->hDC,m_OkTextColor);

//		brushBK.DeleteObject();
//		brushBK.CreateSolidBrush(m_OkBkColor);
	}
	else
	{
		::SetBkColor(lpDrawItemStruct->hDC,m_BkColor);
		::SetTextColor(lpDrawItemStruct->hDC,m_TextColor);
	}

	FillRect(lpDrawItemStruct->hDC,&lpDrawItemStruct->rcItem,brushBK); 

    TextOut(lpDrawItemStruct->hDC,      
        lpDrawItemStruct->rcItem.left+5,      
        lpDrawItemStruct->rcItem.top+5,      
        tci.pszText,      
        lstrlen(tci.pszText));
} 

BOOL SoCTabCtr::OnEraseBkgnd(CDC* pDC) 
{ 
    CBrush brushBK(m_BkColor);
    CRect rect; 
    this->GetClientRect(rect); 
    pDC->FillRect(rect,&brushBK); 
    return TRUE;
}
