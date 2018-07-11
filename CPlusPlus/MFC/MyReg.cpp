//MyReg.cpp
#include "MyReg.h"
//���ĳ�����Ƿ����
ULONG TestRegSubKey(HKEY StartKey,char * NewSubKeyName)
{
	HKEY hKey;
	if(ERROR_SUCCESS == RegOpenKeyA(StartKey,NewSubKeyName,&hKey))
	{
		RegCloseKey(hKey);
		return 1;
	}
	return 0;
}
//��������
ULONG NewRegSubKey(HKEY StartKey,char * NewSubKeyName)
{//StartKey ȡ HKEY_CLASSES_ROOT ��HKEY_CURRENT_USER ��HKEY_LOCAL_MACHINE
	//HKEY_USERS��HKEY_PERFORMANCE_DATA��
	HKEY hKey;
	if(ERROR_SUCCESS == RegOpenKeyA(StartKey,NewSubKeyName,&hKey))
	{
		RegCloseKey(hKey);
		return 1;
	}
	if(ERROR_SUCCESS != RegCreateKeyA(StartKey,NewSubKeyName,&hKey))
	{
		return 0;
	}
	RegCloseKey(hKey);
	return 1;
}

//�����Ӽ�
ULONG NewRegKeyValue(HKEY StartKey,char * SubKeyName,char * NewValueName,ULONG NewValueType)
{//NewValueType ȡ REG_DWORD �� REG_SZ �� REG_MULTI_SZ �� REG_BINARY ��
	HKEY  hKey;
	ULONG ValueType;
	ULONG Buffer;
	ULONG BufferLen = 4;

	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_READ|KEY_WRITE,&hKey))
	{
		if(ERROR_SUCCESS != RegCreateKeyA(StartKey,SubKeyName,&hKey))
		{
			return 0;
		}
	}
	else if(ERROR_SUCCESS == RegQueryValueExA(hKey,NewValueName,0,&ValueType,(unsigned char *)&Buffer,&BufferLen))
	{
		if(ValueType == NewValueType)
		{
			RegCloseKey(hKey);
			return 1;
		}
	}
	if(ERROR_SUCCESS != RegSetValueExA(hKey,NewValueName,0,NewValueType,NULL,0))
	{
		RegCloseKey(hKey);
		return 0;
	}
	RegCloseKey(hKey);
	return 1;
}

//��ȡ�Ӽ����ݡ����������Ĭ���Ӽ�ֵ���Ӽ�����
ULONG ReadRegValueData(HKEY StartKey,char * SubKeyName,char * ValueName
					   ,PULONG OutValueType,PVOID InOutBuffer,PULONG InOutBufferLen)
{
	HKEY hKey;
	if((NULL == OutValueType) || (NULL == InOutBuffer) || (NULL == InOutBufferLen))
	{
		return 0;
	}
	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_READ,&hKey))
	{
		return 0;
	}
	if(ERROR_SUCCESS != RegQueryValueExA(hKey,ValueName,0,OutValueType,(unsigned char *)InOutBuffer,InOutBufferLen))
	{
		RegCloseKey(hKey);
		return 0;
	}
	RegCloseKey(hKey);
	return 1;
}


//��������Ĭ��ֵ�ַ���
ULONG SetRegKeyStr(HKEY StartKey,char * SubKeyName,char * DataBuffer)
{
	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_WRITE,&hKey))
	{
		if(ERROR_SUCCESS != RegCreateKeyA(StartKey,SubKeyName,&hKey))
		{
			return 0;
		}
	}
	if(ERROR_SUCCESS != RegSetValueExA(hKey,NULL,0,REG_SZ,(unsigned char *)DataBuffer,strlen(DataBuffer)))
	{
		RegCloseKey(hKey);
		return 0;
	}
	RegCloseKey(hKey);
	return 1;
}

//�����Ӽ�����ֵ
ULONG SetRegKeyValueData(HKEY StartKey,char * SubKeyName,char * ValueName,ULONG NewValueType
						 ,PVOID DataBuffer,ULONG DataLen)
{
	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_WRITE,&hKey))
	{
		if(ERROR_SUCCESS != RegCreateKeyA(StartKey,SubKeyName,&hKey))
		{
			return 0;
		}
	}
	if(ERROR_SUCCESS != RegSetValueExA(hKey,ValueName,0,NewValueType,(unsigned char *)DataBuffer,DataLen))
	{
		RegCloseKey(hKey);
		return 0;
	}
	RegCloseKey(hKey);
	return 1;
}

//��ѯ������Ŀ
ULONG NumRegSubKey(HKEY StartKey,char * SubKeyName)
{
	HKEY  hKey;
	char  achClass[MAX_PATH] = {0};
	ULONG cchClassName = MAX_PATH;
	ULONG    cSubKeys = 0;
	ULONG    cbMaxSubKey;
	ULONG    cchMaxClass;
	ULONG    cValues;
	ULONG    cchMaxValue;
	ULONG    cbMaxValueData;
	ULONG    cbSecurityDescriptor;
	FILETIME ftLastWriteTime;

	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_READ|KEY_WRITE,&hKey))
	{
		return 0;
	}
	RegQueryInfoKeyA(
						hKey,                    // ���ľ��
						achClass,                //  ���� 
						&cchClassName,           // �������� 
						NULL,                    // ����
						&cSubKeys,               // �Ӽ�������
						&cbMaxSubKey,            // �Ӽ�����
						&cchMaxClass,            // �೤��
						&cValues,                // �Ӽ���ֵ����
						&cchMaxValue,            // �Ӽ�������
						&cbMaxValueData,         // ��ֵ����
						&cbSecurityDescriptor,   // ��ȫ������
						&ftLastWriteTime);       // ���дʱ�� 
	RegCloseKey(hKey);
	return cSubKeys;
}

