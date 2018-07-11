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
	{"�ɹ�"},
	{"�ڴ治��"},
	{"û�ҵ��豸"},
	{"��ȡ�豸��Ϣʧ��"},
	{"���豸ʧ��"},
	{"�������ݵ��豸ʧ��"},
	{"��ȡ�豸��Ӧ����ʧ��"},
	{"��������"},
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
	//����һ��GUID�Ľṹ��HidGuid������HID�豸�Ľӿ���GUID��
	GUID HidGuid;
	//����һ��DEVINFO�ľ��hDevInfoSet�������ȡ�����豸��Ϣ���Ͼ����
	HDEVINFO hDevInfoSet;
	//����MemberIndex����ʾ��ǰ�������ڼ����豸��0��ʾ��һ���豸��
	DWORD MemberIndex;
	//DevInterfaceData�����������豸�������ӿ���Ϣ
	SP_DEVICE_INTERFACE_DATA DevInterfaceData;
	//����һ��BOOL���������溯�������Ƿ񷵻سɹ�
	BOOL Result;
	//����һ��RequiredSize�ı���������������Ҫ������ϸ��Ϣ�Ļ��峤�ȡ�
	DWORD RequiredSize;
	//����һ��ָ���豸��ϸ��Ϣ�Ľṹ��ָ�롣
	PSP_DEVICE_INTERFACE_DETAIL_DATA	pDevDetailData;
	//����һ������������豸�ľ����
	HANDLE hDevHandle;
	//����һ��HIDD_ATTRIBUTES�Ľṹ������������豸�����ԡ�
	HIDD_ATTRIBUTES DevAttributes;

	//��DevInterfaceData�ṹ���cbSize��ʼ��Ϊ�ṹ���С
	DevInterfaceData.cbSize=sizeof(DevInterfaceData);
	//��DevAttributes�ṹ���Size��ʼ��Ϊ�ṹ���С
	DevAttributes.Size=sizeof(DevAttributes);
	
	//��ʼ���豸δ�ҵ�
