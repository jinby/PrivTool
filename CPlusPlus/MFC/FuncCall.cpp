//FuncCall.cpp
//全局基本功能函数
#include "MainHeaderFile.h"

void __stdcall MoveWindowTo(CWnd * pWnd,long InWidth,long InHeight)
{//移动窗口到屏幕中间位置
	long cx,cy;
	RECT MovRect;
	//屏幕横框度
	cx = GetSystemMetrics(SM_CXSCREEN);
	//屏幕纵长度
	cy = GetSystemMetrics(SM_CYSCREEN) - 50;
	if(NULL == pWnd)
	{
		return;
	}
	if((0 == InWidth) || (0 == InHeight))
	{
		pWnd->GetClientRect(&MovRect);
	}
	else
	{
		MovRect.right  = InWidth;
		MovRect.bottom = InHeight;
	}

	if((cx < MovRect.right) || (cy < MovRect.bottom))
	{
		MovRect.left   = 0;
		MovRect.top    = 0;
		MovRect.right  = cx;
		MovRect.bottom = cy;
	}
	else
	{
		MovRect.left    = (cx - MovRect.right) / 2;
		MovRect.top     = (cy - MovRect.bottom) / 2;
		MovRect.right  += MovRect.left;
		MovRect.bottom += MovRect.top;
	}
	pWnd->SetWindowPos(NULL,MovRect.left,MovRect.top,MovRect.right,MovRect.bottom,0);
}

void __stdcall MoveWindowTo(CWnd * pWnd,UCHAR  SetWidthAndHeight,UCHAR OnWidthAndHeight)
{//屏幕除任务栏分割成SetWidthAndHeight份，窗口取OnWidthAndHeight份，并移动到屏幕中心
	long cx,cy;
	RECT MovRect;
	//屏幕横框度
	cx = GetSystemMetrics(SM_CXSCREEN);
	//屏幕纵长度
	cy = GetSystemMetrics(SM_CYSCREEN) - 50;

	if((NULL == pWnd) || (0 == SetWidthAndHeight) || (0 == OnWidthAndHeight) 
		|| (SetWidthAndHeight <= OnWidthAndHeight))
	{
		return;
	}
	MovRect.left = ((cx / SetWidthAndHeight) * (SetWidthAndHeight - OnWidthAndHeight)) / 2;
	MovRect.top  = ((cy / SetWidthAndHeight) * (SetWidthAndHeight - OnWidthAndHeight)) / 2;
	MovRect.right  = cx - (MovRect.left * 2);
	MovRect.bottom = cy - (MovRect.top * 2);
	pWnd->SetWindowPos(NULL,MovRect.left,MovRect.top,MovRect.right,MovRect.bottom,0);
}
//打开创建目录
void __stdcall CreatePath()
{
	::CreateDirectoryA(StrExeFilePath + "\\RCS",NULL);
	::CreateDirectoryA(StrResourceBmpPath,NULL);
	::CreateDirectoryA(StrResourceIcoPath,NULL);
	::CreateDirectoryA(StrResourceCurPath,NULL);
}
//初始化有关全局文件名字符串
void __stdcall InilinExeFileInfo()
{
	char  Buffer[1024];
	ULONG Len,Offset;

	GetModuleFileNameA(NULL,Buffer,1024);

	//程序自身全路径文件名
	StrExeFilePathName = Buffer;

	Len                = StrExeFilePathName.GetLength();
	Offset             = StrExeFilePathName.ReverseFind('\\') + 1;

	//程序自身简单文件名
	StrExeFileName     = StrExeFilePathName.Right(Len - Offset);

	//程序自身目录名
	StrExeFilePath     = StrExeFilePathName.Left(Offset);
	::CreateDirectoryA(StrExeFilePath + "\\RCS",NULL);

	//资源位图文件目录
	StrResourceBmpPath = StrExeFilePath + "\\RCS\\bmp";
	::CreateDirectoryA(StrResourceBmpPath,NULL);

	//资源ICO文件目录
	StrResourceIcoPath = StrExeFilePath + "\\RCS\\ico";
	::CreateDirectoryA(StrResourceIcoPath,NULL);

	//资源CUR文件目录
	StrResourceCurPath = StrExeFilePath + "\\RCS\\cur";
	::CreateDirectoryA(StrResourceCurPath,NULL);

	do
	{
		StrOnPEFilePathName  = GetCommandLine() + 1;
		Offset = StrOnPEFilePathName.ReverseFind(':');
		if((0 == Offset) || (-1 == Offset))
		{
			break;
		}
		Offset = StrOnPEFilePathName.GetLength() - Offset + 1;
		StrOnPEFilePathName = StrOnPEFilePathName.Right(Offset);
		StrOnPEFilePathName.Replace("\"","");
		StrOnPEFileName = StrOnPEFilePathName.Right(StrOnPEFilePathName.GetLength() 
		          - StrOnPEFilePathName.ReverseFind('\\') - 1);
		return;
	}while(0);
	
	StrOnPEFilePathName = StrExeFilePathName;
	StrOnPEFileName     = StrExeFileName;
}

