// Reader.cpp: implementation of the CReader class.
//
//////////////////////////////////////////////////////////////////////
#include "stdAfx.h"
#include "reader.h"
#include <winscard.h>
#pragma comment(lib,"winscard.lib")

struct stru_Tyreader
{
	CReader::DECL_TY_Open TY_Open;
	CReader::DECL_TY_Close TY_Close;
	CReader::DECL_TY_reset TY_reset;
	CReader::DECL_TY_tsi_api TY_tsi_api;
	CReader::DECL_TY_CardExist TY_CardExist;
	CReader::DECL_TY_Dreset TY_Dreset;
	CReader::DECL_TY_Dtsi_api TY_Dtsi_api;
};
static stru_Tyreader tyreader;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void HexToAscii(unsigned char * pHex, unsigned char * pASSCHex, int nLen)
{
	unsigned char Nibble[2];
	for (int i = 0; i < nLen; i ++)
	{
		Nibble[0] = (pHex[i] & 0xF0) >> 4;	
		Nibble[1] = pHex[i] & 0x0F;
		for (int j = 0; j < 2; j ++)
		{
			if (Nibble[j] < 10)
				Nibble[j] += 0x30;
			else
			{
				if (Nibble[j] < 16)
					Nibble[j] = Nibble[j] - 10 + 'A';
			}
			*pASSCHex ++ = Nibble[j];
		}	// for (int j = ...)
	}	// for (int i = ...)
}

BYTE ASCHex2Hex(BYTE bASCHex)
{
	if ((bASCHex >= '0')&& (bASCHex <= '9'))
		return bASCHex - '0';
	else if ((bASCHex >= 'a')&& (bASCHex <= 'z'))
		return bASCHex - 'a' + 0x0a;
	else if ((bASCHex >= 'A')&& (bASCHex <= 'Z'))
		return bASCHex - 'A' + 0x0a;
	return 0;
}
void ToHex(BYTE * ASCHex,int nLen,BYTE *pHex)
{
	for (int i=0; i<nLen/2; i++)
	{
		pHex[i] = (ASCHex2Hex(ASCHex[i*2])<<4) + ASCHex2Hex(ASCHex[i*2+1]);
	}
}

CReader::CReader()
{
	m_handle_from=HANDLE_OPEN;
	m_handle=NULL;
	m_nReaderType=READER_PCSC1;
	m_nSlot=INVALIDATE_SLOT;
	m_nBaudrate=COM_BAUDRATE;
	m_dwProtocol=NULL;
	m_hContext=NULL;
	m_hModule=NULL;
	m_nLastErrorCode=SUCCESS;

}

CReader::~CReader()
{
}

int CReader::SetHandle(unsigned int hHandle)
{
	if(hHandle<0)
		return ERR_HANDLE;
	this->m_handle=hHandle;
	m_handle_from=HANDLE_ATTATCH;
	return SUCCESS;
}

int CReader::SetType(unsigned int nReaderType)
{
	if(CReader::READER_INVALIDATE_TYPE==nReaderType){
		this->m_nReaderType=READER_PCSC1;
	}else{
		this->m_nReaderType=nReaderType;
	}

	if(!this->CheckComReader()){
		return SUCCESS;
	}
	
	//已经导入了DLL
	if(m_hModule)
		return SUCCESS;

	m_hModule=LoadLibrary("Tyreader.dll");
	if(NULL==m_hModule)
		return ERR_LOAD_TYREADER_DLL;

	tyreader.TY_Open=(DECL_TY_Open)GetProcAddress(m_hModule,"TY_Open");
	tyreader.TY_Close=(DECL_TY_Close)GetProcAddress(m_hModule,"TY_Close");
	tyreader.TY_reset=(DECL_TY_reset)GetProcAddress(m_hModule,"TY_reset");
	tyreader.TY_tsi_api=(DECL_TY_tsi_api)GetProcAddress(m_hModule,"TY_tsi_api");
	tyreader.TY_CardExist=(DECL_TY_CardExist)GetProcAddress(m_hModule,"TY_CardExist");
	tyreader.TY_Dreset=(DECL_TY_Dreset)GetProcAddress(m_hModule,"TY_Dreset");
	tyreader.TY_Dtsi_api=(DECL_TY_Dtsi_api)GetProcAddress(m_hModule,"TY_Dtsi_api");

	if(!tyreader.TY_Open && !tyreader.TY_Close && !tyreader.TY_reset
		&& !tyreader.TY_tsi_api && !tyreader.TY_CardExist
		&& !tyreader.TY_Dreset && !tyreader.TY_Dtsi_api)
		return ERR_LOAD_TYREADER_FUNC;

	return SUCCESS;
}

