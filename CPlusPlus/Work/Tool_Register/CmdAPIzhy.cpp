/*---------------------------------------------------------------------------
**��Ȩ���� (C)2011, �人������Ϣ��ҵ�ɷ����޹�˾��
**
**�ļ����ƣ�CmdAPIzhy.cpp
**�ļ���ʶ�������ù���ƻ���
**����ժҪ����Ҫ�������ļ������ݣ�������Ҫģ�顢�������书�ܵ�ʵ��
**����˵����
**��ǰ�汾��
**��    �ߣ�zhangheyong
**������ڣ�2011:9:2   15:48
**
**�޸ļ�¼1��
** �޸����ڣ�
** �� �� �ţ�
** �� �� �ˣ�
** �޸����ݣ�
**�޸ļ�¼2��
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "CmdAPIzhy.h"

//��������������С
const long MAX_COMMAND_SIZE = 10000;

//��ȡMAC������
// char szFetCmd[] = "ipconfig /all";
char szFetCmd[] = "getmac";

//����MAC��ַ��ǰ����Ϣ
//const CString str4Search = "Physical Address. . . . . . . . . : ";
const CString str4Search = "-";

//��ȡ�����Ӳ����Ϣ -- ������������macֵΪ��Ч��Ϣ
/*
1. ����һ�������ܵ���

2. ����һ��IPCONFIG �Ľ��̣���������ض��򵽹ܵ���

3. �ӹܵ���ȡ�����з��ص�������Ϣ���뻺����lpszBuffer��

4. �ӻ�����lpszBuffer�л�ó�ȡ��MAC��ַ����
	  
*/
BOOL zhyapi_GetComputerNETMAC(CStringArray &strNETMAC)
{
	//��ʼ������MAC��ַ������
	strNETMAC.RemoveAll();
	
	BOOL bret; 
	
	SECURITY_ATTRIBUTES sa; 
	
	HANDLE hReadPipe = NULL,hWritePipe = NULL; 
	
	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	
	sa.lpSecurityDescriptor = NULL; 
	
	sa.bInheritHandle = TRUE; 
	
	//�����ܵ�
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if(!bret)
	{
		return FALSE;
	}
	
	//���������д�����Ϣ
	STARTUPINFO si; 
	
	//���ؽ�����Ϣ
	PROCESS_INFORMATION pi; 
	
	si.cb = sizeof(STARTUPINFO); 
	GetStartupInfo(&si); 
	
	si.hStdError = hWritePipe; 
	si.hStdOutput = hWritePipe; 
	si.wShowWindow = SW_HIDE; //���������д���
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	
	bret = FALSE;
	do 
	{
		//������ȡ�����н���
		bret = CreateProcess (NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ); 
		
		char szBuffer[MAX_COMMAND_SIZE+1]; //�������������������
		
		if (bret) 
		{ 
			WaitForSingleObject (pi.hProcess, INFINITE); 
			unsigned long count;

			bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
			if(!bret)
			{
				//�ر����еľ��
				break;
			}
			else
			{
				//�ֽ��MACֵ
				bret = zhyapi_ParseNetMAC(szBuffer,strNETMAC);
			}
		}
	} while (FALSE);

	

	//�ر����еľ��
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

//����getmac�������еķ���ֵ�зֽ��MACֵ
/*

  �����ַ            ��������
  =================== ==========================================================
  00-26-C7-5B-BC-BC   ý�屻�Ͽ�
  F0-DE-F1-0B-87-44   \Device\Tcpip_{6A58066E-6FD3-48EC-A4A3-93D3A3D7428D}
  00-50-56-C0-00-01   \Device\Tcpip_{0EF07D56-7CB8-47BD-9646-E046476D01F5}
  00-50-56-C0-00-08   \Device\Tcpip_{DE04BCD7-9771-453B-A1B7-319B6B469763}
  ��ͣ��              �Ͽ�
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
