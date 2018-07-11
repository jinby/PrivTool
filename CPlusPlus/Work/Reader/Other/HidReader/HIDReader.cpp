// HIDReader1.cpp: implementation of the CHIDReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HIDReader.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

extern "C" {
#include "hidsdi.h"
#include "setupapi.h"
}

#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif




void OutputTrace(const char *format, ...)
{
#ifdef _DEBUG

	char buf[4096], *p = buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);
// 	while ( p > buf && isspace(p[-1]) )
// 		*--p = '\0';
// 	*p++ = '\r';
// 	*p++ = '\n';
	*(p++)   = '\0';
	OutputDebugString(buf);
#endif
}

void OutputTraceByte(PBYTE pData, DWORD dwLen)
{
#ifdef _DEBUG
	char buf[4096] = {0};
	for ( int i=0; i<dwLen; i++)
	{
		char buf2[20]={0};
		sprintf(buf2, "%02X", pData[i]);
		strcat(buf, buf2);
	}

	OutputDebugString(buf);
#endif
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SAFE_CLOSE_HANDLE(handle) if(NULL != handle) {CloseHandle(handle);handle = NULL;}
#define SAFE_DELETE(p) if(NULL != (p)) {delete (p);(p)=NULL;}
#define SAFE_DELETE_ARRAY(p) if(NULL != p) {delete []p; p = NULL;}

#define WriteLog(p,len,filename) {FILE* pFile = fopen(filename,"ab");\
	if(NULL != pFile) \
{ \
  SYSTEMTIME sysTime; GetLocalTime(&sysTime);\
  char szAreaBuf[256] = {0};\
  sprintf(szAreaBuf,"/***%02d:%02d:%02d***/",sysTime.wHour,sysTime.wMinute,sysTime.wSecond);\
  fwrite(szAreaBuf,1,strlen(szAreaBuf),pFile);\
  fwrite(p,1,len,pFile);	\
  fclose(pFile);\
}}

enum ProcessName
{
	eOpen,
	eClose,
	eReset,
	ePowerOff,
	eApplication,
	eNoProcess,
};


static int s_lastError=0;
static int s_lastProcess=0;
static USHORT s_VendorID = 0xA625;
static USHORT s_ProductID = 0x0815;
static USHORT s_VersionNumber = 0x0111;
static const int s_iPackageDataSize = 31; 
static const int s_iPackageSize = 32; 
// static USHORT s_VendorID = 0x0471;
// static USHORT s_ProductID = 0xa112;
// static USHORT s_VersionNumber = 0x0111;
const BYTE reset_cmd[5] = {0xFF, 0xFF, 0x00, 0x00, 0x00};
const BYTE power_off_cmd[5] = {0xFF, 0xFF, 0x01, 0x00, 0x00};
#define CONTACTLESS_SLOT	(0x00)
#define SAM1_SLOT			(0x01)
#define SAM2_SLOT			(0x02)
#define CONTACT_SLOT	(0x03)

#define  HID_MAX_ERRORNUM 8
enum {
 	HID_ERROR_OK = 0,
 	HID_ERROR_OUTMEMERY,
 	HID_ERROR_NODEVICE,
 	HIDD_ERROR_GETDEVINFO,
 	HID_ERROR_OPENDEV_FAILED,
 	HID_ERROR_WRITE_FAILED,
 	HID_ERROR_READ_FAILED,
 	HID_ERROR_PARAM,
 	HID_ERROR_NO_CARD,
};

static char s_errorInfo[][101] =
{
	{"成功"},
	{"内存不够"},
	{"没找到设备"},
	{"获取设备信息失败"},
	{"打开设备失败"},
	{"发送数据到设备失败"},
	{"读取设备响应数据失败"},
	{"参数错误"},
};

unsigned long ulErr = ERROR_OPEN_FAILED;


CHIDReader::CHIDReader()
{
	s_lastError = 0;
	m_iInputBufferSize = 0;
	m_iOutputBufferSize = 0;
	m_hFile = 0 ;

}

CHIDReader::~CHIDReader()
{

}

