#pragma once

#include <string>
using namespace std;

class CRegistry
{
	// Construction 
public: 
	CRegistry(HKEY hKey=HKEY_LOCAL_MACHINE); 
public: 
	bool SaveKey(string sFileName); 
	bool RestoreKey(string sFileName); 
	bool Read(string sValueName, string& sVal); 
	bool Read(string sValueName, DWORD& dwVal); 
	bool Read(string sValueName, int& nVal); 
	bool Write(string sSubKey, string sVal); 
	bool Write(string sSubKey, DWORD dwVal); 
	bool Write(string sSubKey, int nVal); 
	bool DeleteKey(HKEY hKey, string sSubKey); 
	bool DeleteValue(string sValueName); 
	void Close(); 
	bool Open(string sSubKey); 
	bool CreateKey(string sSubKey); 
	virtual ~CRegistry(); 

protected: 
	HKEY m_hKey; 
	SYSTEM_INFO m_si;  
	REGSAM m_samDesiredOpen;
};
