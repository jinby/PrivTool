#ifndef _DBGOUT_INCLUDED_
#define _DBGOUT_INCLUDED_

#ifdef _LOG_OUTPUT_FILE

#include "tchar.h"
#include "stdio.h"

#define TRACE_WINDOW_0			0
#define TRACE_WINDOW_1			1
#define TRACE_WINDOW_2			2
#define TRACE_WINDOW_3			3
#define TRACE_WINDOW_4			4
#define TRACE_WINDOW_5			5
#define TRACE_WINDOW_6			6
#define TRACE_WINDOW_7			7
#define TRACE_WINDOW_8			8
#define TRACE_WINDOW_9			9

static TCHAR tszWindowName[] = _T("TRACE WINDOW");
static TCHAR tszClassName[] =  _T("TRACE WINDOW");

static int g_nTraceLevel = -1;

#define DEFAULT_TRACE_LEVEL		0

inline void GetTraceLevel(int* pLevel)
{
	if(pLevel == NULL)
		return;
	*pLevel = DEFAULT_TRACE_LEVEL;
	HKEY hRegKey = NULL;
	LONG rv = RegOpenKey(HKEY_CURRENT_USER, "Software\\Tianyu Info", &hRegKey);
	if(rv != ERROR_SUCCESS)
		return;

	DWORD dwType;
	DWORD dwValue, dwSize = sizeof(DWORD);
	rv = RegQueryValueEx(hRegKey, "TraceLevel", NULL, &dwType, (LPBYTE)&dwValue, &dwSize);
	RegCloseKey(hRegKey);
	if(rv != ERROR_SUCCESS){
		return;
	}

	*pLevel = dwValue;
}

inline void DbgOutA(LPCSTR p, LPCTSTR lpszWindowName, LPCTSTR lpszClassName)
{
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (lpszClassName, lpszWindowName); 
    if (hWnd)
    {  
        cd.dwData = 0;
        cd.cbData = (strlen(p)+1)*sizeof(char);
        cd.lpData = (void *)p; 
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
}

inline void DbgOutW(LPCWSTR p, LPCTSTR lpszWindowName, LPCTSTR lpszClassName)
{
    COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (lpszClassName, lpszWindowName); 
    if (hWnd)
    {  
        cd.dwData = 0xFEFF;
        cd.cbData = (wcslen(p)+1)*sizeof(wchar_t);
        cd.lpData = (void *)p; 
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);  
    } 
}

inline void DbgOut(LPCTSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);

    TCHAR buffer [16*1024*sizeof(TCHAR)];
	wvsprintf(buffer, pFormat, args);

	TCHAR szWindowName[MAX_PATH];
	TCHAR szClassName[MAX_PATH];
	wsprintf(szWindowName, "%s %d", tszWindowName, 0);
	wsprintf(szClassName, "%s %d", tszClassName, 0);

    #ifdef UNICODE
    DbgOutW(buffer, szWindowName, szClassName);
    #else
    DbgOutA(buffer, szWindowName, szClassName);
    #endif

    va_end(args);
}

inline DWORD DbgOutLastError(LPCTSTR pFormat, ...)
{
   if (::GetLastError() == 0) 
        return 0;
   
	va_list args;
	va_start(args, pFormat);

    TCHAR buffer [1024*sizeof(TCHAR)];
	wvsprintf(buffer, pFormat, args);

    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             GetLastError(),
                             MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);
  
    lstrcat (buffer, _T(" : "));
    lstrcat (buffer, (TCHAR*)pMessage);
    
    ::DbgOut(buffer);
    
    if(result)
        ::LocalFree(pMessage);
   
    va_end(args);
    return result;
}

inline DWORD DbgOutError(DWORD dwError)
{
    TCHAR buffer [1024*sizeof(TCHAR)];
	sprintf(buffer, _T("Error Code = %08X  Description = "), dwError);
	
    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             dwError,
                             MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);
  
	if(result == 0)
		lstrcat(buffer, _T("Can't find the error message"));
	else
		lstrcat (buffer, (TCHAR*)pMessage);
    
    ::DbgOut(buffer);
    
    if(result)
        ::LocalFree(pMessage);

	return result;
}

