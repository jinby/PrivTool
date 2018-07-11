//FuncCall.h

#pragma once

//移动窗口到屏幕中间位置
void __stdcall MoveWindowTo(CWnd * pWnd,long InWidth,long InHeight);
//屏幕除任务栏分割成SetWidthAndHeight份，窗口取OnWidthAndHeight份，并移动到屏幕中心
void __stdcall MoveWindowTo(CWnd * pWnd,UCHAR  SetWidthAndHeight,UCHAR OnWidthAndHeight);
//初始化有关全局文件名字符串
void __stdcall InilinExeFileInfo();
//打开创建目录
void __stdcall CreatePath();
//获取图标高度和宽度
CSize __stdcall GethIconSize(HICON InhIcon);
//设置窗口透明度
void  __stdcall SetWindowAttributes(CWnd * pWnd,long Attributes);
//检查本地文件MD5
long  __stdcall  CheckFileMd5();
//设置系统PE文件关联信息
unsigned int AFX_CDECL ThreadSetLinkInfo(PVOID p);