int CReader::SetType(CString strType)
{
	unsigned int nReaderType;

	if(!strType.Left(3).CompareNoCase("COM"))
		nReaderType = atoi(strType.Right(1).GetBuffer(0))-1;
	else
		nReaderType = atoi(strType.Right(1).GetBuffer(0))-1+1000;

	return this->SetType(nReaderType);
}


void CReader::SetBaudrate(int nReaderBaudrate)
{
	this->m_nBaudrate=nReaderBaudrate;
}

void CReader::SetCardSlot(int nSlot)
{
	this->m_nSlot=nSlot;
}

int CReader::GetType()
{
	return this->m_nReaderType;
}

void CReader::GetStrType(char *szType)
{
	char readerName[14][16] = {{"READER_COM1"},{"READER_COM2"},{"READER_COM3"},{"READER_COM4"}
	,{"READER_COM5"},{"READER_COM6"},{"READER_COM7"},{"READER_COM8"}
	,{"READER_COM9"},{"READER_COM10"},{"READER_PCSC1"},{"READER_PCSC2"}
	,{"READER_PCSC3"},{"READER_PCSC4"}};

	int nType = m_nReaderType;
	if(nType>=1000)
		nType = nType - 1000 + 10;
	if(nType>=0 && nType<=13)
		strcpy(szType,readerName[nType]);
	else
		szType = "";
}

int CReader::GetBaudrate()
{
	return this->m_nBaudrate;
}

int CReader::GetCardSlot()
{
	return this->m_nSlot;
}

int CReader::GetHandle()
{
	return this->m_handle;
}

int CReader::CheckReaderType()
{
	if(!CheckPcscReader() && !CheckComReader())
		return ERR_READERTYPE;

	//校验是否导入了DLL
	if(CheckComReader())
	{
		if(!tyreader.TY_Open && !tyreader.TY_Close && !tyreader.TY_reset
			&& !tyreader.TY_tsi_api && !tyreader.TY_CardExist
			&& !tyreader.TY_Dreset && !tyreader.TY_Dtsi_api)
			return ERR_LOAD_TYREADER_FUNC;
	}

	return SUCCESS;
}

BOOL CReader::CheckPcscReader()
{
	if(this->m_nReaderType<READER_PCSC1 || this->m_nReaderType>READER_PCSC_MAX)
		return FALSE;
	return TRUE;
}

BOOL CReader::CheckComReader()
{
	if(this->m_nReaderType<READER_COM1 || this->m_nReaderType>READER_COM_MAX)
		return FALSE;
	return TRUE;
}

int CReader::ListPcscReaderName(CStringArray& strArray)
{	
	//如果不是PCSC，直接返回正确
	if(!CheckPcscReader()){
		return ERR_READER;
	}

	DWORD lRet = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_hContext);
	if (lRet != SCARD_S_SUCCESS){
		return ERR_READER;
	}

	DWORD dwReadersLength;
	lRet = SCardListReaders(m_hContext, NULL, NULL, &dwReadersLength);
	if (lRet != SCARD_S_SUCCESS){
		return ERR_READER;
	}

	LPTSTR szListReader = (LPTSTR)new char[dwReadersLength+100];
	memset(szListReader,0,dwReadersLength+100);
	lRet = SCardListReaders(m_hContext, NULL, szListReader, &dwReadersLength);
	if (lRet != SCARD_S_SUCCESS){
		return ERR_READER;
	}

	strArray.RemoveAll();
	char* p=szListReader;
	while((*p)!=0)
	{
		int len=strlen(p);
		if(strcmp(p,"")!=0)
		{
			CString str=p;
			strArray.Add(str);
		}
		p=p+strlen(p)+1;
	}

	delete []szListReader;

	return SUCCESS;
}