int CHIDReader::SearchDevice(USHORT vendorID, USHORT ProductID, USHORT VersionNum, TCHAR* pDevName)
{
	//定义一个GUID的结构体HidGuid来保存HID设备的接口类GUID。
	GUID HidGuid;
	//定义一个DEVINFO的句柄hDevInfoSet来保存获取到的设备信息集合句柄。
	HDEVINFO hDevInfoSet;
	//定义MemberIndex，表示当前搜索到第几个设备，0表示第一个设备。
	DWORD MemberIndex;
	//DevInterfaceData，用来保存设备的驱动接口信息
	SP_DEVICE_INTERFACE_DATA DevInterfaceData;
	//定义一个BOOL变量，保存函数调用是否返回成功
	BOOL Result;
	//定义一个RequiredSize的变量，用来接收需要保存详细信息的缓冲长度。
	DWORD RequiredSize;
	//定义一个指向设备详细信息的结构体指针。
	PSP_DEVICE_INTERFACE_DETAIL_DATA	pDevDetailData;
	//定义一个用来保存打开设备的句柄。
	HANDLE hDevHandle;
	//定义一个HIDD_ATTRIBUTES的结构体变量，保存设备的属性。
	HIDD_ATTRIBUTES DevAttributes;

	//对DevInterfaceData结构体的cbSize初始化为结构体大小
	DevInterfaceData.cbSize=sizeof(DevInterfaceData);
	//对DevAttributes结构体的Size初始化为结构体大小
	DevAttributes.Size=sizeof(DevAttributes);
	
	//初始化设备未找到
//	MyDevFound=FALSE;
	HidD_GetHidGuid(&HidGuid);

	//根据HidGuid来获取设备信息集合。其中Flags参数设置为
	//DIGCF_DEVICEINTERFACE|DIGCF_PRESENT，前者表示使用的GUID为
	//接口类GUID，后者表示只列举正在使用的设备，因为我们这里只
	//查找已经连接上的设备。返回的句柄保存在hDevinfo中。注意设备
	//信息集合在使用完毕后，要使用函数SetupDiDestroyDeviceInfoList
	//销毁，不然会造成内存泄漏。
	hDevInfoSet=SetupDiGetClassDevs(&HidGuid,  NULL, NULL, DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);
 
	//然后对设备集合中每个设备进行列举，检查是否是我们要找的设备
	//当找到我们指定的设备，或者设备已经查找完毕时，就退出查找。
	//首先指向第一个设备，即将MemberIndex置为0。
	MemberIndex=0;
	
	while(1)
	{

		//调用SetupDiEnumDeviceInterfaces在设备信息集合中获取编号为
		//MemberIndex的设备信息。
		Result=SetupDiEnumDeviceInterfaces(hDevInfoSet, NULL, &HidGuid, MemberIndex, &DevInterfaceData);

		//如果获取信息失败，则说明设备已经查找完毕，退出循环。
		if(Result==FALSE) break;

		//将MemberIndex指向下一个设备
		MemberIndex++;

		//如果获取信息成功，则继续获取该设备的详细信息。在获取设备
		//详细信息时，需要先知道保存详细信息需要多大的缓冲区，这通过
		//第一次调用函数SetupDiGetDeviceInterfaceDetail来获取。这时
		//提供缓冲区和长度都为NULL的参数，并提供一个用来保存需要多大
		//缓冲区的变量RequiredSize。
		Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData,	NULL, NULL, &RequiredSize, NULL);

		//然后，分配一个大小为RequiredSize缓冲区，用来保存设备详细信息。
		pDevDetailData=(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(RequiredSize);
		if(pDevDetailData==NULL) //如果内存不足，则直接返回。
		{
			SetupDiDestroyDeviceInfoList(hDevInfoSet);
			return -1;
		}

		//并设置pDevDetailData的cbSize为结构体的大小（注意只是结构体大小，
		//不包括后面缓冲区）。
		pDevDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		//然后再次调用SetupDiGetDeviceInterfaceDetail函数来获取设备的
		//详细信息。这次调用设置使用的缓冲区以及缓冲区大小。
		Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData, pDevDetailData, RequiredSize, NULL,	NULL);

		//将设备路径复制出来，然后销毁刚刚申请的内存。
		TCHAR szDevPathName[MAX_PATH] = {0};
		_tcscpy(szDevPathName,pDevDetailData->DevicePath);
	//	char* MyDevPathName=pDevDetailData->DevicePath;
		free(pDevDetailData);

		//如果调用失败，则查找下一个设备。
		if(Result==FALSE) continue;

		//如果调用成功，则使用不带读写访问的CreateFile函数
		//来获取设备的属性，包括VID、PID、版本号等。
		//对于一些独占设备（例如USB键盘），使用读访问方式是无法打开的，
		//而使用不带读写访问的格式才可以打开这些设备，从而获取设备的属性。
		hDevHandle=CreateFile(szDevPathName, NULL, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL,	NULL);

		//如果打开成功，则获取设备属性。
		if(hDevHandle!=INVALID_HANDLE_VALUE)
		{
			//获取设备的属性并保存在DevAttributes结构体中
			Result=HidD_GetAttributes(hDevHandle, &DevAttributes);

			//关闭刚刚打开的设备
			CloseHandle(hDevHandle);

			//获取失败，查找下一个
			if(Result==FALSE) continue;

			//如果获取成功，则将属性中的VID、PID以及设备版本号与我们需要的
			//进行比较，如果都一致的话，则说明它就是我们要找的设备。
			if(DevAttributes.VendorID==vendorID) //如果VID相等
				if(DevAttributes.ProductID==ProductID) //并且PID相等
				//	if(DevAttributes.VersionNumber==VersionNum) //并且设备版本号相等
					{
						_tcscpy(pDevName,szDevPathName);
						SetupDiDestroyDeviceInfoList(hDevInfoSet);
						return 0;
					}
		}
		//如果打开失败，则查找下一个设备
		else continue;
	}

	//调用SetupDiDestroyDeviceInfoList函数销毁设备信息集合
	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	return -1;
}

