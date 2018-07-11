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
	//	ON_WM_LBUTTONDBLCLK()//���˫����Ϣ
	ON_WM_CONTEXTMENU()//����Ҽ���Ϣ
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoCListCtrl message handlers

void SoCListCtrl::PreSubclassWindow() 
{	
	SetExtendedStyle( GetExtendedStyle()
		             | LVS_EX_FULLROWSELECT //����ѡ��ʱ��������ʾ����
		             | LVS_EX_GRIDLINES     //ϸС�����
		            // | LVS_EX_HEADERDRAGDROP //�б����Ϸ�
					 );

	if(NULL != this->GetHeaderCtrl())
	{
		//�����б��ڵ�����
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
	case CDDS_ITEMPREPAINT://UpdateWindow�����þͻ�ִ���������ˢ��
		{
			COLORREF ItemColor;
			if(MapItemTextColor.Lookup(nmcd.dwItemSpec, ItemColor))
			{
				lplvdr->clrText   = ItemColor;   //���ı���ɫ
			//	lplvdr->clrTextBk = RGB(0,255,0);//������ɫ
				*pResult = CDRF_DODEFAULT;
			}
			if(MapItemBkColor.Lookup(nmcd.dwItemSpec, ItemColor))
			{
				lplvdr->clrTextBk = ItemColor;//������ɫ
			}
		}
		break;
	}
}

//�������ı���ɫ
void SoCListCtrl::SetItemTextColor(DWORD iItem, COLORREF color)
{
	MapItemTextColor.SetAt(iItem, color);
	this->RedrawItems(iItem, iItem);
	UpdateWindow();
}

//���������ɫ
void SoCListCtrl::SetItemBkColor(DWORD iItem, COLORREF color)
{
	MapItemBkColor.SetAt(iItem, color);
	this->RedrawItems(iItem, iItem);
	UpdateWindow();
}

//���ӿ���
long SoCListCtrl::AddItem()
{
	long i = GetItemCount();
	InsertItem(i,"");
	return i;
}

void SoCListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) //����Ҽ���Ϣ
{
	if(!GetItemCount())//û���κ���
	{
		return;
	}

	POSITION pos = GetFirstSelectedItemPosition();//��ȡѡ����
	if(NULL == pos)//�ж���û��ѡ����
	{
		return;
	}
	int nItem=GetNextSelectedItem(pos);//ȡ��ѡ�е�������
	CString Str;
//	Str.Format("����Ҽ���Ϣ,��ǰѡ�е�%d��",nItem);
//	MessageBox(Str);
}
//��ʼ���п��
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
//�����п��
void SoCListCtrl::ESetColumnWidth(int nSubItem,CString InStr)
{
	int  iWidth = GetStringWidth(InStr) + 10;
	SetColumnWidth(nSubItem,iWidth);
}

//��ʼ���б�����
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

//�������ַ���
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