//获取图标高度和宽度
CSize __stdcall GethIconSize(HICON InhIcon)
{
	//获取ICO图标高度和宽度
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

//设置窗口透明度
void  __stdcall SetWindowAttributes(CWnd * pWnd,long Attributes)
{
	::SetWindowLong(pWnd->GetSafeHwnd()
		         ,GWL_EXSTYLE
		         ,GetWindowLong(pWnd->GetSafeHwnd()
				 ,GWL_EXSTYLE)^0x80000);

	HINSTANCE hInst = LoadLibrary("User32.DLL");
	if(hInst)
	{
		typedef BOOL (WINAPI * MYFUNC)(HWND,COLORREF,ULONG,DWORD);
		MYFUNC fun = NULL;
		fun = (MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes");
		if (fun)
		{
			fun(pWnd->GetSafeHwnd(),0,Attributes,2);
			//中间参数表示窗口透明度，值越低透明度越高，取值范围0~255；
		}
		FreeLibrary(hInst);
	}
}

//检查本地文件MD5
long  __stdcall  CheckFileMd5()
{
	UCHAR        Md5Str[68] = {0};
	UCHAR        TempDataMd5[16];
	ULONG        CheckMd5[4];
	PULONG       pCheckMd5;
	MD5Check     Md5;
	CFile        cFile;
	long         DataLen , i;
	PUCHAR       pData = NULL, p;
	PEDataDump   PeDump;

	PIMAGE_DOS_HEADER   Module;
	PIMAGE_NT_HEADERS   NtHeader;

	if(!cFile.Open(StrExeFilePathName,CFile::shareDenyNone|CFile::typeBinary))
	{
		return 1;
	}
	if(0 >= (DataLen = cFile.GetLength()))
	{
		return 1;
	}
	while(!pData)
	{
		pData = new UCHAR[DataLen + 10];
	}
	if(DataLen != cFile.Read(pData,DataLen))
	{	
		delete pData;
		return 1;
	}
	Module = (PIMAGE_DOS_HEADER)pData;
	NtHeader = (PIMAGE_NT_HEADERS)((ULONG)Module + Module->e_lfanew);

	CheckMd5[0] = NtHeader->FileHeader.TimeDateStamp;
	CheckMd5[1] = NtHeader->FileHeader.PointerToSymbolTable;
	CheckMd5[2] = NtHeader->FileHeader.NumberOfSymbols;
	CheckMd5[3] = *(PULONG)Module->e_res;

	NtHeader->OptionalHeader.CheckSum = 0;

	NtHeader->FileHeader.TimeDateStamp = 0;
	NtHeader->FileHeader.PointerToSymbolTable = 0;
	NtHeader->FileHeader.NumberOfSymbols = 0;
	*(PULONG)Module->e_res = 0;

	Md5.MD5GetMemBufferCheck((TempDataMd5),(pData),DataLen);
//	Md5.MD5OnCheckDataToString((char*)Md5Str,(TempDataMd5));

	pCheckMd5 = (PULONG)TempDataMd5;
	delete pData;

	for(i = 0; i < 4; i++)
	{
		if(pCheckMd5[i] != CheckMd5[i])
		{
//			MessageBox(NULL,"Md5错误","Md5错误",0);
			return 1;
		}
	}
//	MessageBox(NULL,"Md5准确","Md5",0);
	return 0;
}

void InilinLinkRegPEFile(CString & FileEx,CString & cLinkSubKeyName)
{

	FileEx.MakeUpper();

	if((".OCX" == FileEx) || (".EXE" == FileEx) || (".DLL" == FileEx) 
		|| (".SYS" == FileEx))
	{
		return;
	}
	if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)FileEx))
	{
		return;
	}
	if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)FileEx
			           ,(char *)(LPCTSTR)cLinkSubKeyName))
	{
		return;
	}
}