int CHIDReader::PackageData(char* pIn, int iInLen,char**pOut,int* iOutLen)
{
	//每个数据包的有效直接数大小
	*pOut = NULL;
	*iOutLen = 0;
	int iPackageNum = (iInLen+s_iPackageDataSize-1)/s_iPackageDataSize;
	*iOutLen = iPackageNum*(s_iPackageDataSize+1);
	*pOut = new char[*iOutLen];
	if(NULL == *pOut)
	{
	//	s_lastError = HID_ERROR_OUTMEMERY;
		s_lastError = -5;
		return s_lastError;
	}
	memset(*pOut, 0, *iOutLen);

	int iStart = 0;
	for (int i=0; i<iPackageNum; i++)
	{
		if(i == iPackageNum-1)
		{
			(*pOut)[iStart] = iInLen-i*s_iPackageDataSize;
			memcpy(*pOut+iStart+1,pIn+i*s_iPackageDataSize,iInLen-i*s_iPackageDataSize);
		}
		else
		{
			(*pOut)[iStart] = 0x20;
			memcpy(*pOut+iStart+1,pIn+i*s_iPackageDataSize,s_iPackageDataSize);
			iStart += s_iPackageDataSize+1;
		}
	}

	return 0;
}
HANDLE CHIDReader::_ICC_Reader_Open(void)
{
	
	s_lastProcess = eOpen;
	TCHAR szDevPathName[MAX_PATH] = {0};
	if(0 != SearchDevice(s_VendorID,s_ProductID,s_VersionNumber,szDevPathName))
	{
		//s_lastError = /*HID_ERROR_NODEVICE*/0;
		return (HANDLE) ERROR_OPEN_FAILED; // 
	}
	
	m_hFile=CreateFile(szDevPathName, GENERIC_READ|GENERIC_WRITE,  FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,  OPEN_EXISTING,  0, NULL);
//	m_hWriteHandle=CreateFile(szDevPathName, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
	if(INVALID_HANDLE_VALUE == m_hFile)
	{
		//s_lastError = /*HID_ERROR_OPENDEV_FAILED*/;
		return (HANDLE) ERROR_OPEN_FAILED; // 
	}

	BOOL bRet;
	//获取设备属性
	PHIDP_PREPARSED_DATA preparsedData = NULL;
	bRet = HidD_GetPreparsedData(m_hFile,&preparsedData);
	if(FALSE == bRet)
	{
		//s_lastError = /*HIDD_ERROR_GETDEVINFO*/0;
		return (HANDLE) ERROR_OPEN_FAILED; // 
	}

	_HIDP_CAPS caps;
	if(HIDP_STATUS_SUCCESS != HidP_GetCaps(preparsedData,&caps))
	{
		HidD_FreePreparsedData(preparsedData);
		//s_lastError = /*HIDD_ERROR_GETDEVINFO*/0;
		return (HANDLE) ERROR_OPEN_FAILED; // 
	}
	HidD_FreePreparsedData(preparsedData);

	m_iInputBufferSize = caps.InputReportByteLength;
	m_iOutputBufferSize = caps.OutputReportByteLength;
	
//	CreateThread(NULL,0,ReadReport,this,0,0);
	s_lastError = (int)m_hFile;
	return   /*(int)*/m_hFile;
	//return 0;
}
int CHIDReader::_ICC_Reader_Close(void)
{
	if (m_hFile == 0)
	{
		s_lastError = -5;
		return ERROR_INVALID_HANDLE;
	}

	if(FALSE == CloseHandle((HANDLE)m_hFile))
	{
		s_lastError = /*HID_ERROR_PARAM*/-5;
		return  ERROR_INVALID_HANDLE/*s_lastError*/-5; // 失败返回-5，陶良升于2012-08-27修改
	}

	m_hFile = 0;
	s_lastError = 0;
	return 0;
}


