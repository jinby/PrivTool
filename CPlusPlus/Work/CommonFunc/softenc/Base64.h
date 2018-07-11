//base64.h
#ifndef _BASE64_H_
#define _BASE64_H_

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
BOOL  __stdcall SC_Base64Encode(
							   IN LPBYTE pbInData, 
							   IN DWORD dwInDataLen, 
							   OUT LPBYTE pbOutData,
							   IN OUT LPDWORD pdwOutDataLen
							   );

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
BOOL  __stdcall SC_Base64Decode(
							   IN LPBYTE pbInData, 
							   IN DWORD dwInDataLen, 
							   OUT LPBYTE pbOutData,
							   IN OUT LPDWORD pdwOutDataLen
							   );


#endif
