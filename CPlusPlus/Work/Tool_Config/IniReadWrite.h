//// Source: http://www.codeproject.com/Articles/10809/A-Small-Class-to-Read-INI-File ////

#ifndef INIREADER_H
#define INIREADER_H

class CIniReadWrite
{
public:
 CIniReadWrite(LPCTSTR szFileName); 
 
 int ReadInteger(LPCTSTR szSection, LPCTSTR szKey, int iDefaultValue);
 float ReadFloat(LPCTSTR szSection, LPCTSTR szKey, float fltDefaultValue);
 bool ReadBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolDefaultValue);
 LPTSTR ReadString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefaultValue);
 
 void WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int iValue);
 void WriteFloat(LPCTSTR szSection, LPCTSTR szKey, float fltValue);
 void WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolValue);
 void WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue);
 
private:
  TCHAR m_szFileName[255];
};
#endif //INIREADER_H