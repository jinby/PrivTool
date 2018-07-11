// SoCListCtrl.cpp : implementation file
//

#include "MainHeaderFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoCListCtrl

SoCListCtrl::SoCListCtrl()
{
}

SoCListCtrl::~SoCListCtrl()
{
}


BEGIN_MESSAGE_MAP(SoCListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(SoCListCtrl)
	//	ON_WM_LBUTTONDBLCLK()//鼠标双击消息
	ON_WM_CONTEXTMENU()//鼠标右键消息
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoCListCtrl message handlers

void SoCListCtrl::PreSubclassWindow() 
{	
	SetExtendedStyle( GetExtendedStyle()
		             | LVS_EX_FULLROWSELECT //设置选中时，整行显示高亮
		             | LVS_EX_GRIDLINES     //细小表格线
		            // | LVS_EX_HEADERDRAGDROP //列表项拖放
					 );

	if(NULL != this->GetHeaderCtrl())
	{
		//设置列表窗口的子类
		m_ListHeader.SubclassWindow(this->GetHeaderCtrl()->GetSafeHwnd());
	}

	CListCtrl::PreSubclassWindow();
}
void SoCListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = CDRF_DODEFAULT;
	NMLVCUSTOMDRAW * lplvdr=(NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW &nmcd = lplvdr->nmcd;
	
	switch(lplvdr->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:		
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;	
	case CDDS_ITEMPREPAINT://UpdateWindow被调用就会执行这里的项刷新
		{
			COLORREF ItemColor;
			if(MapItemTextColor.Lookup(nmcd.dwItemSpec, ItemColor))
			{
				lplvdr->clrText   = ItemColor;   //行文本颜色
			//	lplvdr->clrTextBk = RGB(0,255,0);//背景颜色
				*pResult = CDRF_DODEFAULT;
			}
			if(MapItemBkColor.Lookup(nmcd.dwItemSpec, ItemColor))
			{
				lplvdr->clrTextBk = ItemColor;//背景颜色
			}
		}
		break;
	}
}

//设置项文本颜色
void SoCListCtrl::SetItemTextColor(DWORD iItem, COLORREF color)
{
	MapItemTextColor.SetAt(iItem, color);
	this->RedrawItems(iItem, iItem);
	UpdateWindow();
}

//设置项背景颜色
void SoCListCtrl::SetItemBkColor(DWORD iItem, COLORREF color)
{
	MapItemBkColor.SetAt(iItem, color);
	this->RedrawItems(iItem, iItem);
	UpdateWindow();
}

//增加空项
long SoCListCtrl::AddItem()
{
	long i = GetItemCount();
	InsertItem(i,"");
	return i;
}

void SoCListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) //鼠标右键消息
{
	if(!GetItemCount())//没有任何项
	{
		return;
	}

	POSITION pos = GetFirstSelectedItemPosition();//获取选中项
	if(NULL == pos)//判断有没有选中项
	{
		return;
	}
	int nItem=GetNextSelectedItem(pos);//取得选中的索引号
	CString Str;
//	Str.Format("鼠标右键消息,当前选中第%d项",nItem);
//	MessageBox(Str);
}
//初始化列宽度
void SoCListCtrl::InilinColumnWidth()
{
	CString   Str;     
	int       iWidth = 90;
	int       i,Max = m_ListHeader.GetItemCount();
	for(i=0; i < Max; i++)
	{
		Str    = m_ListHeader.GetItemString(i);
		iWidth = GetStringWidth(Str) + 10;
		SetColumnWidth(i,iWidth);
	}
}
//设置列宽度
void SoCListCtrl::ESetColumnWidth(int nSubItem,CString InStr)
{
	int  iWidth = GetStringWidth(InStr) + 10;
	SetColumnWidth(nSubItem,iWidth);
}

//初始化列表列项
void SoCListCtrl::InilinListInsertColumn(CString & AddrStr)
{
	int      StrLen,Offset,iWidth,i;
	CString  HeaderStr;

	StrLen = AddrStr.GetLength();
	if(0 == StrLen)
	{
		return;
	}
	Offset    = 0;
	i = 0;
	while(0 != (StrLen  = AddrStr.GetLength()))
	{
		Offset = AddrStr.Find(";");
		if(-1 == Offset)
		{
			Offset = StrLen;
		}
		HeaderStr = AddrStr.Left(Offset);
		AddrStr   = AddrStr.Right(StrLen - 1 - Offset);
		iWidth    = GetStringWidth(HeaderStr) + 10;
		InsertColumn(i,HeaderStr,LVCFMT_LEFT,iWidth,-1);
		i++;
	}
}

//设置项字符串
void SoCListCtrl::SetItemStr(int nItem, int nSubItem,CString & InOutStr)
{
	int i = GetItemCount();
	int StrLen,Offset;
	int iWidth;
	CString  AddrStr;

	if(nItem >= i)
	{
		AddItem();
		nItem = i;
	}
	Offset = 0;
	while(0 != (StrLen  = InOutStr.GetLength()))
	{
		Offset = InOutStr.Find(";");
		if(-1 == Offset)
		{
			Offset = StrLen;
		}
		AddrStr  = InOutStr.Left(Offset);
		InOutStr = InOutStr.Right(StrLen - 1 - Offset);
		SetItemText(nItem,nSubItem,AddrStr);

		iWidth = GetStringWidth(AddrStr);
		if(iWidth > (GetColumnWidth(nSubItem) - 20))
		{
			SetColumnWidth(nSubItem,iWidth + 20 );
		}
		nSubItem++;
	}
}