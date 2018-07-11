// RegisterTool.cpp: implementation of the CRegisterTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegisterTool.h"
#include "glb.h"
#include "NetFunc.h"
#include "CmdAPIzhy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BYTE regkey[17]="\x67\x0A\x2D\x33\x2B\x70\x48\x26\x64\xB2\x31\x44\x7B\x98\xE9\x3B";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegisterTool::CRegisterTool()
{
}

CRegisterTool::~CRegisterTool()
{

}

//生成注册请求文件
BOOL CRegisterTool::GenerateRegRequest()
{
	char szCurName[MAX_PATH] = {0};
	sprintf(szCurName, _T("RegsterReq.txt"));
	FILE* hFile = fopen(szCurName, "w");
	if(!hFile)
	{
		AfxMessageBox("创建注册申请文件 RegsterReq.txt 失败");
		return FALSE;
	}

	CString sCompMac;
	if (!GenerateMac(sCompMac))
	{
		AfxMessageBox("生成注册申请码失败");
		fclose(hFile);
		return FALSE;
	}
	fwrite(sCompMac, sCompMac.GetLength(), 1, hFile);
	fclose(hFile);

	return TRUE;
}

//检查注册信息
BOOL CRegisterTool::CheckReg()
{
	CString sSN,sReg;
	CString str;
	BYTE expire[10];

	CStringArray computerInfo;
	GetComuterNetInfo(computerInfo);

	int i = 0;
	for (i = 0; i < computerInfo.GetSize(); i++)
	{
		CString sRegValueName;
		if (i ==0)
			sRegValueName = "RegisterInfo";
		else
			sRegValueName.Format("RegisterInfo%d", i);
		GetRegisterInfo(computerInfo[i], sRegValueName, sSN,sReg,expire);
		//if (CheckRegisterNo(sSN,sReg)){
		//	return TRUE;
		if (!CheckRegisterNo(sSN,sReg)){
			str = "您的注册信息不对，需要注册，请将RegsterReq.txt文件发送给武汉天喻信息管理人员"; 
			GenerateRegRequest();
			AfxMessageBox(str);
			return FALSE;
		}
	}
	str.Format("i = %d,computerInfo.GetSize() = %d\n",i,computerInfo.GetSize());
	//TRACE_LINE(str.GetBuffer(0));

	if (i > computerInfo.GetSize())
	{
		str = "您的注册信息不对，需要注册，请将RegsterReq.txt文件发送给武汉天喻信息管理人员"; 
		GenerateRegRequest();
		AfxMessageBox(str);
		return FALSE;	
	}

	BYTE sn[17];
	CTime currenttime=CTime::GetCurrentTime(); 
	HexToAscii(expire,sn,7);
	sn[14]=0x00;
	CString sexp;
	sexp.Format("%s",(char *)sn);
	//AfxMessageBox(sexp, 0, 0 );
	CTime expiredate(atoi(sexp.Mid(0,4)),atoi(sexp.Mid(4,2)),atoi(sexp.Mid(6,2)),atoi(sexp.Mid(8,2)),atoi(sexp.Mid(10,2)),atoi(sexp.Mid(12,2)));
	if (currenttime>expiredate){
		str = "您的程序使用期限已到，请将RegsterReq.txt文件发送给武汉天喻信息管理人员重新注册";
		//20081020
		GenerateRegRequest();
		AfxMessageBox(str);
		return FALSE;
	}
	return TRUE;
}

//检查注册号和注册码的正确性
//str 注册号  str1 注册码
BOOL CRegisterTool::CheckRegisterNo(CString &str,CString &str1){
	CString regno=GenerateRegisterNo(str);
	str1.Remove(' '); 
	if (!str1.CompareNoCase(regno))
		return TRUE;
	else
		return FALSE;
}