inline void DbgOutEx(int nIndex, LPCTSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);

    TCHAR buffer [1024*sizeof(TCHAR)*16];
	wvsprintf(buffer, pFormat, args);

	TCHAR szWindowName[MAX_PATH];
	TCHAR szClassName[MAX_PATH];
	wsprintf(szWindowName, "%s %d", tszWindowName, nIndex);
	wsprintf(szClassName, "%s %d", tszClassName, nIndex);

    #ifdef UNICODE
    DbgOutW(buffer, szWindowName, szClassName);
    #else
    DbgOutA(buffer, szWindowName, szClassName);
    #endif

    va_end(args);
}

inline DWORD DbgOutLastErrorEx(int nIndex, LPCTSTR pFormat, ...)
{
   if (::GetLastError() == 0) 
        return 0;
   
	va_list args;
	va_start(args, pFormat);

    TCHAR buffer [1024*sizeof(TCHAR)];
	wvsprintf(buffer, pFormat, args);

    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             GetLastError(),
                             MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);
  
    lstrcat(buffer, _T(" : "));
    lstrcat(buffer, (TCHAR*)pMessage);
    
    ::DbgOutEx(nIndex, buffer);
    
    if(result)
        ::LocalFree(pMessage);
   
    va_end(args);
    return result;
}

inline DWORD DbgOutErrorEx(int nIndex, DWORD dwError)
{
    TCHAR buffer [1024*sizeof(TCHAR)];
	sprintf(buffer, _T("Error Code = %08X  Description = "), dwError);
	
    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             dwError,
                             MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);
  
	if(result == 0)
		lstrcat(buffer, _T("Can't find the error message"));
	else
		lstrcat (buffer, (TCHAR*)pMessage);
    
    ::DbgOutEx(nIndex, buffer);
    
    if(result)
        ::LocalFree(pMessage);

	return result;
}

inline void DbgOutLevel(int nLevel, LPCTSTR pFormat, ...)
{
	if(g_nTraceLevel < 0)
		GetTraceLevel(&g_nTraceLevel);
	if(g_nTraceLevel < nLevel)
		return;

	va_list args;
	va_start(args, pFormat);

    TCHAR buffer [1024*sizeof(TCHAR)*16];
	wvsprintf(buffer, pFormat, args);

	TCHAR szWindowName[MAX_PATH];
	TCHAR szClassName[MAX_PATH];
	wsprintf(szWindowName, "%s %d", tszWindowName, 0);
	wsprintf(szClassName, "%s %d", tszClassName, 0);

    #ifdef UNICODE
    DbgOutW(buffer, szWindowName, szClassName);
    #else
    DbgOutA(buffer, szWindowName, szClassName);
    #endif

    va_end(args);
}

inline DWORD DbgOutLastErrorLevel(int nLevel, LPCTSTR pFormat, ...)
{
   if (::GetLastError() == 0) 
        return 0;
   
	va_list args;
	va_start(args, pFormat);

    TCHAR buffer [1024*sizeof(TCHAR)];
	wvsprintf(buffer, pFormat, args);

    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             GetLastError(),
                             MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);
  
    lstrcat(buffer, _T(" : "));
    lstrcat(buffer, (TCHAR*)pMessage);
    
    ::DbgOutLevel(nLevel, buffer);
    
    if(result)
        ::LocalFree(pMessage);
   
    va_end(args);
    return result;
}

inline DWORD DbgOutErrorLevel(int nLevel, DWORD dwError)
{
    TCHAR buffer [1024*sizeof(TCHAR)];
	sprintf(buffer, _T("Error Code = %08X  Description = "), dwError);
	
    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             dwError,
                             MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);
  
	if(result == 0)
		lstrcat(buffer, _T("Can't find the error message"));
	else
		lstrcat (buffer, (TCHAR*)pMessage);
    
    ::DbgOutLevel(nLevel, buffer);
    
    if(result)
        ::LocalFree(pMessage);

	return result;
}



#ifdef _TRACE_INFO

#define TRACE_LINE				::DbgOut
#define TRACE_LASTERROR			::DbgOutLastError
#define TRACE_ERROR				::DbgOutError

#define TRACE_LINE_EX			::DbgOutEx
#define TRACE_LASTERROR_EX		::DbgOutLastErrorEx
#define TRACE_ERROR_EX			::DbgOutErrorEx

#define TRACE_LINE_LEVEL		::DbgOutLevel
#define TRACE_LASTERROR_LEVEL	::DbgOutLastErrorLevel
#define TRACE_ERROR_LEVEL		::DbgOutErrorLevel