//	MyDevFound=FALSE;
	HidD_GetHidGuid(&HidGuid);

	//����HidGuid����ȡ�豸��Ϣ���ϡ�����Flags��������Ϊ
	//DIGCF_DEVICEINTERFACE|DIGCF_PRESENT��ǰ�߱�ʾʹ�õ�GUIDΪ
	//�ӿ���GUID�����߱�ʾֻ�о�����ʹ�õ��豸����Ϊ��������ֻ
	//�����Ѿ������ϵ��豸�����صľ��������hDevinfo�С�ע���豸
	//��Ϣ������ʹ����Ϻ�Ҫʹ�ú���SetupDiDestroyDeviceInfoList
	//���٣���Ȼ������ڴ�й©��
	hDevInfoSet=SetupDiGetClassDevs(&HidGuid,  NULL, NULL, DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);
 
	//Ȼ����豸������ÿ���豸�����о٣�����Ƿ�������Ҫ�ҵ��豸
	//���ҵ�����ָ�����豸�������豸�Ѿ��������ʱ�����˳����ҡ�
	//����ָ���һ���豸������MemberIndex��Ϊ0��
	MemberIndex=0;
	
	while(1)
	{

		//����SetupDiEnumDeviceInterfaces���豸��Ϣ�����л�ȡ���Ϊ
		//MemberIndex���豸��Ϣ��
		Result=SetupDiEnumDeviceInterfaces(hDevInfoSet, NULL, &HidGuid, MemberIndex, &DevInterfaceData);

		//�����ȡ��Ϣʧ�ܣ���˵���豸�Ѿ�������ϣ��˳�ѭ����
		if(Result==FALSE) break;

		//��MemberIndexָ����һ���豸
		MemberIndex++;

		//�����ȡ��Ϣ�ɹ����������ȡ���豸����ϸ��Ϣ���ڻ�ȡ�豸
		//��ϸ��Ϣʱ����Ҫ��֪��������ϸ��Ϣ��Ҫ���Ļ���������ͨ��
		//��һ�ε��ú���SetupDiGetDeviceInterfaceDetail����ȡ����ʱ
		//�ṩ�������ͳ��ȶ�ΪNULL�Ĳ��������ṩһ������������Ҫ���
		//�������ı���RequiredSize��
		Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData,	NULL, NULL, &RequiredSize, NULL);

		//Ȼ�󣬷���һ����СΪRequiredSize�����������������豸��ϸ��Ϣ��
		pDevDetailData=(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(RequiredSize);
		if(pDevDetailData==NULL) //����ڴ治�㣬��ֱ�ӷ��ء�
		{
			SetupDiDestroyDeviceInfoList(hDevInfoSet);
			return -1;
		}

		//������pDevDetailData��cbSizeΪ�ṹ��Ĵ�С��ע��ֻ�ǽṹ���С��
		//���������滺��������
		pDevDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		//Ȼ���ٴε���SetupDiGetDeviceInterfaceDetail��������ȡ�豸��
		//��ϸ��Ϣ����ε�������ʹ�õĻ������Լ���������С��
		Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet, &DevInterfaceData, pDevDetailData, RequiredSize, NULL,	NULL);

		//���豸·�����Ƴ�����Ȼ�����ٸո�������ڴ档
		TCHAR szDevPathName[MAX_PATH] = {0};
		_tcscpy(szDevPathName,pDevDetailData->DevicePath);
	//	char* MyDevPathName=pDevDetailData->DevicePath;
		free(pDevDetailData);

		//�������ʧ�ܣ��������һ���豸��
		if(Result==FALSE) continue;

		//������óɹ�����ʹ�ò�����д���ʵ�CreateFile����
		//����ȡ�豸�����ԣ�����VID��PID���汾�ŵȡ�
		//����һЩ��ռ�豸������USB���̣���ʹ�ö����ʷ�ʽ���޷��򿪵ģ�
		//��ʹ�ò�����д���ʵĸ�ʽ�ſ��Դ���Щ�豸���Ӷ���ȡ�豸�����ԡ�
		hDevHandle=CreateFile(szDevPathName, NULL, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL,	NULL);

		//����򿪳ɹ������ȡ�豸���ԡ�
		if(hDevHandle!=INVALID_HANDLE_VALUE)
		{
			//��ȡ�豸�����Բ�������DevAttributes�ṹ����
			Result=HidD_GetAttributes(hDevHandle, &DevAttributes);

			//�رոոմ򿪵��豸
			CloseHandle(hDevHandle);

			//��ȡʧ�ܣ�������һ��
			if(Result==FALSE) continue;

			//�����ȡ�ɹ����������е�VID��PID�Լ��豸�汾����������Ҫ��
			//���бȽϣ������һ�µĻ�����˵������������Ҫ�ҵ��豸��
			if(DevAttributes.VendorID==vendorID) //���VID���
				if(DevAttributes.ProductID==ProductID) //����PID���
				//	if(DevAttributes.VersionNumber==VersionNum) //�����豸�汾�����
					{
						_tcscpy(pDevName,szDevPathName);
						SetupDiDestroyDeviceInfoList(hDevInfoSet);
						return 0;
					}
		}
		//�����ʧ�ܣ��������һ���豸
		else continue;
	}

	//����SetupDiDestroyDeviceInfoList���������豸��Ϣ����
	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	return -1;
}

