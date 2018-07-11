/*---------------------------------------------------------------------------
**版权所有 (C)2011, 武汉天喻信息产业股份有限公司。
**
**文件名称：CmdAPIzhy.cpp
**文件标识：见配置管理计划书
**内容摘要：简要描述本文件的内容，包括主要模块、函数及其功能的实现
**其它说明：
**当前版本：
**作    者：zhangheyong
**完成日期：2011:9:2   15:48
**
**修改记录1：
** 修改日期：
** 版 本 号：
** 修 改 人：
** 修改内容：
**修改记录2：
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "CmdAPIzhy.h"

//命令行输出缓冲大小
const long MAX_COMMAND_SIZE = 10000;

//获取MAC命令行
// char szFetCmd[] = "ipconfig /all";
char szFetCmd[] = "getmac";

//网卡MAC地址的前导信息
//const CString str4Search = "Physical Address. . . . . . . . . : ";
const CString str4Search = "-";

//获取计算机硬件信息 -- 这里以网卡的mac值为有效信息
/*
1. 创建一个无名管道。

2. 创建一个IPCONFIG 的进程，并将输出重定向到管道。

3. 从管道获取命令行返回的所有信息放入缓冲区lpszBuffer。

4. 从缓冲区lpszBuffer中获得抽取出MAC地址串。
	  
*/
BOOL zhyapi_GetComputerNETMAC(CStringArray &strNETMAC)
{
	//初始化返回MAC地址缓冲区
	strNETMAC.RemoveAll();
	
	BOOL bret; 
	
	SECURITY_ATTRIBUTES sa; 
	
	HANDLE hReadPipe = NULL,hWritePipe = NULL; 
	
	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	
	sa.lpSecurityDescriptor = NULL; 
	
	sa.bInheritHandle = TRUE; 
	
	//创建管道
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if(!bret)
	{
		return FALSE;
	}
	
	//控制命令行窗口信息
	STARTUPINFO si; 
	
	//返回进程信息
	PROCESS_INFORMATION pi; 
	
	si.cb = sizeof(STARTUPINFO); 
	GetStartupInfo(&si); 
	
	si.hStdError = hWritePipe; 
	si.hStdOutput = hWritePipe; 
	si.wShowWindow = SW_HIDE; //隐藏命令行窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	
	bret = FALSE;
	do 
	{
		//创建获取命令行进程
		bret = CreateProcess (NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ); 
		
		char szBuffer[MAX_COMMAND_SIZE+1]; //放置命令行输出缓冲区
		
		if (bret) 
		{ 
			WaitForSingleObject (pi.hProcess, INFINITE); 
			unsigned long count;

			bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
			if(!bret)
			{
				//关闭所有的句柄
				break;
			}
			else
			{
				//分解出MAC值
				bret = zhyapi_ParseNetMAC(szBuffer,strNETMAC);
			}
		}
	} while (FALSE);

	

	//关闭所有的句柄
	if (NULL != pi.hProcess)
	{
		CloseHandle(pi.hProcess); 
	}

	if (NULL != pi.hThread)
	{
		CloseHandle(pi.hThread);
	}

	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	
	return bret;	
}

//根据getmac命令行中的反馈值中分解出MAC值
/*

  物理地址            传输名称
  =================== ==========================================================
  00-26-C7-5B-BC-BC   媒体被断开
  F0-DE-F1-0B-87-44   \Device\Tcpip_{6A58066E-6FD3-48EC-A4A3-93D3A3D7428D}
  00-50-56-C0-00-01   \Device\Tcpip_{0EF07D56-7CB8-47BD-9646-E046476D01F5}
  00-50-56-C0-00-08   \Device\Tcpip_{DE04BCD7-9771-453B-A1B7-319B6B469763}
  已停用              断开
*/
BOOL zhyapi_ParseNetMAC(const CString strBuf,CStringArray &strNETMAC)
{
	CString strTmpBuf = strBuf;
// 	strTmpBuf.Remove(' ');
	int iBegin = 0,iEnd = 0;
	iBegin = strTmpBuf.Find("-",0);
	if (0>iBegin)
	{
		return FALSE;
	}
	strTmpBuf = strTmpBuf.Mid(iBegin-2);
	iBegin = 0;
	do 
	{
		iEnd = strTmpBuf.Find("\x0D\x0A",iBegin);
		if (0 > iEnd)
		{
			break;
		}

		CString strMac = strTmpBuf.Mid(iBegin,17);
		
		int iRemove = strMac.Remove('-');
		iBegin = iEnd + 2;
		if (iRemove == 0)
		{
			continue;
		}
		strNETMAC.Add(strMac);
		
	} while (TRUE);

	return TRUE;
}