int CReader::Open()
{
	int rt=CheckReaderType();
	if(SUCCESS!=rt)
		return rt;

	if(CheckComReader())
	{
		this->m_handle=tyreader.TY_Open(m_nBaudrate,this->m_nReaderType);
		if(this->m_handle<=0)
			return ERR_COM_OPEN;
		else
			return SUCCESS;
	}else{
		char PcscReaderName[200];
		CStringArray strArray;
		int rt=this->ListPcscReaderName(strArray);
		if(SUCCESS!=rt)
			return rt;
		
		int index=m_nReaderType-READER_PCSC1;
		//如果要打开的pcsc机具没有连接好，则直接返回,20060724 modify by lihua
		if(strArray.GetSize()<=index)
			return READER_ERROR;
		strcpy(PcscReaderName,(LPCTSTR)strArray.GetAt(index));

		LONG lRet = SCardConnect(m_hContext,(LPCTSTR)strArray.GetAt(this->GetType()-1000),SCARD_SHARE_SHARED,
			SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,	
			(ULONG*)(&this->m_handle),	&m_dwProtocol);
		if(SCARD_S_SUCCESS!=lRet){ 
			return ERR_CARD;
		}
		if(NULL==this->m_handle){   //如果不能获得句柄
			return ERR_READER;
		}
		return SUCCESS;
	}
}

int CReader::Close()
{
	int rt=CheckReaderType();
	if(SUCCESS!=rt)
		return rt;

	//遵循谁打开，谁释放的原则
	if(HANDLE_OPEN!=m_handle_from)
		return SUCCESS;

	if(CheckComReader()){
		if(this->m_handle>0){
			tyreader.TY_Close(this->m_handle);
			this->m_handle=NULL;
		}
	}else{
		if(NULL!=this->m_handle){
			SCardDisconnect(this->m_handle,SCARD_LEAVE_CARD);
			SCardReleaseContext(m_hContext);
			this->m_handle=NULL;
		}
	}
	
	//释放DLL
	if(m_hModule){
		FreeLibrary(m_hModule);
		m_hModule=NULL;
	}

	return SUCCESS;
}

int CReader::HEX_Reset(BYTE *lpRes,WORD &nResLen)
{
	int rt=CheckReaderType();
	if(SUCCESS!=rt)
		return rt;

	if(CheckComReader()){
		int nStatus;
		if(INVALIDATE_SLOT==this->m_nSlot)
			nStatus=tyreader.TY_reset(this->m_handle,&nResLen,lpRes);
		else
			nStatus=tyreader.TY_Dreset(this->m_handle,this->m_nSlot,&nResLen,lpRes);
		if(0x9000!=nStatus)
			return ERR_COM_RESET;
		else
			return SUCCESS;
	}else{
		DWORD lRet;
		DWORD resetLen;
		if (!this->m_handle)
			return ERR_READER;
		lRet = SCardReconnect(this->m_handle,SCARD_SHARE_SHARED,
			SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,	
			SCARD_RESET_CARD,&m_dwProtocol);
		if (lRet != SCARD_S_SUCCESS)
			return ERR_CARD;
		
		int nTime=100;
		lRet=SCARD_E_NOT_READY;
		do{
			Sleep(100);
			DWORD dwState,lRet;
			lRet = SCardState(this->m_handle,&dwState,&m_dwProtocol,(LPBYTE)lpRes,&resetLen);
			nResLen=(unsigned short)resetLen;
			if(SCARD_S_SUCCESS==lRet){
				return SUCCESS;
			}
			nTime+=100;
		}while(nTime<600);
		return ERR_CARD;
	}
}
	