int CHIDReader::PackageData(char* pIn, int iInLen,char**pOut,int* iOutLen)
{
	//ÿ�����ݰ�����Чֱ������С
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
	//��ȡ�豸����
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
		return  ERROR_INVALID_HANDLE/*s_lastError*/-5; // ʧ�ܷ���-5����������2012-08-27�޸�
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
		return /*s_lastError*/-5; // ʧ�ܷ���-5����������2012-08-27�޸�
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
			 s_lastError = -11;  // �ǽ�ʧ��ʱ������-11��������2013-06-08�޸�
		 else
			 s_lastError = -2; //�Ӵ�ʧ��ʱ������-2��������2013-06-08�޸�
		 return s_lastError; 
	 }
	 while(1)
	 {
		 bRet = HidD_GetFeature(hFile, buf, 33);
		 if (FALSE == bRet)
		 {
			 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				 s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-08�޸�
			 else
				 s_lastError = -2; //�ǽ�/�Ӵ�ʧ��ʱ������-2��������2013-06-08�޸�
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
			 s_lastError = -1;  //��Ƭ���Ͳ���ʱ������-1��������2013-06-13�޸�
			 return -1;
		 }

		 if (buf[2] == 0xFC)
		 {
			 s_lastError = -4;  //��Ƭ���Ͳ���ʱ������-1��������2013-06-13�޸�
			 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				 s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-08�޸�
			 return s_lastError;
		 }

		 // ˵��û��ȡ����λ��Ϣ
		 if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
			 s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-08�޸�
		 else
			 s_lastError = -2; //�ǽ�/�Ӵ�ʧ��ʱ������-2��������2013-06-08�޸�
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
			s_lastError = -11;  // �ǽ�ʧ��ʱ������-11��������2013-06-08�޸�
		else
			s_lastError = -2; //�Ӵ�ʧ��ʱ������-2��������2013-06-08�޸�
		return s_lastError; 
	}
	while(1)
	{
		bRet = HidD_GetFeature(hFile, buf, 33);
		if (FALSE == bRet)
		{
			if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-08�޸�
			else
				s_lastError = -2; //�ǽ�/�Ӵ�ʧ��ʱ������-2��������2013-06-08�޸�
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
			s_lastError = -1;  //��Ƭ���Ͳ���ʱ������-1��������2013-06-13�޸�
			return -1;
		}

		if (buf[2] == 0xFC)
		{
			s_lastError = -4;  //��Ƭ���Ͳ���ʱ������-1��������2013-06-13�޸�
			if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-08�޸�
			return s_lastError;
		}

		// ˵��û��ȡ����λ��Ϣ
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
			s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-08�޸�
		else
			s_lastError = -2; //�ǽ�/�Ӵ�ʧ��ʱ������-2��������2013-06-08�޸�
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
		return /*s_lastError*/-5; // ʧ�ܷ���-5����������2012-08-27�޸�
	}
	while(1)
	{
		bRet = HidD_GetFeature(hFile, buf, 33);
		if (FALSE == bRet)
		{
			s_lastError = /*HID_ERROR_READ_FAILED*/-5;
			return /*s_lastError*/-5; // ʧ�ܷ���-5����������2012-08-27�޸�
		}
		if(buf[1] <= 0x20)
		{
			break;
		}
	}
	if (buf[1] == 1)
	{
// 		s_lastError = -5;
// 		return -5; // ʧ�ܷ���-5����������2012-08-27�޸ģ�ע����ǰû����һ��
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

// �����
// case1  5
// case2  5
// case3  5+lc
// case4  5+lc+1

/*	if (Length_of_Command_APDU < 5)
	{
		*RespLen = 0;
		s_lastError = -6;
		return -6; // ʧ�ܷ���-6����������2012-08-27�޸ģ���ʾ��֧�ָ�����
	}*/
	
	unsigned char nLength = *(Command_APDU+4);
/*	if((nLength+5) != Length_of_Command_APDU 
		&& (nLength+5+1) != Length_of_Command_APDU)
	{
		if (Length_of_Command_APDU != 5)
		{
			*RespLen = 0;
			s_lastError = -7;
			return -7; // ����-7����ʾ���Ȳ��ԣ���������2012-08-27�޸�
		}
	}*/

	//�ְ�,ÿ����32���ֽڣ���һ���ֽ�Ϊ0x20����ʾ����İ��л������ݣ�����Ϊʵ�ʵ����ݳ��ȣ�ʣ���ֽڵ�Ϊ��Ч���ݣ�������Ч����Ϊ���͵�ָ��
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

	//���ͻ�������һ�ֽ�Ϊ0 �� ReportID �����豸�ж��壩
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
		bRet = HidD_SetFeature(hFile, pSendBuf, 33);			//win 98 ��2000��XP �У��˺���ͨ�����ƶ˵㴫������
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
			//�������������ݸ�ʽ�İ�����������
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
		s_lastError = -2; //�ǽ�/�Ӵ��޿�������-2��������2013-06-18�޸�
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-18�޸�
		return s_lastError;
	}

	if ((*Response_APDU) == -4)
	{
		s_lastError = -4; //��Ƭ��Ӧ��ʱ������-4��������2013-06-13�޸�
		return s_lastError;
	}
	
	(*RespLen) --;

	memmove(Response_APDU, Response_APDU+1, (*RespLen));

	OutputTrace(" = ");
	OutputTraceByte((BYTE*)Response_APDU, *RespLen);
	OutputTrace("\r\n");
	
	s_lastError = 0;

/*	if ((*(Response_APDU+(*RespLen)-2) == 0x6D) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //������2013-06-09�޸�
	{
		s_lastError = -6;
		return s_lastError;
	}*/

	if ((*(Response_APDU+(*RespLen)-2) == 0x6E) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //������2013-06-09�޸�
	{
		s_lastError = -6;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x6A) && (*(Response_APDU+(*RespLen)-1) == -122)) //������2013-06-09�޸�
	{
		s_lastError = -8;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x67) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //������2013-06-09�޸�
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

