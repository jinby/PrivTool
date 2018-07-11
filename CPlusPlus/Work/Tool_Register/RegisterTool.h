// RegisterTool.h: interface for the CRegisterTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTERTOOL_H__75A51B3A_4BC3_47A9_AB64_F546DC33B815__INCLUDED_)
#define AFX_REGISTERTOOL_H__75A51B3A_4BC3_47A9_AB64_F546DC33B815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegisterTool  
{
public:
	CRegisterTool();
	virtual ~CRegisterTool();

	BOOL CheckReg();//¼ì²é×¢²áÐÅÏ¢

protected:
	BOOL GenerateRegRequest();
	void GetRegisterInfo(CString sComptrMac, CString sRegValueName, CString &m_sn,CString &m_regno,BYTE *expire);
	BOOL CheckRegisterNo(CString &str,CString &str1);
	CString GenerateRegisterNo(CString &str);
	void SpreadSubKey(unsigned char *KeyValue, unsigned char *Data, unsigned char ArithFlag=0);

};

#endif // !defined(AFX_REGISTERTOOL_H__75A51B3A_4BC3_47A9_AB64_F546DC33B815__INCLUDED_)