void SetLinkRegPEFile(CString & FileEx,CString & cLinkSubKeyName)
{
	CString TempStr0,TempStr1,TempStr2;
	char  Buffer[128] = {0};
	ULONG ValueType,BufferLen = 120;

	if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)FileEx))
	{
		return;
	}
	do
	{
		if(!ReadRegValueData(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)FileEx,NULL
		                ,&ValueType,(PVOID)Buffer,&BufferLen))
		{
			break;
		}
		if((0 == BufferLen) || (0 == Buffer[0]))
		{
			break;
		}
		if(!NewRegSubKey(HKEY_CLASSES_ROOT,Buffer))
		{
			break;
		}
		TempStr1 = "WinPE格式文件";
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,Buffer,(char *)(LPCTSTR)TempStr1))
		{
			break;
		}
		TempStr0 = "\\";
		TempStr0 = Buffer + TempStr0 + "\\shell\\" + cLinkSubKeyName;
		if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr0))
		{
			break;
		}
		TempStr1 = ShellStr;
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr0
			           ,(char *)(LPCTSTR)TempStr1))
		{
			break;
		}
		TempStr0 += "\\command";
		TempStr1  = StrExeFilePathName + " %1";
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr0
			           ,(char *)(LPCTSTR)TempStr1))
		{
			break;
		}
		return;
	}while(0);
	SetRegKeyValueData(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)FileEx,NULL,REG_SZ
			              ,(char*)(LPCTSTR)cLinkSubKeyName
						  ,(ULONG)cLinkSubKeyName.GetLength());
}

ULONG StartThreadSetLinkInfo = 0;
//设置系统PE文件关联信息
unsigned int AFX_CDECL ThreadSetLinkInfo(PVOID p)
{
	ULONG Count;
	CString TempStr,TempStr0,TempStr1,TempStr2,TempStr3;
	if(StartThreadSetLinkInfo)
	{
		return 0;
	}
	StartThreadSetLinkInfo = 1;

	do
	{
		TempStr  = "WinPE.Info"; 
		TempStr1 = StrExeFilePathName + " %1";
		TempStr2 = StrExeFilePathName + ",0";

		TempStr0 = ".dll";
		SetLinkRegPEFile(TempStr0,TempStr);

		TempStr0 = ".exe";
		SetLinkRegPEFile(TempStr0,TempStr);

		TempStr0 = ".sys";
		SetLinkRegPEFile(TempStr0,TempStr);

		TempStr0 = ".ocx";
		SetLinkRegPEFile(TempStr0,TempStr);

		if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr))
		{
			break;
		}
		TempStr0 = "WinPE格式文件";
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr,(char *)(LPCTSTR)TempStr0))
		{
			break;
		}

		TempStr3 = TempStr + "\\DefaultIcon";
		if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr3))
		{
			break;
		}
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr3
			           ,(char *)(LPCTSTR)TempStr2))
		{
			break;
		}

		TempStr3 = TempStr + "\\shell\\" + LimkH;
		if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr3))
		{
			break;
		}
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr3
			           ,(char *)(LPCTSTR)ShellStr))
		{
			break;
		}

		TempStr3 += "\\command";
		if(!NewRegSubKey(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr3))
		{
			break;
		}
		if(!SetRegKeyStr(HKEY_CLASSES_ROOT,(char *)(LPCTSTR)TempStr3
			           ,(char *)(LPCTSTR)TempStr1))
		{
			break;
		}

		Count = 0;
		while("" != LinkPeFile[Count])
		{
			InilinLinkRegPEFile(LinkPeFile[Count],TempStr);
			Count++;
		}
		break;
	}while(0);

	StartThreadSetLinkInfo = 0;
	return 0;
}