//MyReg.cpp
#include "MyReg.h"
//检查某子项是否存在
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
//创建子项
ULONG NewRegSubKey(HKEY StartKey,char * NewSubKeyName)
{//StartKey 取 HKEY_CLASSES_ROOT 、HKEY_CURRENT_USER 、HKEY_LOCAL_MACHINE
	//HKEY_USERS、HKEY_PERFORMANCE_DATA等
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

//创建子键
ULONG NewRegKeyValue(HKEY StartKey,char * SubKeyName,char * NewValueName,ULONG NewValueType)
{//NewValueType 取 REG_DWORD 、 REG_SZ 、 REG_MULTI_SZ 、 REG_BINARY 等
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

//读取子键数据、包括子项的默认子键值和子键类型
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


//设置子项默认值字符串
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

//设置子键数据值
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

//查询子项数目
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
						hKey,                    // 键的句柄
						achClass,                //  类名 
						&cchClassName,           // 类名长度 
						NULL,                    // 保留
						&cSubKeys,               // 子键的数量
						&cbMaxSubKey,            // 子键长度
						&cchMaxClass,            // 类长度
						&cValues,                // 子键键值数量
						&cchMaxValue,            // 子键名长度
						&cbMaxValueData,         // 键值长度
						&cbSecurityDescriptor,   // 安全描述符
						&ftLastWriteTime);       // 最后写时间 
	RegCloseKey(hKey);
	return cSubKeys;
}

//查询子键数目
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
						hKey,                    // 键的句柄
						achClass,                //  类名 
						&cchClassName,           // 类名长度 
						NULL,                    // 保留
						&cSubKeys,               // 子键的数量
						&cbMaxSubKey,            // 子键长度
						&cchMaxClass,            // 类长度
						&cValues,                // 子键键值数量
						&cchMaxValue,            // 子键名长度
						&cbMaxValueData,         // 键值长度
						&cbSecurityDescriptor,   // 安全描述符
						&ftLastWriteTime);       // 最后写时间 
	RegCloseKey(hKey);
	return cValues;
}

//读取索引子项名
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
//读取索引子键名
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

	if(NewRegSubKey(HKEY_CURRENT_USER,"测试项"))
	{
		printf("创建子项成功");
	}
	if(NewRegSubKey(HKEY_CURRENT_USER,"测试项1\\000"))
	{
		printf("创建子项成功");
	}
	if(NewRegKeyValue(HKEY_CURRENT_USER,"测试项","000",REG_SZ))
	{
		printf("创建子键成功");
	}
	if(NewRegKeyValue(HKEY_CURRENT_USER,"测试项","000",REG_BINARY))
	{
		printf("创建子键成功");
	}
	if(SetRegKeyStr(HKEY_CURRENT_USER,"测试项","0123456789"))
	{
		printf("设置子键成功");
	}
	if(ReadRegValueData(HKEY_CURRENT_USER,"测试项",NULL,&ValueType,DataBuffer,&BufferLen))
	{
		printf("查询子键成功");
	}
	memset(DataBuffer,0,128);
	BufferLen = 128;
	if(ReadRegValueData(HKEY_CURRENT_USER,"测试项","000",&ValueType,DataBuffer,&BufferLen))
	{
		printf("查询子键成功\r\n\r\n");
	}

	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"测试项");
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,NULL);
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"AppEvents");
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"Software");
	BufferLen = NumRegSubKey(HKEY_CURRENT_USER,"Software\\ACD Systems");

	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"测试项");
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,NULL);
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"AppEvents");
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"Software");
	BufferLen = NumRegSubKeyValue(HKEY_CURRENT_USER,"Software\\ACD Systems");

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyOrdName(HKEY_CURRENT_USER,NULL,5,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyOrdName(HKEY_CURRENT_USER,"Software",5,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyOrdName(HKEY_CURRENT_USER,"Software\\ACD Systems",5,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,NULL,0,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,"Software",0,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,"Software\\ACD Systems",1,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}

	BufferLen = 128;
	memset(DataBuffer,0,128);
	if(ReadRegSubKeyValueOrdName(HKEY_CURRENT_USER,NULL,1,DataBuffer,&BufferLen))
	{
		printf("查询子项成功:%s\r\n\r\n",DataBuffer);
	}
}
*/