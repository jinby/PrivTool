// HIDReader1.h: interface for the CHIDReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HIDREADER1_H__00FD355A_AC52_41C1_9103_01DA846C5151__INCLUDED_)
#define AFX_HIDREADER1_H__00FD355A_AC52_41C1_9103_01DA846C5151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void OutputTrace(const char *format, ...);
void OutputTraceByte(PBYTE pData, DWORD dwLen);

class CHIDReader  
{
public:
	CHIDReader();
	virtual ~CHIDReader();

	 HANDLE  _ICC_Reader_Open(void);
	 //int  ICC_Reader_Close(int ReaderHandle);
	 int _ICC_Reader_Close(int& ReaderHandle);
	 int _ICC_Reader_Close(void);
	 int  _ICC_Reader_Reset(int ReaderHandle, int ICC_Slot_No, char* Response, int* RespLen);
	 int  _ICC_Reader_Reset(int ICC_Slot_No, unsigned char *Response, unsigned char *RespLen);
	 int  _ICC_Reader_PowerOff(int ReaderHandle, int ICC_Slot_No);
	 int  _ICC_Reader_Application(int ReaderHandle, 
								int ICC_Slot_No,
								int Length_of_Command_APDU,
								char* Command_APDU,
								char* Response_APDU,
								int* RespLen);
	int  _ICC_Reader_Application(int ICC_Slot_No,
		 int Length_of_Command_APDU,
		 char* Command_APDU,
		 char* Response_APDU,
		 int* RespLen);
	
	 int  _ICC_Reader_GetLastError(void);
	 int  _ICC_Reader_GetLastErrorInfo(char* info);
	 int  _ICC_Reader_Version(char* info);

private:
	int SearchDevice(USHORT vendorID, USHORT ProductID, USHORT VersionNum, TCHAR* pDevName);
	int PackageData(char* pIn, int iInLen,char**pOut,int* iOutLen);			//pOut调用者释放
public:
	HANDLE m_hFile;					//设备句柄
	int m_iInputBufferSize;			//读缓冲区大小
	int m_iOutputBufferSize;		//写缓冲区大小
};

#endif // !defined(AFX_HIDREADER1_H__00FD355A_AC52_41C1_9103_01DA846C5151__INCLUDED_)
