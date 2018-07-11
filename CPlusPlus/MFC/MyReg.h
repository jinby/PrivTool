//MyReg.h
#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

//检查某子项是否存在
ULONG TestRegSubKey(HKEY StartKey,char * NewSubKeyName);
//创建子项
ULONG NewRegSubKey(HKEY StartKey,char * NewSubKeyName);
//创建子键
ULONG NewRegKeyValue(HKEY StartKey,char * SubKeyName,char * NewValueName,ULONG NewValueType);
//读取子键数据、包括子项的默认子键值和子键类型
ULONG ReadRegValueData(HKEY StartKey,char * SubKeyName,char * ValueName
					   ,PULONG OutValueType,PVOID InOutBuffer,PULONG InOutBufferLen);
//设置子项默认值字符串
ULONG SetRegKeyStr(HKEY StartKey,char * SubKeyName,char * DataBuffer);
//设置子键数据值
ULONG SetRegKeyValueData(HKEY StartKey,char * SubKeyName,char * ValueName,ULONG NewValueType
						 ,PVOID DataBuffer,ULONG DataLen);
//查询子项数目
ULONG NumRegSubKey(HKEY StartKey,char * SubKeyName);
//查询子键数目
ULONG NumRegSubKeyValue(HKEY StartKey,char * SubKeyName);
//读取索引子项名
ULONG ReadRegSubKeyOrdName(HKEY StartKey,char * SubKeyName,ULONG Ord,char* OutName,PULONG OutNameLen);
//读取索引子键名
ULONG ReadRegSubKeyValueOrdName(HKEY StartKey,char * SubKeyName,ULONG Ord,char* OutName,PULONG OutNameLen);

