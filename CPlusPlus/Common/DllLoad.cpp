#include "StdAfx.h"
#include "DllLoad.h"

CFunList::CFunList()
{
	m_nCount = 0;
	m_hLib = 0;
	m_vecFunDispense.clear();
}

CFunList::CFunList(const char*	pthDllName)
{
	m_strLibName = pthDllName;
	m_hLib = 0;
	m_nCount = 0;
	InitialLibrary();
}

BOOL CFunList::SetDllName(const char* pthDllName)
{
	m_strLibName = pthDllName;
	InitialLibrary();
	return TRUE;
}

CFunList::~CFunList(void)
{
	memset(&m_csRefCount,0,sizeof(m_csRefCount));
	ReleaseLibrary();
}

BOOL CFunList::InitialLibrary()
{
	if(m_hLib)
		ReleaseLibrary();

	
	

	BOOL bRet = FALSE;
	m_hLib = LoadLibraryEx(m_strLibName.c_str(),0,0);
	if(m_hLib == NULL)
		return FALSE;


	LoadFunInterface(m_hLib,m_stFP,iReadCardBas)
		LoadFunInterface(m_hLib,m_stFP,iReadCardBas_HSM_Step1)
		LoadFunInterface(m_hLib,m_stFP,iReadCardBas_HSM_Step2)
		LoadFunInterface(m_hLib,m_stFP,iReadCard)
		LoadFunInterface(m_hLib,m_stFP,iReadCard_HSM_Step1)
		LoadFunInterface(m_hLib,m_stFP,iReadCard_HSM_Step2)
		LoadFunInterface(m_hLib,m_stFP,iWriteCard)
		LoadFunInterface(m_hLib,m_stFP,iWriteCard_HSM_Step1)
		LoadFunInterface(m_hLib,m_stFP,iWriteCard_HSM_Step2)
		LoadFunInterface(m_hLib,m_stFP,iVerifyPIN)
		LoadFunInterface(m_hLib,m_stFP,iChangePIN)
		LoadFunInterface(m_hLib,m_stFP,iReloadPIN)
		LoadFunInterface(m_hLib,m_stFP,iReloadPIN_HSM_Step1)
		LoadFunInterface(m_hLib,m_stFP,iReloadPIN_HSM_Step2)
		LoadFunInterface(m_hLib,m_stFP,iReloadPIN_HSM_Step3)
		LoadFunInterface(m_hLib,m_stFP,iUnblockPIN)
		LoadFunInterface(m_hLib,m_stFP,iUnblockPIN_HSM_Step1)
		LoadFunInterface(m_hLib,m_stFP,iUnblockPIN_HSM_Step2)
		LoadFunInterface(m_hLib,m_stFP,iUnblockPIN_HSM_Step3)
		LoadFunInterface(m_hLib,m_stFP,iDoDebit)
		LoadFunInterface(m_hLib,m_stFP,iDoDebit_HSM_Step1)
		LoadFunInterface(m_hLib,m_stFP,iDoDebit_HSM_Step2)
		LoadFunInterface(m_hLib,m_stFP,iReadDebitRecord)


	//初始化关键代码段结构体
	InitializeCriticalSection(&m_csRefCount);

	if(!InterfaceIsValid())
	{
		ReleaseLibrary();
		return FALSE;
	}

	return TRUE;
}
BOOL CFunList::ReleaseLibrary()
{

	memset(&m_stFP,0,sizeof(m_stFP));
	if(m_hLib != NULL)
	{
		DeleteCriticalSection(&m_csRefCount);
		FreeLibrary(m_hLib);
		m_hLib = 0;
		return TRUE;
	}

	return FALSE;
}

BOOL CFunList::InterfaceIsValid()
{
	DWORD		dwCountFun = sizeof(m_stFP)/sizeof(DWORD);
	DWORD		dwTemp = 0;
	for(DWORD i = 0;i < dwCountFun;i++)
	{
		if( memcmp((LPVOID)((DWORD)(&m_stFP) + (i<<2)),&dwTemp,4) == 0 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

int CFunList::AddRef()
{
	EnterCriticalSection(&m_csRefCount);
	m_nCount++;
	LeaveCriticalSection(&m_csRefCount);

	return m_nCount;
}

int CFunList::RelRef()
{
	EnterCriticalSection(&m_csRefCount);
	m_nCount--;
	LeaveCriticalSection(&m_csRefCount);
	return m_nCount;
}