int  CHIDReader::_ICC_Reader_Close(int& ReaderHandle)
{
	CString str;
	str.Format(TEXT("%d\n"), ReaderHandle);
	TRACE(str);
	s_lastProcess = eClose;

	if (ReaderHandle == 0)
	{
		s_lastError = -5;
		return -5;
	}

	if(FALSE == CloseHandle((HANDLE)ReaderHandle))
	{
		s_lastError = /*HID_ERROR_PARAM*/-5;
		return /*s_lastError*/-5; // 失败返回-5，陶良升于2012-08-27修改
	}
	
	ReaderHandle = 0;
	s_lastError = 0;
	return 0;
}
 int  CHIDReader::_ICC_Reader_Reset(int ICC_Slot_No, unsigned char *Response, unsigned char *RespLen)
 {
	 s_lastProcess = eReset;

	 if (m_hFile == 0)
	 {
		 s_lastError = -5;
		 return -5;
	 }

	 if (ICC_Slot_No < 0 || ICC_Slot_No >3)
	 {
		 s_lastProcess = -8;
		 return -8;
	 }

	 BOOL bRet = FALSE;
	 HANDLE hFile = (HANDLE)m_hFile;
	 BYTE buf[65] = {0};
	 buf[1] = 0x06;
	 buf[2] = (BYTE)ICC_Slot_No;
	 memcpy(buf+3, reset_cmd, 5);
	 bRet = HidD_SetFeature(hFile, buf, 33);
	 if(FALSE == bRet)
	 {
		 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
			 s_lastError = -11;  // 非接失败时，返回-11，贺苗于2013-06-08修改
		 else
			 s_lastError = -2; //接触失败时，返回-2，贺苗于2013-06-08修改
		 return s_lastError; 
	 }
	 while(1)
	 {
		 bRet = HidD_GetFeature(hFile, buf, 33);
		 if (FALSE == bRet)
		 {
			 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				 s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-08修改
			 else
				 s_lastError = -2; //非接/接触失败时，返回-2，贺苗于2013-06-08修改
			 return s_lastError; 
		 }
		 if(buf[1] <= 0x20)
		 {
			 break;
		 }
	 }

	 if (buf[1] == 1)
	 {
		 if (buf[2] == 0xFD)
		 {
			 s_lastError = -3;
			 return -3;
		 }
		 if (buf[2] == 0xFF)
		 {
			 s_lastError = -1;  //卡片类型不对时，返回-1，贺苗于2013-06-13修改
			 return -1;
		 }

		 if (buf[2] == 0xFC)
		 {
			 s_lastError = -4;  //卡片类型不对时，返回-1，贺苗于2013-06-13修改
			 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				 s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-08修改
			 return s_lastError;
		 }

		 // 说明没有取到复位信息
		 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
			 s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-08修改
		 else
			 s_lastError = -2; //非接/接触失败时，返回-2，贺苗于2013-06-08修改
		 return s_lastError; 
	 }
	 memcpy(Response, buf+3, buf[1]-1);
	 *RespLen = buf[1]-1;
	 s_lastError = 0;
	 return 0;
 }


