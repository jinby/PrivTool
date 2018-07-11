#include "stdafx.h"
#include "FingerBoard.h"
#include "ExceptionEx.hpp"
#include "../SSCardDriver/iniRW.h"
#include "../SSCardDriver/CommonFunc.h"
#include "../SSCardDriver/Registry.h"
#include "../SSCardDriver/EnumType.h"

//程序通过判断s_com长度, 来判断是否需要查找密码键盘
static string s_com = "";


//完全读出要求的数据
BOOL FullReadFile(HANDLE hFile,                // handle of file to read
				  LPBYTE lpBuffer,             // pointer to buffer that receives data
				  DWORD nNumberOfBytesToRead,  // number of bytes to read
				  LPOVERLAPPED lpOverlapped,
				  int nTryCount)    // pointer to structure for data
{
	DWORD dwReaded = 0;
	DWORD dwRest = nNumberOfBytesToRead;

	while(dwRest)
	{
		nTryCount --;
		if ( nTryCount < 0 )
			return FALSE; //超时
		if(!ReadFile(hFile, (void*)lpBuffer, dwRest, &dwReaded, lpOverlapped))
			return FALSE;
		dwRest -= dwReaded;
		lpBuffer += dwReaded;
	}
	return TRUE;
}

//完全写入要求的数据
BOOL FullWriteFile(HANDLE hFile,                    // handle to file to write to
				   LPBYTE lpBuffer,                // pointer to data to write to file
				   DWORD nNumberOfBytesToWrite,     // number of bytes to write
				   LPOVERLAPPED lpOverlapped)        // pointer to structure for overlapped I/O
{
	DWORD dwWritten = 0;
	DWORD dwRest = nNumberOfBytesToWrite;

	while(dwRest)
	{
		if(!WriteFile(hFile, (LPVOID)lpBuffer, dwRest, &dwWritten, lpOverlapped))
		{
			int ierr = GetLastError();
			return FALSE;
		}
			
		dwRest -= dwWritten;
		lpBuffer += dwWritten;
	}
	return TRUE;
}

void SetupCom(HANDLE hcom)
{
	SetupComm(hcom,1024,1024);//输入缓冲区和输出缓冲区的大小都是1024
	COMMTIMEOUTS TimeOuts;//设定读超时
	TimeOuts.ReadIntervalTimeout=2000;
	TimeOuts.ReadTotalTimeoutMultiplier=100;
	TimeOuts.ReadTotalTimeoutConstant=9000;//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=2000;
	SetCommTimeouts(hcom,&TimeOuts);//设置超时

	DCB dcb;
	GetCommState(hcom,&dcb);
	dcb.BaudRate=9600;//波特率为9600
	dcb.ByteSize=8;//每个字节有8位
	dcb.Parity=NOPARITY;//无奇偶校验位
	dcb.StopBits=ONESTOPBIT;//1个停止位
	SetCommState(hcom,&dcb);
}

void ClearCom(HANDLE hcom)
{
	BOOL bres = PurgeComm(hcom,PURGE_TXCLEAR|PURGE_RXCLEAR);
	if ( !bres )
	{
		int ires = GetLastError();
		return;
	}
}

void SendCom(HANDLE hcom, PBYTE pbInput, int iInput)
{
	ClearCom(hcom);

	BOOL bres = FullWriteFile(hcom, pbInput, iInput, NULL );
	if ( !bres )
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_SEND,"密码键盘传输失败" );
}

void RecvCom(HANDLE hcom, PBYTE pbOutput, int iOutput)
{
	BOOL bres = FullReadFile(hcom, pbOutput, iOutput, NULL);
	if ( !bres )
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_RECV,"密码键盘接受失败" );
}

void RecvComSF(HANDLE hcom, PBYTE pbOutput, int& iOutput)
{
	BOOL bres;
	int icount = 0;
	BYTE abyread[1024];
	bool bstart = false;
	do
	{
		bres = FullReadFile(hcom, abyread+icount, 1, NULL);
		if ( !bres)
			throw new ExceptionEx( SCD_ERR_PIN_INPUTOVERTIME , "");

		if ( isStartByte(abyread[icount])  )
		{
			//abyread[0] = abyread[icount];
			icount = 0;
		}
		else if ( isFinishByte(abyread[icount]) )
		{
			icount++;
			memcpy(pbOutput, abyread, icount);
			iOutput =  icount;
			break;
		}
		else
			icount++;

		//if ( iinputCount <0  )
	}
	while(1);
}

bool isStartByte(BYTE bOutput)
{
	return bOutput == 0x08;
}

bool isFinishByte(BYTE bOutput)
{
	return bOutput == 0x0D;
}

bool isFingerBoardVersion(PBYTE pbOutput)
{
	return memcmp(pbOutput, "Ver5.200",8) == 0;
}

string getMidPin(PBYTE pbInput, int iInput)
{
	char sinput[1024] = {0};
	memcpy(sinput, pbInput, iInput-1);

	if ( strlen(sinput) < 4 || strlen(sinput) > 16 )
		throw new ExceptionEx(SCD_ERR_PIN_LENGTH,"");

	if( strlen(sinput) % 2 )
		strcat(sinput,"F");

	return string(sinput);
}

void resetpin(HANDLE hcom)
{
	BYTE abyInput[1024] = {0};
	int iInput = 1024;

	BYTE abyOutput[1024] = {0};
	memcpy(abyInput, "\x1B\x52", 2);

	SendCom(hcom, abyInput, 2);
}