int CReader::HEX_SendCmd(BYTE* pCmd,WORD nCmdLen,BYTE *lpRes,WORD &nResLen)
{
	int rt=CheckReaderType();
	if(SUCCESS!=rt)
		return rt;

	BYTE byCmd[512] = {0};
	BYTE byRsp[1024] = {0};

	memcpy(byCmd,pCmd,nCmdLen);
	TRACE_LINE(";下发的命令数据\n");
	TRACE_DATA(byCmd,nCmdLen);

	if(CheckComReader()){
		int nStatus;
		if(INVALIDATE_SLOT==this->m_nSlot)
			nStatus=tyreader.TY_tsi_api(this->m_handle,nCmdLen,pCmd,&nResLen,lpRes);
		else
			nStatus=tyreader.TY_Dtsi_api(this->m_handle,this->m_nSlot,nCmdLen,pCmd,&nResLen,lpRes);
		//记录最后一次执行的命令的返回码
		m_nLastErrorCode=nStatus;

		memcpy(byRsp,lpRes,nResLen);
		TRACE_LINE(";响应的状态码：0x%x",nStatus);
		
		if (nResLen > 0)
		{
			TRACE_LINE("\n;响应数据为:");
			TRACE_DATA(lpRes,nResLen);
		}
		else
		{
			TRACE_LINE("\n");
			
		}
		TRACE_LINE("\n");

		return nStatus;		
	}else{
		SCARD_IO_REQUEST  ScardIoRequest;
		int nStatus;
		DWORD RespLen;
		ScardIoRequest.dwProtocol = m_dwProtocol;
		ScardIoRequest.cbPciLength = sizeof(ScardIoRequest);
		RespLen=260;
		DWORD lRet = SCardTransmit(this->m_handle, &ScardIoRequest, 
			pCmd, nCmdLen, NULL, 
			lpRes, &RespLen);
		if ((lRet == SCARD_S_SUCCESS) && (RespLen >1))
		{
			nResLen=(WORD)RespLen;
			nStatus=lpRes[nResLen-2]*0x100+lpRes[nResLen-1];
			nResLen=nResLen-2;
		}
		else{
			nStatus=0x8400;
			nResLen=0;
		}
		//记录最后一次执行的命令的返回码
		m_nLastErrorCode=nStatus;

		memcpy(byRsp,lpRes,nResLen);
		TRACE_LINE(";响应的状态码：0x%x",nStatus);

		if (nResLen > 0)
		{
			TRACE_LINE("\n;响应数据为:");
			TRACE_DATA(lpRes,nResLen);
		}
		else
		{
			TRACE_LINE("\n");

		}
		TRACE_LINE("\n");

		return nStatus;
	}
}

int CReader::ASCII_Reset(char* lpRes)
{
	unsigned char temp[300];
	unsigned short nResLen;
	int rt=this->HEX_Reset(temp,nResLen);
	if(SUCCESS!=rt)
		return rt;
	if(NULL==lpRes)
		return SUCCESS;
	HexToAscii(temp,(BYTE*)lpRes,nResLen);
	lpRes[nResLen*2]=0;
	return SUCCESS;
}

int CReader::ASCII_SendCmd(char* pCmd,char*lpRes)
{
	char szCmd[600];
	memset(szCmd,0,600);
	int pos = 0;

	for(int i=0; i<(int)strlen(pCmd); i++)
	{
		if(pCmd[i] != 0x20)
			szCmd[pos++] = pCmd[i];
	}

	//除掉空格
	CString strCmd=szCmd;
	strCmd.TrimLeft();
	strCmd.TrimRight();
	//strCmd.Remove(' ');

	//转换为16进制格式
	BYTE Res[400],HexCmd[400];
	WORD nResLen;
	ToHex((BYTE*)szCmd,strCmd.GetLength(),HexCmd);
	strCmd.ReleaseBuffer();

	int rt=this->HEX_SendCmd(HexCmd,strCmd.GetLength()/2,Res,nResLen);
	if(NULL==lpRes)
		return rt;
	if(VerifySuccessCode(rt))
	{
		HexToAscii(Res,(BYTE*)lpRes,nResLen);
		lpRes[nResLen*2]=0;
	}
	
	return rt;
}

void CReader::throw_HEX_Reset(BYTE *lpRes,WORD &nResLen)
{
	int rt=HEX_Reset(lpRes,nResLen);
	if(SUCCESS!=rt)
		throw rt;
}

void CReader::throw_HEX_SendCmd(BYTE* pCmd,WORD nCmdLen,BYTE *lpRes,WORD &nResLen)
{
	int rt=HEX_SendCmd(pCmd,nCmdLen,lpRes,nResLen);
	if(!VerifySuccessCode(rt))
		throw rt;
}

void CReader::throw_ASCII_Reset(char* lpRes,int nHopeValue)
{
	int rt=ASCII_Reset(lpRes);
	if(SUCCESS_HOPE_NO_VALUE!=nHopeValue){
		if(rt!=nHopeValue)
			throw rt;
	}else{
		if(SUCCESS!=rt)
			throw rt;
	}
}

