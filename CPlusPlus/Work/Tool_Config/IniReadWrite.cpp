//// Source: http://www.codeproject.com/Articles/10809/A-Small-Class-to-Read-INI-File ////
#include "StdAfx.h"
#include "IniReadWrite.h"
#include <iostream>

CIniReadWrite::CIniReadWrite(LPCTSTR szFileName)
{
 memset(m_szFileName, 0x00, sizeof(m_szFileName));
 memcpy(m_szFileName, szFileName, _tcslen(szFileName)*sizeof(TCHAR));
}

int CIniReadWrite::ReadInteger(LPCTSTR szSection, LPCTSTR szKey, int iDefaultValue)
{
 int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 
 return iResult;
}

float CIniReadWrite::ReadFloat(LPCTSTR szSection, LPCTSTR szKey, float fltDefaultValue)
{
 TCHAR szResult[255];
 TCHAR szDefault[255];
 float fltResult;
 _stprintf(szDefault, TEXT("%f"),fltDefaultValue);
 GetPrivateProfileString(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
 fltResult =  (float)atof(szResult);
 return fltResult;
}

bool CIniReadWrite::ReadBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolDefaultValue)
{
 TCHAR szResult[255];
 TCHAR szDefault[255];
 bool bolResult;
 _stprintf(szDefault, TEXT("%s"), bolDefaultValue? TEXT("True") : TEXT("False"));
 GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
 bolResult =  (_tcscmp(szResult, TEXT("True")) == 0 || 
		_tcscmp(szResult, TEXT("true")) == 0) ? true : false;
 return bolResult;
}

LPTSTR CIniReadWrite::ReadString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefaultValue)
{
 LPTSTR szResult = new TCHAR[255];
 memset(szResult, 0x00, sizeof(szResult));
 GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, 255, m_szFileName); 
 return szResult;
}

void CIniReadWrite::WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int iValue)
{
 TCHAR szValue[255];
 _stprintf(szValue, TEXT("%d"), iValue);
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}

void CIniReadWrite::WriteFloat(LPCTSTR szSection, LPCTSTR szKey, float fltValue)
{
 TCHAR szValue[255];
 _stprintf(szValue, TEXT("%f"), fltValue);
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}

void CIniReadWrite::WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolValue)
{
 TCHAR szValue[255];
 _stprintf(szValue, TEXT("%s"), bolValue ? TEXT("True") : TEXT("False"));
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}

void CIniReadWrite::WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue)
{
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName);
}