//��ѯ�Ӽ���Ŀ
ULONG NumRegSubKeyValue(HKEY StartKey,char * SubKeyName)
{
	HKEY  hKey;
	char  achClass[MAX_PATH] = {0};
	ULONG cchClassName = MAX_PATH;
	ULONG    cSubKeys = 0;
	ULONG    cbMaxSubKey;
	ULONG    cchMaxClass;
	ULONG    cValues = 0;
	ULONG    cchMaxValue;
	ULONG    cbMaxValueData;
	ULONG    cbSecurityDescriptor;
	FILETIME ftLastWriteTime;

	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_READ|KEY_WRITE,&hKey))
	{
		return 0;
	}
	RegQueryInfoKeyA(
						hKey,                    // ���ľ��
						achClass,                //  ���� 
						&cchClassName,           // �������� 
						NULL,                    // ����
						&cSubKeys,               // �Ӽ�������
						&cbMaxSubKey,            // �Ӽ�����
						&cchMaxClass,            // �೤��
						&cValues,                // �Ӽ���ֵ����
						&cchMaxValue,            // �Ӽ�������
						&cbMaxValueData,         // ��ֵ����
						&cbSecurityDescriptor,   // ��ȫ������
						&ftLastWriteTime);       // ���дʱ�� 
	RegCloseKey(hKey);
	return cValues;
}

//��ȡ����������
ULONG ReadRegSubKeyOrdName(HKEY StartKey,char * SubKeyName,ULONG Ord,char* OutName,PULONG OutNameLen)
{
	HKEY  hKey;
	ULONG retCode = 1;
	FILETIME ftLastWriteTime;

	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_READ|KEY_WRITE,&hKey))
	{
		return 0;
	}
	if(ERROR_SUCCESS != RegEnumKeyExA(hKey,
							Ord,
							OutName, 
							OutNameLen, 
							NULL, 
							NULL, 
							NULL, 
							&ftLastWriteTime))
	{
		retCode = 0;
	}
	RegCloseKey(hKey);
	return retCode;
}
//��ȡ�����Ӽ���
ULONG ReadRegSubKeyValueOrdName(HKEY StartKey,char * SubKeyName,ULONG Ord,char* OutName,PULONG OutNameLen)
{
	HKEY  hKey;
	ULONG retCode = 1;

	if(ERROR_SUCCESS != RegOpenKeyExA(StartKey,SubKeyName,0,KEY_READ|KEY_WRITE,&hKey))
	{
		return 0;
	}
	if(ERROR_SUCCESS != RegEnumValueA(hKey,
							Ord,
							OutName, 
							OutNameLen, 
							NULL, 
							NULL, 
							NULL, 
							NULL))
	{
		retCode = 0;
	}
	RegCloseKey(hKey);
	return retCode;
}

/*
void main()
{
	char DataBuffer[128] = {0};
	ULONG BufferLen = 128 , ValueType = 0;

	if(NewRegSubKey(HKEY_CURRENT_USER,"������"))
	{
		printf("��������ɹ�");
	}
	if(NewRegSubKey(HKEY_CURRENT_USER,"������1\\000"))
	{
		printf("��������ɹ�");
	}
	if(NewRegKeyValue(HKEY_CURRENT_USER,"������","000",REG_SZ))
	{
		printf("�����Ӽ��ɹ�");
	}
	if(NewRegKeyValue(HKEY_CURRENT_USER,"������","000",REG_BINARY))
	{
		printf("�����Ӽ��ɹ�");
	}
	if(SetRegKeyStr(HKEY_CURRENT_USER,"������","0123456789"))
	{
		printf("�����Ӽ��ɹ�");
	}
	if(ReadRegValueData(HKEY_CURRENT_USER,"������",NULL,&ValueType,DataBuffer,&BufferLen))
	{
		printf("��ѯ�Ӽ��ɹ�");
	}
	memset(DataBuffer,0,128);
	BufferLen = 128;
	if(ReadRegValueData(HKEY_CURRENT_USER,"������","000",&ValueType,DataBuffer,&BufferLen))
	{
		printf("��ѯ�Ӽ��ɹ�\r\n\r\n");
	}

	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"������");
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,NULL);
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"AppEvents");
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"Software");
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"Software\\ACD Systems");

	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"������");
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,NULL);
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"AppEvents");
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"Software");
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"Software\\ACD Systems");

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyOrdName(HKEY_CURRENT_USER,NULL,5,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyOrdName(HKEY_CURRENT_USER,"Software",5,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyOrdName(HKEY_CURRENT_USER,"Software\\ACD Systems",5,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,NULL,0,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,"Software",0,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,"Software\\ACD Systems",1,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,NULL,1,DataBuffer,&BufferLen))
	{
		printf("��ѯ����ɹ�:%s\r\n\r\n",DataBuffer);
	}
}
*/