int  CHIDReader::_ICC_Reader_Reset(int ReaderHandle, int ICC_Slot_No, char* Response, int* RespLen)
{
	s_lastProcess = eReset;

	if (ReaderHandle == 0)
	{
		s_lastError = -5;
		return -5;
	}

	if (ICC_Slot_No < 0 || ICC_Slot_No >3)
	{
		s_lastProcess = -8;
		return -8;
	}

	BOOL bRet = FALSE;
	HANDLE hFile = (HANDLE)ReaderHandle;
	BYTE buf[65] = {0};
	buf[1] = 0x06;
	buf[2] = (BYTE)ICC_Slot_No;
	memcpy(buf+3, reset_cmd, 5);
	bRet = HidD_SetFeature(hFile, buf, 33);
	if(FALSE == bRet)
	{
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
			s_lastError = -11;  // 非接失败时，返回-11，贺苗于2013-06-08修改
		else
			s_lastError = -2; //接触失败时，返回-2，贺苗于2013-06-08修改
		return s_lastError; 
	}
	while(1)
	{
		bRet = HidD_GetFeature(hFile, buf, 33);
		if (FALSE == bRet)
		{
			if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-08修改
			else
				s_lastError = -2; //非接/接触失败时，返回-2，贺苗于2013-06-08修改
			return s_lastError; 
		}
		if(buf[1] <= 0x20)
		{
			break;
		}
	}

	if (buf[1] == 1)
	{
		if (buf[2] == 0xFD)
		{
			s_lastError = -3;
			return -3;
		}
		if (buf[2] == 0xFF)
		{
			s_lastError = -1;  //卡片类型不对时，返回-1，贺苗于2013-06-13修改
			return -1;
		}

		if (buf[2] == 0xFC)
		{
			s_lastError = -4;  //卡片类型不对时，返回-1，贺苗于2013-06-13修改
			if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-08修改
			return s_lastError;
		}

		// 说明没有取到复位信息
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
			s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-08修改
		else
			s_lastError = -2; //非接/接触失败时，返回-2，贺苗于2013-06-08修改
		return s_lastError; 
	}
	memcpy(Response, buf+3, buf[1]-1);
	*RespLen = buf[1]-1;
	s_lastError = 0;
	return 0;
}