class CTraceFunction{
private:
	TCHAR*	m_szFunctionName;
	int m_nIndex;
public:
	CTraceFunction(LPCTSTR lpszFunctionName, int nIndex = 0)
	{
		m_szFunctionName = NULL;
		m_nIndex = nIndex;

		if(lpszFunctionName){
			int nLen = _tcslen(lpszFunctionName) + 1;
			m_szFunctionName = new TCHAR[nLen];
			if(m_szFunctionName)
				memcpy(m_szFunctionName, lpszFunctionName, nLen);
		}

		if(m_szFunctionName){
			TRACE_LINE_EX(m_nIndex, "\n============================================\n");
			TRACE_LINE_EX(m_nIndex, "Enter Function: %s\n", m_szFunctionName);
		}
	}

	~CTraceFunction()
	{
		if(m_szFunctionName){
			TRACE_LINE_EX(m_nIndex, "\nExit Function: %s\n", m_szFunctionName);
			TRACE_LINE_EX(m_nIndex, "============================================\n");
			delete []m_szFunctionName;
		}
	}
};

class CTraceFunctionLevel{
private:
	TCHAR* m_szFunctionName;
	int m_nLevel;
public:
	CTraceFunctionLevel(LPCTSTR lpszFunctionName, int nLevel = 0)
	{
		m_szFunctionName = NULL;
		m_nLevel = nLevel;

		if(lpszFunctionName){
			int nLen = _tcslen(lpszFunctionName) + 1;
			m_szFunctionName = new TCHAR[nLen];
			memset(m_szFunctionName,0,nLen);
			
			if(m_szFunctionName)
				memcpy(m_szFunctionName, lpszFunctionName, nLen);
		}

		if(m_szFunctionName){
			TRACE_LINE_EX(m_nLevel, "\n============================================\n");
			TRACE_LINE_EX(m_nLevel, "Enter Function: %s\n", m_szFunctionName);
		}
	}

	~CTraceFunctionLevel()
	{
		if(m_szFunctionName){
			TRACE_LINE_EX(m_nLevel, "\nExit Function: %s\n", m_szFunctionName);
			TRACE_LINE_EX(m_nLevel, "============================================\n");
			delete m_szFunctionName;
		}
	}
};

#define TRACE_FUNCTION(szFuncName); \
	CTraceFunction tf(szFuncName, 0); \

#define TRACE_DATA(pbData, dwDataLen);\
{\
	if(pbData != NULL){\
		for(DWORD i = 0; i < dwDataLen; i++)\
			TRACE_LINE("%02X", pbData[i]);\
		TRACE_LINE("\n");\
	}\
}\

#define TRACE_FUNCTION_EX(nIndex, szFuncName); \
	CTraceFunction tf(szFuncName, nIndex); \

#define TRACE_DATA_EX(nIndex, pbData, dwDataLen);\
{\
	if(pbData != NULL){\
		for(DWORD i = 0; i < dwDataLen; i++)\
			TRACE_LINE_EX(nIndex, "%02X", pbData[i]);\
		TRACE_LINE_EX(nIndex, "\n");\
	}\
}\

#define TRACE_DATA_LEVEL(nLevel, pbData, dwDataLen);\
{\
	if(pbData != NULL){\
		for(DWORD i = 0; i < dwDataLen; i++)\
			TRACE_LINE_LEVEL(nLevel, "%02X", pbData[i]);\
		TRACE_LINE_LEVEL(nLevel, "\n");\
	}\
}\
	
#define TRACE_FUNCTION_LEVEL(nLevel, szFuncName); \
	CTraceFunctionLevel tf(szFuncName, nLevel); \

#else

#define TRACE_LINE										//
#define TRACE_ERROR										//
#define TRACE_LASTERROR									//
#define TRACE_FUNCTION(szFuncName)						//
#define TRACE_DATA(pbData, dwDataLen)					//

#define TRACE_LINE_EX									//
#define TRACE_LASTERROR_EX								//
#define TRACE_ERROR_EX									//
#define TRACE_FUNCTION_EX(nIndex, szFuncName)			//
#define TRACE_DATA_EX(nIndex, pbData, dwDataLen)		//

#define TRACE_LINE_LEVEL								//
#define TRACE_LASTERROR_LEVEL							//
#define TRACE_ERROR_LEVEL								//
#define TRACE_FUNCTION_LEVEL(nLevel, szFuncName)		//
#define TRACE_DATA_LEVEL(nLevel, pbData, dwDataLen)		//

#endif	//ifdef _TRACE_INFO

#endif  //ifndef _LOG_OUTPUT_FILE
#endif	//ifndef _DBGOUT_INCLUDED_
