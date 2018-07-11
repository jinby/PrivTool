
#include "stdafx.h"   
#include "Registry.h"   

void SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)  
{  
	if (NULL==lpSystemInfo)    
		return;  
	typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);  
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress( GetModuleHandle("kernel32"), "GetNativeSystemInfo");;  
	if (NULL != fnGetNativeSystemInfo)  
	{  
		fnGetNativeSystemInfo(lpSystemInfo);  
	}  
	else  
	{  
		GetSystemInfo(lpSystemInfo);  
	} 
}  



/////////////////////////////////////////////////////////////////////////////   
// CRegistry   

CRegistry::CRegistry(HKEY hKey)   
{   
	m_hKey=hKey;   
	m_samDesiredOpen = KEY_READ|KEY_WRITE;

	SafeGetNativeSystemInfo(&m_si); 
	if (m_si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||  
		m_si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )  
		m_samDesiredOpen |= KEY_WOW64_64KEY;  
}

CRegistry::~CRegistry()   
{   
	Close();   
}   

/////////////////////////////////////////////////////////////////////////////   
// CRegistry Functions   

bool CRegistry::CreateKey(string sSubKey)   
{   
	RegEnableReflectionKey(m_hKey);  
	HKEY hKey;   
	DWORD dw;   
	long lReturn=RegCreateKeyEx(m_hKey,sSubKey.c_str(),0L,NULL,REG_OPTION_NON_VOLATILE,m_samDesiredOpen,NULL,&hKey,&dw);   

	if(lReturn==ERROR_SUCCESS)   
	{   
		m_hKey=hKey;   
		return true;   
	}   

	return false;   

}   

bool CRegistry::Open(string sSubKey)   
{  
	HKEY hKey;   
	long lReturn=RegOpenKeyEx(m_hKey,sSubKey.c_str(),0L,m_samDesiredOpen,&hKey);   

	if(lReturn==ERROR_SUCCESS)   
	{   
		m_hKey=hKey;   
		return true;   
	}
	return false;   

}   

void CRegistry::Close()   
{   
	if(m_hKey)   
	{   
		RegCloseKey(m_hKey);   
		m_hKey=NULL;   
	}   
}   

bool CRegistry::DeleteValue(string sValueName)   
{     
	long lReturn=RegDeleteValue(m_hKey,sValueName.c_str());   

	if(lReturn==ERROR_SUCCESS)   
		return true;   
	return false;   

}   

bool CRegistry::DeleteKey(HKEY hKey, string sSubKey)   
{   
	long lReturn=RegDeleteValue(hKey,sSubKey.c_str());   

	if(lReturn==ERROR_SUCCESS)   
		return true;   
	return false;   
}   

bool CRegistry::Write(string sSubKey, int nVal)   
{   
	DWORD dwValue;   
	dwValue=(DWORD)nVal;   

	long lReturn=RegSetValueEx(m_hKey,sSubKey.c_str(),0L,REG_DWORD,(const BYTE *) &dwValue,sizeof(DWORD));   

	if(lReturn==ERROR_SUCCESS)   
		return true;   

	return false;   

}   

bool CRegistry::Write(string sSubKey, DWORD dwVal)   
{   
	long lReturn=RegSetValueEx(m_hKey,sSubKey.c_str(),0L,REG_DWORD,(const BYTE *) &dwVal,sizeof(DWORD));   

	if(lReturn==ERROR_SUCCESS)   
		return true;   

	return false;   

}   

bool CRegistry::Write(string sValueName, string sValue)   
{   
	long lReturn=RegSetValueEx(m_hKey,sValueName.c_str(),0L,REG_SZ,(const BYTE *)sValue.c_str(),sValue.size()+1);   

	if(lReturn==ERROR_SUCCESS)   
		return true;   

	return false;   

}   


bool CRegistry::Read(string sValueName, int& nVal)   
{   
	DWORD dwType;   
	DWORD dwSize=sizeof(DWORD);   
	DWORD dwDest;   
	long lReturn=RegQueryValueEx(m_hKey,sValueName.c_str(),NULL,&dwType,(BYTE *)&dwDest,&dwSize);   

	if(lReturn==ERROR_SUCCESS)   
	{   
		nVal=(int)dwDest;   
		return true;   
	}   
	return false;   

}   

bool CRegistry::Read(string sValueName, DWORD& dwVal)   
{   

	DWORD dwType;   
	DWORD dwSize=sizeof(DWORD);   
	DWORD dwDest;   
	long lReturn=RegQueryValueEx(m_hKey,sValueName.c_str(),NULL,&dwType,(BYTE *)&dwDest,&dwSize);   

	if(lReturn==ERROR_SUCCESS)   
	{   
		dwVal=dwDest;   
		return true;   
	}   
	return false;   
}   




bool CRegistry::RestoreKey(string sFileName)   
{
	long lReturn=RegRestoreKey(m_hKey,sFileName.c_str(),REG_WHOLE_HIVE_VOLATILE);   

	if(lReturn==ERROR_SUCCESS)   
		return true;   

	return false;   
}   

bool CRegistry::SaveKey(string sFileName)
{
	long lReturn=RegSaveKey(m_hKey,sFileName.c_str(),NULL);   

	if(lReturn==ERROR_SUCCESS)   
		return true;   

	return false;   
}

bool CRegistry::Read(string sValueName, string& sVal)   
{   
	DWORD dwType;   
	DWORD dwSize=200;   
	char szString[2550];   

	long lReturn=RegQueryValueEx(m_hKey,sValueName.c_str(),NULL,&dwType,(BYTE *)szString,&dwSize);   

	if(lReturn==ERROR_SUCCESS)   
	{   
		sVal=szString;   
		return true;   
	}   
	return false;   

}  
