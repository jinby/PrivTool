//base64.h
#ifndef _BASE64_H_
#define _BASE64_H_

/////////////////////////////////////////////////////////////////////
//
//	Base64

//-------------------------------------------------------------------
//	功能：
//		Base64编码
//
//	返回：
//		TRUE:成功	FALSE:失败
//
//  参数：
//		LPBYTE pbInData			输入数据
//		DWORD dwInDataLen		输入数据的长度
//		LPBYTE pbOutData		输出的Base64编码
//		LPDWORD pdwOutDataLen	Base64编码后的长度
//
//  说明：
//-------------------------------------------------------------------
BOOL  __stdcall SC_Base64Encode(
							   IN LPBYTE pbInData, 
							   IN DWORD dwInDataLen, 
							   OUT LPBYTE pbOutData,
							   IN OUT LPDWORD pdwOutDataLen
							   );

//-------------------------------------------------------------------
//	功能：
//		Base64解码
//
//	返回：
//		TRUE:成功	FALSE:失败
//
//  参数：
//		LPBYTE pbInData			Base64编码
//		DWORD dwInDataLen		Base64编码的长度
//		LPBYTE pbOutData		输出的数据
//		LPDWORD pdwOutDataLen	输出数据的长度
//
//  说明：
//-------------------------------------------------------------------
BOOL  __stdcall SC_Base64Decode(
							   IN LPBYTE pbInData, 
							   IN DWORD dwInDataLen, 
							   OUT LPBYTE pbOutData,
							   IN OUT LPDWORD pdwOutDataLen
							   );


#endif
