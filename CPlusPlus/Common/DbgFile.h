/*
 *	TRACE信息到一个日志文件中
 */
#ifndef _DBGFILE_H_
#define _DBGFILE_H_


#include "tchar.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <string>
using namespace std;
#pragma warning(disable:4996)

//////////////////////////////////////////////////////////////////////////
//Trace functions
static FILE* g_hFile = NULL;
static const char *szFileName = "trace.log";
//#define MAX_PATH 256


inline void InitFile()
{
	if(g_hFile != NULL)
		return;
	char szCurName[MAX_PATH] = {0};
	int index = 0;
	extern string GetDLLPath();
	string sdllpath = GetDLLPath();

	while(1)
	{
		sprintf(szCurName, _T("%s\\trace%d.log"), sdllpath.c_str(), index++);
		g_hFile = fopen(szCurName, "r");
		if(!g_hFile)
			break;
		fclose(g_hFile);
		g_hFile = NULL;
	}
	g_hFile = fopen(szCurName, "w");
}
inline void TraceLine(char * pFormat, ...)
{
	char buffer[20000];
	va_list args;

	if(g_hFile == NULL)
		InitFile();
	if(g_hFile == NULL)
		return;

	
	va_start(args, pFormat);
 	vsprintf(buffer, pFormat, args);
    va_end(args);
	
	fwrite(buffer, strlen(buffer), 1, g_hFile);

	fflush(g_hFile);
}

inline void TraceData(unsigned char *pbBuf, unsigned int dwBufLen)
{
	unsigned int i = 0;
	for(i=0; i<dwBufLen; i++)
		TraceLine("%02X", pbBuf[i]);
	
	TraceLine("\n");
}


inline void TraceTime()
{
	time_t t = time(0);
	struct tm *ptm = localtime(&t);
	TraceLine("%s\n", asctime(ptm));
}

inline void DbgOutLastError(LPCTSTR pFormat, ...)
{
   if (::GetLastError() == 0) 
        return ;
   
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
	if(result == 0)
		lstrcat(buffer, _T("Can't find the error message"));
	else
		lstrcat (buffer, (TCHAR*)pMessage);
    
    TraceLine(buffer);
    
    if(result)
        ::LocalFree(pMessage);
   
    va_end(args);
    return ;
}

inline void DbgOutError(DWORD dwError)
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
    
    TraceLine(buffer);
    
    if(result)
        ::LocalFree(pMessage);

	return ;
}

#ifdef _TRACE_INFO
/*#define TRACE_LASTERROR			::DbgOutLastError*/
#define TRACE_ERROR				::DbgOutError
#else
#define TRACE_LASTERROR			(void(0))
#define TRACE_ERROR				(void(0))
#endif


#ifdef _TRACE_INFO

#define TRACE_LINE	TraceLine
#define TRACE_DATA(A, B) TraceData(A, B)
#define TRACE_TIME()	TraceTime()




#ifdef __cplusplus
class CTraceFile
{
public:
	CTraceFile(FILE **phFile)
	{
		m_phFile = phFile;
	}
	~CTraceFile()
	{
		if((*m_phFile) != NULL)
			fclose(*m_phFile);
	}

protected:
private:
	FILE **m_phFile;
};

static CTraceFile g_TraceFile(&g_hFile);


class CTraceFunction{
private:
	TCHAR*	m_szFunctionName;
public:
	CTraceFunction(LPCTSTR lpszFunctionName)
	{
		m_szFunctionName = NULL;

		if(lpszFunctionName){
			int nLen = strlen(lpszFunctionName) + 1;
			m_szFunctionName = new TCHAR[nLen];
			memset(m_szFunctionName,0,nLen);
			
			if(m_szFunctionName)
				memcpy(m_szFunctionName, lpszFunctionName, nLen);
		}

		if(m_szFunctionName){
			TRACE_LINE("\n============================================\n");
			TRACE_LINE("Enter Function: %s\n", m_szFunctionName);
			//TRACE_TIME();
		}
	}

	~CTraceFunction()
	{
		if(m_szFunctionName){
			TRACE_LINE("\nExit Function: %s\n", m_szFunctionName);
			TRACE_LINE("============================================\n");
			//TRACE_TIME();
			delete [] m_szFunctionName;
		}
	}
};



#define TRACE_FUNCTION(A) CTraceFunction tf(A)
#else//__cplusplus

#define TRACE_FUNCTION(A) void(0)

#endif//__cplusplus

#define TRACE_RESULT(bRetVal); \
{ \
	if(bRetVal == TRUE) \
		TRACE_LINE("\nFunction execute success!\n"); \
	else \
		TRACE_LINE("\nFunction execute fail!\n"); \
} \


#else//_TRACE_INFO
#define TRACE_RESULT(bRetVal);
#define TRACE_LINE	void(0)
#define TRACE_DATA(A, B) void(0)
#define TRACE_FUNCTION(A) void(0)
#define TRACE_TIME()	void(0)
#endif//_TRACE_INFO

#endif//_DBGFILE_H_
