// Reader.h: interface for the CReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYREADER_H__088BF118_00E3_455D_93F5_7B2B8F6B6FB8__INCLUDED_)
#define AFX_MYREADER_H__088BF118_00E3_455D_93F5_7B2B8F6B6FB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SUCCESS							0               //成功
#define READER_ERROR					2               //读卡器错误,读卡器未连接好或坏读卡器
#define ERR_READERTYPE					200             //不支持的读卡器类型
#define ERR_COM_OPEN					201				//无法打开COM机具
#define ERR_COM_NOCARD					202				//COM机具上没有插入卡片
#define ERR_COM_RESET					203				//COM机具复位失败
#define ERR_READER_HANDLE				204				//PCSC句柄错误
#define ERR_READER						205				//列举读写器失败
#define ERR_CARD						206				//未插入卡片
#define ERR_CARD_INSERT					207				//请重新插入卡片
#define ERR_APDU						208				//执行卡片命令失败
#define ERR_LOAD_TYREADER_DLL			209				//引导Tyreader.dll失败
#define ERR_LOAD_TYREADER_FUNC			210				//获取Tyreader.dll函数地址失败
#define ERR_HANDLE						211				//传递了错误的句柄

void HexToAscii(unsigned char * pHex, unsigned char * pASSCHex, int nLen);
BYTE ASCHex2Hex(BYTE bASCHex);
void ToHex(BYTE * ASCHex,int nLen,BYTE *pHex);

class CReader  
{
	friend struct stru_Tyreader;
private:
typedef int (WINAPI *DECL_TY_Open)(DWORD ReaderBaudRate, DWORD ReaderPort);
typedef WORD (WINAPI *DECL_TY_Close)(int hReader);
typedef WORD (WINAPI *DECL_TY_reset)(int hReader,WORD *rlen,BYTE *resp);
typedef WORD (WINAPI *DECL_TY_tsi_api)(int hReader,WORD slen,BYTE *comm,WORD *rlen,BYTE *resp);
typedef WORD (WINAPI *DECL_TY_CardExist)(int hReader);
typedef WORD (WINAPI *DECL_TY_Dreset)(int hReader,BYTE dev,WORD *rlen,BYTE *resp);
typedef WORD (WINAPI *DECL_TY_Dtsi_api)(int hReader, BYTE dev,WORD slen,BYTE *comm, WORD *rlen, BYTE *resp);

public:
	CReader();
	virtual ~CReader();
	enum{READER_COM1=0
		,READER_COM2,READER_COM3,READER_COM4,READER_COM5
		,READER_COM6,READER_COM7,READER_COM8,READER_COM9
		,READER_COM_MAX=20
		,READER_PCSC1=1000
		,READER_PCSC2,READER_PCSC3,READER_PCSC4
		,READER_PCSC_MAX=1020
		,READER_INVALIDATE_TYPE=0xFFFF};
	enum {TY_SIM=1		//sim卡
		  ,TY_SSB		//社保卡
	};	   
	enum{INVALIDATE_SLOT=-1};
	enum{COM_BAUDRATE=57600};
	enum{HANDLE_ATTATCH,HANDLE_OPEN};		//连接外部的句柄
	enum{SUCCESS_HOPE_NO_VALUE=-1			//设置sendcmd期望返回的值
		,SUCCESS_9000=0x9000
		,SUCCESS_9f00=0x9f00
		,SUCCESS_6100=0x6100
	};										//APDU的正确返回代码

public:
	int SetType(unsigned int nReaderType);
	void SetBaudrate(int nReaderBaudrate);
	void SetCardSlot(int nSlot);
	int SetHandle(unsigned int hHandle);
	int GetType();
	int SetType(CString strType);
	void GetStrType(char *szType);
	int GetBaudrate();
	int GetCardSlot();
	int GetHandle();

	int ListPcscReaderName(CStringArray& strArray);
	int Open();
	int Close();
	int HEX_Reset(BYTE *lpRes,WORD &nResLen);
	int HEX_SendCmd(BYTE* pCmd,WORD nCmdLen,BYTE *lpRes,WORD &nResLen);
	int ASCII_Reset(char* lpRes=NULL);
	int ASCII_SendCmd(char* pCmd,char*lpRes=NULL);
	void throw_HEX_Reset(BYTE *lpRes,WORD &nResLen);
	void throw_HEX_SendCmd(BYTE* pCmd,WORD nCmdLen,BYTE *lpRes,WORD &nResLen);
	void throw_ASCII_Reset(char* lpRes=NULL,int nHopeValue=SUCCESS_HOPE_NO_VALUE);
	void throw_ASCII_SendCmd(char* pCmd,char*lpRes=NULL,int nHopeValue=SUCCESS_HOPE_NO_VALUE,char* cmpText=NULL);
	BOOL CardExist();
	BOOL CardInsertError(int nCardType);

	//对HEX_SendCmd的自动封装，byAutoRspFlag = TRUE表示自动取响应，但是不做处理
	int HEX_SendCmdEx(BYTE* pCmd,WORD nCmdLen,BYTE *lpRes,WORD &nResLen,BYTE byAutoRspFlag = TRUE);

private:
	int CheckReaderType();
	BOOL CheckPcscReader();
	BOOL CheckComReader();
	BOOL Verify9000(int nErrorCode)
	{
		return SUCCESS_9000==nErrorCode;
	}
	BOOL Verify9f00(int nErrorCode)
	{
		return SUCCESS_9f00==(0xff00 & nErrorCode);
	}
	BOOL Verify6100(int nErrorCode)
	{
		return SUCCESS_6100==(0xff00 & nErrorCode);
	}
	BOOL VerifySuccessCode(int nErrorCode)
	{
		if(Verify9000(nErrorCode) || Verify9f00(nErrorCode) || Verify6100(nErrorCode))
		{
			return TRUE;
		}
		return FALSE;
	}
	int GetLastErrorCode()
	{
		return this->m_nLastErrorCode;
	}
private:
	int m_handle_from;			//句柄来源
	int m_handle;				//句柄
	int m_nReaderType;			//读卡器类型
	int m_nSlot;				//卡槽
	int m_nBaudrate;			//波特率
	DWORD m_dwProtocol;			//协议，PCSC
	ULONG m_hContext;			//上下文句柄，PCSC
	HMODULE m_hModule;			//Tyreader.dll的句柄
	int m_nLastErrorCode;		//最后一次执行SendCmd命令的返回码

};

#endif // !defined(AFX_MYREADER_H__088BF118_00E3_455D_93F5_7B2B8F6B6FB8__INCLUDED_)
