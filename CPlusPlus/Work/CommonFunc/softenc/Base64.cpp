//base64.cpp

#include "StdAfx.h"
#include "Base64.h"

static char g_szBase64Table[]=
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/////////////////////////////////////////////////////////////////////
//
//	Base64
//-------------------------------------------------------------------
//	���ܣ�
//		Base64����
//
//	���أ�
//		TRUE:�ɹ�	FALSE:ʧ��
//
//  ������
//		LPBYTE pbInData			��������
//		DWORD dwInDataLen		�������ݵĳ���
//		LPBYTE pbOutData		�����Base64����
//		LPDWORD pdwOutDataLen	Base64�����ĳ���
//
//  ˵����
//-------------------------------------------------------------------
BOOL __stdcall SC_Base64Encode(
							   IN LPBYTE pbInData, 
							   IN DWORD dwInDataLen, 
							   OUT LPBYTE pbOutData,
							   IN OUT LPDWORD pdwOutDataLen
							   )
{
	DWORD dwRest = dwInDataLen%3;
	DWORD dwOutLen = (dwInDataLen/3 + ((dwRest == 0) ? 0:1)) * 4;
	DWORD dwCount = *pdwOutDataLen;
	*pdwOutDataLen = dwOutLen;
	if((dwCount < dwOutLen) || (pbOutData == NULL))
		return FALSE;

	BYTE *pbBuf = pbOutData;

	dwCount = dwInDataLen/3;
	WORD dwUnit = 0;
	while(dwCount > 0)
	{
		pbOutData[0] = 0x3F & (pbInData[0] >> 2);
		dwUnit = MAKEWORD(pbInData[1], pbInData[0]);
		pbOutData[1] = 0x3F & (LOWORD(dwUnit >> 4));
		dwUnit = MAKEWORD(pbInData[2], pbInData[1]);
		pbOutData[2] = 0x3F & (LOWORD(dwUnit >> 6));
		pbOutData[3] = 0x3F & pbInData[2];

		pbInData += 3;
		pbOutData += 4;
		dwCount--;
	}

	if(dwRest == 1)
	{
		pbOutData[0] = 0x3F & (pbInData[0] >> 2);
		dwUnit = MAKEWORD(0, pbInData[0]);
		pbOutData[1] = 0x3F & (LOWORD(dwUnit >> 4));
		pbOutData[2] = pbOutData[3] = '=';
	}
	else if(dwRest == 2)
	{
		pbOutData[0] = 0x3F & (pbInData[0] >> 2);
		dwUnit = MAKEWORD(pbInData[1], pbInData[0]);
		pbOutData[1] = 0x3F & (LOWORD(dwUnit >> 4));
		dwUnit = MAKEWORD(0, pbInData[1]);
		pbOutData[2] = 0x3F & (LOWORD(dwUnit >> 6));
		pbOutData[3] = '=';
	}
	
	while(dwOutLen-- > 0)
	{
		if ( dwOutLen < ( 3 - ( dwRest == 0 ? 3 : dwRest) ) ) continue;
		
		*pbBuf = g_szBase64Table[*pbBuf];
		pbBuf++;
	}
		
	return TRUE;
}

//-------------------------------------------------------------------
//	���ܣ�
//		Base64����
//
//	���أ�
//		TRUE:�ɹ�	FALSE:ʧ��
//
//  ������
//		LPBYTE pbInData			Base64����
//		DWORD dwInDataLen		Base64����ĳ���
//		LPBYTE pbOutData		���������
//		LPDWORD pdwOutDataLen	������ݵĳ���
//
//  ˵����
//-------------------------------------------------------------------
BOOL __stdcall SC_Base64Decode(
							   IN LPBYTE pbInData, 
							   IN DWORD dwInDataLen, 
							   OUT LPBYTE pbOutData,
							   IN OUT LPDWORD pdwOutDataLen
							   )
{
	DWORD dwOutLen = (dwInDataLen/4 ) * 3;
	DWORD dwRest = 0;
	if(pbInData[dwInDataLen - 1] == '=')
	{
		dwRest++;
		if(pbInData[dwInDataLen - 2] == '=')
			dwRest++;
	}
	dwOutLen -= dwRest;

	DWORD dwCount = *pdwOutDataLen;
	*pdwOutDataLen = dwOutLen;
	if((dwCount < dwOutLen) || (pbOutData == NULL))
		return FALSE;
	
	BYTE *pbBuf = new BYTE[dwInDataLen];
	memcpy(pbBuf, pbInData, dwInDataLen);
	pbInData = pbBuf;

	dwOutLen = dwInDataLen - dwRest;
	while(dwOutLen-- > 0)
	{
		if(*pbBuf == '=')
			return FALSE;

		*pbBuf = (BYTE)(strchr(g_szBase64Table, *pbBuf) - g_szBase64Table);
		pbBuf++;
	}
	
	pbBuf = pbInData;
	dwCount = dwInDataLen/4 - 1;
	while(dwCount > 0)
	{
		pbOutData[0] = ((pbBuf[0] << 2) | ((pbBuf[1] >> 4) & 0x03));
		pbOutData[1] = ((pbBuf[1] << 4) | ((pbBuf[2] >> 2) & 0x0F));
		pbOutData[2] = ((pbBuf[2] << 6) | pbBuf[3]);

		pbBuf += 4;
		pbOutData += 3;
		dwCount--;
	}
	
	dwRest = 3 - dwRest;
	if(dwRest > 0)
		pbOutData[0] = ((pbBuf[0] << 2) | ((pbBuf[1] >> 4) & 0x03));
	if(dwRest > 1)
		pbOutData[1] = ((pbBuf[1] << 4) | ((pbBuf[2] >> 2) & 0x0F));
	if(dwRest > 2)
		pbOutData[2] = ((pbBuf[2] << 6) | pbBuf[3]);

	delete pbInData;

	return TRUE;
}