int  CHIDReader::_ICC_Reader_PowerOff(int ReaderHandle, int ICC_Slot_No)
{
	s_lastProcess = ePowerOff;

	if (ReaderHandle == 0)
	{
		s_lastError = -5;
		return -5;
	}

	if (ICC_Slot_No < 0 || ICC_Slot_No >3)
	{
		s_lastProcess = -8;
		return -8;
	}

	BOOL bRet = FALSE;
	HANDLE hFile = (HANDLE)ReaderHandle;
	BYTE buf[65] = {0};
	buf[1] = 0x06;
	buf[2] = (BYTE)ICC_Slot_No;
	memcpy(buf+3, power_off_cmd, 5);
	bRet = HidD_SetFeature(hFile, buf, 33);
	if(FALSE == bRet)
	{
		s_lastError = /*HID_ERROR_WRITE_FAILED*/-5;
		return /*s_lastError*/-5; // 失败返回-5，陶良升于2012-08-27修改
	}
	while(1)
	{
		bRet = HidD_GetFeature(hFile, buf, 33);
		if (FALSE == bRet)
		{
			s_lastError = /*HID_ERROR_READ_FAILED*/-5;
			return /*s_lastError*/-5; // 失败返回-5，陶良升于2012-08-27修改
		}
		if(buf[1] <= 0x20)
		{
			break;
		}
	}
	if (buf[1] == 1)
	{
// 		s_lastError = -5;
// 		return -5; // 失败返回-5，陶良升于2012-08-27修改，注意以前没有这一句
	}
	s_lastError = 0;
	return 0;
}
int  CHIDReader::_ICC_Reader_Application(int ICC_Slot_No,
							int Length_of_Command_APDU,
							char* Command_APDU,
							char* Response_APDU,
							int* RespLen)
{
	const char* NAME = (ICC_Slot_No == 0 ? "USERCARD":"PSAM");

	OutputTrace("(%s) ",NAME);
	OutputTraceByte((BYTE*)Command_APDU, Length_of_Command_APDU);
	s_lastProcess = eApplication;

	if (m_hFile == 0)
	{
		*RespLen = 0;
		s_lastError = -5;
		return -5;
	}

	if (ICC_Slot_No < 0 || ICC_Slot_No >3)
	{
		*RespLen = 0;
		s_lastError = -8;
		return -8;
	}

	BOOL bRet = TRUE;
	HANDLE hFile = (HANDLE)m_hFile;

// 命令长度
// case1  5
// case2  5
// case3  5+lc
// case4  5+lc+1

/*	if (Length_of_Command_APDU < 5)
	{
		*RespLen = 0;
		s_lastError = -6;
		return -6; // 失败返回-6，陶良升于2012-08-27修改，表示不支持该命令
	}*/
	
	unsigned char nLength = *(Command_APDU+4);
/*	if((nLength+5) != Length_of_Command_APDU 
		&& (nLength+5+1) != Length_of_Command_APDU)
	{
		if (Length_of_Command_APDU != 5)
		{
			*RespLen = 0;
			s_lastError = -7;
			return -7; // 返回-7，表示长度不对，陶良升于2012-08-27修改
		}
	}*/

	//分包,每个包32个字节，第一个字节为0x20，表示后面的包中还有数据，否则为实际的数据长度，剩余字节的为有效数据，所有有效数据为发送的指令
	char* pBuf = NULL;
	int iBufLen;
	memmove(Command_APDU+1, Command_APDU, Length_of_Command_APDU);
	Command_APDU[0] = (BYTE)ICC_Slot_No;
	Length_of_Command_APDU ++;
	if(0 != PackageData(Command_APDU,Length_of_Command_APDU, &pBuf, &iBufLen))
	{
		return s_lastError;
	}
	if(iBufLen <= 0 || NULL == pBuf || 0 != iBufLen%s_iPackageSize)
	{
	//	s_lastError = HID_ERROR_WRITE_FAILED;
		s_lastError = -5;
		return s_lastError;
	}

	int iSendLen = 33;

	//发送缓冲区第一字节为0 或 ReportID （如设备有定义）
	char *pSendBuf = new char[iSendLen];
	if(NULL == pSendBuf)
	{
	//	s_lastError=HID_ERROR_OUTMEMERY;
		s_lastError = -5;
		SAFE_DELETE_ARRAY(pBuf);
		return s_lastError;
	}
	memset(pSendBuf, 0x00, iSendLen);
	
	for(int i=0; i<iBufLen/s_iPackageSize; i++)
	{
		memcpy(pSendBuf+1, pBuf+i*s_iPackageSize, s_iPackageSize);
		bRet = HidD_SetFeature(hFile, pSendBuf, 33);			//win 98 、2000、XP 中，此函数通过控制端点传输数据
//		if(bRet == TRUE) WriteLog(pSendBuf, iSendLen, "c:\\hid.txt");
		if(FALSE == bRet)
		{
		//	s_lastError = HID_ERROR_WRITE_FAILED;
			s_lastError = -5;
			SAFE_DELETE_ARRAY(pSendBuf);
			SAFE_DELETE_ARRAY(pBuf);
			return s_lastError;
		}
	}
	SAFE_DELETE_ARRAY(pSendBuf);
	SAFE_DELETE_ARRAY(pBuf);

	char szRecv[256];
	szRecv[0] = 0x00;
	BYTE packet_size = 0x1F;
	*RespLen = 0;
	while(1)
	{
		memset(szRecv,0,sizeof(szRecv));
		bRet = HidD_GetFeature(hFile, szRecv, 33);			
		if(szRecv[1] < 0x20)
		{
			memcpy(Response_APDU+(*RespLen), szRecv+2, packet_size);
			(*RespLen) += szRecv[1];
			break;
		}
		else if(szRecv[1] == 0x20)
		{
			memcpy(Response_APDU+(*RespLen), szRecv+2, packet_size);
			(*RespLen) += packet_size;
		}
		else
		{
			//丢弃不符合数据格式的包，保持链接
		}
	}

	if(FALSE == bRet)
	{
	//	s_lastError = HID_ERROR_READ_FAILED;
		s_lastError = -5;
		return s_lastError;
	}

	if ((*Response_APDU) == -2)
	{
		s_lastError = -2; //非接/接触无卡，返回-2，贺苗于2013-06-18修改
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-18修改
		return s_lastError;
	}

	if ((*Response_APDU) == -4)
	{
		s_lastError = -4; //卡片无应答时，返回-4，贺苗于2013-06-13修改
		return s_lastError;
	}
	
	(*RespLen) --;

	memmove(Response_APDU, Response_APDU+1, (*RespLen));

	OutputTrace(" = ");
	OutputTraceByte((BYTE*)Response_APDU, *RespLen);
	OutputTrace("\r\n");
	
	s_lastError = 0;

/*	if ((*(Response_APDU+(*RespLen)-2) == 0x6D) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //贺苗于2013-06-09修改
	{
		s_lastError = -6;
		return s_lastError;
	}*/

	if ((*(Response_APDU+(*RespLen)-2) == 0x6E) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //贺苗于2013-06-09修改
	{
		s_lastError = -6;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x6A) && (*(Response_APDU+(*RespLen)-1) == -122)) //贺苗于2013-06-09修改
	{
		s_lastError = -8;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x67) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //贺苗于2013-06-09修改
	{
		s_lastError = -7;
		return s_lastError;
	}

	return 0;

}







