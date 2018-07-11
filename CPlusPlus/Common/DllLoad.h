#pragma once
#include <string>
#include <vector>
using namespace std;

//结构体宏定义
#define DeclearFun(FunName) tfn_##FunName lpfn_##FunName

//获取DLL函数指针宏定义
#define		LoadFunInterface(hLib,stName,fnName)						\
	stName.lpfn_##fnName = (tfn_##fnName)GetProcAddress(hLib,(LPCSTR)(#fnName));	\

#define TYCPS_CLIENT_INT		__stdcall

typedef long (__stdcall * tfn_iReadCardBas)(int iType, char* pOutInfo);
typedef long (__stdcall * tfn_iReadCardBas_HSM_Step1)(int iType, char* pOutInfo);
typedef long (__stdcall * tfn_iReadCardBas_HSM_Step2)(char *pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iReadCard )(int iType, int iAuthType, char* pCardInfo, char* pFileAddr, char* pOutInfo);
typedef long (__stdcall * tfn_iReadCard_HSM_Step1)(int iType, char* pCardInfo, char* pFileAddr, char* pOutInfo);
typedef long (__stdcall * tfn_iReadCard_HSM_Step2)(char* pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iWriteCard )(int iType, char* pCardInfo, char* pFileAddr, char* pWriteData, char* pOutInfo);
typedef long (__stdcall * tfn_iWriteCard_HSM_Step1)(int iType, char* pCardInfo, char* pFileAddr, char* pOutInfo);
typedef long (__stdcall * tfn_iWriteCard_HSM_Step2)(char* pKey, char* pWriteData, char* pOutInfo);
typedef long (__stdcall * tfn_iVerifyPIN)(int iType, char* pOutInfo);
typedef long (__stdcall * tfn_iChangePIN)(int iType, char* pOutInfo);
typedef long (__stdcall * tfn_iReloadPIN)(int iType, char* pCardInfo, char* pOutInfo);
typedef long (__stdcall * tfn_iReloadPIN_HSM_Step1)(int iType, char* pCardInfo, char* pOutInfo);
typedef long (__stdcall * tfn_iReloadPIN_HSM_Step2)(char* pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iReloadPIN_HSM_Step3)(char* pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iUnblockPIN)(int iType, char* pCardInfo, char* pOutInfo);
typedef long (__stdcall * tfn_iUnblockPIN_HSM_Step1)(int iType, char* pCardInfo, char* pOutInfo);
typedef long (__stdcall * tfn_iUnblockPIN_HSM_Step2)(char* pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iUnblockPIN_HSM_Step3)(char* pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iDoDebit)(int iType, char* pCardInfo, char* pPayInfo, char* pOutInfo);
typedef long (__stdcall * tfn_iDoDebit_HSM_Step1)(int iType, char* pCardInfo, char* pPayInfo, char* pOutInfo);
typedef long (__stdcall * tfn_iDoDebit_HSM_Step2 )(char* pKey, char* pOutInfo);
typedef long (__stdcall * tfn_iReadDebitRecord)(int iType, char* pOutInfo);


typedef struct _FUN_POINTER_
{
	DeclearFun(iReadCardBas);
	DeclearFun(iReadCardBas_HSM_Step1);
	DeclearFun(iReadCardBas_HSM_Step2);
	DeclearFun(iReadCard);
	DeclearFun(iReadCard_HSM_Step1);
	DeclearFun(iReadCard_HSM_Step2);
	DeclearFun(iWriteCard);
	DeclearFun(iWriteCard_HSM_Step1);
	DeclearFun(iWriteCard_HSM_Step2);
	DeclearFun(iVerifyPIN);
	DeclearFun(iChangePIN);
	DeclearFun(iReloadPIN);
	DeclearFun(iReloadPIN_HSM_Step1);
	DeclearFun(iReloadPIN_HSM_Step2);
	DeclearFun(iReloadPIN_HSM_Step3);
	DeclearFun(iUnblockPIN);
	DeclearFun(iUnblockPIN_HSM_Step1);
	DeclearFun(iUnblockPIN_HSM_Step2);
	DeclearFun(iUnblockPIN_HSM_Step3);
	DeclearFun(iDoDebit);
	DeclearFun(iDoDebit_HSM_Step1);
	DeclearFun(iDoDebit_HSM_Step2);
	DeclearFun(iReadDebitRecord);

}FUN_POINTER,*PFUN_POINTER;

class CFunList
{
public:
	CFunList();
	CFunList(const char* pthDllName);
	~CFunList();

	BOOL InitialLibrary();
	BOOL ReleaseLibrary();
	BOOL InterfaceIsValid();
	BOOL SetDllName(const char*	pthDllName);

	int AddRef();
	int RelRef();
public:
	FUN_POINTER			m_stFP;
	HINSTANCE			m_hLib;
	string				m_strLibName;
	CRITICAL_SECTION	m_csRefCount;
	int					m_nCount;
};