void CReader::throw_ASCII_SendCmd(char* pCmd,char*lpRes,int nHopeValue,char* cmpText)
{
	char resp[1000];
	memset(resp,0,sizeof(resp));
	int rt=ASCII_SendCmd(pCmd,resp);
	if(SUCCESS_HOPE_NO_VALUE!=nHopeValue){
		if(rt!=nHopeValue)
			throw rt;
	}else{
		if(!VerifySuccessCode(rt))
			throw rt;
	}
	if(NULL!=lpRes){
		strcpy(lpRes,resp);
	}
	if(NULL!=cmpText){
		CString strcmpText(cmpText);
		strcmpText.TrimLeft();
		strcmpText.TrimRight();
		strcmpText.Remove(' ');
		char tempCmpText[300];
		sprintf(tempCmpText,strcmpText,strcmpText.GetLength());
		if(0!=_stricmp(resp,tempCmpText))
			throw 1;
	}
}

//通过选择3f00,2F02文件判断是否为插好
BOOL CReader::CardInsertError(int nCardType)
{
	//如果不是PCSC，直接返回正确
	if(!this->CheckPcscReader()){
		return FALSE;
	}

	int rt;
	BYTE cmd[100],resp[100];
	WORD cmdlen,resplen;
	
	if(CReader::TY_SSB==nCardType){
		memcpy(cmd,"\x00\xa4\x00\x00\x02\x3f\x00",7);
		cmdlen=7;
		rt=HEX_SendCmd(cmd,cmdlen,resp,resplen);
		if((rt & 0xff00)==0xff00){   //执行卡片命令时返回ffXX时表示卡未插好
			return TRUE;
		}
	}else{
		memcpy(cmd,"\xa0\xa4\x00\x00\x02\x3f\x00",7);
		cmdlen=7;
		rt=HEX_SendCmd(cmd,cmdlen,resp,resplen);
		if((rt & 0xff00)==0xff00){
			return TRUE;
		}
		memcpy(cmd,"\xa0\xa4\x00\x00\x02\x2f\x02",7);
		cmdlen=7;
		rt=HEX_SendCmd(cmd,cmdlen,resp,resplen);
		if((rt & 0xff00)==0xff00){
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CReader::CardExist()
{
	int rt=CheckReaderType();
	if(SUCCESS!=rt)
		return rt;

	::Sleep(100);

	if(CheckComReader()){
		int rt=tyreader.TY_CardExist(this->m_handle);
		if(0x9000!=rt)
			return FALSE;
		return TRUE;
	}else{
		DWORD lRet;
		DWORD dwProtocol;
		lRet = SCardReconnect(this->m_handle,SCARD_SHARE_SHARED,
				SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,	
				SCARD_LEAVE_CARD,&dwProtocol);
		if (lRet != SCARD_S_SUCCESS)
			return FALSE;

		//Release版本下，因为加了延时，所以运行正常，否则取卡片状态瞬间是不成功的
		/*char lpRes[300];
		if(SUCCESS!=this->ASCII_Reset(lpRes))
			return FALSE;*/

		return TRUE;
	}
}

//对HEX_SendCmd的自动封装，byAutoRspFlag = TRUE表示自动取响应，但是不做处理
int CReader::HEX_SendCmdEx(BYTE* pCmd,WORD nCmdLen,BYTE *lpRes,WORD &nResLen,BYTE byAutoRspFlag/* = TRUE*/)
{
	int nResult = SCARD_S_SUCCESS;
	BYTE abyCmd[512] = {0};

	nResult = HEX_SendCmd(pCmd,nCmdLen,lpRes,nResLen);
	if (nResult != 0x9000)
	{
		if ((nResult&0x6100) == 0x6100)
		{
			if (byAutoRspFlag)
			{
				memset(abyCmd,0x00,sizeof(abyCmd));
				memcpy(abyCmd,"\x00\xc0\x00\x00\x00",5);
				abyCmd[4]= (BYTE)(nResult&0x00ff);
				nResult = HEX_SendCmd(abyCmd,5,lpRes,nResLen);
			}
		}
	}
	return nResult;
}