int  CHIDReader::_ICC_Reader_Application(int ReaderHandle, 
							int ICC_Slot_No,
							int Length_of_Command_APDU,
							char* Command_APDU,
							char* Response_APDU,
							int* RespLen)
{
	s_lastProcess = eApplication;

	if (ReaderHandle == 0)
	{
		*RespLen = 0;
		s_lastError = -5;
		return -5;
	}

	if (ICC_Slot_No < 0 || ICC_Slot_No >3)
	{
		*RespLen = 0;
		s_lastError = -8;
		return -8;
	}

	BOOL bRet = TRUE;
	HANDLE hFile = (HANDLE)ReaderHandle;

// 命令长度
// case1  5
// case2  5
// case3  5+lc
// case4  5+lc+1

/*	if (Length_of_Command_APDU < 5)
	{
		*RespLen = 0;
		s_lastError = -6;
		return -6; // 失败返回-6，陶良升于2012-08-27修改，表示不支持该命令
	}*/
	
	unsigned char nLength = *(Command_APDU+4);
/*	if((nLength+5) != Length_of_Command_APDU 
		&& (nLength+5+1) != Length_of_Command_APDU)
	{
		if (Length_of_Command_APDU != 5)
		{
			*RespLen = 0;
			s_lastError = -7;
			return -7; // 返回-7，表示长度不对，陶良升于2012-08-27修改
		}
	}*/

	//分包,每个包32个字节，第一个字节为0x20，表示后面的包中还有数据，否则为实际的数据长度，剩余字节的为有效数据，所有有效数据为发送的指令
	char* pBuf = NULL;
	int iBufLen;
	memmove(Command_APDU+1, Command_APDU, Length_of_Command_APDU);
	Command_APDU[0] = (BYTE)ICC_Slot_No;
	Length_of_Command_APDU ++;
	if(0 != PackageData(Command_APDU,Length_of_Command_APDU, &pBuf, &iBufLen))
	{
		return s_lastError;
	}
	if(iBufLen <= 0 || NULL == pBuf || 0 != iBufLen%s_iPackageSize)
	{
	//	s_lastError = HID_ERROR_WRITE_FAILED;
		s_lastError = -5;
		return s_lastError;
	}

	int iSendLen = 33;

	//发送缓冲区第一字节为0 或 ReportID （如设备有定义）
	char *pSendBuf = new char[iSendLen];
	if(NULL == pSendBuf)
	{
	//	s_lastError=HID_ERROR_OUTMEMERY;
		s_lastError = -5;
		SAFE_DELETE_ARRAY(pBuf);
		return s_lastError;
	}
	memset(pSendBuf, 0x00, iSendLen);
	
	for(int i=0; i<iBufLen/s_iPackageSize; i++)
	{
		memcpy(pSendBuf+1, pBuf+i*s_iPackageSize, s_iPackageSize);
		bRet = HidD_SetFeature(hFile, pSendBuf, 33);			//win 98 、2000、XP 中，此函数通过控制端点传输数据
//		if(bRet == TRUE) WriteLog(pSendBuf, iSendLen, "c:\\hid.txt");
		if(FALSE == bRet)
		{
		//	s_lastError = HID_ERROR_WRITE_FAILED;
			s_lastError = -5;
			SAFE_DELETE_ARRAY(pSendBuf);
			SAFE_DELETE_ARRAY(pBuf);
			return s_lastError;
		}
	}
	SAFE_DELETE_ARRAY(pSendBuf);
	SAFE_DELETE_ARRAY(pBuf);

	char szRecv[256];
	szRecv[0] = 0x00;
	BYTE packet_size = 0x1F;
	*RespLen = 0;
	while(1)
	{
		memset(szRecv,0,sizeof(szRecv));
		bRet = HidD_GetFeature(hFile, szRecv, 33);			
		if(szRecv[1] < 0x20)
		{
			memcpy(Response_APDU+(*RespLen), szRecv+2, packet_size);
			(*RespLen) += szRecv[1];
			break;
		}
		else if(szRecv[1] == 0x20)
		{
			memcpy(Response_APDU+(*RespLen), szRecv+2, packet_size);
			(*RespLen) += packet_size;
		}
		else
		{
			//丢弃不符合数据格式的包，保持链接
		}
	}

	if(FALSE == bRet)
	{
	//	s_lastError = HID_ERROR_READ_FAILED;
		s_lastError = -5;
		return s_lastError;
	}

	if ((*Response_APDU) == -2)
	{
		s_lastError = -2; //非接/接触无卡，返回-2，贺苗于2013-06-18修改
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2失败时，返回-11，贺苗于2013-06-18修改
		return s_lastError;
	}

	if ((*Response_APDU) == -4)
	{
		s_lastError = -4; //卡片无应答时，返回-4，贺苗于2013-06-13修改
		return s_lastError;
	}
	
	(*RespLen) --;

	memmove(Response_APDU, Response_APDU+1, (*RespLen));
	
	s_lastError = 0;

/*	if ((*(Response_APDU+(*RespLen)-2) == 0x6D) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //贺苗于2013-06-09修改
	{
		s_lastError = -6;
		return s_lastError;
	}*/

	if ((*(Response_APDU+(*RespLen)-2) == 0x6E) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //贺苗于2013-06-09修改
	{
		s_lastError = -6;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x6A) && (*(Response_APDU+(*RespLen)-1) == -122)) //贺苗于2013-06-09修改
	{
		s_lastError = -8;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x67) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //贺苗于2013-06-09修改
	{
		s_lastError = -7;
		return s_lastError;
	}

	return 0;

}