CString CRegisterTool::GenerateRegisterNo(CString &str){
	BYTE s1[17],sn[17],s2[33];
	CString regno;
	int i;
	memcpy(s1,"\x4A\xFB\x92\x12\xFB\xB8\xDA\xB6\xD1\x7E\x1F\x53\xBC\x65\x45\xE8",16);
	memset(sn,0x00,16);
	str.Remove(' '); 
	strcpy((char*)sn,str.Right(8)); 
	SpreadSubKey(s1,sn,0);
	for (i=0;i<8;i++) 
		sn[i+8] = ~sn[i];
	for (i=0;i<16;i++)
		s1[i]=s1[i]^sn[i];
	HexToAscii(s1,s2,16);
	s2[32]=0x00;
	regno.Format("%s",(char*)s2);
	return regno;
}


//////////////////////////////////////////////////////////////////////////
/*
功能：密钥分散(仅做一级分散）
输入：KeyValue--------密钥值（16位）
	  Data------------用于分散的数据(8位）
	  ArithFlag-------算法标识 0:trides 1:des
输出：KeyValue--------分散后的密钥
	fqh  11-9-2000  Ver1.0
*/
/////////////////////////////////////////////////////////////////////////
void CRegisterTool::SpreadSubKey(unsigned char *KeyValue,
				  unsigned char *Data,
				  unsigned char ArithFlag)
{
	unsigned char Temp[8],Temp1[8];
	int i;
	for (i=0;i<8;i++) 
		Temp[i] = Data[i];
	if(ArithFlag)
	{
		des(Temp,KeyValue,0);
		for(i=0;i<8;i++)
			KeyValue[i]=Temp[i];
		return;
	}
	tri_des(Temp,KeyValue,KeyValue+8,0);

	for (i=0;i<8;i++) 
		Temp1[i] = ~Data[i];
	tri_des(Temp1,KeyValue,KeyValue+8,0);
	for(i=0;i<8;i++)
		KeyValue[8+i]=Temp1[i];
	for(i=0;i<8;i++)
		KeyValue[i]=Temp[i];
}

//得到注册信息,返回expire的内容：日期+打开文件的方式
void CRegisterTool::GetRegisterInfo(CString sComptrMac, CString sRegValueName, CString &m_sn,CString &m_regno,BYTE *expire){
	BYTE sninfo[33],snasc[65];
	CString s1;
	
	memset(snasc,0x00,sizeof(snasc));
	//修改为使用网卡信息
//  	DWORD dwLen = MAX_COMPUTERNAME_LENGTH+1;
//  	GetComputerName((char*)snasc, &dwLen );
	memcpy(snasc, sComptrMac, sComptrMac.GetLength());

	//s1=GetProfileString(_T("Register"),_T("RegisterInfo"),"");
	HKEY hKey = NULL;
	long lgRes = ERROR_SUCCESS;
	CString strErrMsg;
	lgRes = RegCreateKey(HKEY_CURRENT_USER, "Software\\WHTYPbocPerson\\Register", &hKey);
	if (ERROR_SUCCESS != lgRes)
	{
		strErrMsg.Format("RegCreateKey err = 0x%x\n",lgRes);
		//TRACE_LINE(strErrMsg.GetBuffer(0));
		return;
	}
	char sReg[128];
	DWORD dwSize = 128;
	lgRes = RegQueryValueEx(hKey, sRegValueName, 0, NULL, (BYTE*)sReg, &dwSize);
	if (ERROR_SUCCESS != lgRes)
	{
		strErrMsg.Format("RegQueryValueEx err = 0x%x\n",lgRes);
		//TRACE_LINE(strErrMsg.GetBuffer(0));
		return;
	}
	s1 = sReg;

	if (s1.IsEmpty())
		return;
	AsciiToHex((BYTE*)(s1.GetBuffer(0)),sninfo,64);
	//复原明文的注册信息
	DesJS(sninfo,regkey,1,32,16);
	//复原原始的序列号	
	for (int i=0;i<8;i++)
		snasc[i]=sninfo[i]^snasc[i];
	snasc[8]=0x00;
	m_sn=snasc;
	//失效日期和类型
	memcpy(expire,sninfo+24,8);
	//注册号
	HexToAscii(sninfo+8,snasc,16);
	m_regno=snasc;
	return;
}