// �����
// case1  5
// case2  5
// case3  5+lc
// case4  5+lc+1

/*	if (Length_of_Command_APDU < 5)
	{
		*RespLen = 0;
		s_lastError = -6;
		return -6; // ʧ�ܷ���-6����������2012-08-27�޸ģ���ʾ��֧�ָ�����
	}*/
	
	unsigned char nLength = *(Command_APDU+4);
/*	if((nLength+5) != Length_of_Command_APDU 
		&& (nLength+5+1) != Length_of_Command_APDU)
	{
		if (Length_of_Command_APDU != 5)
		{
			*RespLen = 0;
			s_lastError = -7;
			return -7; // ����-7����ʾ���Ȳ��ԣ���������2012-08-27�޸�
		}
	}*/

	//�ְ�,ÿ����32���ֽڣ���һ���ֽ�Ϊ0x20����ʾ����İ��л������ݣ�����Ϊʵ�ʵ����ݳ��ȣ�ʣ���ֽڵ�Ϊ��Ч���ݣ�������Ч����Ϊ���͵�ָ��
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

	//���ͻ�������һ�ֽ�Ϊ0 �� ReportID �����豸�ж��壩
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
		bRet = HidD_SetFeature(hFile, pSendBuf, 33);			//win 98 ��2000��XP �У��˺���ͨ�����ƶ˵㴫������
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
			//�������������ݸ�ʽ�İ�����������
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
		s_lastError = -2; //�ǽ�/�Ӵ��޿�������-2��������2013-06-18�޸�
		if ((ICC_Slot_No == 1) || (ICC_Slot_No == 2))
				s_lastError = -11;  // SAM1/SAM2ʧ��ʱ������-11��������2013-06-18�޸�
		return s_lastError;
	}

	if ((*Response_APDU) == -4)
	{
		s_lastError = -4; //��Ƭ��Ӧ��ʱ������-4��������2013-06-13�޸�
		return s_lastError;
	}
	
	(*RespLen) --;

	memmove(Response_APDU, Response_APDU+1, (*RespLen));
	
	s_lastError = 0;

/*	if ((*(Response_APDU+(*RespLen)-2) == 0x6D) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //������2013-06-09�޸�
	{
		s_lastError = -6;
		return s_lastError;
	}*/

	if ((*(Response_APDU+(*RespLen)-2) == 0x6E) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //������2013-06-09�޸�
	{
		s_lastError = -6;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x6A) && (*(Response_APDU+(*RespLen)-1) == -122)) //������2013-06-09�޸�
	{
		s_lastError = -8;
		return s_lastError;
	}

	if ((*(Response_APDU+(*RespLen)-2) == 0x67) && (*(Response_APDU+(*RespLen)-1) == 0x00))  //������2013-06-09�޸�
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
			strcpy(info, "���豸ʧ��");
			break;
		default:
			strcpy(info, "���豸�ɹ�");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case eClose:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "�ر��豸�ɹ�");
			break;
		case -5:
			strcpy(info, "�ӿ��豸����");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case eReset:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "��λ�ɹ�");
			break;
		case -2:
			strcpy(info, "�޿�");
			break;
		case -8:
			strcpy(info, "�����������");
			break;
		case -5:
			strcpy(info, "�ӿ��豸����");
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case ePowerOff:
		switch (s_lastError)
		{
		case 0:
			strcpy(info, "�µ�ɹ�");
			break;
		case -5:
			strcpy(info, "�ӿ��豸����");
			break;
		case -8:
			strcpy(info, "�����������");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	case eApplication:
		switch (s_lastError)
		{
		case -5:
			strcpy(info, "�ӿ��豸����");
			break;
		case -6:
			strcpy(info, "��֧�ָ�����");
			break;
		case -7:
			strcpy(info, "����ȴ���");
			break;
		case -8:
			strcpy(info, "�����������");
			break;
		}
		s_lastError = 0;
		s_lastProcess = eNoProcess;
		break;
	}

	return 0;
}

#define READER_VERSION		"Tianyu          TY522     10"   //ȥ�� 420   //������2013-06-09�޸�
int  CHIDReader::_ICC_Reader_Version(char* info)
{
	strcpy(info, READER_VERSION);
	s_lastError = 0;
	return 0;
}