int  CHIDReader::_ICC_Reader_GetLastError(void)
{
	return s_lastError;
}

int  CHIDReader::_ICC_Reader_GetLastErrorInfo(char* info)
{
// 	int i = s_lastError%HID_MAX_ERRORNUM;
// 	strcpy(info,s_errorInfo[i]);
// 
// 	s_lastError = 0;
// 	return 0;
	switch (s_lastProcess)
	{
	case eOpen:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "打开设备失败");
			break;
		default:
			strcpy(info, "打开设备成功");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case eClose:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "关闭设备成功");
			break;
		case -5:
			strcpy(info, "接口设备故障");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case eReset:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "复位成功");
			break;
		case -2:
			strcpy(info, "无卡");
			break;
		case -8:
			strcpy(info, "命令参数错误");
			break;
		case -5:
			strcpy(info, "接口设备故障");
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case ePowerOff:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "下电成功");
			break;
		case -5:
			strcpy(info, "接口设备故障");
			break;
		case -8:
			strcpy(info, "命令参数错误");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case eApplication:
		switch (s_lastError)
		{
		case -5:
			strcpy(info, "接口设备故障");
			break;
		case -6:
			strcpy(info, "不支持该命令");
			break;
		case -7:
			strcpy(info, "命令长度错误");
			break;
		case -8:
			strcpy(info, "命令参数错误");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	}

	return 0;
}

#define READER_VERSION		"Tianyu          TY522     10"   //去掉 420   //贺苗于2013-06-09修改
int  CHIDReader::_ICC_Reader_Version(char* info)
{
	strcpy(info, READER_VERSION);
	s_lastError = 0;
	return 0;
}