string getpin(HANDLE hcom)
{
	BYTE abyInput[1024] = {0};
	int iInput = 1024;
	BYTE abyOutput[1024] = {0};

	memcpy(abyInput, "\x83", 1);
	SendCom(hcom, abyInput, 1);

	memcpy(abyInput, "\x82", 1);
	SendCom(hcom, abyInput, 1);

	RecvComSF(hcom, abyOutput, iInput);

	if ( !isFinishByte(abyOutput[iInput-1]) )
	{
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_ENDCHAR,"密码键盘接受结束符错误" );
	}

	return getMidPin(abyOutput, iInput);
}

string getNewpin(HANDLE hcom)
{
	BYTE abyInput[1024] = {0};
	int iInput = 1024;
	BYTE abyOutput[1024] = {0};
	int iOutput = 1024;
	BYTE abyOutput2[1024] = {0};
	int iOutput2 = 1024;

	memcpy(abyInput, "\x83", 1);
	SendCom(hcom, abyInput, 1);

	memcpy(abyInput, "\x82", 1);
	SendCom(hcom, abyInput, 1);
	RecvComSF(hcom, abyOutput, iOutput);

	if ( !isFinishByte(abyOutput[iOutput-1]) )
	{
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_ENDCHAR,"密码键盘接受结束符错误" );
	}

	getMidPin(abyOutput, iOutput);

	memcpy(abyInput, "\x83", 1);
	SendCom(hcom, abyInput, 1);

	memcpy(abyInput, "\x81", 1);
	SendCom(hcom, abyInput, 1);
	RecvComSF(hcom, abyOutput2, iOutput2);

	if ( !isFinishByte(abyOutput2[iOutput2-1]) )
	{
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_ENDCHAR,"密码键盘接受结束符错误" );
	}

	if ( iOutput != iOutput2 || memcmp(abyOutput, abyOutput2, iOutput) )
	{
		throw new ExceptionEx(SCD_ERR_PIN_REPEAT,"");
	}

	return getMidPin(abyOutput2, iOutput2);
}

void chechJP(HANDLE hcom)
{
	BYTE abyInput[1024] = {0};
	int iInput = 1024;
	BYTE abyOutput[1024] = {0};

	memcpy(abyInput, "\x83", 1);
	SendCom(hcom, abyInput, 1);

	memcpy(abyInput, "GETVER", 6);
	SendCom(hcom, abyInput, 6);
	RecvCom(hcom, abyOutput, 8);  // 这里本来可以取20个字节,返回应该为Ver5.200+Y1 14.03.14

	if ( !isFingerBoardVersion(abyOutput) )
	{
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_COM,"非密码键盘COM口" );
	}
}


CFingerBoard::CFingerBoard(void)
{
	_scomname = "";
	_hcom = 0;
}

CFingerBoard::~CFingerBoard(void)
{
	Close();
}

std::string CFingerBoard::getPin()
{
	return getpin(_hcom);
}

std::string CFingerBoard::getNewPin()
{
	return getNewpin(_hcom);
}

std::string CFingerBoard::findCom()
{	
	for ( int i=1; i<=10 ;i++)
	{
		string scom = format("COM%d", i);
		_hcom =CreateFile(scom.c_str(), //
			GENERIC_READ|GENERIC_WRITE, //允许读和写
			0, //独占方式
			NULL,
			OPEN_EXISTING, //打开而不是创建
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if(_hcom ==INVALID_HANDLE_VALUE)
			continue;

		SetupCom(_hcom);

		try
		{
			chechJP(_hcom);
			
		}
		catch (ExceptionEx* e)
		{
			delete e;
			continue;
		}

		CloseHandle(_hcom);
		_hcom = INVALID_HANDLE_VALUE;
		
		return scom;
	}

	return "";
}

void CFingerBoard::Open()
{
	/*
	CRegistry cr;

	if ( !cr.Open("SOFTWARE\\WHTY\\TY_FingerBoard") )
	{
		throw new ExceptionEx(SCD_ERR_CARD_OTHER ,"打开注册表失败");
	}

	string sCom;
	if ( !cr.Read("FingerBoard_COM", sCom) )
	{
		throw new ExceptionEx(SCD_ERR_CARD_OTHER ,"读取注册表信息失败");
	}
	*/
	

	
	/*
	string sCom;
	extern string GetDLLPath();
	string sIniFile = GetDLLPath() + "\\ComSetting.ini";
	CIniRW ini(sIniFile.c_str());


	sCom = ini.ReadString("CONFIG","COMNAME","");
	if ( sCom.size() == 0 )
		throw new ExceptionEx(SCD_ERR_FINGERBOARD_INI ,"读取COM信息失败");

	*/
	
	if ( s_com.size() == 0)
		s_com = findCom();

	if ( s_com.size() == 0 )
		throw new ExceptionEx( SCD_ERR_FINGERBOARD_OPEN, format("密码键盘未找到") );

	_hcom =CreateFile(s_com.c_str(), //
		GENERIC_READ|GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(_hcom ==INVALID_HANDLE_VALUE)
	{
		s_com = "";
		throw new ExceptionEx( SCD_ERR_FINGERBOARD_OPEN, format("密码键盘端口[%s]打开失败",s_com.c_str()) );
	}
		
	
	SetupCom(_hcom);
}

void CFingerBoard::Close()
{
	if ( _hcom != INVALID_HANDLE_VALUE )
	{
		CloseHandle(_hcom);
		_hcom = INVALID_HANDLE_VALUE;
	}
}
